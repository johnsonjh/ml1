/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 5 - Error routines (alphabetical order)                   *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


/*** Forward references ***/

#if	ANSI
static	void	mcabrt(void);
static	char	*mestype(INT);
static	void	prid(void);
static	void	prlid(INT *);
static	void	prmiss(void);
static	void	prname(INT *,INT);
static	void	prnfnd(INT);
static	char	*prtype(INT,INT);
static	INT	setype(INT *);
#else
static	VOID	mcabrt();
static	char	*mestype();
static	VOID	prid();
static	VOID	prlid();
static	VOID	prmiss();
static	VOID	prname();
static	VOID	prnfnd();
static	char	*prtype();
static	int	setype();
#endif


VOID erlmd()
/* Multiply-defined label. */
{	prerr();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("Label %d is multiply-defined",meval);
#endif
#if	FBSD64 | L1
	mderpr("Label %ld is multiply-defined",meval);
#endif
	prctxt();
}


VOID erlme()
/* Illegal macro element. */
{	prerr();
	idlen = 1;
	mderid();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("%d is illegal macro element",meval);
#endif
#if	FBSD64 | L1
	mderpr("%ld is illegal macro element",meval);
#endif
	prctxt();
	mcabrt();
}


VOID erlia()
/* Illegal argument. */
{	prerr();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("Argument %d has illegal value",sdb.argno);
#endif
#if	FBSD64 | L1
	mderpr("Argument %ld has illegal value",sdb.argno);
#endif
	idlen = opdb.arglen;
	idpt = eriapt;
	prviz();
	mcabrt();
}


VOID erlovf()
/* Arithmetic overflow. */
{	prerr();
	mderpr("Arithmetic overflow");
	prctxt();
	mcabrt();
}


VOID erlso()
/* Stack overflow. If the current text is the source text, then the
following additional information is given: if there are any
constructions currently unmatched, or if a search is being made for a
label as a result of a forward MC-GO, then appropriate diagnostic
messages are printed. */
{	prerr();
	mderpr("Process aborted for lack of storage");
	if((sdb.dbugsw == DB_SOURCE) && ((skiplv != 0) || (sdb.skval != 0))) {
		mderpr(" possibly due to\n");
		prmiss();
	} else prctxt();
	mihalt();
}


VOID ermtst()
/* Routine to test for mismatches. Prints appropriate diagnostics if any
are found. */
{	if((skiplv != 0) || (sdb.skval != 0)) {
		prerr();
		prmiss();
	}
}


VOID ersic()
/* Illegal input character. */
{	INT *oidpt = idpt;			/* Save current values */
	INT oidlen = idlen;

	prerr();
	mderpr("Illegal input character");
	prctxt();

	idpt = oidpt;				/* Restore previous values */
	idlen = oidlen;
}


VOID ersnw()
/* Illegal macro name after warning. */
{	prerr();
	mderpr("Illegal macro name after warning");
	prviz();
}


#if	ANSI
VOID macerr(INT n)
#else
VOID macerr(n)
INT	n;
#endif
/* System error. This should never (!) occur. */
{	prerr();
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("System error %d\n\n",n);
#endif
#if	FBSD64 | L1
	mderpr("System error %ld\n\n",n);
#endif
	mihalt();
}


static VOID mcabrt()
/* Routine to clear up and print diagnostics after an error in a call of
an operation macro or insert. The aborted construction is given a null
value. Control is returned to the main scanning loop. */
{	opexit();
	prname(sdb.mtchpt,TRUE);
	mderpr(" aborted due to above error\n\n");
	longjmp(bssave,1);
}


