/*-
 * See the file LICENSE for redistribution information.
 *
 * Copyright (c) 1996-2006
 *	Oracle Corporation.  All rights reserved.
 *
 * $Id: mp_fput.c,v 12.22 2006/09/07 20:05:33 bostic Exp $
 */

#include "db_config.h"

#include "db_int.h"
#include "dbinc/log.h"
#include "dbinc/mp.h"

static int __memp_reset_lru __P((DB_ENV *, REGINFO *));

/*
 * __memp_fput_pp --
 *	DB_MPOOLFILE->put pre/post processing.
 *
 * PUBLIC: int __memp_fput_pp __P((DB_MPOOLFILE *, void *, u_int32_t));
 */
int
__memp_fput_pp(dbmfp, pgaddr, flags)
	DB_MPOOLFILE *dbmfp;
	void *pgaddr;
	u_int32_t flags;
{
	DB_ENV *dbenv;
	DB_THREAD_INFO *ip;
	int ret, t_ret;

	dbenv = dbmfp->dbenv;
	PANIC_CHECK(dbenv);

	ENV_ENTER(dbenv, ip);

	ret = __memp_fput(dbmfp, pgaddr, flags);
	if (IS_ENV_REPLICATED(dbenv) &&
	    (t_ret = __op_rep_exit(dbenv)) != 0 && ret == 0)
		ret = t_ret;

	ENV_LEAVE(dbenv, ip);
	return (ret);
}

/*
 * __memp_fput --
 *	DB_MPOOLFILE->put.
 *
 * PUBLIC: int __memp_fput __P((DB_MPOOLFILE *, void *, u_int32_t));
 */
int
__memp_fput(dbmfp, pgaddr, flags)
	DB_MPOOLFILE *dbmfp;
	void *pgaddr;
	u_int32_t flags;
{
	DB_ENV *dbenv;
	DB_MPOOL *dbmp;
	DB_MPOOL_HASH *hp;
	MPOOL *c_mp;
	MPOOLFILE *mfp;
	BH *bhp;
	u_int32_t n_cache;
	int adjust, ret, t_ret;

	dbenv = dbmfp->dbenv;
	MPF_ILLEGAL_BEFORE_OPEN(dbmfp, "DB_MPOOLFILE->put");
	dbmp = dbenv->mp_handle;
	mfp = dbmfp->mfp;
	bhp = (BH *)((u_int8_t *)pgaddr - SSZA(BH, buf));
	ret = 0;

	/*
	 * Check arguments, but don't fail because we want to unpin the page
	 * regardless.  The problem is when running with replication.  There
	 * is a reference count we incremented when __memp_fget was called,
	 * and we need to unpin the page and decrement that reference count.
	 * If we see flag problems, mark the page dirty.
	 */
	if (flags) {
		if (__db_fchk(dbenv, "memp_fput", flags,
		    DB_MPOOL_DISCARD) != 0) {
			flags = 0;
			ret = EINVAL;
			DB_ASSERT(dbenv, 0);
		}
	}

	/*
	 * If we're mapping the file, there's nothing to do.  Because we can
	 * stop mapping the file at any time, we have to check on each buffer
	 * to see if the address we gave the application was part of the map
	 * region.
	 */
	if (dbmfp->addr != NULL && pgaddr >= dbmfp->addr &&
	    (u_int8_t *)pgaddr <= (u_int8_t *)dbmfp->addr + dbmfp->len)
		return (0);

#ifdef DIAGNOSTIC
	/*
	 * Decrement the per-file pinned buffer count (mapped pages aren't
	 * counted).
	 */
	MPOOL_SYSTEM_LOCK(dbenv);
	if (dbmfp->pinref == 0) {
		MPOOL_SYSTEM_UNLOCK(dbenv);
		__db_errx(dbenv,
		    "%s: more pages returned than retrieved", __memp_fn(dbmfp));
		return (__db_panic(dbenv, EACCES));
	}
	--dbmfp->pinref;
	MPOOL_SYSTEM_UNLOCK(dbenv);
#endif

	/* Convert a page address to a buffer header and hash bucket. */
	n_cache = NCACHE(dbmp->reginfo[0].primary, bhp->mf_offset, bhp->pgno);
	c_mp = dbmp->reginfo[n_cache].primary;
	hp = R_ADDR(&dbmp->reginfo[n_cache], c_mp->htab);
	hp = &hp[NBUCKET(c_mp, bhp->mf_offset, bhp->pgno)];

	MUTEX_LOCK(dbenv, hp->mtx_hash);

	if (LF_ISSET(DB_MPOOL_DISCARD))
		F_SET(bhp, BH_DISCARD);

	/*
	 * Check for a reference count going to zero.  This can happen if the
	 * application returns a page twice.
	 */
	if (bhp->ref == 0) {
		__db_errx(dbenv, "%s: page %lu: unpinned page returned",
		    __memp_fn(dbmfp), (u_long)bhp->pgno);
		DB_ASSERT(dbenv, bhp->ref != 0);
		MUTEX_UNLOCK(dbenv, hp->mtx_hash);
		return (__db_panic(dbenv, EACCES));
	}

	/* Note the activity so allocation won't decide to quit. */
	++c_mp->put_counter;

	/*
	 * Mark the file dirty.  Check for a dirty bit on the buffer as well
	 * as the dirty flag because the buffer might have been marked dirty
	 * in the DB_MPOOLFILE->set method.
	 */
	if (F_ISSET(bhp, BH_DIRTY))
		mfp->file_written = 1;

	/*
	 * If more than one reference to the page or a reference other than a
	 * thread waiting to flush the buffer to disk, we're done.  Ignore the
	 * discard flags (for now) and leave the buffer's priority alone.
	 */
	if (--bhp->ref > 1 || (bhp->ref == 1 && !F_ISSET(bhp, BH_LOCKED))) {
		MUTEX_UNLOCK(dbenv, hp->mtx_hash);
		return (0);
	}

	/* The buffer should not be accessed again. */
	MVCC_MPROTECT(bhp->buf, mfp->stat.st_pagesize, 0);

	/* Update priority values. */
	if (F_ISSET(bhp, BH_DISCARD) || mfp->priority == MPOOL_PRI_VERY_LOW)
		bhp->priority = 0;
	else {
		/*
		 * We don't lock the LRU counter or the stat.st_pages field, if
		 * we get garbage (which won't happen on a 32-bit machine), it
		 * only means a buffer has the wrong priority.
		 */
		bhp->priority = c_mp->lru_count;

		adjust = 0;
		if (mfp->priority != 0)
			adjust = (int)c_mp->stat.st_pages / mfp->priority;

		if (F_ISSET(bhp, BH_DIRTY))
			adjust += (int)c_mp->stat.st_pages / MPOOL_PRI_DIRTY;

		if (adjust > 0) {
			if (UINT32_MAX - bhp->priority >= (u_int32_t)adjust)
				bhp->priority += adjust;
		} else if (adjust < 0)
			if (bhp->priority > (u_int32_t)-adjust)
				bhp->priority += adjust;
	}

	if (SH_TAILQ_FIRST(&hp->hash_bucket, __bh) ==
	    SH_TAILQ_LAST(&hp->hash_bucket, hq, __bh))
		hp->hash_priority = BH_PRIORITY(bhp);
	else
		__memp_bucket_reorder(dbenv, hp, bhp);
#ifdef DIAGNOSTIC
	__memp_check_order(dbenv, hp);
#endif

	/*
	 * The sync code has a separate counter for buffers on which it waits.
	 * It reads that value without holding a lock so we update it as the
	 * last thing we do.  Once that value goes to 0, we won't see another
	 * reference to that buffer being returned to the cache until the sync
	 * code has finished, so we're safe as long as we don't let the value
	 * go to 0 before we finish with the buffer.
	 */
	if (F_ISSET(bhp, BH_LOCKED) && bhp->ref_sync != 0)
		--bhp->ref_sync;

	MUTEX_UNLOCK(dbenv, hp->mtx_hash);

	/*
	 * On every buffer put we update the buffer generation number and check
	 * for wraparound.
	 */
	if (++c_mp->lru_count == UINT32_MAX)
		if ((t_ret =
		    __memp_reset_lru(dbenv, dbmp->reginfo)) != 0 && ret == 0)
			ret = t_ret;

	return (ret);
}

