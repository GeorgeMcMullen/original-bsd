/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)iso_pcb.h	7.6 (Berkeley) 02/25/93
 */

/***********************************************************
		Copyright IBM Corporation 1987

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * ARGO Project, Computer Sciences Dept., University of Wisconsin - Madison
 */
/* $Header: iso_pcb.h,v 4.3 88/06/29 15:00:01 hagens Exp $ */
/* $Source: /usr/argo/sys/netiso/RCS/iso_pcb.h,v $ */

#define	MAXX25CRUDLEN	16	/* 16 bytes of call request user data */

/*
 * Common structure pcb for argo protocol implementation.
 */
struct isopcb {
	struct	isopcb			*isop_next,*isop_prev; /* pointers to other pcb's */
	struct	isopcb			*isop_head;	/* pointer back to chain of pcbs for 
								this protocol */
	struct	socket			*isop_socket;	/* back pointer to socket */
	struct	sockaddr_iso	*isop_laddr;
	struct	sockaddr_iso	*isop_faddr;
	struct	route_iso {
		struct	rtentry 	*ro_rt;
		struct	sockaddr_iso ro_dst;
	}						isop_route;			/* CLNP routing entry */
	struct	mbuf			*isop_options;		/* CLNP options */
	struct	mbuf			*isop_optindex;		/* CLNP options index */
	struct	mbuf			*isop_clnpcache;	/* CLNP cached hdr */
	caddr_t					isop_chan;		/* actually struct pklcb * */
	u_short					isop_refcnt;		/* mult TP4 tpcb's -> here */
	u_short					isop_lport;			/* MISLEADLING work var */
	u_short					isop_tuba_cached;	/* for tuba address ref cnts */
	int						isop_x25crud_len;	/* x25 call request ud */
	char					isop_x25crud[MAXX25CRUDLEN];
	struct ifaddr			*isop_ifa;		/* ESIS interface assoc w/sock */
	struct	sockaddr_iso	isop_sladdr,		/* preallocated laddr */
							isop_sfaddr;		/* preallocated faddr */
};

#ifdef sotorawcb
/*
 * Common structure pcb for raw clnp protocol access.
 * Here are clnp specific extensions to the raw control block,
 * and space is allocated to the necessary sockaddrs.
 */
struct rawisopcb {
	struct	rawcb risop_rcb;		/* common control block prefix */
	int		risop_flags;			/* flags, e.g. raw sockopts */
	struct	isopcb risop_isop;		/* space for bound addresses, routes etc.*/
};
#endif

#define	sotoisopcb(so)	((struct isopcb *)(so)->so_pcb)
#define	sotorawisopcb(so)	((struct rawisopcb *)(so)->so_pcb)

#ifdef KERNEL
struct	isopcb *iso_pcblookup();
#endif
