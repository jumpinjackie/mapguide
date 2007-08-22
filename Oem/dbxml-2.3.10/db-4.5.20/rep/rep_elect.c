/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 2004-2006
 *	Oracle Corporation.  All rights reserved.
 *
 * $Id: rep_elect.c,v 12.35 2006/09/13 17:43:40 alanb Exp $
 */

#include "db_config.h"

#include "db_int.h"
#include "dbinc/db_page.h"
#include "dbinc/db_am.h"
#include "dbinc/log.h"

/*
 * We need to check sites == nsites, not more than half
 * like we do in __rep_elect and the VOTE2 code.  The
 * reason is that we want to process all the incoming votes
 * and not short-circuit once we reach more than half.  The
 * real winner's vote may be in the last half.
 */
#define	IS_PHASE1_DONE(rep)    \
    ((rep)->sites >= (rep)->nsites && (rep)->w_priority > 0)

#define	I_HAVE_WON(rep, winner)   \
    ((rep)->votes >= (rep)->nvotes && winner == (rep)->eid)

static void __rep_cmp_vote __P((DB_ENV *, REP *, int, DB_LSN *,
    int, u_int32_t, u_int32_t, u_int32_t));
static int __rep_elect_init
	       __P((DB_ENV *, DB_LSN *, int, int, int *, u_int32_t *));
static int __rep_tally __P((DB_ENV *, REP *, int, int *, u_int32_t, roff_t));
static int __rep_wait __P((DB_ENV *, u_int32_t, int *, u_int32_t));

/*
 * __rep_elect --
 *	Called after master failure to hold/participate in an election for
 *	a new master.
 *
 * PUBLIC:  int __rep_elect __P((DB_ENV *, int, int, int *, u_int32_t));
 */
