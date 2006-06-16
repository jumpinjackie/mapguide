#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_10 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_DUP" "Db::DB_RMW" "Db::DB_SET" */
  /* Offset 9 gives the best switch position.  */
  switch (name[9]) {
  case 'P':
    if (memEQ(name, "Db::DB_DU", 9)) {
    /*                        P     */
      {
	int tmp = DB_DUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_SE", 9)) {
    /*                        T     */
      {
	int tmp = DB_SET;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_RM", 9)) {
    /*                        W     */
      {
	int tmp = DB_RMW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_11 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_EXCL" "Db::DB_HASH" "Db::DB_LAST" "Db::DB_NEXT" "Db::DB_PREV" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'E':
    if (memEQ(name, "Db::DB_EXCL", 11)) {
    /*                      ^          */
      {
	int tmp = DB_EXCL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_HASH", 11)) {
    /*                      ^          */
      {
	int tmp = DB_HASH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LAST", 11)) {
    /*                      ^          */
      {
	int tmp = DB_LAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NEXT", 11)) {
    /*                      ^          */
      {
	int tmp = DB_NEXT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PREV", 11)) {
    /*                      ^          */
      {
	int tmp = DB_PREV;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_12 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AFTER" "Db::DB_BTREE" "Db::DB_FIRST" "Db::DB_FLUSH" "Db::DB_FORCE"
     "Db::DB_QUEUE" "Db::DB_RECNO" "Db::MP_FLUSH" "Db::TXN_SYNC" */
  /* Offset 8 gives the best switch position.  */
  switch (name[8]) {
  case 'E':
    if (memEQ(name, "Db::DB_RECNO", 12)) {
    /*                       ^          */
      {
	int tmp = DB_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_AFTER", 12)) {
    /*                       ^          */
      {
	int tmp = DB_AFTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_FIRST", 12)) {
    /*                       ^          */
      {
	int tmp = DB_FIRST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_FLUSH", 12)) {
    /*                       ^          */
      {
	int tmp = DB_FLUSH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::MP_FLUSH", 12)) {
    /*                       ^          */
      {
	int tmp = MP_FLUSH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_FORCE", 12)) {
    /*                       ^          */
      {
	int tmp = DB_FORCE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::TXN_SYNC", 12)) {
    /*                       ^          */
      {
	int tmp = TXN_SYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_BTREE", 12)) {
    /*                       ^          */
      {
	int tmp = DB_BTREE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_QUEUE", 12)) {
    /*                       ^          */
      {
	int tmp = DB_QUEUE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_13 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_APPEND" "Db::DB_BEFORE" "Db::DB_CHKSUM" "Db::DB_CREATE"
     "Db::DB_DIRECT" "Db::DB_EXTENT" "Db::DB_NOMMAP" "Db::DB_NOSYNC"
     "Db::DB_RDONLY" "Db::DB_RECNUM" "Db::DB_THREAD" "Db::DB_VERIFY" */
  /* Offset 9 gives the best switch position.  */
  switch (name[9]) {
  case 'C':
    if (memEQ(name, "Db::DB_RECNUM", 13)) {
    /*                        ^          */
      {
	int tmp = DB_RECNUM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_CREATE", 13)) {
    /*                        ^          */
      {
	int tmp = DB_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_BEFORE", 13)) {
    /*                        ^          */
      {
	int tmp = DB_BEFORE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_CHKSUM", 13)) {
    /*                        ^          */
#ifdef DB_CHKSUM
      *iv_return = DB_CHKSUM;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_NOMMAP", 13)) {
    /*                        ^          */
      {
	int tmp = DB_NOMMAP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_RDONLY", 13)) {
    /*                        ^          */
      {
	int tmp = DB_RDONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_APPEND", 13)) {
    /*                        ^          */
      {
	int tmp = DB_APPEND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_DIRECT", 13)) {
    /*                        ^          */
      {
	int tmp = DB_DIRECT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_THREAD", 13)) {
    /*                        ^          */
      {
	int tmp = DB_THREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERIFY", 13)) {
    /*                        ^          */
      {
	int tmp = DB_VERIFY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_NOSYNC", 13)) {
    /*                        ^          */
      {
	int tmp = DB_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_EXTENT", 13)) {
    /*                        ^          */
      {
	int tmp = DB_EXTENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_14 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CONSUME" "Db::DB_CURRENT" "Db::DB_DELETED" "Db::DB_DUPSORT"
     "Db::DB_ENCRYPT" "Db::DB_ENV_CDB" "Db::DB_JOINENV" "Db::DB_KEYLAST"
     "Db::DB_LOCK_NG" "Db::DB_NOPANIC" "Db::DB_OK_HASH" "Db::DB_PRIVATE"
     "Db::DB_PR_PAGE" "Db::DB_RECOVER" "Db::DB_SALVAGE" "Db::DB_TIMEOUT"
     "Db::DB_TXN_CKP" "Db::DB_UNKNOWN" "Db::DB_UPGRADE" "Db::TXN_MALLOC"
     "Db::TXN_NOSYNC" "Db::TXN_NOWAIT" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'C':
    if (memEQ(name, "Db::DB_CONSUME", 14)) {
    /*                      ^             */
      {
	int tmp = DB_CONSUME;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_CURRENT", 14)) {
    /*                      ^             */
      {
	int tmp = DB_CURRENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_DELETED", 14)) {
    /*                      ^             */
      {
	int tmp = DB_DELETED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_DUPSORT", 14)) {
    /*                      ^             */
      {
	int tmp = DB_DUPSORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_ENCRYPT", 14)) {
    /*                      ^             */
      {
	int tmp = DB_ENCRYPT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ENV_CDB", 14)) {
    /*                      ^             */
      {
	int tmp = DB_ENV_CDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "Db::DB_JOINENV", 14)) {
    /*                      ^             */
      {
	int tmp = DB_JOINENV;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_KEYLAST", 14)) {
    /*                      ^             */
      {
	int tmp = DB_KEYLAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_NG", 14)) {
    /*                      ^             */
      {
	int tmp = DB_LOCK_NG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NOPANIC", 14)) {
    /*                      ^             */
      {
	int tmp = DB_NOPANIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_OK_HASH", 14)) {
    /*                      ^             */
      {
	int tmp = DB_OK_HASH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PRIVATE", 14)) {
    /*                      ^             */
      {
	int tmp = DB_PRIVATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PR_PAGE", 14)) {
    /*                      ^             */
      {
	int tmp = DB_PR_PAGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_RECOVER", 14)) {
    /*                      ^             */
      {
	int tmp = DB_RECOVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_SALVAGE", 14)) {
    /*                      ^             */
      {
	int tmp = DB_SALVAGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_TIMEOUT", 14)) {
    /*                      ^             */
      {
	int tmp = DB_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_CKP", 14)) {
    /*                      ^             */
      {
	int tmp = DB_TXN_CKP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_UNKNOWN", 14)) {
    /*                      ^             */
      {
	int tmp = DB_UNKNOWN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_UPGRADE", 14)) {
    /*                      ^             */
      {
	int tmp = DB_UPGRADE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::TXN_MALLOC", 14)) {
    /*                      ^             */
      {
	int tmp = TXN_MALLOC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_NOSYNC", 14)) {
    /*                      ^             */
      {
	int tmp = TXN_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_NOWAIT", 14)) {
    /*                      ^             */
      {
	int tmp = TXN_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_15 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ARCH_ABS" "Db::DB_ARCH_LOG" "Db::DB_GET_BOTH" "Db::DB_INIT_CDB"
     "Db::DB_INIT_LOG" "Db::DB_INIT_TXN" "Db::DB_KEYEMPTY" "Db::DB_KEYEXIST"
     "Db::DB_KEYFIRST" "Db::DB_LOCKDOWN" "Db::DB_LOCK_GET" "Db::DB_LOCK_IWR"
     "Db::DB_LOCK_PUT" "Db::DB_LOGMAGIC" "Db::DB_LOG_DISK" "Db::DB_MULTIPLE"
     "Db::DB_NEXT_DUP" "Db::DB_NOSERVER" "Db::DB_NOTFOUND" "Db::DB_OK_BTREE"
     "Db::DB_OK_QUEUE" "Db::DB_OK_RECNO" "Db::DB_POSITION" "Db::DB_QAMMAGIC"
     "Db::DB_REGISTER" "Db::DB_RENUMBER" "Db::DB_SNAPSHOT" "Db::DB_TRUNCATE"
     "Db::DB_TXN_SYNC" "Db::DB_YIELDCPU" "Db::MP_READONLY" */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'A':
    if (memEQ(name, "Db::DB_ARCH_ABS", 15)) {
    /*                           ^         */
      {
	int tmp = DB_ARCH_ABS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TRUNCATE", 15)) {
    /*                           ^         */
      {
	int tmp = DB_TRUNCATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'B':
    if (memEQ(name, "Db::DB_RENUMBER", 15)) {
    /*                           ^         */
      {
	int tmp = DB_RENUMBER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_INIT_CDB", 15)) {
    /*                           ^         */
      {
	int tmp = DB_INIT_CDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_OK_RECNO", 15)) {
    /*                           ^         */
      {
	int tmp = DB_OK_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_YIELDCPU", 15)) {
    /*                           ^         */
      {
	int tmp = DB_YIELDCPU;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_NEXT_DUP", 15)) {
    /*                           ^         */
      {
	int tmp = DB_NEXT_DUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_OK_QUEUE", 15)) {
    /*                           ^         */
      {
	int tmp = DB_OK_QUEUE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_LOCK_GET", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOCK_GET;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOGMAGIC", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOGMAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_QAMMAGIC", 15)) {
    /*                           ^         */
      {
	int tmp = DB_QAMMAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_SNAPSHOT", 15)) {
    /*                           ^         */
      {
	int tmp = DB_SNAPSHOT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_KEYEXIST", 15)) {
    /*                           ^         */
      {
	int tmp = DB_KEYEXIST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_IWR", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOCK_IWR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOG_DISK", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOG_DISK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_POSITION", 15)) {
    /*                           ^         */
      {
	int tmp = DB_POSITION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_ARCH_LOG", 15)) {
    /*                           ^         */
      {
	int tmp = DB_ARCH_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_INIT_LOG", 15)) {
    /*                           ^         */
      {
	int tmp = DB_INIT_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::MP_READONLY", 15)) {
    /*                           ^         */
      {
	int tmp = MP_READONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_GET_BOTH", 15)) {
    /*                           ^         */
      {
	int tmp = DB_GET_BOTH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCKDOWN", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOCKDOWN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_KEYEMPTY", 15)) {
    /*                           ^         */
      {
	int tmp = DB_KEYEMPTY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_PUT", 15)) {
    /*                           ^         */
      {
	int tmp = DB_LOCK_PUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MULTIPLE", 15)) {
    /*                           ^         */
      {
	int tmp = DB_MULTIPLE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_KEYFIRST", 15)) {
    /*                           ^         */
      {
	int tmp = DB_KEYFIRST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_OK_BTREE", 15)) {
    /*                           ^         */
      {
	int tmp = DB_OK_BTREE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_INIT_TXN", 15)) {
    /*                           ^         */
      {
	int tmp = DB_INIT_TXN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REGISTER", 15)) {
    /*                           ^         */
      {
	int tmp = DB_REGISTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_NOTFOUND", 15)) {
    /*                           ^         */
      {
	int tmp = DB_NOTFOUND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "Db::DB_NOSERVER", 15)) {
    /*                           ^         */
      {
	int tmp = DB_NOSERVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_TXN_SYNC", 15)) {
    /*                           ^         */
      {
	int tmp = DB_TXN_SYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_16 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ARCH_DATA" "Db::DB_CDB_ALLDB" "Db::DB_DIRECT_DB"
     "Db::DB_ENV_FATAL" "Db::DB_FAST_STAT" "Db::DB_GET_BOTHC"
     "Db::DB_GET_RECNO" "Db::DB_HASHMAGIC" "Db::DB_INIT_LOCK"
     "Db::DB_JOIN_ITEM" "Db::DB_LOCK_DUMP" "Db::DB_LOCK_READ"
     "Db::DB_LOCK_WAIT" "Db::DB_LOGOLDVER" "Db::DB_MAX_PAGES"
     "Db::DB_MPOOL_NEW" "Db::DB_NEEDSPLIT" "Db::DB_NODUPDATA"
     "Db::DB_NOLOCKING" "Db::DB_OVERWRITE" "Db::DB_PAGE_LOCK"
     "Db::DB_PRINTABLE" "Db::DB_QAMOLDVER" "Db::DB_SET_RANGE"
     "Db::DB_SET_RECNO" "Db::DB_SWAPBYTES" "Db::DB_TXN_ABORT"
     "Db::DB_TXN_APPLY" "Db::DB_TXN_PRINT" "Db::DB_WRITELOCK"
     "Db::DB_WRITEOPEN" "Db::DB_XA_CREATE" */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'A':
    if (memEQ(name, "Db::DB_ARCH_DATA", 16)) {
    /*                      ^               */
      {
	int tmp = DB_ARCH_DATA;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_CDB_ALLDB", 16)) {
    /*                      ^               */
      {
	int tmp = DB_CDB_ALLDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_DIRECT_DB", 16)) {
    /*                      ^               */
      {
	int tmp = DB_DIRECT_DB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_ENV_FATAL", 16)) {
    /*                      ^               */
      {
	int tmp = DB_ENV_FATAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_FAST_STAT", 16)) {
    /*                      ^               */
      {
	int tmp = DB_FAST_STAT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_GET_BOTHC", 16)) {
    /*                      ^               */
      {
	int tmp = DB_GET_BOTHC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_GET_RECNO", 16)) {
    /*                      ^               */
      {
	int tmp = DB_GET_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_HASHMAGIC", 16)) {
    /*                      ^               */
      {
	int tmp = DB_HASHMAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_INIT_LOCK", 16)) {
    /*                      ^               */
      {
	int tmp = DB_INIT_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'J':
    if (memEQ(name, "Db::DB_JOIN_ITEM", 16)) {
    /*                      ^               */
      {
	int tmp = DB_JOIN_ITEM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_LOCK_DUMP", 16)) {
    /*                      ^               */
      {
	int tmp = DB_LOCK_DUMP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_READ", 16)) {
    /*                      ^               */
      {
	int tmp = DB_LOCK_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_WAIT", 16)) {
    /*                      ^               */
      {
	int tmp = DB_LOCK_WAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOGOLDVER", 16)) {
    /*                      ^               */
      {
	int tmp = DB_LOGOLDVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_MAX_PAGES", 16)) {
    /*                      ^               */
      {
	int tmp = DB_MAX_PAGES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_NEW", 16)) {
    /*                      ^               */
      {
	int tmp = DB_MPOOL_NEW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_NEEDSPLIT", 16)) {
    /*                      ^               */
      {
	int tmp = DB_NEEDSPLIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NODUPDATA", 16)) {
    /*                      ^               */
      {
	int tmp = DB_NODUPDATA;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOLOCKING", 16)) {
    /*                      ^               */
      {
	int tmp = DB_NOLOCKING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_OVERWRITE", 16)) {
    /*                      ^               */
      {
	int tmp = DB_OVERWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_PAGE_LOCK", 16)) {
    /*                      ^               */
      {
	int tmp = DB_PAGE_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PRINTABLE", 16)) {
    /*                      ^               */
      {
	int tmp = DB_PRINTABLE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Q':
    if (memEQ(name, "Db::DB_QAMOLDVER", 16)) {
    /*                      ^               */
      {
	int tmp = DB_QAMOLDVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_SET_RANGE", 16)) {
    /*                      ^               */
      {
	int tmp = DB_SET_RANGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SET_RECNO", 16)) {
    /*                      ^               */
      {
	int tmp = DB_SET_RECNO;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SWAPBYTES", 16)) {
    /*                      ^               */
      {
	int tmp = DB_SWAPBYTES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_TXN_ABORT", 16)) {
    /*                      ^               */
      {
	int tmp = DB_TXN_ABORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_APPLY", 16)) {
    /*                      ^               */
      {
	int tmp = DB_TXN_APPLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_PRINT", 16)) {
    /*                      ^               */
      {
	int tmp = DB_TXN_PRINT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_WRITELOCK", 16)) {
    /*                      ^               */
      {
	int tmp = DB_WRITELOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_WRITEOPEN", 16)) {
    /*                      ^               */
      {
	int tmp = DB_WRITEOPEN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'X':
    if (memEQ(name, "Db::DB_XA_CREATE", 16)) {
    /*                      ^               */
      {
	int tmp = DB_XA_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_17 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AGGRESSIVE" "Db::DB_BTREEMAGIC" "Db::DB_DIRECT_LOG"
     "Db::DB_DIRTY_READ" "Db::DB_DONOTINDEX" "Db::DB_ENV_CREATE"
     "Db::DB_ENV_NOMMAP" "Db::DB_ENV_THREAD" "Db::DB_FREE_SPACE"
     "Db::DB_HASHOLDVER" "Db::DB_INIT_MPOOL" "Db::DB_LOCK_IREAD"
     "Db::DB_LOCK_NORUN" "Db::DB_LOCK_TRADE" "Db::DB_LOCK_WRITE"
     "Db::DB_LOGVERSION" "Db::DB_LOG_LOCKED" "Db::DB_LSTAT_FREE"
     "Db::DB_LSTAT_HELD" "Db::DB_MPOOL_LAST" "Db::DB_NEXT_NODUP"
     "Db::DB_NOORDERCHK" "Db::DB_PREV_NODUP" "Db::DB_QAMVERSION"
     "Db::DB_RDWRMASTER" "Db::DB_REP_CLIENT" "Db::DB_REP_IGNORE"
     "Db::DB_REP_MASTER" "Db::DB_STAT_CLEAR" "Db::DB_SYSTEM_MEM"
     "Db::DB_TXNVERSION" "Db::DB_TXN_NOSYNC" "Db::DB_TXN_NOWAIT"
     "Db::DB_VERIFY_BAD" "Db::REP_CONF_BULK" */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'A':
    if (memEQ(name, "Db::DB_RDWRMASTER", 17)) {
    /*                           ^           */
      {
	int tmp = DB_RDWRMASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_MASTER", 17)) {
    /*                           ^           */
      {
	int tmp = DB_REP_MASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_STAT_CLEAR", 17)) {
    /*                           ^           */
      {
	int tmp = DB_STAT_CLEAR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_NOORDERCHK", 17)) {
    /*                           ^           */
      {
	int tmp = DB_NOORDERCHK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_REP_IGNORE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_REP_IGNORE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_ENV_THREAD", 17)) {
    /*                           ^           */
      {
	int tmp = DB_ENV_THREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_DONOTINDEX", 17)) {
    /*                           ^           */
      {
	int tmp = DB_DONOTINDEX;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_IREAD", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOCK_IREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_HASHOLDVER", 17)) {
    /*                           ^           */
      {
	int tmp = DB_HASHOLDVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_CLIENT", 17)) {
    /*                           ^           */
      {
	int tmp = DB_REP_CLIENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_BTREEMAGIC", 17)) {
    /*                           ^           */
      {
	int tmp = DB_BTREEMAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_INIT_MPOOL", 17)) {
    /*                           ^           */
      {
	int tmp = DB_INIT_MPOOL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SYSTEM_MEM", 17)) {
    /*                           ^           */
      {
	int tmp = DB_SYSTEM_MEM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_LOCK_NORUN", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOCK_NORUN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NEXT_NODUP", 17)) {
    /*                           ^           */
      {
	int tmp = DB_NEXT_NODUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PREV_NODUP", 17)) {
    /*                           ^           */
      {
	int tmp = DB_PREV_NODUP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_ENV_NOMMAP", 17)) {
    /*                           ^           */
      {
	int tmp = DB_ENV_NOMMAP;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOG_LOCKED", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOG_LOCKED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_NOSYNC", 17)) {
    /*                           ^           */
      {
	int tmp = DB_TXN_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_NOWAIT", 17)) {
    /*                           ^           */
      {
	int tmp = DB_TXN_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_ENV_CREATE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_ENV_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOGVERSION", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOGVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_QAMVERSION", 17)) {
    /*                           ^           */
      {
	int tmp = DB_QAMVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXNVERSION", 17)) {
    /*                           ^           */
      {
	int tmp = DB_TXNVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_AGGRESSIVE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_AGGRESSIVE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_FREE_SPACE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_FREE_SPACE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_DIRECT_LOG", 17)) {
    /*                           ^           */
      {
	int tmp = DB_DIRECT_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_TRADE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOCK_TRADE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_LOCK_WRITE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LOCK_WRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_VERIFY_BAD", 17)) {
    /*                           ^           */
      {
	int tmp = DB_VERIFY_BAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_DIRTY_READ", 17)) {
    /*                           ^           */
      {
	int tmp = DB_DIRTY_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LSTAT_FREE", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LSTAT_FREE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LSTAT_HELD", 17)) {
    /*                           ^           */
      {
	int tmp = DB_LSTAT_HELD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_LAST", 17)) {
    /*                           ^           */
      {
	int tmp = DB_MPOOL_LAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::REP_CONF_BULK", 17)) {
    /*                           ^           */
      {
	int tmp = REP_CONF_BULK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_18 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_AUTO_COMMIT" "Db::DB_BTREEOLDVER" "Db::DB_CHKSUM_SHA1"
     "Db::DB_DBT_PARTIAL" "Db::DB_EID_INVALID" "Db::DB_ENCRYPT_AES"
     "Db::DB_ENV_DBLOCAL" "Db::DB_ENV_NOPANIC" "Db::DB_ENV_PRIVATE"
     "Db::DB_FILE_ID_LEN" "Db::DB_HANDLE_LOCK" "Db::DB_HASHVERSION"
     "Db::DB_JOIN_NOSORT" "Db::DB_LOCKVERSION" "Db::DB_LOCK_EXPIRE"
     "Db::DB_LOCK_IWRITE" "Db::DB_LOCK_NOWAIT" "Db::DB_LOCK_OLDEST"
     "Db::DB_LOCK_RANDOM" "Db::DB_LOCK_RECORD" "Db::DB_LOCK_REMOVE"
     "Db::DB_LOCK_SWITCH" "Db::DB_LOCK_WWRITE" "Db::DB_MAX_RECORDS"
     "Db::DB_MPOOL_CLEAN" "Db::DB_MPOOL_DIRTY" "Db::DB_NOOVERWRITE"
     "Db::DB_NOSERVER_ID" "Db::DB_ODDFILESIZE" "Db::DB_OLD_VERSION"
     "Db::DB_RECORDCOUNT" "Db::DB_RECORD_LOCK" "Db::DB_REGION_INIT"
     "Db::DB_RENAMEMAGIC" "Db::DB_REP_BULKOVF" "Db::DB_REP_LOCKOUT"
     "Db::DB_REP_NEWSITE" "Db::DB_REP_UNAVAIL" "Db::DB_REVSPLITOFF"
     "Db::DB_RUNRECOVERY" "Db::DB_SEQ_WRAPPED" "Db::DB_SET_TXN_NOW"
     "Db::DB_USE_ENVIRON" "Db::DB_WRITECURSOR" "Db::DB_XIDDATASIZE"
     "Db::MP_OPEN_CALLED" "Db::TXN_COMPENSATE" "Db::TXN_DIRTY_READ"
     "Db::TXN_SYNC_FLAGS" */
  /* Offset 13 gives the best switch position.  */
  switch (name[13]) {
  case 'A':
    if (memEQ(name, "Db::DB_LOCK_RANDOM", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_RANDOM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_UNAVAIL", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REP_UNAVAIL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SEQ_WRAPPED", 18)) {
    /*                            ^           */
      {
	int tmp = DB_SEQ_WRAPPED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_XIDDATASIZE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_XIDDATASIZE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::MP_OPEN_CALLED", 18)) {
    /*                            ^           */
      {
	int tmp = MP_OPEN_CALLED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_MAX_RECORDS", 18)) {
    /*                            ^           */
      {
	int tmp = DB_MAX_RECORDS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_CLEAN", 18)) {
    /*                            ^           */
      {
	int tmp = DB_MPOOL_CLEAN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RECORDCOUNT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_RECORDCOUNT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_LOCKOUT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REP_LOCKOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_FILE_ID_LEN", 18)) {
    /*                            ^           */
      {
	int tmp = DB_FILE_ID_LEN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_DIRTY", 18)) {
    /*                            ^           */
      {
	int tmp = DB_MPOOL_DIRTY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_LOCK_RECORD", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_RECORD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_REMOVE", 18)) {
    /*                            ^           */
#ifdef DB_LOCK_REMOVE
      *iv_return = DB_LOCK_REMOVE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "Db::DB_NOSERVER_ID", 18)) {
    /*                            ^           */
      {
	int tmp = DB_NOSERVER_ID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ODDFILESIZE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_ODDFILESIZE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::TXN_SYNC_FLAGS", 18)) {
    /*                            ^           */
      {
	int tmp = TXN_SYNC_FLAGS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_ENV_PRIVATE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_ENV_PRIVATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REVSPLITOFF", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REVSPLITOFF;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_BTREEOLDVER", 18)) {
    /*                            ^           */
      {
	int tmp = DB_BTREEOLDVER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ENV_DBLOCAL", 18)) {
    /*                            ^           */
      {
	int tmp = DB_ENV_DBLOCAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_OLDEST", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_OLDEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_BULKOVF", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REP_BULKOVF;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_RENAMEMAGIC", 18)) {
    /*                            ^           */
      {
	int tmp = DB_RENAMEMAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_SET_TXN_NOW", 18)) {
    /*                            ^           */
      {
	int tmp = DB_SET_TXN_NOW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_COMPENSATE", 18)) {
    /*                            ^           */
      {
	int tmp = TXN_COMPENSATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_AUTO_COMMIT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_AUTO_COMMIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_JOIN_NOSORT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_JOIN_NOSORT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_NOWAIT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RUNRECOVERY", 18)) {
    /*                            ^           */
      {
	int tmp = DB_RUNRECOVERY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_ENV_NOPANIC", 18)) {
    /*                            ^           */
      {
	int tmp = DB_ENV_NOPANIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_DBT_PARTIAL", 18)) {
    /*                            ^           */
      {
	int tmp = DB_DBT_PARTIAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_HASHVERSION", 18)) {
    /*                            ^           */
      {
	int tmp = DB_HASHVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCKVERSION", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCKVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_OLD_VERSION", 18)) {
    /*                            ^           */
      {
	int tmp = DB_OLD_VERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_ENCRYPT_AES", 18)) {
    /*                            ^           */
      {
	int tmp = DB_ENCRYPT_AES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_WRITECURSOR", 18)) {
    /*                            ^           */
      {
	int tmp = DB_WRITECURSOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "Db::DB_EID_INVALID", 18)) {
    /*                            ^           */
      {
	int tmp = DB_EID_INVALID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_USE_ENVIRON", 18)) {
    /*                            ^           */
      {
	int tmp = DB_USE_ENVIRON;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_LOCK_IWRITE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_IWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_SWITCH", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_SWITCH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_WWRITE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_WWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOOVERWRITE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_NOOVERWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_NEWSITE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REP_NEWSITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'X':
    if (memEQ(name, "Db::DB_LOCK_EXPIRE", 18)) {
    /*                            ^           */
      {
	int tmp = DB_LOCK_EXPIRE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_CHKSUM_SHA1", 18)) {
    /*                            ^           */
#ifdef DB_CHKSUM_SHA1
      *iv_return = DB_CHKSUM_SHA1;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "Db::DB_HANDLE_LOCK", 18)) {
    /*                            ^           */
      {
	int tmp = DB_HANDLE_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RECORD_LOCK", 18)) {
    /*                            ^           */
      {
	int tmp = DB_RECORD_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REGION_INIT", 18)) {
    /*                            ^           */
      {
	int tmp = DB_REGION_INIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_DIRTY_READ", 18)) {
    /*                            ^           */
#ifdef TXN_DIRTY_READ
      *iv_return = TXN_DIRTY_READ;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_19 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_BTREEVERSION" "Db::DB_CONSUME_WAIT" "Db::DB_ENV_LOCKDOWN"
     "Db::DB_ENV_YIELDCPU" "Db::DB_LOCK_DEFAULT" "Db::DB_LOCK_INHERIT"
     "Db::DB_LOCK_PUT_ALL" "Db::DB_LOCK_PUT_OBJ" "Db::DB_LOCK_TIMEOUT"
     "Db::DB_LOCK_UPGRADE" "Db::DB_MPOOL_CREATE" "Db::DB_MULTIPLE_KEY"
     "Db::DB_MUTEX_THREAD" "Db::DB_OPFLAGS_MASK" "Db::DB_ORDERCHKONLY"
     "Db::DB_PRIORITY_LOW" "Db::DB_REGION_MAGIC" "Db::DB_REP_ANYWHERE"
     "Db::DB_SURPRISE_KID" "Db::DB_TEST_POSTLOG" "Db::DB_TEST_PREOPEN"
     "Db::DB_VERIFY_FATAL" "Db::REP_CONF_NOWAIT" "Db::TXN_CHILDCOMMIT"
     "Db::TXN_LOCKTIMEOUT" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'E':
    if (memEQ(name, "Db::DB_MULTIPLE_KEY", 19)) {
    /*                             ^           */
      {
	int tmp = DB_MULTIPLE_KEY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SURPRISE_KID", 19)) {
    /*                             ^           */
      {
	int tmp = DB_SURPRISE_KID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TEST_PREOPEN", 19)) {
    /*                             ^           */
      {
	int tmp = DB_TEST_PREOPEN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_LOCK_DEFAULT", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_DEFAULT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERIFY_FATAL", 19)) {
    /*                             ^           */
      {
	int tmp = DB_VERIFY_FATAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_LOCK_UPGRADE", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_UPGRADE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_LOCK_INHERIT", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_INHERIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MUTEX_THREAD", 19)) {
    /*                             ^           */
      {
	int tmp = DB_MUTEX_THREAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_ENV_LOCKDOWN", 19)) {
    /*                             ^           */
      {
	int tmp = DB_ENV_LOCKDOWN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ORDERCHKONLY", 19)) {
    /*                             ^           */
      {
	int tmp = DB_ORDERCHKONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_ENV_YIELDCPU", 19)) {
    /*                             ^           */
      {
	int tmp = DB_ENV_YIELDCPU;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_LOCK_TIMEOUT", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REGION_MAGIC", 19)) {
    /*                             ^           */
      {
	int tmp = DB_REGION_MAGIC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_LOCKTIMEOUT", 19)) {
    /*                             ^           */
      {
	int tmp = TXN_LOCKTIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::REP_CONF_NOWAIT", 19)) {
    /*                             ^           */
      {
	int tmp = REP_CONF_NOWAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_CHILDCOMMIT", 19)) {
    /*                             ^           */
      {
	int tmp = TXN_CHILDCOMMIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_BTREEVERSION", 19)) {
    /*                             ^           */
      {
	int tmp = DB_BTREEVERSION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_MPOOL_CREATE", 19)) {
    /*                             ^           */
      {
	int tmp = DB_MPOOL_CREATE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_TEST_POSTLOG", 19)) {
    /*                             ^           */
      {
	int tmp = DB_TEST_POSTLOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_LOCK_PUT_ALL", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_PUT_ALL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_PUT_OBJ", 19)) {
    /*                             ^           */
      {
	int tmp = DB_LOCK_PUT_OBJ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'W':
    if (memEQ(name, "Db::DB_REP_ANYWHERE", 19)) {
    /*                             ^           */
      {
	int tmp = DB_REP_ANYWHERE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_PRIORITY_LOW", 19)) {
    /*                             ^           */
      {
	int tmp = DB_PRIORITY_LOW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_CONSUME_WAIT", 19)) {
    /*                             ^           */
      {
	int tmp = DB_CONSUME_WAIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_OPFLAGS_MASK", 19)) {
    /*                             ^           */
      {
	int tmp = DB_OPFLAGS_MASK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CACHED_COUNTS" "Db::DB_COMPACT_FLAGS" "Db::DB_EID_BROADCAST"
     "Db::DB_ENV_CDB_ALLDB" "Db::DB_ENV_DIRECT_DB" "Db::DB_ENV_NOLOCKING"
     "Db::DB_ENV_OVERWRITE" "Db::DB_ENV_RPCCLIENT" "Db::DB_FCNTL_LOCKING"
     "Db::DB_FREELIST_ONLY" "Db::DB_IMMUTABLE_KEY" "Db::DB_LOCK_DEADLOCK"
     "Db::DB_LOCK_MAXLOCKS" "Db::DB_LOCK_MINLOCKS" "Db::DB_LOCK_MINWRITE"
     "Db::DB_LOCK_NOTEXIST" "Db::DB_LOCK_PUT_READ" "Db::DB_LOCK_YOUNGEST"
     "Db::DB_LOGC_BUF_SIZE" "Db::DB_LSTAT_ABORTED" "Db::DB_LSTAT_EXPIRED"
     "Db::DB_LSTAT_PENDING" "Db::DB_LSTAT_WAITING" "Db::DB_MPOOL_DISCARD"
     "Db::DB_NOSERVER_HOME" "Db::DB_PAGE_NOTFOUND" "Db::DB_PRIORITY_HIGH"
     "Db::DB_RECOVER_FATAL" "Db::DB_REP_DUPMASTER" "Db::DB_REP_NEWMASTER"
     "Db::DB_REP_PERMANENT" "Db::DB_REP_REREQUEST" "Db::DB_SECONDARY_BAD"
     "Db::DB_TEST_POSTOPEN" "Db::DB_TEST_POSTSYNC" "Db::DB_TXN_OPENFILES"
     "Db::DB_VERB_DEADLOCK" "Db::DB_VERB_RECOVERY" "Db::DB_VERB_REGISTER"
     "Db::DB_VERB_WAITSFOR" "Db::DB_VERSION_MAJOR" "Db::DB_VERSION_MINOR"
     "Db::DB_VERSION_PATCH" "Db::TXN_WRITE_NOSYNC" */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'A':
    if (memEQ(name, "Db::DB_EID_BROADCAST", 20)) {
    /*                             ^            */
      {
	int tmp = DB_EID_BROADCAST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_DEADLOCK", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_DEADLOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LSTAT_WAITING", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LSTAT_WAITING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERB_DEADLOCK", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERB_DEADLOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'B':
    if (memEQ(name, "Db::DB_LSTAT_ABORTED", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LSTAT_ABORTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::DB_CACHED_COUNTS", 20)) {
    /*                             ^            */
      {
	int tmp = DB_CACHED_COUNTS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ENV_RPCCLIENT", 20)) {
    /*                             ^            */
      {
	int tmp = DB_ENV_RPCCLIENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERB_RECOVERY", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERB_RECOVERY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_ENV_DIRECT_DB", 20)) {
    /*                             ^            */
      {
	int tmp = DB_ENV_DIRECT_DB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LSTAT_PENDING", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LSTAT_PENDING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_REREQUEST", 20)) {
    /*                             ^            */
      {
	int tmp = DB_REP_REREQUEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'F':
    if (memEQ(name, "Db::DB_LOGC_BUF_SIZE", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOGC_BUF_SIZE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_VERB_REGISTER", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERB_REGISTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_MPOOL_DISCARD", 20)) {
    /*                             ^            */
      {
	int tmp = DB_MPOOL_DISCARD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERB_WAITSFOR", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERB_WAITSFOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_IMMUTABLE_KEY", 20)) {
    /*                             ^            */
      {
	int tmp = DB_IMMUTABLE_KEY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_REP_DUPMASTER", 20)) {
    /*                             ^            */
      {
	int tmp = DB_REP_DUPMASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_NEWMASTER", 20)) {
    /*                             ^            */
      {
	int tmp = DB_REP_NEWMASTER;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_REP_PERMANENT", 20)) {
    /*                             ^            */
      {
	int tmp = DB_REP_PERMANENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_LOCK_MINLOCKS", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_MINLOCKS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_MINWRITE", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_MINWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TXN_OPENFILES", 20)) {
    /*                             ^            */
      {
	int tmp = DB_TXN_OPENFILES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::TXN_WRITE_NOSYNC", 20)) {
    /*                             ^            */
      {
	int tmp = TXN_WRITE_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_ENV_NOLOCKING", 20)) {
    /*                             ^            */
      {
	int tmp = DB_ENV_NOLOCKING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_FCNTL_LOCKING", 20)) {
    /*                             ^            */
      {
	int tmp = DB_FCNTL_LOCKING;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_ENV_OVERWRITE", 20)) {
    /*                             ^            */
      {
	int tmp = DB_ENV_OVERWRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_NOSERVER_HOME", 20)) {
    /*                             ^            */
      {
	int tmp = DB_NOSERVER_HOME;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_SECONDARY_BAD", 20)) {
    /*                             ^            */
      {
	int tmp = DB_SECONDARY_BAD;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_TEST_POSTOPEN", 20)) {
    /*                             ^            */
      {
	int tmp = DB_TEST_POSTOPEN;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TEST_POSTSYNC", 20)) {
    /*                             ^            */
      {
	int tmp = DB_TEST_POSTSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_FREELIST_ONLY", 20)) {
    /*                             ^            */
      {
	int tmp = DB_FREELIST_ONLY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_NOTEXIST", 20)) {
    /*                             ^            */
#ifdef DB_LOCK_NOTEXIST
      *iv_return = DB_LOCK_NOTEXIST;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "Db::DB_LOCK_PUT_READ", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_PUT_READ;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PAGE_NOTFOUND", 20)) {
    /*                             ^            */
      {
	int tmp = DB_PAGE_NOTFOUND;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_LOCK_YOUNGEST", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_YOUNGEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'X':
    if (memEQ(name, "Db::DB_LOCK_MAXLOCKS", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LOCK_MAXLOCKS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LSTAT_EXPIRED", 20)) {
    /*                             ^            */
      {
	int tmp = DB_LSTAT_EXPIRED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_PRIORITY_HIGH", 20)) {
    /*                             ^            */
      {
	int tmp = DB_PRIORITY_HIGH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_COMPACT_FLAGS", 20)) {
    /*                             ^            */
      {
	int tmp = DB_COMPACT_FLAGS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_ENV_CDB_ALLDB", 20)) {
    /*                             ^            */
      {
	int tmp = DB_ENV_CDB_ALLDB;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_RECOVER_FATAL", 20)) {
    /*                             ^            */
      {
	int tmp = DB_RECOVER_FATAL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERSION_MAJOR", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERSION_MAJOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERSION_MINOR", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERSION_MINOR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERSION_PATCH", 20)) {
    /*                             ^            */
      {
	int tmp = DB_VERSION_PATCH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return, const char **pv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ENV_DIRECT_LOG" "Db::DB_ENV_SYSTEM_MEM" "Db::DB_ENV_TXN_NOSYNC"
     "Db::DB_GET_BOTH_RANGE" "Db::DB_LOG_SILENT_ERR" "Db::DB_READ_COMMITTED"
     "Db::DB_TEST_ELECTINIT" "Db::DB_TXN_POPENFILES" "Db::DB_VERSION_STRING"
     "Db::REP_CONF_AUTOINIT" */
  /* Offset 20 gives the best switch position.  */
  switch (name[20]) {
  case 'C':
    if (memEQ(name, "Db::DB_ENV_TXN_NOSYN", 20)) {
    /*                                   C      */
      {
	int tmp = DB_ENV_TXN_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_READ_COMMITTE", 20)) {
    /*                                   D      */
      {
	int tmp = DB_READ_COMMITTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_GET_BOTH_RANG", 20)) {
    /*                                   E      */
      {
	int tmp = DB_GET_BOTH_RANGE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_ENV_DIRECT_LO", 20)) {
    /*                                   G      */
      {
	int tmp = DB_ENV_DIRECT_LOG;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERSION_STRIN", 20)) {
    /*                                   G      */
      *pv_return = DB_VERSION_STRING;
      *iv_return = (IV) 17;
      return PERL_constant_ISPVN;
    }
    break;
  case 'M':
    if (memEQ(name, "Db::DB_ENV_SYSTEM_ME", 20)) {
    /*                                   M      */
      {
	int tmp = DB_ENV_SYSTEM_MEM;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_LOG_SILENT_ER", 20)) {
    /*                                   R      */
      {
	int tmp = DB_LOG_SILENT_ERR;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_TXN_POPENFILE", 20)) {
    /*                                   S      */
      {
	int tmp = DB_TXN_POPENFILES;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_TEST_ELECTINI", 20)) {
    /*                                   T      */
      {
	int tmp = DB_TEST_ELECTINIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::REP_CONF_AUTOINI", 20)) {
    /*                                   T      */
      {
	int tmp = REP_CONF_AUTOINIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_22 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_ALREADY_ABORTED" "Db::DB_ENV_AUTO_COMMIT" "Db::DB_ENV_OPEN_CALLED"
     "Db::DB_ENV_REGION_INIT" "Db::DB_LOCK_NOTGRANTED" "Db::DB_MUTEX_ALLOCATED"
     "Db::DB_PR_RECOVERYTEST" "Db::DB_SET_TXN_TIMEOUT" "Db::DB_TEST_PREDESTROY"
     "Db::TXN_READ_COMMITTED" */
  /* Offset 13 gives the best switch position.  */
  switch (name[13]) {
  case 'A':
    if (memEQ(name, "Db::DB_MUTEX_ALLOCATED", 22)) {
    /*                            ^               */
      {
	int tmp = DB_MUTEX_ALLOCATED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'C':
    if (memEQ(name, "Db::TXN_READ_COMMITTED", 22)) {
    /*                            ^               */
      {
	int tmp = TXN_READ_COMMITTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_ENV_OPEN_CALLED", 22)) {
    /*                            ^               */
      {
	int tmp = DB_ENV_OPEN_CALLED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'G':
    if (memEQ(name, "Db::DB_ENV_REGION_INIT", 22)) {
    /*                            ^               */
      {
	int tmp = DB_ENV_REGION_INIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_SET_TXN_TIMEOUT", 22)) {
    /*                            ^               */
      {
	int tmp = DB_SET_TXN_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_LOCK_NOTGRANTED", 22)) {
    /*                            ^               */
      {
	int tmp = DB_LOCK_NOTGRANTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_PR_RECOVERYTEST", 22)) {
    /*                            ^               */
      {
	int tmp = DB_PR_RECOVERYTEST;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_TEST_PREDESTROY", 22)) {
    /*                            ^               */
      {
	int tmp = DB_TEST_PREDESTROY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_ENV_AUTO_COMMIT", 22)) {
    /*                            ^               */
      {
	int tmp = DB_ENV_AUTO_COMMIT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'Y':
    if (memEQ(name, "Db::DB_ALREADY_ABORTED", 22)) {
    /*                            ^               */
      {
	int tmp = DB_ALREADY_ABORTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_23 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_LOCK_GET_TIMEOUT" "Db::DB_LOCK_SET_TIMEOUT"
     "Db::DB_MUTEX_SELF_BLOCK" "Db::DB_PRIORITY_DEFAULT"
     "Db::DB_READ_UNCOMMITTED" "Db::DB_REP_HOLDELECTION"
     "Db::DB_SET_LOCK_TIMEOUT" "Db::DB_TEST_POSTDESTROY"
     "Db::DB_TEST_POSTLOGMETA" "Db::DB_TEST_SUBDB_LOCKS"
     "Db::DB_TXN_FORWARD_ROLL" "Db::DB_TXN_WRITE_NOSYNC"
     "Db::DB_UPDATE_SECONDARY" "Db::DB_USE_ENVIRON_ROOT"
     "Db::DB_VERB_REPLICATION" */
  /* Offset 13 gives the best switch position.  */
  switch (name[13]) {
  case 'C':
    if (memEQ(name, "Db::DB_SET_LOCK_TIMEOUT", 23)) {
    /*                            ^                */
      {
	int tmp = DB_SET_LOCK_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_LOCK_GET_TIMEOUT", 23)) {
    /*                            ^                */
      {
	int tmp = DB_LOCK_GET_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_LOCK_SET_TIMEOUT", 23)) {
    /*                            ^                */
      {
	int tmp = DB_LOCK_SET_TIMEOUT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_VERB_REPLICATION", 23)) {
    /*                            ^                */
      {
	int tmp = DB_VERB_REPLICATION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'I':
    if (memEQ(name, "Db::DB_TXN_WRITE_NOSYNC", 23)) {
    /*                            ^                */
      {
	int tmp = DB_TXN_WRITE_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'L':
    if (memEQ(name, "Db::DB_REP_HOLDELECTION", 23)) {
    /*                            ^                */
      {
	int tmp = DB_REP_HOLDELECTION;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_READ_UNCOMMITTED", 23)) {
    /*                            ^                */
      {
	int tmp = DB_READ_UNCOMMITTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_TEST_POSTDESTROY", 23)) {
    /*                            ^                */
      {
	int tmp = DB_TEST_POSTDESTROY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    if (memEQ(name, "Db::DB_TEST_POSTLOGMETA", 23)) {
    /*                            ^                */
      {
	int tmp = DB_TEST_POSTLOGMETA;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_TXN_FORWARD_ROLL", 23)) {
    /*                            ^                */
      {
	int tmp = DB_TXN_FORWARD_ROLL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'S':
    if (memEQ(name, "Db::DB_MUTEX_SELF_BLOCK", 23)) {
    /*                            ^                */
      {
	int tmp = DB_MUTEX_SELF_BLOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'T':
    if (memEQ(name, "Db::DB_PRIORITY_DEFAULT", 23)) {
    /*                            ^                */
      {
	int tmp = DB_PRIORITY_DEFAULT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'U':
    if (memEQ(name, "Db::DB_TEST_SUBDB_LOCKS", 23)) {
    /*                            ^                */
      {
	int tmp = DB_TEST_SUBDB_LOCKS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'V':
    if (memEQ(name, "Db::DB_USE_ENVIRON_ROOT", 23)) {
    /*                            ^                */
      {
	int tmp = DB_USE_ENVIRON_ROOT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case '_':
    if (memEQ(name, "Db::DB_UPDATE_SECONDARY", 23)) {
    /*                            ^                */
      {
	int tmp = DB_UPDATE_SECONDARY;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_24 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_CXX_NO_EXCEPTIONS" "Db::DB_LOGFILEID_INVALID"
     "Db::DB_PANIC_ENVIRONMENT" "Db::DB_PRIORITY_VERY_LOW"
     "Db::DB_TXN_BACKWARD_ROLL" "Db::REP_CONF_DELAYCLIENT"
     "Db::TXN_READ_UNCOMMITTED" */
  /* Offset 18 gives the best switch position.  */
  switch (name[18]) {
  case 'C':
    if (memEQ(name, "Db::REP_CONF_DELAYCLIENT", 24)) {
    /*                                 ^            */
      {
	int tmp = REP_CONF_DELAYCLIENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'D':
    if (memEQ(name, "Db::DB_TXN_BACKWARD_ROLL", 24)) {
    /*                                 ^            */
      {
	int tmp = DB_TXN_BACKWARD_ROLL;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'M':
    if (memEQ(name, "Db::TXN_READ_UNCOMMITTED", 24)) {
    /*                                 ^            */
      {
	int tmp = TXN_READ_UNCOMMITTED;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'N':
    if (memEQ(name, "Db::DB_LOGFILEID_INVALID", 24)) {
    /*                                 ^            */
      {
	int tmp = DB_LOGFILEID_INVALID;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'O':
    if (memEQ(name, "Db::DB_PANIC_ENVIRONMENT", 24)) {
    /*                                 ^            */
      {
	int tmp = DB_PANIC_ENVIRONMENT;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'P':
    if (memEQ(name, "Db::DB_CXX_NO_EXCEPTIONS", 24)) {
    /*                                 ^            */
      {
	int tmp = DB_CXX_NO_EXCEPTIONS;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'R':
    if (memEQ(name, "Db::DB_PRIORITY_VERY_LOW", 24)) {
    /*                                 ^            */
      {
	int tmp = DB_PRIORITY_VERY_LOW;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_25 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     "Db::DB_LOCK_UPGRADE_WRITE" "Db::DB_MUTEX_LOGICAL_LOCK"
     "Db::DB_PRIORITY_VERY_HIGH" "Db::DB_TXN_BACKWARD_ALLOC" */
  /* Offset 24 gives the best switch position.  */
  switch (name[24]) {
  case 'C':
    if (memEQ(name, "Db::DB_TXN_BACKWARD_ALLO", 24)) {
    /*                                       C      */
      {
	int tmp = DB_TXN_BACKWARD_ALLOC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'E':
    if (memEQ(name, "Db::DB_LOCK_UPGRADE_WRIT", 24)) {
    /*                                       E      */
      {
	int tmp = DB_LOCK_UPGRADE_WRITE;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'H':
    if (memEQ(name, "Db::DB_PRIORITY_VERY_HIG", 24)) {
    /*                                       H      */
      {
	int tmp = DB_PRIORITY_VERY_HIGH;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  case 'K':
    if (memEQ(name, "Db::DB_MUTEX_LOGICAL_LOC", 24)) {
    /*                                       K      */
      {
	int tmp = DB_MUTEX_LOGICAL_LOCK;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return, const char **pv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/spare/local/perls/5.8.6/bin/perl5.8.6 -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV PVN)};
my @names = (qw(),
            {name=>"Db::DB_AFTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AFTER;"},
            {name=>"Db::DB_AGGRESSIVE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AGGRESSIVE;"},
            {name=>"Db::DB_ALREADY_ABORTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ALREADY_ABORTED;"},
            {name=>"Db::DB_APPEND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_APPEND;"},
            {name=>"Db::DB_ARCH_ABS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_ABS;"},
            {name=>"Db::DB_ARCH_DATA", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_DATA;"},
            {name=>"Db::DB_ARCH_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ARCH_LOG;"},
            {name=>"Db::DB_ASSOC_IMMUTABLE_KEY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ASSOC_IMMUTABLE_KEY;"},
            {name=>"Db::DB_AUTO_COMMIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_AUTO_COMMIT;"},
            {name=>"Db::DB_BEFORE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BEFORE;"},
            {name=>"Db::DB_BTREE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BTREE;"},
            {name=>"Db::DB_BTREEMAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BTREEMAGIC;"},
            {name=>"Db::DB_BTREEOLDVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BTREEOLDVER;"},
            {name=>"Db::DB_BTREEVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_BTREEVERSION;"},
            {name=>"Db::DB_CACHED_COUNTS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CACHED_COUNTS;"},
            {name=>"Db::DB_CDB_ALLDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CDB_ALLDB;"},
            {name=>"Db::DB_CHKSUM", type=>"IV", macro=>["#ifdef DB_CHKSUM\n", "#endif\n"], value=>"DB_CHKSUM"},
            {name=>"Db::DB_CHKSUM_SHA1", type=>"IV", macro=>["#ifdef DB_CHKSUM_SHA1\n", "#endif\n"], value=>"DB_CHKSUM_SHA1"},
            {name=>"Db::DB_COMPACT_FLAGS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_COMPACT_FLAGS;"},
            {name=>"Db::DB_CONSUME", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CONSUME;"},
            {name=>"Db::DB_CONSUME_WAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CONSUME_WAIT;"},
            {name=>"Db::DB_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CREATE;"},
            {name=>"Db::DB_CURRENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CURRENT;"},
            {name=>"Db::DB_CXX_NO_EXCEPTIONS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_CXX_NO_EXCEPTIONS;"},
            {name=>"Db::DB_DBT_PARTIAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DBT_PARTIAL;"},
            {name=>"Db::DB_DELETED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DELETED;"},
            {name=>"Db::DB_DIRECT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRECT;"},
            {name=>"Db::DB_DIRECT_DB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRECT_DB;"},
            {name=>"Db::DB_DIRECT_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRECT_LOG;"},
            {name=>"Db::DB_DIRTY_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DIRTY_READ;"},
            {name=>"Db::DB_DONOTINDEX", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DONOTINDEX;"},
            {name=>"Db::DB_DUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DUP;"},
            {name=>"Db::DB_DUPSORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_DUPSORT;"},
            {name=>"Db::DB_EID_BROADCAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EID_BROADCAST;"},
            {name=>"Db::DB_EID_INVALID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EID_INVALID;"},
            {name=>"Db::DB_ENCRYPT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENCRYPT;"},
            {name=>"Db::DB_ENCRYPT_AES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENCRYPT_AES;"},
            {name=>"Db::DB_ENV_AUTO_COMMIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_AUTO_COMMIT;"},
            {name=>"Db::DB_ENV_CDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_CDB;"},
            {name=>"Db::DB_ENV_CDB_ALLDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_CDB_ALLDB;"},
            {name=>"Db::DB_ENV_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_CREATE;"},
            {name=>"Db::DB_ENV_DBLOCAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_DBLOCAL;"},
            {name=>"Db::DB_ENV_DIRECT_DB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_DIRECT_DB;"},
            {name=>"Db::DB_ENV_DIRECT_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_DIRECT_LOG;"},
            {name=>"Db::DB_ENV_FATAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_FATAL;"},
            {name=>"Db::DB_ENV_LOCKDOWN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_LOCKDOWN;"},
            {name=>"Db::DB_ENV_NOLOCKING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_NOLOCKING;"},
            {name=>"Db::DB_ENV_NOMMAP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_NOMMAP;"},
            {name=>"Db::DB_ENV_NOPANIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_NOPANIC;"},
            {name=>"Db::DB_ENV_OPEN_CALLED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_OPEN_CALLED;"},
            {name=>"Db::DB_ENV_OVERWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_OVERWRITE;"},
            {name=>"Db::DB_ENV_PRIVATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_PRIVATE;"},
            {name=>"Db::DB_ENV_REGION_INIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_REGION_INIT;"},
            {name=>"Db::DB_ENV_RPCCLIENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_RPCCLIENT;"},
            {name=>"Db::DB_ENV_RPCCLIENT_GIVEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_RPCCLIENT_GIVEN;"},
            {name=>"Db::DB_ENV_SYSTEM_MEM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_SYSTEM_MEM;"},
            {name=>"Db::DB_ENV_THREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_THREAD;"},
            {name=>"Db::DB_ENV_TXN_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_TXN_NOSYNC;"},
            {name=>"Db::DB_ENV_TXN_WRITE_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_TXN_WRITE_NOSYNC;"},
            {name=>"Db::DB_ENV_YIELDCPU", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ENV_YIELDCPU;"},
            {name=>"Db::DB_EXCL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EXCL;"},
            {name=>"Db::DB_EXTENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_EXTENT;"},
            {name=>"Db::DB_FAST_STAT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FAST_STAT;"},
            {name=>"Db::DB_FCNTL_LOCKING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FCNTL_LOCKING;"},
            {name=>"Db::DB_FILE_ID_LEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FILE_ID_LEN;"},
            {name=>"Db::DB_FIRST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FIRST;"},
            {name=>"Db::DB_FLUSH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FLUSH;"},
            {name=>"Db::DB_FORCE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FORCE;"},
            {name=>"Db::DB_FREELIST_ONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FREELIST_ONLY;"},
            {name=>"Db::DB_FREE_SPACE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_FREE_SPACE;"},
            {name=>"Db::DB_GET_BOTH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_BOTH;"},
            {name=>"Db::DB_GET_BOTHC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_BOTHC;"},
            {name=>"Db::DB_GET_BOTH_RANGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_BOTH_RANGE;"},
            {name=>"Db::DB_GET_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_GET_RECNO;"},
            {name=>"Db::DB_HANDLE_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HANDLE_LOCK;"},
            {name=>"Db::DB_HASH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HASH;"},
            {name=>"Db::DB_HASHMAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HASHMAGIC;"},
            {name=>"Db::DB_HASHOLDVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HASHOLDVER;"},
            {name=>"Db::DB_HASHVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_HASHVERSION;"},
            {name=>"Db::DB_IMMUTABLE_KEY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_IMMUTABLE_KEY;"},
            {name=>"Db::DB_INIT_CDB", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_CDB;"},
            {name=>"Db::DB_INIT_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_LOCK;"},
            {name=>"Db::DB_INIT_LOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_LOG;"},
            {name=>"Db::DB_INIT_MPOOL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_MPOOL;"},
            {name=>"Db::DB_INIT_TXN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_INIT_TXN;"},
            {name=>"Db::DB_JOINENV", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOINENV;"},
            {name=>"Db::DB_JOIN_ITEM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOIN_ITEM;"},
            {name=>"Db::DB_JOIN_NOSORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_JOIN_NOSORT;"},
            {name=>"Db::DB_KEYEMPTY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYEMPTY;"},
            {name=>"Db::DB_KEYEXIST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYEXIST;"},
            {name=>"Db::DB_KEYFIRST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYFIRST;"},
            {name=>"Db::DB_KEYLAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_KEYLAST;"},
            {name=>"Db::DB_LAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LAST;"},
            {name=>"Db::DB_LOCKDOWN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCKDOWN;"},
            {name=>"Db::DB_LOCKVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCKVERSION;"},
            {name=>"Db::DB_LOCK_DEADLOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DEADLOCK;"},
            {name=>"Db::DB_LOCK_DEFAULT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DEFAULT;"},
            {name=>"Db::DB_LOCK_DUMP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_DUMP;"},
            {name=>"Db::DB_LOCK_EXPIRE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_EXPIRE;"},
            {name=>"Db::DB_LOCK_GET", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_GET;"},
            {name=>"Db::DB_LOCK_GET_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_GET_TIMEOUT;"},
            {name=>"Db::DB_LOCK_INHERIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_INHERIT;"},
            {name=>"Db::DB_LOCK_IREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IREAD;"},
            {name=>"Db::DB_LOCK_IWR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IWR;"},
            {name=>"Db::DB_LOCK_IWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_IWRITE;"},
            {name=>"Db::DB_LOCK_MAXLOCKS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MAXLOCKS;"},
            {name=>"Db::DB_LOCK_MINLOCKS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MINLOCKS;"},
            {name=>"Db::DB_LOCK_MINWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_MINWRITE;"},
            {name=>"Db::DB_LOCK_NG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NG;"},
            {name=>"Db::DB_LOCK_NORUN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NORUN;"},
            {name=>"Db::DB_LOCK_NOTEXIST", type=>"IV", macro=>["#ifdef DB_LOCK_NOTEXIST\n", "#endif\n"], value=>"DB_LOCK_NOTEXIST"},
            {name=>"Db::DB_LOCK_NOTGRANTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NOTGRANTED;"},
            {name=>"Db::DB_LOCK_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_NOWAIT;"},
            {name=>"Db::DB_LOCK_OLDEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_OLDEST;"},
            {name=>"Db::DB_LOCK_PUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT;"},
            {name=>"Db::DB_LOCK_PUT_ALL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_ALL;"},
            {name=>"Db::DB_LOCK_PUT_OBJ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_OBJ;"},
            {name=>"Db::DB_LOCK_PUT_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_PUT_READ;"},
            {name=>"Db::DB_LOCK_RANDOM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_RANDOM;"},
            {name=>"Db::DB_LOCK_READ", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_READ;"},
            {name=>"Db::DB_LOCK_RECORD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_RECORD;"},
            {name=>"Db::DB_LOCK_REMOVE", type=>"IV", macro=>["#ifdef DB_LOCK_REMOVE\n", "#endif\n"], value=>"DB_LOCK_REMOVE"},
            {name=>"Db::DB_LOCK_SET_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_SET_TIMEOUT;"},
            {name=>"Db::DB_LOCK_SWITCH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_SWITCH;"},
            {name=>"Db::DB_LOCK_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_TIMEOUT;"},
            {name=>"Db::DB_LOCK_TRADE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_TRADE;"},
            {name=>"Db::DB_LOCK_UPGRADE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_UPGRADE;"},
            {name=>"Db::DB_LOCK_UPGRADE_WRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_UPGRADE_WRITE;"},
            {name=>"Db::DB_LOCK_WAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WAIT;"},
            {name=>"Db::DB_LOCK_WRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WRITE;"},
            {name=>"Db::DB_LOCK_WWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_WWRITE;"},
            {name=>"Db::DB_LOCK_YOUNGEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOCK_YOUNGEST;"},
            {name=>"Db::DB_LOGC_BUF_SIZE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOGC_BUF_SIZE;"},
            {name=>"Db::DB_LOGFILEID_INVALID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOGFILEID_INVALID;"},
            {name=>"Db::DB_LOGMAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOGMAGIC;"},
            {name=>"Db::DB_LOGOLDVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOGOLDVER;"},
            {name=>"Db::DB_LOGVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOGVERSION;"},
            {name=>"Db::DB_LOG_DISK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOG_DISK;"},
            {name=>"Db::DB_LOG_LOCKED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOG_LOCKED;"},
            {name=>"Db::DB_LOG_SILENT_ERR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LOG_SILENT_ERR;"},
            {name=>"Db::DB_LSTAT_ABORTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_ABORTED;"},
            {name=>"Db::DB_LSTAT_EXPIRED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_EXPIRED;"},
            {name=>"Db::DB_LSTAT_FREE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_FREE;"},
            {name=>"Db::DB_LSTAT_HELD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_HELD;"},
            {name=>"Db::DB_LSTAT_PENDING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_PENDING;"},
            {name=>"Db::DB_LSTAT_WAITING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_LSTAT_WAITING;"},
            {name=>"Db::DB_MAX_PAGES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MAX_PAGES;"},
            {name=>"Db::DB_MAX_RECORDS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MAX_RECORDS;"},
            {name=>"Db::DB_MPOOL_CLEAN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_CLEAN;"},
            {name=>"Db::DB_MPOOL_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_CREATE;"},
            {name=>"Db::DB_MPOOL_DIRTY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_DIRTY;"},
            {name=>"Db::DB_MPOOL_DISCARD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_DISCARD;"},
            {name=>"Db::DB_MPOOL_LAST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_LAST;"},
            {name=>"Db::DB_MPOOL_NEW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MPOOL_NEW;"},
            {name=>"Db::DB_MULTIPLE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MULTIPLE;"},
            {name=>"Db::DB_MULTIPLE_KEY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MULTIPLE_KEY;"},
            {name=>"Db::DB_MUTEX_ALLOCATED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MUTEX_ALLOCATED;"},
            {name=>"Db::DB_MUTEX_LOGICAL_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MUTEX_LOGICAL_LOCK;"},
            {name=>"Db::DB_MUTEX_SELF_BLOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MUTEX_SELF_BLOCK;"},
            {name=>"Db::DB_MUTEX_THREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_MUTEX_THREAD;"},
            {name=>"Db::DB_NEEDSPLIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEEDSPLIT;"},
            {name=>"Db::DB_NEXT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT;"},
            {name=>"Db::DB_NEXT_DUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT_DUP;"},
            {name=>"Db::DB_NEXT_NODUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NEXT_NODUP;"},
            {name=>"Db::DB_NODUPDATA", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NODUPDATA;"},
            {name=>"Db::DB_NOLOCKING", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOLOCKING;"},
            {name=>"Db::DB_NOMMAP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOMMAP;"},
            {name=>"Db::DB_NOORDERCHK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOORDERCHK;"},
            {name=>"Db::DB_NOOVERWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOOVERWRITE;"},
            {name=>"Db::DB_NOPANIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOPANIC;"},
            {name=>"Db::DB_NOSERVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER;"},
            {name=>"Db::DB_NOSERVER_HOME", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER_HOME;"},
            {name=>"Db::DB_NOSERVER_ID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSERVER_ID;"},
            {name=>"Db::DB_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOSYNC;"},
            {name=>"Db::DB_NOTFOUND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_NOTFOUND;"},
            {name=>"Db::DB_ODDFILESIZE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ODDFILESIZE;"},
            {name=>"Db::DB_OK_BTREE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OK_BTREE;"},
            {name=>"Db::DB_OK_HASH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OK_HASH;"},
            {name=>"Db::DB_OK_QUEUE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OK_QUEUE;"},
            {name=>"Db::DB_OK_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OK_RECNO;"},
            {name=>"Db::DB_OLD_VERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OLD_VERSION;"},
            {name=>"Db::DB_OPFLAGS_MASK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OPFLAGS_MASK;"},
            {name=>"Db::DB_ORDERCHKONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_ORDERCHKONLY;"},
            {name=>"Db::DB_OVERWRITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_OVERWRITE;"},
            {name=>"Db::DB_PAGE_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PAGE_LOCK;"},
            {name=>"Db::DB_PAGE_NOTFOUND", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PAGE_NOTFOUND;"},
            {name=>"Db::DB_PANIC_ENVIRONMENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PANIC_ENVIRONMENT;"},
            {name=>"Db::DB_POSITION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_POSITION;"},
            {name=>"Db::DB_PREV", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PREV;"},
            {name=>"Db::DB_PREV_NODUP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PREV_NODUP;"},
            {name=>"Db::DB_PRINTABLE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRINTABLE;"},
            {name=>"Db::DB_PRIORITY_DEFAULT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_DEFAULT;"},
            {name=>"Db::DB_PRIORITY_HIGH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_HIGH;"},
            {name=>"Db::DB_PRIORITY_LOW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_LOW;"},
            {name=>"Db::DB_PRIORITY_VERY_HIGH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_VERY_HIGH;"},
            {name=>"Db::DB_PRIORITY_VERY_LOW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIORITY_VERY_LOW;"},
            {name=>"Db::DB_PRIVATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PRIVATE;"},
            {name=>"Db::DB_PR_PAGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PR_PAGE;"},
            {name=>"Db::DB_PR_RECOVERYTEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_PR_RECOVERYTEST;"},
            {name=>"Db::DB_QAMMAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_QAMMAGIC;"},
            {name=>"Db::DB_QAMOLDVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_QAMOLDVER;"},
            {name=>"Db::DB_QAMVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_QAMVERSION;"},
            {name=>"Db::DB_QUEUE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_QUEUE;"},
            {name=>"Db::DB_RDONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RDONLY;"},
            {name=>"Db::DB_RDWRMASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RDWRMASTER;"},
            {name=>"Db::DB_READ_COMMITTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_READ_COMMITTED;"},
            {name=>"Db::DB_READ_UNCOMMITTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_READ_UNCOMMITTED;"},
            {name=>"Db::DB_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECNO;"},
            {name=>"Db::DB_RECNUM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECNUM;"},
            {name=>"Db::DB_RECORDCOUNT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECORDCOUNT;"},
            {name=>"Db::DB_RECORD_LOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECORD_LOCK;"},
            {name=>"Db::DB_RECOVER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECOVER;"},
            {name=>"Db::DB_RECOVER_FATAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RECOVER_FATAL;"},
            {name=>"Db::DB_REGION_INIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REGION_INIT;"},
            {name=>"Db::DB_REGION_MAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REGION_MAGIC;"},
            {name=>"Db::DB_REGISTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REGISTER;"},
            {name=>"Db::DB_RENAMEMAGIC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RENAMEMAGIC;"},
            {name=>"Db::DB_RENUMBER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RENUMBER;"},
            {name=>"Db::DB_REP_ANYWHERE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_ANYWHERE;"},
            {name=>"Db::DB_REP_BULKOVF", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_BULKOVF;"},
            {name=>"Db::DB_REP_CLIENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_CLIENT;"},
            {name=>"Db::DB_REP_DUPMASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_DUPMASTER;"},
            {name=>"Db::DB_REP_HOLDELECTION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_HOLDELECTION;"},
            {name=>"Db::DB_REP_IGNORE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_IGNORE;"},
            {name=>"Db::DB_REP_LOCKOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_LOCKOUT;"},
            {name=>"Db::DB_REP_MASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_MASTER;"},
            {name=>"Db::DB_REP_NEWMASTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_NEWMASTER;"},
            {name=>"Db::DB_REP_NEWSITE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_NEWSITE;"},
            {name=>"Db::DB_REP_PERMANENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_PERMANENT;"},
            {name=>"Db::DB_REP_REREQUEST", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_REREQUEST;"},
            {name=>"Db::DB_REP_UNAVAIL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REP_UNAVAIL;"},
            {name=>"Db::DB_REVSPLITOFF", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_REVSPLITOFF;"},
            {name=>"Db::DB_RMW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RMW;"},
            {name=>"Db::DB_RUNRECOVERY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_RUNRECOVERY;"},
            {name=>"Db::DB_SALVAGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SALVAGE;"},
            {name=>"Db::DB_SECONDARY_BAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SECONDARY_BAD;"},
            {name=>"Db::DB_SEQ_WRAPPED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SEQ_WRAPPED;"},
            {name=>"Db::DB_SET", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET;"},
            {name=>"Db::DB_SET_LOCK_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_LOCK_TIMEOUT;"},
            {name=>"Db::DB_SET_RANGE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_RANGE;"},
            {name=>"Db::DB_SET_RECNO", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_RECNO;"},
            {name=>"Db::DB_SET_TXN_NOW", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_TXN_NOW;"},
            {name=>"Db::DB_SET_TXN_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SET_TXN_TIMEOUT;"},
            {name=>"Db::DB_SNAPSHOT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SNAPSHOT;"},
            {name=>"Db::DB_STAT_CLEAR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_STAT_CLEAR;"},
            {name=>"Db::DB_SURPRISE_KID", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SURPRISE_KID;"},
            {name=>"Db::DB_SWAPBYTES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SWAPBYTES;"},
            {name=>"Db::DB_SYSTEM_MEM", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_SYSTEM_MEM;"},
            {name=>"Db::DB_TEST_ELECTINIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_ELECTINIT;"},
            {name=>"Db::DB_TEST_POSTDESTROY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_POSTDESTROY;"},
            {name=>"Db::DB_TEST_POSTLOG", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_POSTLOG;"},
            {name=>"Db::DB_TEST_POSTLOGMETA", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_POSTLOGMETA;"},
            {name=>"Db::DB_TEST_POSTOPEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_POSTOPEN;"},
            {name=>"Db::DB_TEST_POSTSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_POSTSYNC;"},
            {name=>"Db::DB_TEST_PREDESTROY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_PREDESTROY;"},
            {name=>"Db::DB_TEST_PREOPEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_PREOPEN;"},
            {name=>"Db::DB_TEST_SUBDB_LOCKS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TEST_SUBDB_LOCKS;"},
            {name=>"Db::DB_THREAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_THREAD;"},
            {name=>"Db::DB_TIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TIMEOUT;"},
            {name=>"Db::DB_TRUNCATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TRUNCATE;"},
            {name=>"Db::DB_TXNVERSION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXNVERSION;"},
            {name=>"Db::DB_TXN_ABORT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_ABORT;"},
            {name=>"Db::DB_TXN_APPLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_APPLY;"},
            {name=>"Db::DB_TXN_BACKWARD_ALLOC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_BACKWARD_ALLOC;"},
            {name=>"Db::DB_TXN_BACKWARD_ROLL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_BACKWARD_ROLL;"},
            {name=>"Db::DB_TXN_CKP", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_CKP;"},
            {name=>"Db::DB_TXN_FORWARD_ROLL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_FORWARD_ROLL;"},
            {name=>"Db::DB_TXN_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_NOSYNC;"},
            {name=>"Db::DB_TXN_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_NOWAIT;"},
            {name=>"Db::DB_TXN_OPENFILES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_OPENFILES;"},
            {name=>"Db::DB_TXN_POPENFILES", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_POPENFILES;"},
            {name=>"Db::DB_TXN_PRINT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_PRINT;"},
            {name=>"Db::DB_TXN_SYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_SYNC;"},
            {name=>"Db::DB_TXN_WRITE_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_TXN_WRITE_NOSYNC;"},
            {name=>"Db::DB_UNKNOWN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_UNKNOWN;"},
            {name=>"Db::DB_UPDATE_SECONDARY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_UPDATE_SECONDARY;"},
            {name=>"Db::DB_UPGRADE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_UPGRADE;"},
            {name=>"Db::DB_USE_ENVIRON", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_USE_ENVIRON;"},
            {name=>"Db::DB_USE_ENVIRON_ROOT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_USE_ENVIRON_ROOT;"},
            {name=>"Db::DB_VERB_DEADLOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_DEADLOCK;"},
            {name=>"Db::DB_VERB_RECOVERY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_RECOVERY;"},
            {name=>"Db::DB_VERB_REGISTER", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_REGISTER;"},
            {name=>"Db::DB_VERB_REPLICATION", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_REPLICATION;"},
            {name=>"Db::DB_VERB_WAITSFOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERB_WAITSFOR;"},
            {name=>"Db::DB_VERIFY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERIFY;"},
            {name=>"Db::DB_VERIFY_BAD", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERIFY_BAD;"},
            {name=>"Db::DB_VERIFY_FATAL", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERIFY_FATAL;"},
            {name=>"Db::DB_VERSION_MAJOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_MAJOR;"},
            {name=>"Db::DB_VERSION_MINOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_MINOR;"},
            {name=>"Db::DB_VERSION_PATCH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_VERSION_PATCH;"},
            {name=>"Db::DB_VERSION_STRING", type=>"PVN", macro=>"1", value=>["DB_VERSION_STRING", "17"]},
            {name=>"Db::DB_WRITECURSOR", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_WRITECURSOR;"},
            {name=>"Db::DB_WRITELOCK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_WRITELOCK;"},
            {name=>"Db::DB_WRITEOPEN", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_WRITEOPEN;"},
            {name=>"Db::DB_XA_CREATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_XA_CREATE;"},
            {name=>"Db::DB_XIDDATASIZE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_XIDDATASIZE;"},
            {name=>"Db::DB_YIELDCPU", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = DB_YIELDCPU;"},
            {name=>"Db::MP_FLUSH", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = MP_FLUSH;"},
            {name=>"Db::MP_OPEN_CALLED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = MP_OPEN_CALLED;"},
            {name=>"Db::MP_READONLY", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = MP_READONLY;"},
            {name=>"Db::REP_CONF_AUTOINIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = REP_CONF_AUTOINIT;"},
            {name=>"Db::REP_CONF_BULK", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = REP_CONF_BULK;"},
            {name=>"Db::REP_CONF_DELAYCLIENT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = REP_CONF_DELAYCLIENT;"},
            {name=>"Db::REP_CONF_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = REP_CONF_NOWAIT;"},
            {name=>"Db::TXN_CHILDCOMMIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_CHILDCOMMIT;"},
            {name=>"Db::TXN_COMPENSATE", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_COMPENSATE;"},
            {name=>"Db::TXN_DIRTY_READ", type=>"IV", macro=>["#ifdef TXN_DIRTY_READ\n", "#endif\n"], value=>"TXN_DIRTY_READ"},
            {name=>"Db::TXN_LOCKTIMEOUT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_LOCKTIMEOUT;"},
            {name=>"Db::TXN_MALLOC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_MALLOC;"},
            {name=>"Db::TXN_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_NOSYNC;"},
            {name=>"Db::TXN_NOWAIT", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_NOWAIT;"},
            {name=>"Db::TXN_READ_COMMITTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_READ_COMMITTED;"},
            {name=>"Db::TXN_READ_UNCOMMITTED", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_READ_UNCOMMITTED;"},
            {name=>"Db::TXN_SYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_SYNC;"},
            {name=>"Db::TXN_SYNC_FLAGS", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_SYNC_FLAGS;"},
            {name=>"Db::TXN_WRITE_NOSYNC", type=>"IV", macro=>"1", value=>"(IV)tmp", pre=>"\tint tmp = TXN_WRITE_NOSYNC;"});

print constant_types(); # macro defs
foreach (C_constant ("Db", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "#### XS Section:\n";
print XS_constant ("Db", $types);
__END__
   */

  switch (len) {
  case 10:
    return constant_10 (aTHX_ name, iv_return);
    break;
  case 11:
    return constant_11 (aTHX_ name, iv_return);
    break;
  case 12:
    return constant_12 (aTHX_ name, iv_return);
    break;
  case 13:
    return constant_13 (aTHX_ name, iv_return);
    break;
  case 14:
    return constant_14 (aTHX_ name, iv_return);
    break;
  case 15:
    return constant_15 (aTHX_ name, iv_return);
    break;
  case 16:
    return constant_16 (aTHX_ name, iv_return);
    break;
  case 17:
    return constant_17 (aTHX_ name, iv_return);
    break;
  case 18:
    return constant_18 (aTHX_ name, iv_return);
    break;
  case 19:
    return constant_19 (aTHX_ name, iv_return);
    break;
  case 20:
    return constant_20 (aTHX_ name, iv_return);
    break;
  case 21:
    return constant_21 (aTHX_ name, iv_return, pv_return);
    break;
  case 22:
    return constant_22 (aTHX_ name, iv_return);
    break;
  case 23:
    return constant_23 (aTHX_ name, iv_return);
    break;
  case 24:
    return constant_24 (aTHX_ name, iv_return);
    break;
  case 25:
    return constant_25 (aTHX_ name, iv_return);
    break;
  case 26:
    /* Names all of length 26.  */
    /* "Db::DB_ASSOC_IMMUTABLE_KEY" "Db::DB_ENV_RPCCLIENT_GIVEN" */
    /* Offset 15 gives the best switch position.  */
    switch (name[15]) {
    case 'L':
      if (memEQ(name, "Db::DB_ENV_RPCCLIENT_GIVEN", 26)) {
      /*                              ^                 */
        {
	int tmp = DB_ENV_RPCCLIENT_GIVEN;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    case 'M':
      if (memEQ(name, "Db::DB_ASSOC_IMMUTABLE_KEY", 26)) {
      /*                              ^                 */
        {
	int tmp = DB_ASSOC_IMMUTABLE_KEY;
          *iv_return = (IV)tmp;
          return PERL_constant_ISIV;
        }
      }
      break;
    }
    break;
  case 27:
    if (memEQ(name, "Db::DB_ENV_TXN_WRITE_NOSYNC", 27)) {
      {
	int tmp = DB_ENV_TXN_WRITE_NOSYNC;
        *iv_return = (IV)tmp;
        return PERL_constant_ISIV;
      }
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

