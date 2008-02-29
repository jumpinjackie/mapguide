This directory contains patches to fix bugs found in BerkeleyDB.



db-3.1.17

    qam_stat.patch      Fixes a problem of the stat function for QUEUE
			files clearing the dirty bit of the metadata
			page, so if a stat (or a len(db) in Python)
			was the last thing you did before saving a
			file you could lose the records written to the
			queue since the last sync.



db-3.3.11

    hash_page.patch	Fixes a core dump problem when using a hash db
			in a transaction with the DB_TXN_NOWAIT flag.