int
__rep_elect(dbenv, given_nsites, nvotes, eidp, flags)
	DB_ENV *dbenv;
	int given_nsites, nvotes;
	int *eidp;
	u_int32_t flags;
{
	DB_LOG *dblp;
	DB_LSN lsn;
	DB_REP *db_rep;
	LOG *lp;
	REP *rep;
	int ack, done, in_progress, nsites, priority, realpri, ret, send_vote;
	u_int32_t ctlflags, egen, orig_tally, tiebreaker, to, timeout;
#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#endif
	COMPQUIET(flags, 0);

	PANIC_CHECK(dbenv);
	ENV_REQUIRES_CONFIG_XX(
	    dbenv, rep_handle, "DB_ENV->rep_elect", DB_INIT_REP);

	/* Error checking. */
	if (given_nsites < 0) {
		__db_errx(dbenv,
		    "DB_ENV->rep_elect: nsites may not be negative");
		return (EINVAL);
	}
	if (nvotes < 0) {
		__db_errx(dbenv,
		    "DB_ENV->rep_elect: nvotes may not be negative");
		return (EINVAL);
	}

	db_rep = dbenv->rep_handle;
	rep = db_rep->region;
	dblp = dbenv->lg_handle;
	lp = dblp->reginfo.primary;

	/*
	 * Specifying 0 for nsites signals us to use the value configured
	 * previously via rep_set_nsites.  Similarly, if the given nvotes is 0,
	 * it asks us to compute the value representing a simple majority.
	 */
	nsites = given_nsites == 0 ? db_rep->config_nsites : given_nsites;
	ack = nvotes == 0 ? ELECTION_MAJORITY(nsites) : nvotes;

	/*
	 * XXX
	 * If users give us less than a majority, they run the risk of
	 * having a network partition.  However, this also allows the
	 * scenario of master/1 client to elect the client.  Allow
	 * sub-majority values, but give a warning.
	 */
	if (ack <= (nsites / 2)) {
		__db_errx(dbenv,
    "DB_ENV->rep_elect:WARNING: nvotes (%d) is sub-majority with nsites (%d)",
		    nvotes, nsites);
	}

	if (nsites < ack) {
		__db_errx(dbenv,
    "DB_ENV->rep_elect: nvotes (%d) is larger than nsites (%d)",
		    ack, nsites);
		return (EINVAL);
	}

	timeout = rep->elect_timeout;
	realpri = rep->priority;

	RPRINT(dbenv,
	    (dbenv, &mb, "Start election nsites %d, ack %d, priority %d",
	    nsites, ack, realpri));

	/*
	 * Special case when having an election while running with
	 * sites of potentially mixed versions.  We set a bit indicating
	 * we're an electable site, but set our priority to 0.
	 * Old sites will never elect us, with 0 priority, but if all
	 * we have are new sites, then we can elect the best electable
	 * site of the group.
	 *     Thus 'priority' is this special, possibly-fake, effective
	 * priority that we'll use for this election, while 'realpri' is our
	 * real, configured priority, as retrieved from REP region.
	 */
	ctlflags = realpri != 0 ? REPCTL_ELECTABLE : 0;
	LOG_SYSTEM_LOCK(dbenv);
	lsn = lp->lsn;
	priority = lp->persist.version != DB_LOGVERSION ? 0 : realpri;
#ifdef	CONFIG_TEST
	/*
	 * This allows us to unit test the ELECTABLE flag simply by
	 * using the priority values.
	 */
	if (priority > 0 && priority <= 10) {
		RPRINT(dbenv,
		    (dbenv, &mb,
	   "Artificially setting priority 0 (ELECTABLE) for CONFIG_TEST mode"));
		DB_ASSERT(dbenv, ctlflags == REPCTL_ELECTABLE);
		priority = 0;
	}
#endif
	LOG_SYSTEM_UNLOCK(dbenv);

	orig_tally = 0;
	to = timeout;
	if ((ret = __rep_elect_init(dbenv,
	    &lsn, nsites, ack, &in_progress, &orig_tally)) != 0) {
		if (ret == DB_REP_NEWMASTER) {
			ret = 0;
			*eidp = rep->eid;
		}
		goto err;
	}
	/*
	 * If another thread is in the middle of an election we
	 * just quietly return and not interfere.
	 */
	if (in_progress) {
		*eidp = rep->master_id;
		return (0);
	}
	__os_clock(dbenv, &rep->esec, &rep->eusec);
restart:
	/* Generate a randomized tiebreaker value. */
	__os_unique_id(dbenv, &tiebreaker);

	REP_SYSTEM_LOCK(dbenv);
	F_SET(rep, REP_F_EPHASE1 | REP_F_NOARCHIVE);
	F_CLR(rep, REP_F_TALLY);

	/*
	 * We are about to participate at this egen.  We must
	 * write out the next egen before participating in this one
	 * so that if we crash we can never participate in this egen
	 * again.
	 */
	if ((ret = __rep_write_egen(dbenv, rep->egen + 1)) != 0)
		goto lockdone;

	/* Tally our own vote */
	if (__rep_tally(dbenv, rep, rep->eid, &rep->sites, rep->egen,
	    rep->tally_off) != 0) {
		ret = EINVAL;
		goto lockdone;
	}
	__rep_cmp_vote(dbenv, rep, rep->eid, &lsn, priority, rep->gen,
	    tiebreaker, ctlflags);

	RPRINT(dbenv, (dbenv, &mb, "Beginning an election"));

	/* Now send vote */
	send_vote = DB_EID_INVALID;
	egen = rep->egen;
	done = IS_PHASE1_DONE(rep);
	REP_SYSTEM_UNLOCK(dbenv);
	__rep_send_vote(dbenv, &lsn, nsites, ack, priority, tiebreaker, egen,
	    DB_EID_BROADCAST, REP_VOTE1, ctlflags);
	DB_ENV_TEST_RECOVERY(dbenv, DB_TEST_ELECTVOTE1, ret, NULL);
	if (done) {
		REP_SYSTEM_LOCK(dbenv);
		goto vote;
	}
	ret = __rep_wait(dbenv, to, eidp, REP_F_EPHASE1);
	switch (ret) {
		case 0:
			/* Check if election complete or phase complete. */
			if (*eidp != DB_EID_INVALID && !IN_ELECTION(rep)) {
				RPRINT(dbenv, (dbenv, &mb,
				    "Ended election phase 1"));
				goto edone;
			}
			goto phase2;
		case DB_REP_EGENCHG:
			if (to > timeout)
				to = timeout;
			to = (to * 8) / 10;
			RPRINT(dbenv, (dbenv, &mb,
"Egen changed while waiting. Now %lu.  New timeout %lu, orig timeout %lu",
			    (u_long)rep->egen, (u_long)to, (u_long)timeout));
			/*
			 * If the egen changed while we were sleeping, that
			 * means we're probably late to the next election,
			 * so we'll backoff our timeout so that we don't get
			 * into an out-of-phase election scenario.
			 *
			 * Backoff to 80% of the current timeout.
			 */
			goto restart;
		case DB_TIMEOUT:
			break;
		default:
			goto err;
	}
	/*
	 * If we got here, we haven't heard from everyone, but we've
	 * run out of time, so it's time to decide if we have enough
	 * votes to pick a winner and if so, to send out a vote to
	 * the winner.
	 */
	REP_SYSTEM_LOCK(dbenv);
	/*
	 * If our egen changed while we were waiting.  We need to
	 * essentially reinitialize our election.
	 */
	if (egen != rep->egen) {
		REP_SYSTEM_UNLOCK(dbenv);
		RPRINT(dbenv, (dbenv, &mb, "Egen changed from %lu to %lu",
		    (u_long)egen, (u_long)rep->egen));
		goto restart;
	}
	if (rep->sites >= rep->nvotes) {
vote:
		/* We think we've seen enough to cast a vote. */
		send_vote = rep->winner;
		/*
		 * See if we won.  This will make sure we
		 * don't count ourselves twice if we're racing
		 * with incoming votes.
		 */
		if (rep->winner == rep->eid) {
			(void)__rep_tally(dbenv, rep, rep->eid, &rep->votes,
			    egen, rep->v2tally_off);
			RPRINT(dbenv, (dbenv, &mb,
			    "Counted my vote %d", rep->votes));
		}
		F_SET(rep, REP_F_EPHASE2);
		F_CLR(rep, REP_F_EPHASE1);
	}
	REP_SYSTEM_UNLOCK(dbenv);
	if (send_vote == DB_EID_INVALID) {
		/* We do not have enough votes to elect. */
		RPRINT(dbenv, (dbenv, &mb,
		    "Not enough votes to elect: recvd %d of %d from %d sites",
		    rep->sites, rep->nvotes, rep->nsites));
		ret = DB_REP_UNAVAIL;
		goto err;

	} else {
		/*
		 * We have seen enough vote1's.  Now we need to wait
		 * for all the vote2's.
		 */
		if (send_vote != rep->eid) {
			RPRINT(dbenv, (dbenv, &mb, "Sending vote"));
			__rep_send_vote(dbenv, NULL, 0, 0, 0, 0, egen,
			    send_vote, REP_VOTE2, 0);
			/*
			 * If we are NOT the new master we want to send
			 * our vote to the winner, and wait longer.  The
			 * reason is that the winner may be "behind" us
			 * in the election waiting and if the master is
			 * down, the winner will wait the full timeout
			 * and we want to give the winner enough time to
			 * process all the votes.  Otherwise we could
			 * incorrectly return DB_REP_UNAVAIL and start a
			 * new election before the winner can declare
			 * itself.
			 */
			to = to * 2;

		}

phase2:
		if (I_HAVE_WON(rep, rep->winner)) {
			RPRINT(dbenv, (dbenv, &mb,
			    "Skipping phase2 wait: already got %d votes",
			    rep->votes));
			REP_SYSTEM_LOCK(dbenv);
			goto i_won;
		}
		ret = __rep_wait(dbenv, to, eidp, REP_F_EPHASE2);
		RPRINT(dbenv, (dbenv, &mb,
		    "Ended election phase 2 %d", ret));
		switch (ret) {
			case 0:
				if (*eidp != DB_EID_INVALID)				
					goto edone;
				ret = DB_REP_UNAVAIL;
				break;
			case DB_REP_EGENCHG:
				if (to > timeout)
					to = timeout;
				to = (to * 8) / 10;
				RPRINT(dbenv, (dbenv, &mb,
"While waiting egen changed to %lu.  Phase 2 New timeout %lu, orig timeout %lu",
				    (u_long)rep->egen,
				    (u_long)to, (u_long)timeout));
				goto restart;
			case DB_TIMEOUT:
				ret = DB_REP_UNAVAIL;
				break;
			default:
				goto err;
		}
		REP_SYSTEM_LOCK(dbenv);
		if (egen != rep->egen) {
			REP_SYSTEM_UNLOCK(dbenv);
			RPRINT(dbenv, (dbenv, &mb,
			    "Egen ph2 changed from %lu to %lu",
			    (u_long)egen, (u_long)rep->egen));
			goto restart;
		}
		done = rep->votes >= rep->nvotes;
		RPRINT(dbenv, (dbenv, &mb,
		    "After phase 2: votes %d, nvotes %d, nsites %d",
		    rep->votes, rep->nvotes, rep->nsites));
		if (I_HAVE_WON(rep, rep->winner)) {
i_won:			__rep_elect_master(dbenv, rep, eidp);
			ret = 0;
			goto lockdone;
		}
		REP_SYSTEM_UNLOCK(dbenv);
	}

err:	REP_SYSTEM_LOCK(dbenv);
lockdone:
	/*
	 * If we get here because of a non-election error, then we
	 * did not tally our vote.  The only non-election error is
	 * from elect_init where we were unable to grow_sites.  In
	 * that case we do not want to discard all known election info.
	 */
	if (ret == 0 || ret == DB_REP_UNAVAIL)
		__rep_elect_done(dbenv, rep);
	else if (orig_tally)
		F_SET(rep, orig_tally);

	/*
	 * If the election finished elsewhere, we need to decrement
	 * the elect_th anyway.
	 */
	if (0) {
edone:		REP_SYSTEM_LOCK(dbenv);
	}
	rep->elect_th = 0;

	RPRINT(dbenv, (dbenv, &mb,
	    "Ended election with %d, sites %d, egen %lu, flags 0x%lx",
	    ret, rep->sites, (u_long)rep->egen, (u_long)rep->flags));
	REP_SYSTEM_UNLOCK(dbenv);

DB_TEST_RECOVERY_LABEL
	return (ret);
}

