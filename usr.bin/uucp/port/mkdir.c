#ifndef lint
static char sccsid[] = "@(#)mkdir.c	5.1 (Berkeley) 03/22/85";
#endif

#ifndef BSD4_2
#include <stdio.h>
/*
 * make a directory. Also make sure that the directory is owned
 * by the right userid
 */
mkdir(path, mode)
char *path;
int mode;
{
	int pid, status, w;

	if (pid=fork()) {
		while ((w = wait(&status)) != pid && w != -1)
			;
		(void) chmod(path, mode);
	} else {
		(void) umask(mode);
		(void) execlp("mkdir", "mkdir", path, (char *)NULL);
		perror(path);
		_exit(1);
	}
}
#endif !BSD4_2
