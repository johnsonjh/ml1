/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 1 - Machine independent initialisation and finalisation   *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


/*** Copyright notices ***/

#if	FBSD32 | FBSD64
#pragma	clang diagnostic push
#pragma	clang diagnostic ignored "-Wunused-const-variable"
#endif
static CONST char copyr1[] = "Copyright (C) R.D. Eager MMXVIII\n";
static CONST char copyr2[] = "Copyright (C) P.J. Brown MCMLXVII\n";
#if	FBSD32 | FBSD64
#pragma	clang diagnostic pop
#endif


/*** Global variables ***/


/*** Blocks of variables whose relative positions must not be changed ***/

struct	sdbf	sdb;			/* Scanning description block */
struct	opdbf	opdb;			/* Operation macro description block */
struct	oabf	oab;			/* OPT-ALL block */


/*** Variables requiring dynamic initialisation ***/

INT	*endpt;				/* Points at end of backwards stack */
INT	glbwsw;				/* Global warning switch */
INT	*lfpt;				/* Points at last used location on backwards stack */
INT	*pvarpt;			/* Points at permanent variables */
INT	pvnum;				/* Number of permanent variables */
#if	CVARS
INT	*cvarpt;			/* Points at character string variables */
INT	cvnum;				/* Number of character string variables */
INT	cvsize;				/* Size of each character string variable */
INT	exprsw;				/* Returns expression type from 'gmeadd' */
#endif


/*** Miscellaneous variables ***/

INT	beslin;				/* Best-so-far value of 'sdb.linect' */
INT	*bespt;				/* Best-so-far value of 'sdb.spt' */
INT	bestpl;				/* Switch value used in basic scan routine */
INT	*bfndpt;			/* Best-so-far value of 'fndpt' */
INT	bindic;				/* Best-so-far value of 'indic' */
INT	*binfpt;			/* Best-so-far value of 'infopt' */
INT	caltyp;				/* First number in information block */
INT	*chanpt;			/* Used in chaining */
INT	chlink;				/* Used in chaining */
INT	*cllfpt;			/* Points at top entry after scanning information is stacked */
INT	copdsw;				/* For skips; reflects setting of delimiter option */
INT	coptsw;				/* For skips; reflects setting of text option */
INT	*delpt;				/* Head of chain of delimiters searching for */
INT	*eriapt;			/* Points at value of operation macro argument */
INT	*ffpt;				/* Points to first free location on forwards stack */
INT	*htabpt;			/* Points at current hash table */
INT	idlen;				/* Length of current identifier */
INT	*idpt;				/* Points at current identifier */
INT	indic;				/* Contents of nextlink */
INT	*infopt;			/* Points beyond currently matched LID */
INT	invoct;				/* Count of macro calls */
INT	*knpt;				/* Points to node marker entry in delimiter chain */
INT	levl;				/* Level of macros and inserts */
INT	masksw;				/* Used to indicate which types of construction are recognised */
INT	meval;				/* Miscellaneous; used for numerical values calculated at macro time */
INT	*nargpt;			/* Points at argument vector + 1 */
INT	nestlv;				/* Nesting level of calls and skips during scanning */
INT	offset;				/* Offset in hash table */
INT	oplev;				/* Level of operation macros and inserts */
INT	skiplv;				/* Level of skip nesting */
INT	*stffpt;			/* Points at start of global macros */
INT	*svarpt;			/* Points at system variables */
INT	*tempt;				/* Temporary */
INT	tlinct;				/* Temporary storage for 'sdb.linect' */


/*** Variables used in processing structure representations ***/

INT	consw;				/* For syntax checking ('evtree' and 'getdel') */
INT	delct;				/* Count of delimiters */
INT	exitsw;				/* For syntax checking ('evtree') */
INT	keysw;				/* For syntax checking ('evtree' and 'getdel') */
INT	*lnodpt;			/* Points at topmost node entry on backwards stack */
INT	*ndefpt;			/* Destination of newly defined construction */
INT	*nnodpt;			/* Points at current node entry on backwards stack */
INT	*nodept;			/* Head of chain of links to be attached to next delimiter */
INT	nodesw;				/* For syntax checking ('evtree' and 'getdel') */
INT	*ollfpt;			/* Previous value of 'lfpt' */
INT	optlev;				/* Level of OPT-ALL brackets */