/*
 * __rep_vote1 --
 *	Handle incoming vote1 message on a client.
 *
 * PUBLIC: int __rep_vote1 __P((DB_ENV *, REP_CONTROL *, DBT *, int *));
 */
int
__rep_vote1(dbenv, rp, rec, eidp)
	DB_ENV *dbenv;
	REP_CONTROL *rp;
	DBT *rec;
	int *eidp;
{
	DB_LOG *dblp;
	DB_LSN lsn;
	DB_REP *db_rep;
	DBT data_dbt;
	LOG *lp;
	REP *rep;
	REP_OLD_VOTE_INFO *ovi;
	REP_VOTE_INFO tmpvi, *vi;
	u_int32_t egen;
	int master, ret;
#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#endif

	ret = 0;
	db_rep = dbenv->rep_handle;
	rep = db_rep->region;
	dblp = dbenv->lg_handle;
	lp = dblp->reginfo.primary;

	if (F_ISSET(rep, REP_F_MASTER)) {
		RPRINT(dbenv, (dbenv, &mb, "Master received vote"));
		LOG_SYSTEM_LOCK(dbenv);
		lsn = lp->lsn;
		LOG_SYSTEM_UNLOCK(dbenv);
		(void)__rep_send_message(dbenv,
		    DB_EID_BROADCAST, REP_NEWMASTER, &lsn, NULL, 0, 0);
		return (ret);
	}

	if (rep->version == DB_REPVERSION_42) {
		ovi = (REP_OLD_VOTE_INFO *)rec->data;
		tmpvi.egen = ovi->egen;
		tmpvi.nsites = ovi->nsites;
		tmpvi.nvotes = ovi->nsites / 2 + 1;
		tmpvi.priority = ovi->priority;
		tmpvi.tiebreaker = ovi->tiebreaker;
		vi = &tmpvi;
	} else
		vi = (REP_VOTE_INFO *)rec->data;
	REP_SYSTEM_LOCK(dbenv);

	/*
	 * If we get a vote from a later election gen, we
	 * clear everything from the current one, and we'll
	 * start over by tallying it.  If we get an old vote,
	 * send an ALIVE to the old participant.
	 */
	RPRINT(dbenv, (dbenv, &mb,
	    "Received vote1 egen %lu, egen %lu",
	    (u_long)vi->egen, (u_long)rep->egen));
	if (vi->egen < rep->egen) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Received old vote %lu, egen %lu, ignoring vote1",
		    (u_long)vi->egen, (u_long)rep->egen));
		egen = rep->egen;
		REP_SYSTEM_UNLOCK(dbenv);
		data_dbt.data = &egen;
		data_dbt.size = sizeof(egen);
		(void)__rep_send_message(dbenv,
		    *eidp, REP_ALIVE, &rp->lsn, &data_dbt, 0, 0);
		return (ret);
	}
	if (vi->egen > rep->egen) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Received VOTE1 from egen %lu, my egen %lu; reset",
		    (u_long)vi->egen, (u_long)rep->egen));
		__rep_elect_done(dbenv, rep);
		rep->egen = vi->egen;
	}

	/*
	 * If this site (sender of the VOTE1) is the first to the party, simply
	 * initialize values from the message.  Otherwise, see if the site knows
	 * about more sites, and/or requires more votes, than we do.
	 */
	if (!IN_ELECTION_TALLY(rep)) {
		F_SET(rep, REP_F_TALLY);
		rep->nsites = vi->nsites;
		rep->nvotes = vi->nvotes;
	} else {
		if (vi->nsites > rep->nsites)
			rep->nsites = vi->nsites;
		if (vi->nvotes > rep->nvotes)
			rep->nvotes = vi->nvotes;
	}

	/*
	 * We are keeping the vote, let's see if that changes our
	 * count of the number of sites.
	 */
	if (rep->sites + 1 > rep->nsites)
		rep->nsites = rep->sites + 1;
	if (rep->nsites > rep->asites &&
	    (ret = __rep_grow_sites(dbenv, rep->nsites)) != 0) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Grow sites returned error %d", ret));
		goto err;
	}

	/*
	 * Ignore vote1's if we're in phase 2.
	 */
	if (F_ISSET(rep, REP_F_EPHASE2)) {
		RPRINT(dbenv, (dbenv, &mb,
		    "In phase 2, ignoring vote1"));
		goto err;
	}

	/*
	 * Record this vote.  If we get back non-zero, we
	 * ignore the vote.
	 */
	if ((ret = __rep_tally(dbenv, rep, *eidp, &rep->sites,
	    vi->egen, rep->tally_off)) != 0) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Tally returned %d, sites %d",
		    ret, rep->sites));
		ret = 0;
		goto err;
	}
	RPRINT(dbenv, (dbenv, &mb,
	    "Incoming vote: (eid)%d (pri)%d %s (gen)%lu (egen)%lu [%lu,%lu]",
	    *eidp, vi->priority,
	    F_ISSET(rp, REPCTL_ELECTABLE) ? "ELECTABLE" : "",
	    (u_long)rp->gen, (u_long)vi->egen,
	    (u_long)rp->lsn.file, (u_long)rp->lsn.offset));
