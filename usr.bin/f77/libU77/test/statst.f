C
C Copyright (c) 1980 The Regents of the University of California.
C All rights reserved.
C
C %sccs.include.proprietary.f%
C
C	@(#)statst.f	5.2 (Berkeley) 04/12/91
C

	character*24 ctime
	integer stata(11), statb(11), i1, i2
	open(1, file='statst.f')
	i1 = stat('statst.f', stata)
	i2 = fstat(1, statb)
	write(*,*) i1, i2, ierrno()
	write(*,'(8r,2i12)') (stata(i),statb(i),i=1,8)
	write(*,'(a,2x,a)') (ctime(stata(i)), ctime(statb(i)), i=9,11)
	end