/*
 * __memp_reset_lru --
 *	Reset the cache LRU counter.
 */
static int
__memp_reset_lru(dbenv, infop)
	DB_ENV *dbenv;
	REGINFO *infop;
{
	BH *bhp, *tbhp;
	DB_MPOOL_HASH *hp;
	MPOOL *c_mp;
	u_int32_t bucket;

	c_mp = infop->primary;

	/*
	 * Update the counter so all future allocations will start at the
	 * bottom.
	 */
	c_mp->lru_count -= MPOOL_BASE_DECREMENT;

	/* Adjust the priority of every buffer in the system. */
	for (hp = R_ADDR(infop, c_mp->htab),
	    bucket = 0; bucket < c_mp->htab_buckets; ++hp, ++bucket) {
		/*
		 * Skip empty buckets.
		 *
		 * We can check for empty buckets before locking as we
		 * only care if the pointer is zero or non-zero.
		 */
		if (SH_TAILQ_FIRST(&hp->hash_bucket, __bh) == NULL)
			continue;

		MUTEX_LOCK(dbenv, hp->mtx_hash);
		SH_TAILQ_FOREACH(bhp, &hp->hash_bucket, hq, __bh)
			for (tbhp = bhp; tbhp != NULL;
			    tbhp = SH_CHAIN_PREV(tbhp, vc, __bh)) {
				if (tbhp->priority != UINT32_MAX &&
				    tbhp->priority > MPOOL_BASE_DECREMENT)
					tbhp->priority -= MPOOL_BASE_DECREMENT;
			}
		MUTEX_UNLOCK(dbenv, hp->mtx_hash);
	}

	return (0);
}
