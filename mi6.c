/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 6 - Table initialisation code                             *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


/*** Forward references ***/

#if	ANSI
static	void	do_entry(INT **,INT **);
static	void	init_delchn(void);
static	void	init_ghshtb(void);
#else
static	VOID	do_entry();
static	VOID	init_delchn();
static	VOID	init_ghshtb();
#endif


/*** Table of operation macro names ***/

static	INT	nametab[] = {

#if	CVARS
	0,ENDCHN,6,'M','C','C','V','A','R',362,TY_MACRO,OPMK,MC_CVAR,
#endif
	0,ENDCHN,5,'M','C','D','E','F',262,TY_MACRO,LOCMK,MC_DEF,
	0,ENDCHN,6,'M','C','S','K','I','P',337,TY_MACRO,LOCMK,MC_SKIP,
	0,ENDCHN,5,'M','C','I','N','S',325,TY_MACRO,LOCMK,MC_INS,
	0,ENDCHN,6,'M','C','W','A','R','N',317,TY_MACRO,LOCMK,MC_WARN,

	0,ENDCHN,6,'M','C','D','E','F','G',211,TY_MACRO,OPMK,MC_DEF,
	0,ENDCHN,7,'M','C','S','K','I','P','G',285,TY_MACRO,OPMK,MC_SKIP,
	0,ENDCHN,6,'M','C','I','N','S','G',272,TY_MACRO,OPMK,MC_INS,
	0,ENDCHN,7,'M','C','W','A','R','N','G',263,TY_MACRO,OPMK,MC_WARN,

	0,ENDCHN,7,'M','C','N','O','D','E','F',ENDCHN,TY_MACRO,LOCMK,MC_NODEF,
	0,ENDCHN,8,'M','C','N','O','S','K','I','P',ENDCHN,TY_MACRO,LOCMK,MC_NOSKIP,
	0,ENDCHN,7,'M','C','N','O','I','N','S',ENDCHN,TY_MACRO,LOCMK,MC_NOINS,
	0,ENDCHN,8,'M','C','N','O','W','A','R','N',ENDCHN,TY_MACRO,LOCMK,MC_NOWARN,

	0,ENDCHN,5,'M','C','S','E','T',162,TY_MACRO,OPMK,MC_SET,
	0,ENDCHN,4,'M','C','G','O',111,TY_MACRO,OPMK,MC_GO,
	0,ENDCHN,7,'M','C','A','L','T','E','R',142,TY_MACRO,OPMK,MC_ALTER,
	0,ENDCHN,6,'M','C','N','O','T','E',155,TY_MACRO,OPMK,MC_NOTE,
	0,ENDCHN,6,'M','C','P','V','A','R',142,TY_MACRO,OPMK,MC_PVAR,

	0,ENDCHN,6,'M','C','L','E','N','G',WTHSMK,1,'(',116,TY_MACRO,OPMK,MC_LENG,
	0,ENDCHN,5,'M','C','S','U','B',WTHSMK,1,'(',91,TY_MACRO,OPMK,MC_SUB,

	0,ENDCHN,6,'M','C','S','T','O','P',98,TY_MACRO,LOCMK,MC_STOP,

	/* This is the table of secondary delimiters (for operation macros)
	and keywords. Each operation macro entry is linked to the chain of
	its secondary delimiters (implicitly: by the code for it). The
	keywords are linked via separate global pointers. The entire table
	has a separate chain running through it; this links each individual
	entry; this allows the operation macro MC-ALTER to locate and change
	a secondary delimiter or a keyword. */

	8,
	8,4,'V','A','R','S',2,
	6,
	6,2,'A','S',81,
	8,
	ENDCHN,4,'S','S','A','S',73,

	6,
	6,2,'I','F',12,
	10,
	65,6,'U','N','L','E','S','S',2,
	6,
	6,2,'G','E',51,
	6,
	6,2,'G','R',45,
	6,
	6,2,'E','N',39,
	6,
	6,2,'B','C',33,
	5,
	ENDCHN,1,'=',28,

	6,
	ENDCHN,2,'T','O',22,
	5,
	ENDCHN,1,',',2,
	5,
	ENDCHN,1,',',2,
	5,
	ENDCHN,1,')',ENDCHN,
	5,
	5,1,',',2,
	5,
	ENDCHN,1,'\n',ENDCHN,

	/* Chain of layout characters */

	9,
	86,5,'S','P','A','C','E',ENDCHN,
	6,
	74,2,'N','L',ENDCHN,
	7,
	69,3,'T','A','B',ENDCHN,
	6,
	63,2,'S','L',ENDCHN,
	10,
	54,6,'S','P','A','C','E','S',ENDCHN,

	/* Node marker */

	5,
	ENDCHN,1,'N',ENDCHN,

	/* Keyword chain - subchain of delimiter chain */

	8,KW_WITH,4,'W','I','T','H',ENDCHN,
	9,KW_WITHS,5,'W','I','T','H','S',ENDCHN,
	7,KW_OPT,3,'O','P','T',ENDCHN,
	6,KW_OR,2,'O','R',ENDCHN,
	7,KW_ALL,3,'A','L','L',ENDCHN,
	ENDCHN,

	/* Dummy last entry */

	ENDCHN,0,

	/* Machine-dependent tables */

	' ','\n','\t',SLREP,' '};

