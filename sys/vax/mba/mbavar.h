/*	mbavar.h	3.2	10/14/12	*/

/*
 * VAX Massbus adapter registers
 */

struct mba_regs
{
	int	mba_csr;
	int	mba_cr;
	int	mba_sr;
	int	mba_var;
	int	mba_bcr;
};

/*
 * NOTE:
 *	mba_erb at displacement 0x400
 *	mba_as at displacement 0x410
 *	mba_map at displacement 0x800
 */

#define MBA0		0x80064000
#define MBA1		0x80066000

#define MBA_ERB		0x400
#define MBA_MAP		0x800

#define MBA0_MAP	(MBA0 + 0x800)
#define MBA1_MAP	(MBA1 + 0x800)

#define MBAEBITS	0xe0770

#define MBAIE		0x4