VOID prctxt()
/* Routine to print the current text. */
{	INT erbloc[EDBSZ];			/* Error block - area for saving the EDB */

	fmove((INT) EDBSZ,at_edb,erbloc);
						/* Save the current EDB */
	mderpr("\n\ndetected in\n");
	if(sdb.dbugsw == DB_EVAL) goto erop;

prct2:
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr("line %d of ",sdb.linect);
#endif
#if	FBSD64 | L1
	mderpr("line %ld of ",sdb.linect);
#endif

	for (;;) {
		switch(sdb.dbugsw) {
			case DB_SOURCE:		/* In source text */
				mderpr("source text\n\n");
				fmove((INT) EDBSZ,erbloc,at_edb);
						/* Restore the EDB */
				return;

			case DB_OPARG:
			case DB_SUBARG:		/* In argument */
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
				mderpr("argument %d of ",sdb.argno);
#endif
#if	FBSD64 | L1
				mderpr("argument %ld of ",sdb.argno);
#endif
				goto erop;

			case DB_DELIM:		/* In delimiter */
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
				mderpr("delimiter %d of ",sdb.argno);
#endif
#if	FBSD64 | L1
				mderpr("delimiter %ld of ",sdb.argno);
#endif

			erop:			/* In operation macro, argument or delimiter */
				sdb.argpt = sdb.dbugpt;
#if	IBMC
#pragma	checkout(suspend)
#endif
				sdb.dbugpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->mtchpt);
						/* Stacked value of 'sdb.mtchpt' */
#if	IBMC
#pragma	checkout(resume)
#endif
				if(sdb.dbugsw != DB_EVAL) {
					prname(sdb.dbugpt,FALSE);
					mderpr(" evaluated in\n");
					if(sdb.dbugsw != DB_OPARG) break;
				}

#if	IBMC
#pragma	checkout(suspend)
#endif
			case DB_REPL:;		/* In macro */
#if	IBMC
#pragma	checkout(resume)
#endif
				prname(sdb.dbugpt,FALSE);
				mderpr(" with ");
				if(*sdb.argpt == 0) mderpr("no ");
				mderpr("arguments");
				sdb.argno = 0;
				sdb.dbugpt = sdb.argpt;

				while(*sdb.argpt != sdb.argno) {
					sdb.argno++;
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
					mderpr("\n%3d) ",sdb.argno);
#endif
#if	FBSD64 | L1
					mderpr("\n%3ld) ",sdb.argno);
#endif
					setpts(DB_SUBARG);
					prid();
				}

				mderpr("\ncalled from\n");
				break;

			default:
				macerr((INT) 16);
		}

		fmove((INT) EDBSZ,sdb.stakpt + 1,at_edb);
						/* Restore the EDB */
		if(sdb.mchlin == sdb.linect) goto prct2;
		if(*sdb.spt == '\n') {
			sdb.linect--;
			if(sdb.linect == sdb.mchlin) goto prct2;
		}
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
		mderpr("lines %d to %d of ",sdb.mchlin,sdb.linect);
#endif
#if	FBSD64 | L1
		mderpr("lines %ld to %ld of ",sdb.mchlin,sdb.linect);
#endif
	}
}


VOID prenv()
/* Routine to print the version number of the machine-independent logic,
and the names of all the constructions in the current environment. The
built-in operation macro names are not printed. */
{	INT i;
	INT *j;
	INT *ptr;
	INT type;

	mderpr("\n\n\nVersion %s\n",MIVERSION);

	for(i = TY_STOP; i <= TY_SKIP; i++) {
		mderpr("\n%ss are\n\n",prtype(i,TRUE));

		for(j = sdb.hashpt; j < sdb.hashpt + LHV; j++) {
			ptr = j;
			for(;;) {
				ptr = (INT *) *ptr;
				if(ptr == NULLPT) break;

				if((stffpt <= ptr) && (ptr < endpt)) {
						/* Eliminate built-in macros */
					type = setype(ptr + 1);

					if(type == i) {
						if((type != TY_STOP) && !ckvaly(ptr,type)) continue;
						prlid(ptr + 1);
						mderpr("\n");
					}
				}
			}
		}
	}
}


VOID prerr()
/* Routine to introduce diagnostic output. The count of errors (in S5)
is updated. */
{	mderpr("\n\nError(s)\n");
	(*at_s5)++;
}


static VOID prid()
/* Routine to print the atom described by 'idpt' and 'idlen',
restricting length to TEXMAX characters. */
{	INT sw;
	INT *pt = (INT *) NULL;		/* To satisfy optimisers */

	/* Handle null atom */

	if(idlen == 0) {
		mderpr("(null)");
		return;
	}

	/* Test if layout character */

	if(idlen == 1) {
		pt = lulayk(TRUE);
		if(pt != NULLPT) {			/* Print keyword for layout characters */
			idlen = pt[2];
			idpt = pt + 3;
			mderpr("(");
			mderid();
			mderpr(")");
			return;
		}
	}

	/* Print ordinary text, restricting length if necessary */

	for(;;) {
		sw = FALSE;
		if(idlen > TEXMAX) {
			sw = TRUE;
			pt = idpt + idlen - HTMAX;
			idlen = HTMAX;
		}
		mderid();
		if(!sw) break;
		idpt = pt;
		mderpr("  ---  ");
	}
}


