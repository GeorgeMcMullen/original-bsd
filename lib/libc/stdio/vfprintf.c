/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that this notice is preserved and that due credit is given
 * to the University of California at Berkeley. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission. This software
 * is provided ``as is'' without express or implied warranty.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)vfprintf.c	5.5 (Berkeley) 05/09/88";
#endif /* LIBC_SCCS and not lint */

#include <sys/param.h>
#include <varargs.h>
#include <stdio.h>
#include <ctype.h>

#define	MAXBUF		120			/* should hold any number */
#define	DEFPREC		6			/* default precision */

#define	PUTC(ch, fd)	{++cnt; putc(ch, fd);}

#define	LONGINT		0x01
#define	LONGDBL		0x02
#define	SHORTINT	0x04
#define	GETARG(r) \
	r = argsize&LONGINT ? va_arg(argp, long) : \
	    argsize&SHORTINT ? va_arg(argp, short) : va_arg(argp, int);

x_doprnt(fmt, argp, fp)
	register char *fmt;
	va_list argp;
	register FILE *fp;
{
	register u_long reg_ulong;
	register long reg_long;
	register int base;
	register char *digs, *bp, *t, padc;
	double _double;
	char argsize, printsign, buf[MAXBUF], *ecvt(), *fcvt();
	int alternate, cnt, decpt, n, ladjust, width, prec, sign, size;

	for (cnt = 0; *fmt; ++fmt) {
		if (*fmt != '%') {
			PUTC(*fmt, fp);
			continue;
		}

		alternate = ladjust = width = 0;
		prec = -1;
		padc = ' ';
		argsize = printsign = '\0';

flags:		switch (*++fmt) {
		case '#':
			alternate = 1;
			goto flags;
		case '%':			/* "%#%" prints as "%" */
			PUTC('%', fp);
			continue;
		case '*':
			/*
			 * ``A negative field width argument is taken as a
			 * - flag followed by a  positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			if ((width = va_arg(argp, int)) >= 0)
				goto flags;
			width = -width;
			/*FALLTHROUGH*/
		case '-':
			ladjust = 1;
			goto flags;
		case '+':
			printsign = '+';
			goto flags;
		case '.':
			if (*++fmt == '*')
				prec = va_arg(argp, int);
			else if (isdigit(*fmt)) {
				prec = 0;
				do {
					prec = 10 * prec + *fmt - '0';
				} while isdigit(*++fmt);
				--fmt;
			}
			else {
				prec = 0;
				--fmt;
				goto flags;
			}
			if (prec < 0)
				prec = -1;
			goto flags;
		case '0':
			padc = '0';
			/*FALLTHROUGH*/
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			do {
				width = 10 * width + *fmt - '0';
			} while isdigit(*++fmt);
			--fmt;
		case 'L':
			argsize |= LONGDBL;
			goto flags;
		case 'h':
			argsize |= SHORTINT;
			goto flags;
		case 'l':
			argsize |= LONGINT;
			goto flags;
		}

		digs = "0123456789abcdef";

		switch (*fmt) {
		case 'c':
			PUTC(va_arg(argp, int), fp);
			break;
		case 'd':
		case 'i':
			GETARG(reg_long);
			if (reg_long < 0) {
				reg_ulong = -reg_long;
				printsign = '-';
			}
			else {
				reg_ulong = reg_long;
			}
			if (printsign)
				PUTC(printsign, fp);
			base = 10;
			goto num1;
		case 'E':
		case 'e':
			if (prec == -1)
				prec = DEFPREC;
			_double = va_arg(argp, double);
			t = fcvt(_double, prec + 1, &decpt, &sign);
gise:			bp = buf;
			*bp++ = *t ? *t++ : '0';
			if (alternate || prec > 0)
				*bp++ = '.';
			while (prec--)
				*bp++ = *t ? *t++ : '0';
			*bp++ = *fmt;
			if (decpt > 0 || !_double) {
				*bp++ = '+';
				--decpt;
			}
			else {
				*bp++ = '-';
				decpt = -decpt + 1;
			}
			/* exponents <= 99 in ANSI X3J11 */
			*bp++ = (int)(decpt / 10) + '0';
			*bp++ = (int)(decpt % 10) + '0';
			goto pbuf;
		case 'f':
			if (prec == -1)
				prec = DEFPREC;
			_double = va_arg(argp, double);
			t = fcvt(_double, prec + 1, &decpt, &sign);
gisf:			bp = buf;
			if (decpt >= 0)
				for (;;) {
					*bp++ = *t ? *t++ : '0';
					if (!--decpt)
						break;
				}
			if (alternate || prec > 0) {
				if (decpt < 0)
					*bp++ = '0';
				*bp++ = '.';
			}
			while (decpt++) {
				*bp++ = '0';
				--prec;
			}
			while (prec--)
				*bp++ = *t ? *t++ : '0';
pbuf:			size = bp - buf;
			if (sign || printsign)
				PUTC(sign ? '-' : printsign, fp);
			if (size < width && !ladjust)
				do {
					PUTC(padc, fp);
				} while (--width > size);
			for (t = buf; t < bp; ++t)
				PUTC(*t, fp);
			for (; width > size; --width)
				PUTC(padc, fp);
			break;
		case 'G':
		case 'g': {
			int gotoe;

			if (prec == -1)
				prec = DEFPREC;
			_double = va_arg(argp, double);
			t = fcvt(_double, prec + 1, &decpt, &sign);
			gotoe = decpt > prec;
			if (!alternate) {
				for (bp = t + prec + decpt; prec &&
				    *--bp == '0'; --prec);
			}
			if (gotoe || decpt < -3) {
				*fmt -= 2;
				goto gise;
			}
			--*fmt;
			goto gisf;
		}
		case 'n':
			*(va_arg(argp, int *)) = cnt;
			break;
		case 'o':
			GETARG(reg_ulong);
			base = 8;
			if (!reg_ulong || !alternate)
				goto num1;
			bp = buf + sizeof(buf) - 1;
			do {
				*bp-- = digs[reg_ulong % base];
				reg_ulong /= base;
			} while(reg_ulong);
			size = &buf[sizeof(buf) - 1] - bp;
			if (size < --width && !ladjust)
				do {
					PUTC(padc, fp);
				} while (--width > size);
			PUTC('0', fp);
			goto num2;
			break;
		case 'p':
		case 's':
			if (!(bp = va_arg(argp, char *)))
				bp = "(null)";
			if (width > 0 && !ladjust) {
				char *savep;

				savep = bp;
				for (n = 0; *bp && (prec < 0 || n < prec);
				    n++, bp++);
				bp = savep;
				while (n++ < width)
					PUTC(' ', fp);
			}
			for (n = 0; *bp; ++bp) {
				if (++n > prec && prec >= 0)
					break;
				PUTC(*bp, fp);
			}
			if (n < width && ladjust)
				do {
					PUTC(' ', fp);
				} while (++n < width);
			break;
		case 'u':
			GETARG(reg_ulong);
			base = 10;
			goto num1;
		case 'X':
			digs = "0123456789ABCDEF";
			/*FALLTHROUGH*/
		case 'x':
			GETARG(reg_ulong);
			if (alternate && reg_ulong) {
				PUTC('0', fp);
				PUTC(*fmt, fp);
			}
			base = 16;
num1:			bp = buf + sizeof(buf) - 1;
			do {
				*bp-- = digs[reg_ulong % base];
				reg_ulong /= base;
			} while(reg_ulong);
			size = &buf[sizeof(buf) - 1] - bp;
			for (; size < prec; *bp-- = '0', ++size);
			if (size < width && !ladjust)
				do {
					PUTC(padc, fp);
				} while (--width > size);
num2:			while (++bp != &buf[MAXBUF])
				PUTC(*bp, fp);
			for (; width > size; --width)
				PUTC(padc, fp);
			break;
		case '\0':		/* "%?" prints ?, unless ? is NULL */
			return(ferror(fp) ? -1 : cnt);
		default:
			PUTC(*fmt, fp);
		}
	}
	return(ferror(fp) ? -1 : cnt);
}