#ifdef DIAGNOSTIC
	if (rep->sites > 1)
		RPRINT(dbenv, (dbenv, &mb,
    "Existing vote: (eid)%d (pri)%d (gen)%lu (sites)%d [%lu,%lu]",
		    rep->winner, rep->w_priority,
		    (u_long)rep->w_gen, rep->sites,
		    (u_long)rep->w_lsn.file,
		    (u_long)rep->w_lsn.offset));
#endif
	__rep_cmp_vote(dbenv, rep, *eidp, &rp->lsn, vi->priority,
	    rp->gen, vi->tiebreaker, rp->flags);
	/*
	 * If you get a vote and you're not in an election, we've
	 * already recorded this vote.  But that is all we need
	 * to do.
	 */
	if (!IN_ELECTION(rep)) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Not in election, but received vote1 0x%x",
		    rep->flags));
		ret = DB_REP_HOLDELECTION;
		goto err;
	}

	master = rep->winner;
	lsn = rep->w_lsn;
	if (IS_PHASE1_DONE(rep)) {
		RPRINT(dbenv,
		    (dbenv, &mb, "Phase1 election done"));
		RPRINT(dbenv, (dbenv, &mb, "Voting for %d%s",
		    master, master == rep->eid ? "(self)" : ""));
		egen = rep->egen;
		F_SET(rep, REP_F_EPHASE2);
		F_CLR(rep, REP_F_EPHASE1);
		if (master == rep->eid) {
			(void)__rep_tally(dbenv, rep, rep->eid,
			    &rep->votes, egen, rep->v2tally_off);
			RPRINT(dbenv, (dbenv, &mb,
			    "After phase 1 done: counted vote %d of %d",
			    rep->votes, rep->nvotes));
			if (I_HAVE_WON(rep, rep->winner)) {
				__rep_elect_master(dbenv, rep, eidp);
				ret = DB_REP_NEWMASTER;
			}
			goto err;
		}
		REP_SYSTEM_UNLOCK(dbenv);

		/* Vote for someone else. */
		__rep_send_vote(dbenv, NULL, 0, 0, 0, 0, egen,
		    master, REP_VOTE2, 0);
	} else