/*** Variables used in processing macro expressions or inserts ***/

INT	*varpt;				/* Points at vector of variables */


/*** Names of table items - dynamically initialised ***/

INT	*das;				/* Head of AS/SSAS chain for MC-DEF */
INT	*delchn;			/* Chain of secondary delimiters */
INT	*dge;				/* Head of relation chain for MC-GO */
INT	*dif;				/* Head of IF/UNLESS chain for MC-GO */
INT	*ghshtb[LHV+5];			/* Global hash table */
INT	*keychn;			/* Head of keyword chain */
INT	*knrep;				/* Points at current node marker */
INT	*kspacs;			/* Points at SPACES keyword */
INT	*laychn;			/* Chain of names of layout characters */
INT	*spcsrp;			/* Points at representation of SPACE */


/*** More miscellaneous variables ***/

INT	*at_edb;			/* \                                                  */
INT	*at_sdb;			/* |__ Addresses of blocks of scanning data           */
INT	*at_opdb;			/* |                                                  */
INT	*at_all;			/* /                                                  */
INT	*at_s1;				/* \                                                  */
INT	*at_s2;				/* |__ Addresses of S-variables; for efficient access */
INT	*at_s5;				/* |                                                  */
#if	SPECAN
INT	*at_s6;				/* /                                                  */
#endif
INT	*convarea;			/* Pointer to number conversion area for 'mdconv' */
INT	nlsw;				/* TRUE if last character read was a newline, otherwise FALSE */


/*** Machine-dependent variables ***/

INT	*at_s10;			/* \                                                  */
INT	*at_s12;			/* |                                                  */
INT	*at_s16;			/* |                                                  */
INT	*at_s17;			/* |                                                  */
INT	*at_s19;			/* |-- Addresses of S-variables; for efficient access */
INT	*at_s20;			/* |                                                  */
INT	*at_s21;			/* |                                                  */
INT	*at_s22;			/* |                                                  */
INT	*at_s23;			/* |                                                  */
INT	*at_s24;			/*                                                    */

/*** Save areas for 'setjmp' calls ***/

jmp_buf	bssave;				/* Transfers control to 'bsnext' */
jmp_buf	bstsave;			/* Transfers control to 'bstrex' */
jmp_buf	entsave;			/* Transfers control to 'entext' */
jmp_buf	evpsave;			/* Transfers control to 'evopt' */
jmp_buf	evrsave;			/* Transfers control to 'evor' */
jmp_buf	evxsave;			/* Transfers control to 'evexit' */


/*** Local variables ***/

static	jmp_buf	exsave;			/* Transfers control to 'miexit' */
static	INT	svec[SVARNUM+1];	/* System variables (stored in reverse) */
static	INT	result;			/* Number of processing errors */