#if	ANSI
static VOID prlid(INT *ptr)
#else
static VOID prlid(ptr)
INT	*ptr;
#endif
/* Routine to print a LID - parameter points at orlink. */
{	for(;;) {
		idlen = ptr[1];
		idpt = ptr + 2;
		ptr = idpt + idlen;		/* Move past atom */
		prid();
		if(*ptr == WITHMK) continue;	/* ...A WITH B... */
		if(*ptr != WTHSMK) break;	/* ...A WITHS B... */
		mderpr(" ");
	}
}


static VOID prmiss()
/* Function to print names of unmatched constructions. */
{	INT lchlink;

	if(nestlv != 0) {
		lfpt = cllfpt;
		for(;;) {
			mderpr("Delimiter ");
			for(;;) {
				prlid(delpt);
				lchlink = *delpt;
				if(lchlink == ENDCHN) break;
				mderpr(" or ");
				delpt += lchlink;
			}
			mderpr(" of ");
			prname(sdb.mtchpt,FALSE);
			prnfnd(sdb.mchlin);
			if(!decalv()) break;
		}
		skiplv = 0;
		if(sdb.skval < 0) sdb.skval = -sdb.skval - 1;
	}

	if(sdb.skval != 0) {
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
		mderpr("Label %d referenced",sdb.skval);
#endif
#if	FBSD64 | L1
		mderpr("Label %ld referenced",sdb.skval);
#endif
		prnfnd(sdb.sklin);
		sdb.skval = 0;
	}
	prctxt();
}


#if	ANSI
static VOID prname(INT *ptr,INT first)
#else
static VOID prname(ptr,first)
INT	*ptr;
INT	first;
#endif
/* Routine to print the type and name of a construction - 'ptr'
points at orlink. */
{	mderpr("%s ",prtype(setype(ptr),first));
	prlid(ptr);
}


#if	ANSI
static VOID prnfnd(INT line)
#else
static VOID prnfnd(line)
INT	line;
#endif
/* Routine to print "not found" message, with line number. */
{
#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	mderpr(" in line %d of current text not found\n",line);
#endif
#if	FBSD64 | L1
	mderpr(" in line %ld of current text not found\n",line);
#endif
}


#if	ANSI
static char *mestype(INT type)
#else
static char *mestype(type)
INT	type;
#endif
{	switch(type) {
		case TY_STOP  :	return("stop");
		case TY_MACRO :	return("macro");
		case TY_WARN  :	return("warning");
		case TY_INSERT:	return("insert");
		case TY_SKIP  :	return("skip");
		default       :	macerr((INT) 17);

				/* NOTREACHED */
	}
#if	IBMC
#pragma	checkout(suspend)
#endif
#if	FBSD32 | FBSD64
#pragma	clang diagnostic push
#pragma	clang diagnostic ignored "-Wreturn-type"
#endif
}
#if	FBSD32 | FBSD64
#pragma	clang diagnostic pop
#endif
#if	IBMC
#pragma	checkout(resume)
#endif


#if	ANSI
static char *prtype(INT type,INT first)
#else
static char *prtype(type,first)
INT	type;
INT	first;
#endif
/* Function to return a string describing a particular type of
construction. */
{	char *mes;
	static char typvec[8];

	mes = mestype(type);
	if(first) {
		INT i = 0;

		while((typvec[i] = mes[i]) != '\0') i++;
		typvec[0] = (char) (toupper((int) typvec[0]));
		return(typvec);
	}
	else return(mes);
}


VOID prviz()
/* Routine to print fuller information, after an illegal macro name or
argument has been detected. */
{	mderpr(", viz \"");
	prid();
	mderpr("\"");
	prctxt();
}


#if	ANSI
static INT setype(INT *ptr)
#else
static INT setype(ptr)
INT	*ptr;
#endif
/* Function to return the type of a construction - parameter points at
orlink. */
{	ptr++;

	for(;;) {
		ptr = *ptr + ptr + 1;
		if((*ptr != WITHMK) && (*ptr != WTHSMK)) break;
		ptr++;
	}

	if(*ptr == SPCSMK) ptr++;

	return(ptr[1]);
}


/*
 ***********************
 *                     *
 *   End of module 5   *
 *                     *
 ***********************
 */