err:		REP_SYSTEM_UNLOCK(dbenv);
	return (ret);
}

/*
 * __rep_vote2 --
 *	Handle incoming vote2 message on a client.
 *
 * PUBLIC: int __rep_vote2 __P((DB_ENV *, DBT *, int *));
 */
int
__rep_vote2(dbenv, rec, eidp)
	DB_ENV *dbenv;
	DBT *rec;
	int *eidp;
{
	DB_LOG *dblp;
	DB_LSN lsn;
	DB_REP *db_rep;
	LOG *lp;
	REP *rep;
	REP_OLD_VOTE_INFO *ovi;
	REP_VOTE_INFO tmpvi, *vi;
	int ret;
#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#endif

	ret = 0;
	db_rep = dbenv->rep_handle;
	rep = db_rep->region;
	dblp = dbenv->lg_handle;
	lp = dblp->reginfo.primary;

	RPRINT(dbenv, (dbenv, &mb, "We received a vote%s",
	    F_ISSET(rep, REP_F_MASTER) ? " (master)" : ""));
	if (F_ISSET(rep, REP_F_MASTER)) {
		LOG_SYSTEM_LOCK(dbenv);
		lsn = lp->lsn;
		LOG_SYSTEM_UNLOCK(dbenv);
		rep->stat.st_elections_won++;
		(void)__rep_send_message(dbenv,
		    DB_EID_BROADCAST, REP_NEWMASTER, &lsn, NULL, 0, 0);
		return (ret);
	}

	REP_SYSTEM_LOCK(dbenv);

	/* If we have priority 0, we should never get a vote. */
	DB_ASSERT(dbenv, rep->priority != 0);

	/*
	 * We might be the last to the party and we haven't had
	 * time to tally all the vote1's, but others have and
	 * decided we're the winner.  So, if we're in the process
	 * of tallying sites, keep the vote so that when our
	 * election thread catches up we'll have the votes we
	 * already received.
	 */
	if (rep->version == DB_REPVERSION_42) {
		ovi = (REP_OLD_VOTE_INFO *)rec->data;
		tmpvi.egen = ovi->egen;
		tmpvi.nsites = ovi->nsites;
		tmpvi.nvotes = ovi->nsites / 2 + 1;
		tmpvi.priority = ovi->priority;
		tmpvi.tiebreaker = ovi->tiebreaker;
		vi = &tmpvi;
	} else
		vi = (REP_VOTE_INFO *)rec->data;
	if (!IN_ELECTION_TALLY(rep) && vi->egen >= rep->egen) {
		RPRINT(dbenv, (dbenv, &mb,
		    "Not in election gen %lu, at %lu, got vote",
		    (u_long)vi->egen, (u_long)rep->egen));
		ret = DB_REP_HOLDELECTION;
		goto err;
	}

	/*
	 * Record this vote.  In a VOTE2, the only valid entry
	 * in the REP_VOTE_INFO is the election generation.
	 *
	 * There are several things which can go wrong that we
	 * need to account for:
	 * 1. If we receive a latent VOTE2 from an earlier election,
	 * we want to ignore it.
	 * 2. If we receive a VOTE2 from a site from which we never
	 * received a VOTE1, we want to record it, because we simply
	 * may be processing messages out of order or its vote1 got lost,
	 * but that site got all the votes it needed to send it.
	 * 3. If we have received a duplicate VOTE2 from this election
	 * from the same site we want to ignore it.
	 * 4. If this is from the current election and someone is
	 * really voting for us, then we finally get to record it.
	 */
	/*
	 * Case 1.
	 */
	if (vi->egen != rep->egen) {
		RPRINT(dbenv, (dbenv, &mb, "Bad vote egen %lu.  Mine %lu",
		    (u_long)vi->egen, (u_long)rep->egen));
		ret = 0;
		goto err;
	}

	/*
	 * __rep_tally takes care of cases 2, 3 and 4.
	 */
	if ((ret = __rep_tally(dbenv, rep, *eidp, &rep->votes,
	    vi->egen, rep->v2tally_off)) != 0) {
		ret = 0;
		goto err;
	}
	RPRINT(dbenv, (dbenv, &mb, "Counted vote %d of %d",
	    rep->votes, rep->nvotes));
	if (I_HAVE_WON(rep, rep->winner)) {
		__rep_elect_master(dbenv, rep, eidp);
		ret = DB_REP_NEWMASTER;
	}

err:	REP_SYSTEM_UNLOCK(dbenv);
	return (ret);
}

