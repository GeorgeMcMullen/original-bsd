/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)user.h	7.11 (Berkeley) 05/15/90
 */

#ifdef KERNEL
#include "machine/pcb.h"
#include "dmap.h"
#include "time.h"
#include "resource.h"
#include "namei.h"
#include "ucred.h"
#else
#include <machine/pcb.h>
#include <sys/dmap.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/namei.h>
#include <sys/ucred.h>
#endif

/*
 * Per process structure containing data that
 * isn't needed in core when the process is swapped out.
 */
 
struct	user {
	struct	pcb u_pcb;
	struct	proc *u_procp;		/* pointer to proc structure */
	int	*u_ar0;			/* address of users saved R0 */
/* syscall parameters, results and catches */
	int	u_arg[8];		/* arguments to current system call */
	int	*u_ap;			/* pointer to arglist */
	union {				/* syscall return values */
		struct	{
			int	R_val1;
			int	R_val2;
		} u_rv;
#define	r_val1	u_rv.R_val1
#define	r_val2	u_rv.R_val2
		off_t	r_off;
		time_t	r_time;
	} u_r;
	int	u_error;		/* return error code */

/* 1.1 - processes and protection */
#define u_cred u_nd.ni_cred
#define u_uid	u_cred->cr_uid		/* effective user id */
#define u_gid	u_cred->cr_gid		/* effective group id */

/* 1.2 - memory management */
	segsz_t u_tsize;		/* text size (clicks) */
	segsz_t u_dsize;		/* data size (clicks) */
	segsz_t u_ssize;		/* stack size (clicks) */
	struct	dmap u_dmap;		/* disk map for data segment */
	struct	dmap u_smap;		/* disk map for stack segment */
	struct	dmap u_cdmap;		/* temp data segment disk map */
	struct	dmap u_csmap;		/* temp stack segment disk map */
	label_t u_ssave;		/* label variable for swapping */
	segsz_t u_odsize, u_ossize;	/* for (clumsy) expansion swaps */
	time_t	u_outime;		/* user time at last sample */
	struct	mapmem *u_mmap;		/* list of mapped memory regions */

/* 1.3 - signal management */
	sig_t	u_signal[NSIG];		/* disposition of signals */
	int	u_sigmask[NSIG];	/* signals to be blocked */
	int	u_sigonstack;		/* signals to take on sigstack */
	int	u_sigintr;		/* signals that interrupt syscalls */
	int	u_oldmask;		/* saved mask from before sigpause */
	struct	sigstack u_sigstack;	/* sp & on stack state variable */
#define	u_onstack	u_sigstack.ss_onstack
#define	u_sigsp		u_sigstack.ss_sp
#define	u_sig	u_arg[0]		/* for core dump/debugger XXX */
#define	u_code	u_arg[1]		/* for core dump/debugger XXX */

/* 1.4 - descriptor management */
	struct	file *u_ofile[NOFILE];	/* file structures for open files */
	char	u_pofile[NOFILE];	/* per-process flags of open files */
	int	u_lastfile;		/* high-water mark of u_ofile */
#define	UF_EXCLOSE 	0x1		/* auto-close on exec */
#define	UF_MAPPED 	0x2		/* mapped from device */
#define u_cdir u_nd.ni_cdir		/* current directory */
#define u_rdir u_nd.ni_rdir		/* root directory of current process */
	short	u_cmask;		/* mask for file creation */

/* 1.5 - timing and statistics */
	struct	rusage u_ru;		/* stats for this proc */
	struct	rusage u_cru;		/* sum of stats for reaped children */
	struct	itimerval u_timer[3];
	struct	timeval u_start;
	short	u_acflag;

	struct uprof {			/* profile arguments */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} u_prof;

/* 1.6 - resource controls */
	struct	rlimit u_rlimit[RLIM_NLIMITS];

/* namei & co. */
	struct	nameidata u_nd;

	long	u_spare[8];
	int	u_stack[1];
};

/* u_eosys values */
#define	JUSTRETURN	1
#define	RESTARTSYS	2
#define NORMALRETURN	3

/* u_error codes */
#ifdef KERNEL
#include "errno.h"
#else
#include <errno.h>
#endif

#ifdef KERNEL
extern	struct user u;
extern	struct user swaputl;
extern	struct user forkutl;
extern	struct user xswaputl;
extern	struct user xswap2utl;
extern	struct user pushutl;
extern	struct user vfutl;
#endif
