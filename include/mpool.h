/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)mpool.h	8.1 (Berkeley) 06/02/93
 */

/*
 * The memory pool scheme is a simple one.  Each in memory page is referenced
 * by a bucket which is threaded in three ways.  All active pages are threaded
 * on a hash chain (hashed by the page number) and an lru chain.  Inactive
 * pages are threaded on a free chain.  Each reference to a memory pool is
 * handed an MPOOL which is the opaque cookie passed to all of the memory
 * routines.
 */
#define	HASHSIZE	128
#define	HASHKEY(pgno)	((pgno - 1) % HASHSIZE)

/* The BKT structures are the elements of the lists. */
typedef struct BKT {
	struct BKT	*hnext;		/* next hash bucket */
	struct BKT	*hprev;		/* previous hash bucket */
	struct BKT	*cnext;		/* next free/lru bucket */
	struct BKT	*cprev;		/* previous free/lru bucket */
	void		*page;		/* page */
	pgno_t		pgno;		/* page number */

#define	MPOOL_DIRTY	0x01		/* page needs to be written */
#define	MPOOL_PINNED	0x02		/* page is pinned into memory */
	unsigned long	flags;		/* flags */
} BKT;

/* The BKTHDR structures are the heads of the lists. */
typedef struct BKTHDR {
	struct BKT	*hnext;		/* next hash bucket */
	struct BKT	*hprev;		/* previous hash bucket */
	struct BKT	*cnext;		/* next free/lru bucket */
	struct BKT	*cprev;		/* previous free/lru bucket */
} BKTHDR;

typedef struct MPOOL {
	BKTHDR	free;			/* The free list. */
	BKTHDR	lru;			/* The LRU list. */
	BKTHDR	hashtable[HASHSIZE];	/* Hashed list by page number. */
	pgno_t	curcache;		/* Current number of cached pages. */
	pgno_t	maxcache;		/* Max number of cached pages. */
	pgno_t	npages;			/* Number of pages in the file. */
	u_long	pagesize;		/* File page size. */
	int	fd;			/* File descriptor. */
					/* Page in conversion routine. */
	void    (*pgin) __P((void *, pgno_t, void *));
					/* Page out conversion routine. */
	void    (*pgout) __P((void *, pgno_t, void *));
	void	*pgcookie;		/* Cookie for page in/out routines. */
#ifdef STATISTICS
	unsigned long	cachehit;
	unsigned long	cachemiss;
	unsigned long	pagealloc;
	unsigned long	pageflush;
	unsigned long	pageget;
	unsigned long	pagenew;
	unsigned long	pageput;
	unsigned long	pageread;
	unsigned long	pagewrite;
#endif
} MPOOL;

#ifdef __MPOOLINTERFACE_PRIVATE
/* Macros to insert/delete into/from hash chain. */
#define rmhash(bp) { \
        (bp)->hprev->hnext = (bp)->hnext; \
        (bp)->hnext->hprev = (bp)->hprev; \
}
#define inshash(bp, pg) { \
	hp = &mp->hashtable[HASHKEY(pg)]; \
        (bp)->hnext = hp->hnext; \
        (bp)->hprev = (struct BKT *)hp; \
        hp->hnext->hprev = (bp); \
        hp->hnext = (bp); \
}

/* Macros to insert/delete into/from lru and free chains. */
#define	rmchain(bp) { \
        (bp)->cprev->cnext = (bp)->cnext; \
        (bp)->cnext->cprev = (bp)->cprev; \
}
#define inschain(bp, dp) { \
        (bp)->cnext = (dp)->cnext; \
        (bp)->cprev = (struct BKT *)(dp); \
        (dp)->cnext->cprev = (bp); \
        (dp)->cnext = (bp); \
}
#endif

__BEGIN_DECLS
MPOOL	*mpool_open __P((DBT *, int, pgno_t, pgno_t));
void	 mpool_filter __P((MPOOL *, void (*)(void *, pgno_t, void *),
	    void (*)(void *, pgno_t, void *), void *));
void	*mpool_new __P((MPOOL *, pgno_t *));
void	*mpool_get __P((MPOOL *, pgno_t, u_int));
int	 mpool_put __P((MPOOL *, void *, u_int));
int	 mpool_sync __P((MPOOL *));
int	 mpool_close __P((MPOOL *));
#ifdef STATISTICS
void	 mpool_stat __P((MPOOL *));
#endif
__END_DECLS