/*
 * __rep_tally --
 *	Handle incoming vote message on a client.  Called with the db_rep
 *	mutex held.  This function will return 0 if we successfully tally
 *	the vote and non-zero if the vote is ignored.  This will record
 *	both VOTE1 and VOTE2 records, depending on which region offset the
 *	caller passed in.
 */
static int
__rep_tally(dbenv, rep, eid, countp, egen, vtoff)
	DB_ENV *dbenv;
	REP *rep;
	int eid, *countp;
	u_int32_t egen;
	roff_t vtoff;
{
	REP_VTALLY *tally, *vtp;
	int i;
#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#else
	COMPQUIET(rep, NULL);
#endif

	tally = R_ADDR((REGINFO *)dbenv->reginfo, vtoff);
	i = 0;
	vtp = &tally[i];
	while (i < *countp) {
		/*
		 * Ignore votes from earlier elections (i.e. we've heard
		 * from this site in this election, but its vote from an
		 * earlier election got delayed and we received it now).
		 * However, if we happened to hear from an earlier vote
		 * and we recorded it and we're now hearing from a later
		 * election we want to keep the updated one.  Note that
		 * updating the entry will not increase the count.
		 * Also ignore votes that are duplicates.
		 */
		if (vtp->eid == eid) {
			RPRINT(dbenv, (dbenv, &mb,
			    "Tally found[%d] (%d, %lu), this vote (%d, %lu)",
				    i, vtp->eid, (u_long)vtp->egen,
				    eid, (u_long)egen));
			if (vtp->egen >= egen)
				return (1);
			else {
				vtp->egen = egen;
				return (0);
			}
		}
		i++;
		vtp = &tally[i];
	}
	/*
	 * If we get here, we have a new voter we haven't
	 * seen before.  Tally this vote.
	 */
#ifdef DIAGNOSTIC
	if (vtoff == rep->tally_off)
		RPRINT(dbenv, (dbenv, &mb, "Tallying VOTE1[%d] (%d, %lu)",
		    i, eid, (u_long)egen));
	else
		RPRINT(dbenv, (dbenv, &mb, "Tallying VOTE2[%d] (%d, %lu)",
		    i, eid, (u_long)egen));
#endif
	vtp->eid = eid;
	vtp->egen = egen;
	(*countp)++;
	return (0);
}

