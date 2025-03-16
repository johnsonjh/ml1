/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module MD - Machine dependent logic                              *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */

/*
 * Edit History:
 *
 * 1.1  --  First version in C.
 * 2.1  --  Added facility to rewind input streams.
 * 2.2  --  Generalised number of output streams; now selected by MAXOUT.
 * 3.0  --  Machine dependent version change for machine independent version
 *          change to CKA.
 * 3.1  --  Change to make S18 into a bit mask controlling all end of
 *          process messages.
 * 3.2  --  Added support for Zortech C, and function prototypes. Some
 *          type changes and casting added.
 * 3.3  --  Added support for VAXC under VMS.
 * 4.0  --  Added support for Microsoft C under OS/2 and DOS, and added ANSI
 *          C support; removed ICL Perq and Lattice C support.
 * 4.1  --  Changes to add S24 (start of line flags for output
 *          streams).
 *      --  Correction to suppress output on unassigned streams.
 * 4.2  --  Include for 'stdio.h' moved to ml1.h so that NULL is available
 *          to all modules.
 *      --  Correction to Microsoft C code for handling argv[0].
 * 4.3  --  Update for version CKF of MI-logic.
 * 4.4  --  Update for version CKG of MI-logic.
 *          Include for 'stdio.h' removed from md.c.
 *          Update to handle IBM C Set/2.
 * 4.5  --  Removed illegal character check on IBM PC implementations, since
 *          all values are legal characters.
 * 4.6  --  Machine dependent version change for machine independent version
 *          change to CKI.
 * 4.7  --  Removed check for low valued character codes (so that all
 *          character code values between 0 and 255 are accepted) on
 *          BSD4 version.
 * 4.8  --  Machine dependent version change for machine independent version
 *          change to CKL.
 * 4.9  --  Update to handle Microsoft Windows (32 bit); machine independent
 *          version changed to CKM.
 * 4.10 --  Update to handle gcc on FreeBSD and possibly others; machine
 *          independent version changed to CKN.
 * 4.11 --  Update to handle clang on FreeBSD; machine independent version
 *          changed to CKO.
 * 4.12 --  Update to handle 64 bit FreeBSD; machine independent version
 *          changed to CKP.
 *
 */

#include "ml1.h"
#if	VMS
#include <ssdef.h>
#endif


/* Use of S-variables
   ------------------

S1-S9 are used by the machine-independent logic
-----------------------------------------------

  S1   -  0 => Don't insert startlines on input
          1 => Insert startlines on input
  S2   -  Source text line number
  S3   -  0 => Give error message if warning marker not followed by macro name
          1 => Don't give error message if warning marker not followed by macro name
  S4   -  0 => Don't suppress context print-out on MC-NOTE call
          1 => Suppress context print-out on MC-NOTE call
  S5   -  Count of processing errors
  S6   -  Character to be treated as alphanumeric
  S7   -  Not used
  S8   -  Not used
  S9   -  Not used

S10 upwards are used by the machine-dependent logic
---------------------------------------------------

  S10  -  Input switch:
          0 => end of process
          1 -> MAXIN selects stream
          101 -> 100+MAXIN selects and rewinds stream
  S11  -  Not used
  S12  -  Remaining line quota on debugging file
  S13  -  Not used
  S14  -  Not used
  S15  -  Not used
  S16  -  Character to be translated on input
  S17  -  Character to replace S16
  S18  -  Bit 2**0 = 0 => Don't give environment printout at end of process
          Bit 2**0 = 1 => Give environment printout at end of process
          Bit 2**1 = 0 => Don't output end of process statistics
          Bit 2**1 = 1 => Give end of process statistics
  S19  -  Line number of output text
  S20  -  0 => Don't list output text
          1 => List output text without line numbers
          2 => List output text with line numbers
  S21  -  0        => Don't send output at all
          Bit 2**0 => Send output to output stream 1
          Bit 2**1 => Send output to output stream 2
          Bit 2**2 => Send output to output stream 3
          Bit 2**3 => Send output to output stream 4
  S22  -  0 => Don't send output to output stream 2
          1 => Send output to output stream 2
  S23  -  Reverted input switch
  S24  -  0        => No stream at start of line
          Bit 2**0 => Output stream 1 at start of line
          Bit 2**1 => Output stream 2 at start of line
          Bit 2**2 => Output stream 3 at start of line
          Bit 2**3 => Output stream 4 at start of line

*/