#if	ANSI
INT milogic(INT *workspace,INT size)
#else
INT milogic(workspace,size)
INT	*workspace;
INT	size;
#endif
/* Machine-independent initialisation code. */
{	INT i;

#if	IBMC
#pragma	checkout(suspend)
#endif
	sdb.hashpt = (INT *) &ghshtb[0];
#if	IBMC
#pragma	checkout(resume)
#endif

	/* Set up system variables, initialising to zero */

	svarpt = &svec[SVARNUM];		/* S-variables are stored backwards */
	svec[SVARNUM] = SVARNUM;
	for(i = 0; i < SVARNUM; i++) svec[i] = 0;
	at_s1 = svarpt - 1;			/* Address of S1 - for efficiency */
	at_s2 = svarpt - 2;			/* Address of S2 - for efficiency */
	at_s5 = svarpt - 5;			/* Address of S5 - for efficiency */
#if	SPECAN
	at_s6 = svarpt - 6;			/* Address of S6 - for efficiency */
#endif

	/* Initialise any non-zero system variables */

#if	SPECAN
	*at_s6 = -1;				/* Character code that will never match */
#endif

	/* Set up pointers to blocks of variables which will need to be
	moved around */

#if	IBMC
#pragma	checkout(suspend)
#endif
	at_sdb = (INT *) &sdb;
	at_opdb = (INT *) &opdb;
	at_all = (INT *) &oab;
	at_edb = (INT *) &sdb.stakpt;
#if	IBMC
#pragma	checkout(resume)
#endif

	/* Set up pointers to main workspace */

	stffpt = ffpt = workspace;
	endpt = lfpt = workspace + size;

#if	DEBUGGING
	{	INT *p = ffpt;

		while(p < endpt) *p++ = (INT) 0;
	}
#endif

	/* Allocate number conversion area */

	{	INT n = MAXINT;
		INT nsize = 1;			/* Allow for minus sign */

		while(n > 0) {
			n = n/10;
			nsize++;
		}
		bumpff(nsize);			/* Take space from forwards stack */
		convarea = ffpt - 1;		/* Global pointer to end of conversion area */
	}

	/* Reserve and zeroise permanent variables */

	for(i = 0; i < PVARNUM; i++) ffpt[i] = 0;
	bumpff((INT) (PVARNUM+1));
	pvarpt = ffpt - 1;
	*pvarpt = pvnum = PVARNUM;

#if	CVARS

	/* Set up initial pointer to character string variable area */

	bumpff((INT) 1);
	cvarpt = ffpt - 1;
	cvnum = 0;
	*cvarpt = cvnum;			/* None to start with (size not known yet) */
	cvsize = 0;				/* Until we know */
#endif

	glbwsw = 7;

	sdb.argpt = sdb.tvarpt = sdb.stoppt = sdb.labpt = sdb.stakpt = NULLPT;

	sdb.skval = levl = invoct = skiplv = nestlv = oplev = tlinct = 0;
	bindic = ENDCHN;
	sdb.linect = 1;
	sdb.dbugsw = DB_SOURCE;
	nlsw = TRUE;

	init_tables();				/* Initialise global hash table, and delimiter chains */

	mdinit();				/* Machine dependent initialisation */

	/* Set up initial hash table */

	declf((INT) 1);				/* Stack null pointer to 'next' hash table */
	*lfpt = (INT) NULLPT;
#if	IBMC
#pragma	checkout(suspend)
#endif
	sdb.hashpt = (INT *) &ghshtb[0];
#if	IBMC
#pragma	checkout(resume)
#endif
	stkhsh();

	/* Set initial limits on local constructions */

	for(i = 0; i <= 3; i++) sdb.hashpt[LHV + i] = (INT) endpt;
	sdb.hashpt[LHV + 4] = 7;

	knpt = knrep;
	sdb.spt = ffpt - 1;
	sdb.inffpt = ffpt;

	/* Set up exit closure */

	if(setjmp(exsave) != 0) goto miexit;

	/* Start scanning */

	basic_scan();

miexit:

	return(result);
}


VOID mihalt()
/*  Machine-independent  finalisation code.  Calls the machine-dependent
finalisation   code,   prints   statistics,   and   returns    to    the
machine-dependent  logic.   'result'  is  set  to  the  number  of  ML/I
processing errors.  */
{	INT lines;

	mdfinal();

	if((svarpt[-18] & 2) != 0) {
		lines = *at_s2;
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
		mderpr("\nAt end of process: %d line%s, %d call%s\n",lines,lines == 1 ? "": "s",invoct,invoct == 1 ? "": "s");
#endif
#if	FBSD64 | L1
		mderpr("\nAt end of process: %ld line%s, %ld call%s\n",lines,lines == 1 ? "": "s",invoct,invoct == 1 ? "": "s");
#endif
	}

	result = *at_s5;
	longjmp(exsave,1);
}

/*
 ***********************
 *                     *
 *   End of module 1   *
 *                     *
 ***********************
 */