/*
 * __rep_cmp_vote --
 *	Compare incoming vote1 message on a client.  Called with the db_rep
 *	mutex held.
 *
 */
static void
__rep_cmp_vote(dbenv, rep, eid, lsnp, priority, gen, tiebreaker, flags)
	DB_ENV *dbenv;
	REP *rep;
	int eid;
	DB_LSN *lsnp;
	int priority;
	u_int32_t flags, gen, tiebreaker;
{
	int cmp;

#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#else
	COMPQUIET(dbenv, NULL);
#endif
	cmp = LOG_COMPARE(lsnp, &rep->w_lsn);
	/*
	 * If we've seen more than one, compare us to the best so far.
	 * If we're the first, make ourselves the winner to start.
	 */
	if (rep->sites > 1 &&
	    (priority != 0 || LF_ISSET(REPCTL_ELECTABLE))) {
		/*
		 * Special case, if we have a mixed version group of sites,
		 * we set priority to 0, but set the ELECTABLE flag so that
		 * all sites talking at lower versions can correctly elect.
		 * If a non-zero priority comes in and current winner is
		 * zero priority (but was electable), then the non-zero
		 * site takes precedence no matter what its LSN is.
		 *
		 * Then LSN is determinant only if we're comparing
		 * like-styled version/priorities.  I.e. both with
		 * 0/ELECTABLE priority or both with non-zero priority.
		 * Then actual priority value if LSNs
		 * are equal, then tiebreaker if both are equal.
		 */
		if ((priority != 0 && rep->w_priority == 0) ||
		    (((priority == 0 && rep->w_priority == 0) ||
		     (priority != 0 && rep->w_priority != 0)) && cmp > 0) ||
		    (cmp == 0 && (priority > rep->w_priority ||
		    (priority == rep->w_priority &&
		    (tiebreaker > rep->w_tiebreaker))))) {
			RPRINT(dbenv, (dbenv, &mb, "Accepting new vote"));
			rep->winner = eid;
			rep->w_priority = priority;
			rep->w_lsn = *lsnp;
			rep->w_gen = gen;
			rep->w_tiebreaker = tiebreaker;
		}
	} else if (rep->sites == 1) {
		if (priority != 0 || LF_ISSET(REPCTL_ELECTABLE)) {
			/* Make ourselves the winner to start. */
			rep->winner = eid;
			rep->w_priority = priority;
			rep->w_gen = gen;
			rep->w_lsn = *lsnp;
			rep->w_tiebreaker = tiebreaker;
		} else {
			rep->winner = DB_EID_INVALID;
			rep->w_priority = -1;
			rep->w_gen = 0;
			ZERO_LSN(rep->w_lsn);
			rep->w_tiebreaker = 0;
		}
	}
	return;
}