/*** Machine dependent definitions ***/

#if	ATT3B | BSD4 | FBSD32| FBSD64 | IBMC | L1 | MSC | VMS | WIN
#define	PROGNAME	progname	/* Present OK on these systems */
#endif
#if	ZTC | ZTCX
#define	PROGNAME	"ml1"	/* Name not known */
#endif
#define	MAXIN		5	/* Maximum number of input files */
#define	MAXOUT		4	/* Maximum number of output files */
#define	ERRWIDTH	72	/* Maximum length of line in debugging file */
#define	DEFWORK		5000	/* Default workspace size */
#define	ERRQUOT		500	/* Default lines limit on debugging file */
#define	ERRCHAR		'?'	/* Substituted for illegal input characters */


/*** Version number and system description ***/

#define MDVERSION	"4.12"		/* Version number of machine dependent logic */

#if	ATT3B
#define	MDSYSTEM	"AT&T 3B"	/* System on which program runs */
#endif
#if	BSD4
#define	MDSYSTEM	"4.3BSD"	/* System on which program runs */
#endif
#if	FBSD32
#define	MDSYSTEM	"FreeBSD (32 bit)" /* System on which program runs */
#endif
#if	FBSD64
#define	MDSYSTEM	"FreeBSD (64 bit)" /* System on which program runs */
#endif
#if	IBMC
#define	MDSYSTEM	"OS/2"		/* System on which program runs */
#endif
#if	L1
#define	MDSYSTEM	"Olivetti L1"	/* System on which program runs */
#endif
#if	MSC
#define	MDSYSTEM	"OS/2 and DOS"	/* System on which program runs */
#endif
#if	VMS
#define	MDSYSTEM	"VMS"		/* System on which program runs */
#endif
#if	WIN
#define	MDSYSTEM	"Windows"	/* System on which program runs */
#endif
#if	ZTC
#define	MDSYSTEM	"IBM PC"	/* System on which program runs */
#endif
#if	ZTCX
#define	MDSYSTEM	"DOS extended" /* System on which program runs */
#endif


/*** Variables private to the machine dependent logic ***/

static	INT	errchs;		/* Count of characters on current line of debugging file */
static	INT	nlpend;		/* TRUE if last input character was a newline, otherwise FALSE */
static	FILE	*in_fps[MAXIN+1];/* Vector of input file pointers */
static	FILE	*out_fps[MAXOUT+1];/* Vector of output file pointers */
static	FILE	*list_fp;	/* File pointer for listing file */
static	FILE	*err_fp;	/* File pointer for debugging file */
#if	ATT3B | BSD4 | FBSD32 | FBSD64 | IBMC | L1 | MSC | VMS | WIN
static	char	*progname;	/* Pointer to argv[0] */
#endif


/*** External references to system interface ***/

#if	ANSI
#include <stdlib.h>
#include <string.h>
#endif
#if	ZTC
#include <io.h>
#endif
#if	!ANSI
extern		exit();
extern	FILE	*fopen();
extern		fprintf();
extern	int	fseek();
#if	L1
extern	char	*malloc();
#endif
extern	char	*sbrk();
extern	int	strlen();
#endif

/*** Forward references ***/

#if	ANSI
static	void	eputc(char);
static	void	fail(char *,...);
static	void	listing_error(void);
static	void	mdabort(void);
static	FILE	*openio(char *,char *);
static	void	write_error(INT);
#else
static	VOID	eputc();
static	VOID	fail();
static	VOID	listing_error();
static	VOID	mdabort();
static	FILE	*openio();
static	VOID	write_error();
#endif