static	INT	*names;


VOID init_tables()
/* Initialises the global hash table, and inserts the names of the
operation macros. The secondary delimiter chain is set up and linked to
the operation macro entries. */
{	names = &nametab[0];

	init_ghshtb();
	init_delchn();
}


static VOID init_ghshtb()
/* Initialises the hash table entries for the operation macros. */
{	INT i;
	INT *ptr;

	for(i = 0; i < LHV; i++) ghshtb[i] = NULLPT;
						/* Initialise heads of hash chains */

	for(;;) {
		idlen = names[2];		/* Length of next operation macro name */
		idpt = names + 3;		/* Point to start of name */

		ptr = mdfind();			/* Get hash table pointer */
		*names = *ptr;			/* Link item into hash chain */
		*ptr = (INT) names;		/* Update head of chain in hash table */

		names = idpt + idlen;		/* Point beyond the end of the atom */

		if(*names == WTHSMK) names = names + names[1] + 2;
						/* Handle multi-atom names */

		names = names + 4;		/* Move to next entry */

		if(names[-1] == MC_STOP) break;
	}
}


#if	ANSI
static VOID do_entry(INT **mdc,INT **sdc)
#else
static VOID do_entry(mdc,sdc)
INT	**mdc;
INT	**sdc;
#endif
/* Routine to move down the name table, optionally setting up addresses of
items in a master delimiter chain 'mdc' and/or a secondary delimiter chain
'sdc'. */
#if	IBMC
#pragma	checkout(suspend)
#endif
{	if(mdc != (INT **) NULLPT) *mdc = names;
	if(sdc != (INT **) NULLPT) *sdc = names + 1;
#if	IBMC
#pragma	checkout(resume)
#endif

	names = names + *names;
}


static VOID init_delchn()
/* Routine to initialise addresses of items in master delimiter chain
and secondary delimiter chains. */
#if	IBMC
#pragma	checkout(suspend)
#endif
{	do_entry(&delchn,(INT **) NULLPT);		/* VARS */
	do_entry((INT **) NULLPT,&das);			/* AS */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* SSAS */
	do_entry((INT **) NULLPT,&dif);			/* IF */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* UNLESS */
	do_entry((INT **) NULLPT,&dge);			/* GE */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* GR */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* EN */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* BC */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* EQ */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* TO */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* , */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* , */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* ) */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* , */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* <newline> */
	do_entry(&laychn,(INT **) NULLPT);		/* SPACE */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* NL */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* TAB */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* SL */
	do_entry(&kspacs,(INT **) NULLPT);		/* SPACES */
	do_entry(&knrep,(INT **) NULLPT);		/* N */
	do_entry(&keychn,(INT **) NULLPT);		/* WITH */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* WITHS */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* OPT */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* OR */
	do_entry((INT **) NULLPT,(INT **) NULLPT);	/* ALL */
#if	IBMC
#pragma	checkout(resume)
#endif

	spcsrp = kspacs + kspacs[1] + 1;
	knrep = knrep + 3;

#if	DEBUGGING
	if(*names != ENDCHN) macerr((INT) 18);
#endif
}

/*
 ***********************
 *                     *
 *   End of module 6   *
 *                     *
 ***********************
 */