/*
 * __rep_elect_init
 *	Initialize an election.  Sets beginp non-zero if the election is
 * already in progress; makes it 0 otherwise.
 */
static int
__rep_elect_init(dbenv, lsnp, nsites, nvotes, beginp, otally)
	DB_ENV *dbenv;
	DB_LSN *lsnp;
	int nsites, nvotes;
	int *beginp;
	u_int32_t *otally;
{
	DB_REP *db_rep;
	REP *rep;
	int ret;

	db_rep = dbenv->rep_handle;
	rep = db_rep->region;

	ret = 0;

	/* We may miscount, as we don't hold the replication mutex here. */
	rep->stat.st_elections++;

	/* If we are already a master; simply broadcast that fact and return. */
	if (F_ISSET(rep, REP_F_MASTER)) {
		(void)__rep_send_message(dbenv,
		    DB_EID_BROADCAST, REP_NEWMASTER, lsnp, NULL, 0, 0);
		rep->stat.st_elections_won++;
		return (DB_REP_NEWMASTER);
	}

	REP_SYSTEM_LOCK(dbenv);
	if (otally != NULL)
		*otally = F_ISSET(rep, REP_F_TALLY);
	*beginp = IN_ELECTION(rep) || rep->elect_th;
	if (!*beginp) {
		/*
		 * Make sure that we always initialize all the election fields
		 * before putting ourselves in an election state.  That means
		 * issuing calls that can fail (allocation) before setting all
		 * the variables.
		 */
		if (nsites > rep->asites &&
		    (ret = __rep_grow_sites(dbenv, nsites)) != 0)
			goto err;
		DB_ENV_TEST_RECOVERY(dbenv, DB_TEST_ELECTINIT, ret, NULL);
		rep->elect_th = 1;
		/*
		 * If we're the first to the party, we simply set initial
		 * values: pre-existing values would be left over from previous
		 * election.
		 */
		if (!IN_ELECTION_TALLY(rep)) {
			rep->nsites = nsites;
			rep->nvotes = nvotes;
		} else {
			if (nsites > rep->nsites)
				rep->nsites = nsites;
			if (nvotes > rep->nvotes)
				rep->nvotes = nvotes;
		}
	}
DB_TEST_RECOVERY_LABEL
err:	REP_SYSTEM_UNLOCK(dbenv);
	return (ret);
}

/*
 * __rep_elect_master
 *	Set up for new master from election.  Must be called with
 *	the replication region mutex held.
 *
 * PUBLIC: void __rep_elect_master __P((DB_ENV *, REP *, int *));
 */
void
__rep_elect_master(dbenv, rep, eidp)
	DB_ENV *dbenv;
	REP *rep;
	int *eidp;
{
#ifdef DIAGNOSTIC
	DB_MSGBUF mb;
#else
	COMPQUIET(dbenv, NULL);
#endif
	rep->master_id = rep->eid;
	F_SET(rep, REP_F_MASTERELECT);
	if (eidp != NULL)
		*eidp = rep->master_id;
	rep->stat.st_elections_won++;
	RPRINT(dbenv, (dbenv, &mb,
	    "Got enough votes to win; election done; winner is %d, gen %lu",
	    rep->master_id, (u_long)rep->gen));
}

static int
__rep_wait(dbenv, timeout, eidp, flags)
	DB_ENV *dbenv;
	u_int32_t timeout;
	int *eidp;
	u_int32_t flags;
{
	DB_REP *db_rep;
	REP *rep;
	int done, echg;
	u_int32_t egen, sleeptime;

	done = echg = 0;
	db_rep = dbenv->rep_handle;
	rep = db_rep->region;
	egen = rep->egen;

	/*
	 * The user specifies an overall timeout function, but checking
	 * is cheap and the timeout may be a generous upper bound.
	 * Sleep repeatedly for the smaller of .5s and timeout/10.
	 */
	sleeptime = (timeout > 5000000) ? 500000 : timeout / 10;
	if (sleeptime == 0)
		sleeptime++;
	while (timeout > 0) {
		__os_sleep(dbenv, 0, sleeptime);
		REP_SYSTEM_LOCK(dbenv);
		echg = egen != rep->egen;
		done = !F_ISSET(rep, flags);

		*eidp = rep->master_id;
		REP_SYSTEM_UNLOCK(dbenv);

		if (done)
			return (0);

		if (echg)
			return (DB_REP_EGENCHG);

		if (timeout > sleeptime)
			timeout -= sleeptime;
		else
			timeout = 0;
	}
	return (DB_TIMEOUT);
}