#if	VMS
int main(argc,argv)
int	argc;
char	*argv[];
#endif
#if	ATT3B | BSD4 | L1
VOID main(argc,argv)
int	argc;
char	*argv[];
#endif
#if	MSC | WIN | ZTC | ZTCX
VOID main(int argc,char *argv[])
#endif
#if	FBSD32 | FBSD64 | IBMC
int main(int argc,char *argv[])
#endif
/* Main initialisation routine. Reads and decodes the parameter string,
opens files, and obtains workspace. */
{	INT worksize = DEFWORK;
	INT *workad;
	int in_count = 0;
	int out_count = 0;
	int i;
	INT res;
	int argp = 1;
#if	MSC | WIN
	char *ptr;
#endif

#if	ATT3B | BSD4 | FBSD32 | FBSD64 | IBMC | L1 | VMS
	progname = argv[0];
#endif
#if	MSC | WIN
	progname = argv[0];
	ptr = strrchr(progname, '\\');
	if(ptr != (char *) NULL) progname = ++ptr;
	ptr = strrchr(progname, '.');
	if(ptr != (char *) NULL) *ptr = '\0';
	for(i = 0; progname[i] != '\0'; i++)
		progname[i] = (char) tolower(progname[i]);
#endif

	list_fp = (FILE *) NULL;
	err_fp = stderr;			/* Default setting */

	/* Set up I/O files from parameters */

	for(i = 0; i <= MAXIN; i++) in_fps[i] = (FILE *) NULL;
						/* 'in_fps[0]' must be NULL */
	for(i = 0; i <= MAXOUT; i++) out_fps[i] = (FILE *) NULL;

	while(argp < argc) {
		char *s = argv[argp++];

#if	IBMC | MSC | WIN
		if((strlen(s) > 1) && (*s == '-' || *s == '/')) {
#endif
#if	ATT3B | BSD4 | FBSD32 | FBSD64 | L1 | VMS | ZTC | ZTCX
		if((strlen(s) > 1) && (*s == '-')) {
#endif
			for(;;) {
				char ch = *s++;

				if(ch == '\0') break;
				switch(ch) {
					case  '-':
					case  '/':
					case  ' ':	break;

					case  'V':
					case  'v':	fprintf(stderr,"%s: %s version %s (%s)\n",
								PROGNAME,MDSYSTEM,MDVERSION,MIVERSION);
							break;

					case  'W':
					case  'w':	worksize = 0;
							while(isdigit((ch = *s++))) {
								worksize = worksize*10 + (ch - '0');
							}
#if	ATT3B | BSD4 | FBSD32 | FBSD64 | IBMC | MSC | VMS | WIN | ZTC
							if(worksize < 200) {
#endif
#if	L1
							if(worksize < 200L || worksize > 12000L) {
							/* Max needed in case of segment wrapround */
#endif
#if	ZTCX
							if(worksize < 200) {
#endif
								fail("bad value for 'w' flag");
							}
							s--;	/* Backspace to re-read terminator */
							break;

					case  'L':
					case  'l':	if(argp >= argc) fail("no file for 'l' flag");
							list_fp = openio(argv[argp++],"w");
							break;

					case  'D':
					case  'd':	if(argp >= argc) fail("no file for 'd' flag");
							err_fp = openio(argv[argp++],"w");
							break;

					case  'O':
					case  'o':	if(argp >= argc) fail("no file for 'o' flag");
							if(out_count >= MAXOUT) fail("too many output files");
							out_fps[++out_count] = openio(argv[argp++],"w");
							break;

					default  :	fail("flag '%c' not recognised",ch);
				}
			}
		} else {
			if(in_count >= MAXIN) fail("too many input files");
			in_fps[++in_count] = openio(s,"r");
		}
	}

	/* Ensure that at least one input file and one output file are
	in use */

	if(in_fps[1] == (FILE *) NULL) in_fps[1] = stdin;
	if(out_fps[1] == (FILE *) NULL) out_fps[1] = stdout;

#if	ATT3B | BSD4
	workad = (INT *) sbrk(((int) worksize)*sizeof(INT));
	if(workad == ((INT *) -1)) fail("cannot get workspace");
#endif
#if	FBSD32 | FBSD64 | IBMC | L1 | MSC | VMS | WIN | ZTC | ZTCX
	workad = (INT *) malloc(((unsigned) worksize)*sizeof(INT));
	if(workad == ((INT *) NULL)) fail("cannot get workspace");
#endif

	errchs = 0;

	/* Pass control to the machine-independent logic */

	res = milogic(workad,worksize);	/* Address and size, in words */

	/* Close all files. Not strictly necessary as 'exit' is supposed
	to do this, but some systems might not bother... */

	for(i = 1; i <= MAXIN; i++) {
		FILE *fp = in_fps[i];

		if(fp != (FILE *) NULL) fclose(fp);
	}
	for(i = 1; i <= MAXOUT; i++) {
		FILE *fp = out_fps[i];

		if(fp != (FILE *) NULL) fclose(fp);
	}
	if(list_fp != (FILE *) NULL) fclose(list_fp);
	fclose(err_fp);

#if	VMS
	exit(res == 0 ? SS$_NORMAL: SS$_ABORT);
#endif
#if	ATT3B | BSD4 | L1
	exit(res == 0 ? 0: 254);
#endif
#if	FBSD32 | FBSD64 | MSC | WIN | ZTC | ZTCX
	exit(res == 0 ? EXIT_SUCCESS: EXIT_FAILURE);
#endif
#if	IBMC
	return(res == 0 ? EXIT_SUCCESS: EXIT_FAILURE);
#endif
}


/* VARARGS1 */

#if	ANSI
static VOID fail(char *mes,...)
/* Print the error message 'mes', with optional arguments. */
{	va_list ap;

	fprintf(stderr,"%s: ",PROGNAME);

	va_start(ap,mes);
	vfprintf(stderr,mes,ap);
	va_end(ap);

	fputc('\n',stderr);

	exit(EXIT_FAILURE);
}
#else
static VOID fail(mes,a,b,c)
char	*mes;
INT	a,b,c;
/* Print the error message 'mes', with optional arguments. */
{	fprintf(stderr,"%s: ",PROGNAME);
	fprintf(stderr,mes,a,b,c);
	fputc('\n',stderr);
	exit(255);
}
#endif


#if	ANSI
static FILE *openio(char *name,char *mode)
#else
static FILE *openio(name,mode)
char	*name;
char	*mode;
#endif
/* Try to open the file 'name' in mode 'mode', only returning if successful.
The name '-' is treated as standard input or standard output, as appropriate.
*/
{	FILE *fp;

	if((name[0] == '-') && (name[1] == 0)) {
		return(mode[0] == 'r' ? stdin: stdout);
	}

	fp = fopen(name,mode);
	if(fp == (FILE *) NULL) fail("cannot open '%s'",name);

	return(fp);
}


/* VARARGS1 */

#if	ANSI
VOID mderpr(char *format,...)
/* Standard ML/I machine-dependent routine; used to output information
to the debugging file. Handles wrap-round of long lines of output, and
enforces the lines limit controlled by the value of S12. */
{	char s[100];
	unsigned i;
	va_list ap;

	va_start(ap,format);
	vsprintf(s,format,ap);
	va_end(ap);

	for(i = 0; i < strlen(s); i++) eputc(s[i]);
}
#else
VOID mderpr(format,a,b,c,d)
char	*format;
INT	a,b,c,d;
/* Standard ML/I machine-dependent routine; used to output information
to the debugging file. Handles wrap-round of long lines of output, and
enforces the lines limit controlled by the value of S12. */
{	char s[100];
	int i;

	sprintf(s,format,a,b,c,d);

	for(i = 0; i < strlen(s); i++) eputc(s[i]);
}
#endif


#if	ANSI
static VOID eputc(char c)
#else
static VOID eputc(c)
char	c;
#endif
/* Outputs the character 'c' to the debugging file. Handles wrap-round
of long lines of output, and enforces the lines limit controlled by
the value of S12. */
{	if(c == '\n') {
		errchs = 0;
		if(--*at_s12 < 0) {
			*at_s12 = MAXINT;	/* To allow environment dump, etc. */
			eputc('\n');
			prerr();
			mderpr("\nDebugging file lines quota exhausted\n");
			mdabort();
		}
	} else {
		if(errchs == ERRWIDTH) {
			eputc('\n');
			errchs = 0;
		}
		errchs++;
	}
	fputc(c, err_fp);
}


VOID mderid()
/* Prints the identifier specified by 'idpt' and 'idlen' to the debugging file.
*/
{	int i;
	int c;

	for(i = 0; i < (int) idlen; i++) {
		c = (int) idpt[i];

		if(c == SLREP) mderpr("(SL)"); else eputc((char) c);
	}
}


static VOID mdabort()
/* Routine called to abort the process after a fatal error detected by
the machine-dependent logic. Prints a standard error message, then
enters normal shutdown sequence. */
{	mderpr("\nProcess aborted due to above error\n");
	mihalt();
}


INT *mdfind()
/* Standard ML/I machine-dependent function; the main hash function.
'idpt' points to the identifier to be hashed (an unpacked string).
'idlen' is its length. This version assumes LHV is a power of two. */
{	INT r = idlen;

	if(r > 2) r += idpt[2];
	r += idpt[0];

	offset = ((r*113) >> 1) & (LHV-1);
	return(sdb.hashpt + offset);
}


INT mdread()
/* The main input routine. Handles input switching (using S10) and
input translation (using S16 and S17). */
{	INT s10 = *at_s10;		/* Make copy for speed */
	INT c;
	int rewflag,ret;
	FILE *fp;

	if(s10 == 0) return(EOFCH);		/* User forced end of file */

	/* Handle legal values of S10 greater than 100. These indicate that 100
	should be subtracted from the value of S10, and the result used to
	select the input stream. Additionally, the stream is rewound
	(repositioned at its start). */

	if(101 <= (int) s10 && (int) s10 <= MAXIN + 100) {
		s10 -= 100;
		*at_s10 = s10;
		rewflag = TRUE;
	} else rewflag = FALSE;

	/* Check for values of S10 outside the permitted range. Illegal
	values of S10 are replaced by zero; this is trapped because
	in_fps[0] is NULL. */

	if((s10 < 1) || (s10 > MAXIN)) s10 = 0;

	fp = in_fps[s10];
	if(fp == (FILE *) NULL) {
		*at_s12 += 10;			/* Ensure this message comes out */
		prerr();
		mderpr("\nS10 has illegal value");
		mdconv(*at_s10);		/* Convert S10 value to identifier */
		prviz();
		mdabort();
	}

	/* Rewind the stream if requested */

	if(rewflag == TRUE) {
#if	ANSI
		ret = fseek(fp,0L,SEEK_SET);	/* Reposition at start */
#else
		ret = fseek(fp,0L,0);		/* Reposition at start */
#endif
		if(ret == -1) {			/* Error */
			*at_s12 += 10;		/* Ensure this message comes out */
			prerr();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
			mderpr("\nCannot rewind input stream %d\n",s10);
#endif
#if	FBSD64 | L1
			mderpr("\nCannot rewind input stream %ld\n",s10);
#endif
			mdabort();
		}
	}

	c = getc(fp);

	/* Action at end of file. If S10 is not the "revert file"
	(whose value is in S23) then input is switched to the revert
	file; otherwise the process is terminated. */

	if(c == EOF) {
		if(s10 == *at_s23) return(EOFCH);
		*at_s10 = *at_s23;
		c = mdread();
		return(c);
	}

	if(c == *at_s16) c = *at_s17;

#if	!BSD4 & !FBSD32 & !FBSD64 & !IBMC & !MSC & !WIN & !ZTC & !ZTCX
	if((c & 0x80) != 0) {
		ersic();			/* Illegal input character */
		c = ERRCHAR;			/* Substitute the 'error character' */
	}
#endif

	return(c);
}


#if	ANSI
VOID mdconv(INT value)
#else
VOID mdconv(value)
INT	value;
#endif
/* Converts 'value' to a minimum-width, unpacked character string,
pointed to by 'idpt'. The string is stored in the area 'convarea' which
is allocated by the machine-independent logic. The length of the string
is set into 'idlen'. */
{	int neg = FALSE;

	idpt = convarea;
	idlen = 0;

	if(value < 0) {
		value = -value;
		neg = TRUE;
	}

	if(value == 0) {
		*idpt = '0';
		idlen = 1;
		return;
	} else {
		for(;;) {
			*idpt-- = value%10 + '0';
			idlen++;
			value /= 10;
			if(value == 0) break;
		}
	}

	if(neg) {
		*idpt = '-';
		idlen++;
	} else idpt++;
}


INT mdnum()
/* Converts the unpacked string pointed at by 'idpt' into a decimal
number. 'idlen' gives the length of the string. Yields FALSE if
'idpt' does not point at a valid digit, otherwise generally yields
TRUE. However, in the case where 'idpt' points at a valid digit but
succeeding characters up to 'sdb.spt' are not all valid digits, 'erlia()'
is called. */
{	INT c = *idpt;
	INT *i;

	meval = 0;
	if(!isdigit(c)) return(FALSE);

	for(i = idpt; i <= sdb.spt; i++) {
		c = *i;
		if(!isdigit(c)) erlia();
		meval = meval*10 + c - '0';
	}
	return(TRUE);
}


VOID mdinit()
/* Routine to perform any machine-dependent initialisation which cannot
be done until machine-independent initialisation has been completed. */
{	nlpend = TRUE;

	/* Obtain addresses of commonly used system variables.
	This is to improve efficiency. */

	at_s10 = svarpt - 10;
	at_s12 = svarpt - 12;
	at_s16 = svarpt - 16;
	at_s17 = svarpt - 17;
	at_s19 = svarpt - 19;
	at_s20 = svarpt - 20;
	at_s21 = svarpt - 21;
	at_s22 = svarpt - 22;
	at_s23 = svarpt - 23;
	at_s24 = svarpt - 24;

	/* Initialise system variables */

	*at_s10 = 1;				/* Initial input from first input file */
	*at_s12 = ERRQUOT;			/* Initial lines limit on debugging file */
	*at_s16 = -1;				/* No input translation */
	*at_s21 = 1;				/* Initial output only to primary output */
	*at_s23 = 1;				/* Reverted input file is initially first input file */
	*at_s24 = (1 << MAXOUT) - 1;		/* Start of line flags */
	svarpt[-18] = 0;			/* Print end of process messages only on demand */
}


VOID mdfinal()
/* Routine to perform machine-dependent finalisation. */
{	*at_s12 = MAXINT;			/* Ensure statistics are output OK */

	if((svarpt[-18] & 1) != 0) prenv();
}


#if	ANSI
VOID fmove(INT len,INT *src,INT *dest)
#else
VOID fmove(len,src,dest)
INT	len;
INT	*src,*dest;
#endif
/* Moves 'len' INTs from 'src' to 'dest', starting at 'src' and 'dest'.
*/
{	INT i;

	for(i = 0; i < len; i++) dest[i] = src[i];
}


#if	ANSI
VOID bmove(INT len,INT *src,INT *dest)
#else
VOID bmove(len,src,dest)
INT	len;
INT	*src,*dest;
#endif
/* Moves 'len' words from 'src' to 'dest', starting at 'src+len-1' and
'dest+len-1'. */
{	int i;

	for(i = (int) len - 1; i >= 0; i--)
		dest[i] = src[i];
}


#if	ANSI
VOID mdouch(INT c)
#else
VOID mdouch(c)
INT	c;
#endif
/* Main output routine. Handles multi-file output, and output listing
if required. */
{	INT s20 = *at_s20;
	INT s21 = *at_s21;
	INT s24 = *at_s24;

	if(nlpend) {
		(*at_s19)++;			/* Update output line number */
		if((list_fp != (FILE *) NULL) && (s20 == 2)) {
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
			fprintf(list_fp,"%5d.   ",*at_s19);
#endif
#if	FBSD64 | L1
			fprintf(list_fp,"%5ld.   ",*at_s19);
#endif
		}
	}

	nlpend = (c == '\n');

	if((list_fp != (FILE *) NULL) && (s20 != 0)) {
		if(fputc((int) c, list_fp) == EOF) listing_error();
	}

	if(s21 != 0) {
		INT i;

		for(i = 1; i <= MAXOUT; i++) {
			INT bit = 1 << (i-1);

			if((s21 & bit) != 0 && out_fps[i] != (FILE *) NULL) {
				if(fputc((int) c, out_fps[i]) == EOF)
					write_error(i);
				if(c == '\n') s24 |= bit;
				else s24 &= (~bit);
			}
		}
	}

	if(*at_s22 != 0 && (s21 & 2) == 0) {
		if(fputc((int) c, out_fps[2]) == EOF)
					write_error(2);
		if(c == '\n') s24 |= 2;
		else s24 &= (~2);
	}
	*at_s24 = s24;
}


#if	ANSI
static VOID write_error(INT file_number)
#else
static VOID write_error(file_number)
INT	file_number;
#endif
/* Routine called after a failure to write to one of the output files.
A suitable message is sent to the debugging file, and the process is
aborted. */
{	*at_s12 += 10;				/* Ensure this message comes out */
	prerr();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("\nError while writing to output stream %d\n",file_number);
#endif
#if	FBSD64 | L1
	mderpr("\nError while writing to output stream %ld\n",file_number);
#endif
	mdabort();
}


static VOID listing_error()
/* Routine called after a failure to write to the listing file.
A suitable message is sent to the debugging file, and the process is
aborted. */
{	*at_s12 += 10;				/* Ensure this message comes out */
	prerr();
	mderpr("\nError while writing to listing file\n");
	mdabort();
}


#if	ANSI
INT mddiv(INT a,INT b)
#else
INT mddiv(a,b)
INT	a,b;
#endif
/* Divides 'a' by 'b' according to the (somewhat strange) rules in Section
2.6.3 of the User's Manual. 'b' can never be zero; this is detected by
the machine-independent logic. */
{	int neg = FALSE;
	INT res;

	if(a < 0) {
		a = -a;
		neg = TRUE;
	}

	if(b < 0) {
		b = -b;
		neg = (int) (neg ? FALSE: TRUE);
	}

	res = a/b;

	/* Round result towards minus infinity */

	if(neg && (a%b != 0)) res++;

	return(neg ? -res: res);
}


#if	DEBUGGING
#if	ANSI
static	VOID	mddump(INT *,INT *);
#else
static	VOID	mddump();
#endif
/* Routine called on a program error or abort, to dump the ML/I stacks. */
VOID mdpostmort(VOID)
{	INT *n;

#if	MSC | ZTC
	fprintf(stderr,"\n\nBackwards stack:   (lfpt = %4x)\n",(INT) lfpt);
#endif
#if	ATT3B | FBSD32 | IBMC | BSD4 | VMS | WIN | ZTCX
	fprintf(stderr,"\n\nBackwards stack:   (lfpt = %8x)\n",(INT) lfpt);
#endif
#if	FBSD64 | L1
	fprintf(stderr,"\n\nBackwards stack:   (lfpt = %8lx)\n",(INT) lfpt);
#endif
	n = lfpt - 30;
	if(n < ffpt) n = ffpt;
	mddump(n,endpt);
#if	MSC | ZTC
	fprintf(stderr,"\n\nForwards stack:    (ffpt = %4x)\n",(INT) ffpt);
#endif
#if	ATT3B | BSD4 | FBSD32 | IBMC | VMS | WIN | ZTCX
	fprintf(stderr,"\n\nForwards stack:    (ffpt = %8x)\n",(INT) ffpt);
#endif
#if	FBSD64 | L1
	fprintf(stderr,"\n\nForwards stack:    (ffpt = %8lx)\n",(INT) ffpt);
#endif
	n = ffpt + 30;
	if(n > lfpt) n = lfpt;
	mddump(stffpt,n);
	fputc('\n', stderr);
}


#if	ANSI
static VOID mddump(INT *strt,INT *fin)
#else
static VOID mddump(strt,fin)
INT	*strt;
INT	*fin;
#endif
/* Routine used by 'postmort'. */
{	INT online = 8;
	INT *i;

	for(i = strt; i <= fin; i++) {
		if(online == 8) {
			online = 0;
#if	MSC | ZTC
			fprintf(stderr,"\n%4x:  ",(int) i);
#endif
#if	ATT3B | BSD4 | FBSD32 | IBMC | VMS | WIN | ZTCX
			fprintf(stderr,"\n%8x:  ",(int) i);
#endif
#if	FBSD64 | L1
			fprintf(stderr,"\n%8lx:  ",(INT) i);
#endif
		}
#if	MSC | ZTC
		fprintf(stderr,"  %4x",*i);
#endif
#if	ATT3B | BSD4 | FBSD32 | IBMC | VMS | WIN | ZTCX
		fprintf(stderr,"  %8x",*i);
#endif
#if	FBSD64 | L1
		fprintf(stderr,"  %8lx",*i);
#endif
		online++;
	}
}
#endif

/*
 ***********************
 *                     *
 *   End of module MD  *
 *                     *
 ***********************
 */

