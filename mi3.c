/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 3 - Main subroutines                                      *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


INT advnce()
/* Function to advance to next character in the current text; yields
FALSE if at end of current text, otherwise yields  TRUE. */
{	INT c;
	INT n;

	sdb.spt++;
	if(sdb.spt == sdb.stoppt) return(FALSE);
	if(sdb.spt != ffpt) return(TRUE);

	/* Stack is empty, and source text is being scanned - read in more text */

	c = mdread();

	if(nlsw && (c != EOFCH)) {		/* Avoid lone EOFCH being treated as a line */
		n = ++(*at_s2);			/* Update and copy S2 */

		if(tlinct == sdb.linect) tlinct = n;
		sdb.linect = n;

		if(*at_s1 != 0)	{		/* Insert startline */
			*ffpt = SLREP;
			bumpff((INT) 1);
		}
	}

	if(c == EOFCH) {			/* End of input */
		ffpt = sdb.spt;
		if(!nlsw) {			/* Add preceding newline, if necessary */
			*ffpt = '\n';
			bumpff((INT) 1);
		}
		*ffpt = EOFCH;
	} else {
		nlsw = (c == '\n');
		*ffpt = c;
	}
	bumpff((INT) 1);
	return(TRUE);
}


#if	ANSI
VOID bumpff(INT n)
#else
VOID bumpff(n)
INT	n;
#endif
/* Routine to increase 'ffpt', checking for stack overflow. */
{	ffpt += n;
	if(ffpt >= lfpt) erlso();
}


VOID chatom()
/* Routine to check that the argument described by 'sdb.argno' and
'opdb.arglen' is exactly one atom. */
{	if(opdb.arglen == 0) erlia();

	gtatom();

	if(idlen != opdb.arglen) erlia();
}


VOID chekid()
/* Routine to check that the current text consists (after removal of
leading and trailing spaces) of exactly one atom. */
{	if(!gsatom()) erlia();
	if(gsatom()) erlia();
}


#if	ANSI
INT ckvaly(INT *ptr,INT type)
#else
INT ckvaly(ptr,type)
INT	*ptr,type;
#endif
/* Function to check whether a specified construction is currently
valid; yields TRUE if it is, otherwise yields FALSE. */
{	if(ptr > endpt) return(TRUE);		/* Outside stacks - must be an operation macro */
	return(ptr < (INT *)(sdb.hashpt[LHV+type-1]));
}


#if	ANSI
INT cmpare(INT *ptr)
#else
INT cmpare(ptr)
INT	*ptr;
#endif
/* Function to compare two atoms; yields FALSE if comparison fails,
otherwise yields TRUE. On return, 'infopt' points beyond the matched
atom. */
{	INT i;

	if(idlen != *++ptr) return(FALSE);
	infopt = ++ptr + idlen;

	for(i = 0; i < idlen; i++) {
		if(ptr[i] != idpt[i]) return(FALSE);
	}
	return(TRUE);
}


#if	ANSI
static VOID corect(INT n,INT **ptr)
#else
static VOID corect(n,ptr)
INT	n;
INT	**ptr;
#endif
/* Routine to relocate a pointer - called from 'mkroom' and 'mkcroom'. */
{	if((ndefpt <= *ptr) && (*ptr <= sdb.inffpt)) *ptr += n;
}


INT decalv()
/* Routine to restore previously stacked scan information at the end of
a nested construction. Yields FALSE if level of calls has become zero,
otherwise yields TRUE. */
{	nestlv--;
	if(nestlv == 0) return(FALSE);
	delpt = (INT *) (lfpt[0]);
	sdb.mtchpt = (INT *) (lfpt[1]);
	sdb.mchlin = lfpt[2];
	cllfpt = lfpt = lfpt + 3;
	return(TRUE);
}


#if	ANSI
VOID declf(INT n)
#else
VOID declf(n)
INT	n;
#endif
/* Routine to decrease 'lfpt', checking for stack overflow. */
{
#if	ATT3B | BSD4 | FBSD32 | FBSD64 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	lfpt -= n;
	if(ffpt >= lfpt) erlso();
#endif
#if	L1
	if(ffpt + n >= lfpt) erlso();
	lfpt -= n;
#endif
}


VOID er1tst()
/* Routine to check the validity of a macro element, as described by
'varpt' and 'meval'; gives error if not valid. */
{	if((varpt == NULLPT) || (meval > *varpt) || (meval <= 0)) erlme();
}


VOID encall()
/* Routine to stack and re-initialise scanning information when a nested
construction is encountered. 'cllfpt' is set to point to the latest
block of information stacked (this is used by 'prmiss' to "back up" if
stack overflow occurs). */
{	if(nestlv != 0) {
		declf((INT) 3);
		lfpt[0] = (INT) delpt;
		lfpt[1] = (INT) sdb.mtchpt;
		lfpt[2] = sdb.mchlin;
	}
	cllfpt = lfpt;
	delpt = binfpt + bindic - 2;
	sdb.mtchpt = bfndpt;
	sdb.mchlin = sdb.linect;
	nestlv++;
}


INT gargch()
/* Function to set 'sdb.spt' to point at first non-space character in the
current text; yields FALSE if end of current text reached, otherwise
yields TRUE. */
{	while(advnce()) {
		if(*sdb.spt != ' ') return(TRUE);
	}
	return(FALSE);
}


VOID getexp()
/* Routine to read an expression from the current text, and evaluate it.
Constants and variables are permitted; evaluation is from left to right
with no operator precedence. */
{	INT varsw = TRUE;
	INT sum = 0;
	INT negval = FALSE;
	INT *olidpt = sdb.spt;
#if	!ANSI
	INT *dumpt;
#endif
	INT opsw = 0;
	INT op1 = 0;			/* To satisfy optimisers */

	meval = 0;

	while(gsatom()) {
		INT op = *idpt;

		switch(op) {
			default:
				if(!varsw) erlia();
#if	ANSI
				if(!mdnum()) (void) gmeadd();
#else
				if(!mdnum()) dumpt = gmeadd();
#endif
				if(negval) meval = -meval;
				varsw = FALSE;
				switch(opsw) {
					case 1:
						meval *= op1;

#if	IBMC
#pragma	checkout(suspend)
#endif
					case 0:;
#if	IBMC
#pragma	checkout(resume)
#endif
						break;

					case 2:
						if(meval == 0) erlovf();
						meval = mddiv(op1,meval);
						break;

					case 3: meval &= op1;
						break;

					case 4: meval |= op1;
						break;

					default:
						macerr((INT) 9);
				}
				opsw = 0;
				negval = FALSE;
				continue;

				/* Minus and plus */

			case '-':
				negval = !negval;

#if	IBMC
#pragma	checkout(suspend)
#endif
			case '+':;
#if	IBMC
#pragma	checkout(resume)
#endif
				sum += meval;
				varsw = TRUE;
				meval = 0;
				continue;

				/* Logical and/or */

			case '/':
			case '*':
			case '&':
			case '|':
				opsw = op == '*' ? 1:
				       op == '/' ? 2:
				       op == '&' ? 3:
						   4;
				if(varsw) erlia();
				op1 = meval;
				varsw = TRUE;
				meval = 0;
				continue;
		}
	}
	if(varsw) erlia();
	meval += sum;
	idpt = olidpt;
}


INT *gmeadd()
/* Function to decode a (possibly subscripted) macro variable name;
yields the address of the macro variable. In the case of a numeric
variable, 'meval' is set to the previous contents and 'exprsw'
is set to 0; otherwise 'meval' is undefined and 'exprsw' is set
to 1. */
{	INT *flagpt = idpt;
	INT *res;

	for (;;) {
		INT c = *idpt;

		if((c != 'P') &&
		   (c != 'S') &&
#if	CVARS
		   (c != 'C') &&
#endif
		   (c != 'T')) {
			if(idpt == flagpt) break;
			if(!mdnum()) break;
			for(;;) {
				c = *--idpt;
#if	CVARS
				if(c == 'C' && idpt != flagpt) erlia();
					/* Cannot use character variable as subscript */
#endif
				varpt = (c == 'T') ? sdb.tvarpt:
					(c == 'S') ? svarpt:
#if	CVARS
					(c == 'C') ? cvarpt:
#endif
						     pvarpt;
				er1tst();
				res = varpt - meval;
				meval = varpt[-meval];
				if(idpt == flagpt) {
#if	CVARS
					exprsw = (c == 'C') ? 1 : 0;
#endif
					return(res);
				}
			}
		} else {
			if(idpt == sdb.spt) break;
			idpt++;
		}
	}
	erlia();

	/* NOTREACHED */
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


INT gsatom()
/* Function to extract the next non-space atom from the current text.
On exit, 'idpt' and 'idlen' describe the atom. Yields FALSE if at end
of current text, otherwise yields TRUE. */
{	if(gargch()) {
		if(*sdb.spt != EOFCH) {
			sdb.spt--;
			gtatom();
			return(TRUE);
		}
	}
	sdb.spt--;
	return(FALSE);
}


VOID gtatom()
/* Routine to extract the next atom from the current text. On exit,
'idpt' and 'idlen' describe the atom. */
{	if((levl == 0) && (sdb.skval >= 0) && (ffpt - sdb.spt == 1)) {
						/* Clear redundant information from forwards stack */
		ffpt = sdb.inffpt;
		sdb.spt = ffpt - 1;
	}

	if(!advnce()) longjmp(entsave,1);

	if(*sdb.spt == '\n') sdb.linect++;

	idpt = sdb.spt;
	idlen = 1;

	{	INT c = *sdb.spt;

#if	SPECAN
		if(xisalnum(c) || (c == *at_s6)) {
#else
		if(xisalnum(c)) {
#endif
			while(advnce()) {
				c = *sdb.spt;
#if	SPECAN
				if(!(xisalnum(c) || (c == *at_s6))) break;
#else
				if(!xisalnum(c)) break;
#endif
				idlen++;
			}

			/* End of identifier */

			sdb.spt--;		/* Backspace over character which terminated the atom */
		}
	}
}


#if	ANSI
INT ludel(INT *ptr)
#else
INT ludel(ptr)
INT	*ptr;
#endif
/* Function to search the chain headed by 'ptr' for the atom described
by 'idpt' and 'idlen'; yields TRUE if it is found, otherwise yields
FALSE. */
{	chanpt = ptr;

#if	DEBUGGING
	if(chanpt == NULLPT) macerr((INT) 10);
#endif

	for(;;) {
		chlink = *chanpt;
		if(cmpare(chanpt + 1)) return(TRUE);
		if(chlink == ENDCHN) return(FALSE);
		chanpt += chlink;
	}
#if	IBMC
#pragma	checkout(suspend)
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif


#if	ANSI
INT *lulayk(INT pridcall)
#else
INT *lulayk(pridcall)
INT	pridcall;
#endif
/* Function to yield the address of the entry for the keyword of a
layout character, or NULLPT if not found. */
{	INT *ptr = laychn;
	INT *tptr;
	INT *cptr;

	for(;;) {
		tptr = ptr + 1;			/* Point to LID */
		cptr = *tptr + tptr;		/* Point to actual character */

		/* Compare with list of layout characters,
		unless called from 'prid' routine */

		if(!pridcall && cmpare(tptr)) {
			idpt = cptr;
			idlen = 1;
			return(NULLPT);
		}

		if(*cptr == *idpt) return(ptr);
		if(ptr == kspacs) return(NULLPT);
						/* All the layout characters have been tried */

		ptr += *ptr;
	}
#if	IBMC
#pragma	checkout(suspend)
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif


#if	ANSI
VOID macexp(INT wrsw)
#else
VOID macexp(wrsw)
INT	wrsw;
#endif
/* Evaluates a macro expression. */
{	if(wrsw) erlia();

	sdb.spt--;			/* Backspace to before first character of expression */

#if	CVARS
	if(sdb.spt[1] != 'C') {
#endif
		getexp();
		mdconv(meval);
#if	CVARS
	} else {
		INT *cp;

		idpt = ++sdb.spt;
		idlen = sdb.stoppt - sdb.spt;
		sdb.spt = sdb.stoppt - 1;

		cp = gmeadd();
		cp = (INT *) *cp;
		idpt = cp + 1;
		idlen = *cp;
	}
#endif

	opexit();
}


#if	CVARS
#if	ANSI
VOID mkcroom(INT m)
#else
VOID mkcroom(m)
INT	m;
#endif
/* Routine to create space on the forwards stack immediately below the
character string variable pointer array (above the character variable storage
space and also above the permanent variables). This space is then used to
allocate 'm' additional character string variables. 'ndefpt' is left pointing
at the first newly allocated cell. */
{	INT **tpt;
	INT tsw;
	int n = (int) (m*(cvsize+2));		/* Extra space needed */

	ndefpt = cvarpt - cvnum;		/* Pointer to space about to be created */
	bumpff((INT) n);			/* Make space at top of forwards stack */
	bmove(ffpt - ndefpt - n,ndefpt,ndefpt + n);
	sdb.dbugpt = sdb.stakpt;

	for(;;) {
#if	IBMC
#pragma	checkout(suspend)
#endif
		tsw = ((struct sdbf *)(sdb.dbugpt))->dbugsw;	/* Previous 'sdb.dbugsw' */
		if(tsw == DB_REPL) break;
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->spt));
						/* Relocate 'sdb.spt' */
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->inffpt));
						/* Relocate 'sdb.inffpt' */
		if(tsw == DB_SOURCE) goto mrexit;
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->stoppt));
						/* Relocate 'sdb.stoppt' */
		if(tsw == DB_OPARG) {
			tpt = (INT **) (((struct sdbf *)(sdb.dbugpt))->dbugpt);
			goto mkr2;
		}
	mkr1:
		sdb.dbugpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->stakpt);
	}

	tpt = (INT **) (((struct sdbf *)(sdb.dbugpt))->argpt);
#if	IBMC
#pragma	checkout(resume)
#endif
mkr2:
	tpt--;
	if(((INT *) *tpt) == NULLPT) goto mkr1;
	corect(n,tpt);
	goto mkr2;
mrexit:
	sdb.inffpt += n;
	cvarpt += n;			/* Relocate 'cvarpt' */
#if	IBMC
#pragma	checkout(suspend)
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif
#endif


#if	ANSI
VOID mkroom(INT n)
#else
VOID mkroom(n)
INT	n;
#endif
/* Routine to create space on the forwards stack immediately below the
permanent variables (above the global definitions). The number of extra
cells required is given by 'n'. Used to make space for a new global
definition, or for additional permanent variables. */
{	INT **tpt;
	INT tsw;
#if	CVARS
	int i;
#endif

	ndefpt = pvarpt - pvnum;		/* Pointer to space about to be created */
	bumpff(n);				/* Make space at top of forwards stack */
	bmove(ffpt - ndefpt - n,ndefpt,ndefpt + n);
	sdb.dbugpt = sdb.stakpt;

	for(;;) {
#if	IBMC
#pragma	checkout(suspend)
#endif
		tsw = ((struct sdbf *)(sdb.dbugpt))->dbugsw;	/* Previous 'sdb.dbugsw' */
		if(tsw == DB_REPL) break;
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->spt));
						/* Relocate 'sdb.spt' */
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->inffpt));
						/* Relocate 'sdb.inffpt' */
		if(tsw == DB_SOURCE) goto mrexit;
		corect(n,&(((struct sdbf *)(sdb.dbugpt))->stoppt));
						/* Relocate 'sdb.stoppt' */
		if(tsw == DB_OPARG) {
			tpt = (INT **) (((struct sdbf *)(sdb.dbugpt))->dbugpt);
			goto mkr2;
		}
	mkr1:
		sdb.dbugpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->stakpt);
	}

	tpt = (INT **) (((struct sdbf *)(sdb.dbugpt))->argpt);
#if	IBMC
#pragma	checkout(resume)
#endif
mkr2:
	tpt--;
	if(((INT *) *tpt) == NULLPT) goto mkr1;
	corect(n,tpt);
	goto mkr2;
mrexit:
	pvarpt += n;
	sdb.inffpt += n;
#if	CVARS
	cvarpt += n;			/* Relocate 'cvarpt' */
	for(i = 1; i <= (int) cvnum; i++)
#if	IBMC
#pragma	checkout(suspend)
#endif
		((INT **) cvarpt)[-i] += n;
					/* Relocate character string variable pointers */
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif


VOID opexit()
{	unopdb();
	unsdb();
}


#if	ANSI
VOID prarg(INT sw)
#else
VOID prarg(sw)
INT	sw;
#endif
{	sdb.hashpt = opdb.mhshpt;
	setpts(sw);
	sdb.spt = idpt - 1;
	sdb.stoppt = idpt + idlen;
	if((sw == DB_EVAL) || (sw == DB_ROPARG)) {
						/* Delete spaces if A or operation macro argument */
		sdb.dbugsw = (sw == DB_EVAL) ? DB_SUBARG: DB_OPARG;
		if(gsatom()) {
			sdb.spt = idpt - 1;
			for(;;) {
				if((*--sdb.stoppt) != ' ') break;
			}
			sdb.stoppt++;
		}
	} else sdb.dbugsw = sw;
	prscan();
}


VOID prscan()
/* Routine to initialise variables in SDB, before scanning a new piece
of text. */
{	sdb.ohsw = FALSE;
	sdb.skval = 0;
	sdb.linect = 1;
	sdb.labpt = NULLPT;
}


INT ressp()
/* Yields FALSE if not at end of call, otherwise yields TRUE. */
{	sdb.spt = bespt;
	bespt = ZEROPT;
	if(bindic != EXCLMK) {
		sdb.linect = beslin;
		return(bindic == ENDCHN);
	}

	/* Exclusive delimiter */

	idlen = 0;
	bindic = ENDCHN;
	sdb.spt = idpt - 1;
	if(*idpt == '\n') sdb.linect--;
	return(TRUE);
}


VOID sbstpl()
/* Routine to set 'bestpl', the switch which determines what is to
happen to scanned text. */
{	bestpl = BP_COPY;			/* By default, copy text to output */
	if(oplev != 0) bestpl = BP_STACK;	/* If scanning for delimiter, stack text instead */
	if(sdb.skval != 0) bestpl = BP_NULL;	/* If scanning for label, discard text */
}


#if	ANSI
VOID setpts(INT sw)
#else
VOID setpts(sw)
INT	sw;
#endif
/* Routine to set up 'idpt' and 'idlen' to describe the argument
(if 'sw' = 'DB_ROPARG' or 'DB_SUBARG' or 'DB_EVAL') or delimiter
(if  'sw' = 'DB_DELIM') specified by 'sdb.argno'. */
{	INT n = sdb.argno*2;

	if(sw == DB_DELIM) n++;
#if	DEBUGGING
	if((sw != DB_ROPARG) && (sw != DB_SUBARG) && (sw != DB_EVAL) && (sw != DB_DELIM)) macerr((INT) 11);
#endif
	idpt = (INT *) (sdb.dbugpt[-n]);
	idlen = ((INT *) (sdb.dbugpt[-n-1])) - idpt;
}


#if	ANSI
INT sklab(INT l)
#else
INT sklab(l)
INT	l;
#endif
/* Function to determine whether a given label is present in the current
text; yields TRUE if it is, otherwise yields FALSE. 'chanpt' is set to
the offset of the label from the end of the text. */
{	chanpt = sdb.labpt;
	while(chanpt != NULLPT) {
		if(chanpt[1] == l) {
			chanpt += 2;
			return(TRUE);
		}
		chanpt = (INT *) *chanpt;
	}
	return(FALSE);
}


VOID stkhsh()
/* Routine to stack the current hash table on the backwards stack. */
{	sdb.ohsw = TRUE;
	declf((INT) (LHV + 5));
	fmove((INT) (LHV + 5),sdb.hashpt,lfpt);
	sdb.hashpt = lfpt;
}


VOID subchk()
{	if(meval <= opdb.optyp) {
		if(meval <= 0) meval += opdb.optyp;
		if(meval > opdb.sqnum) return;
	}

	/* MC-SUB has null value - perform null insert */

	idlen = 0;
	opexit();
	longjmp(bstsave,1);
}


VOID tebest()
{	INT *fndpt = infopt - idlen - 2;	/* Point to orlink */
	INT *tidpt = idpt;
	INT *tspt = sdb.spt;			/* Save current position while looking ahead */
	INT type;
#if	!ANSI
	INT dumint;
#endif

	tlinct = sdb.linect;

	for(;;) {
		if(*infopt == SPCSMK) {		/* This possibility ends with SPACES - move over any spaces in current text */
			infopt++;
#if	ANSI
			(void) gargch();
#else
			dumint = gargch();
#endif
			sdb.spt--;		/* Move back over last (non-space) character */
		}

		if(tewith(infopt)) {		/* Possibility contains WITH or WITHS */
		tbwith:
			if(!advnce()) break;	/* End of the current text - no match */
			sdb.spt--;
			gtatom();
			if((*idpt == ' ') && (indic == WTHSMK)) goto tbwith;
			if(!cmpare(infopt)) break;
						/* Subsequent atom does not match */
			continue;
		}

		if(indic == EXCLMK) {		/* Exclusive delimiter */
			if(bindic != EXCLMK) goto tb3;
		} else {
			if(bindic == EXCLMK) break;
						/* Not exclusive delimiter */
		}
		if(bespt >= sdb.spt) break;	/* A longer construction name has already been found */

	tb3:
		if(htabpt == NULLPT) {
			/* Secondary delimiter case */

			bestpl = BP_SDELIM;
		} else {
			infopt++;
			if(*infopt == TY_STOP) {
						/* Case of stop marker */
				if((levl != 0) || ((sdb.skval == 0) && (skiplv == 0))) break;
				bestpl = BP_STOP;
			} else {
				if((*infopt & masksw) == 0) break;
				type = *infopt;
				if((type == TY_INSERT) && (masksw == 1)) break;
				if(!ckvaly(chanpt,type)) break;
						/* This name is not currently valid */
				infopt++;
				bestpl = type;
			}
		}

		bfndpt = fndpt;
		binfpt = infopt;
		bindic = indic;
		bespt = sdb.spt;
		beslin = sdb.linect;
		break;
	}

	idpt = tidpt;
	sdb.spt = tspt;
	idlen = (sdb.spt - idpt) + 1;
	sdb.linect = tlinct;
}


#if	ANSI
INT tesdel(INT *ptr)
#else
INT tesdel(ptr)
INT	*ptr;
#endif
/* Function to search a specified secondary delimiter chain, looking for
a match; yields the position within chain of the matched delimiter. */
{	INT item = 0;

	setpts(DB_DELIM);
	chanpt = ptr;

#if	DEBUGGING
	if(chanpt == NULLPT) macerr((INT) 12);
#endif

	for(;;) {
		chlink = *chanpt;
		if(cmpare(chanpt)) return(item);
		item++;
		if(chlink == ENDCHN) macerr((INT) 13);
		chanpt += chlink;
	}
#if	IBMC
#pragma	checkout(suspend)
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif


#if	ANSI
INT tewith(INT *ptr)
#else
INT tewith(ptr)
INT	*ptr;
#endif
/* Function to check if the item pointed at by its parameter is WITHMK
or WTHSMK; 'indic' is set to the item. Yields TRUE if WITH or WITHS
found, otherwise yields FALSE. */
{	indic = *ptr;
	return((indic == WITHMK) || (indic == WTHSMK));
}


VOID unopdb()
{	ffpt = sdb.inffpt;
	levl--;
	if(--oplev != 0) {
		INT *tpt = opdb.topspt;

		fmove((INT) OPDBSZ,tpt,at_opdb);
		lfpt = tpt + OPDBSZ;
	}
}


VOID unsdb()
{	if(sdb.dbugsw == DB_REPL) levl--;
	tempt = sdb.stakpt;
	fmove((INT) SDBSZ,tempt,at_sdb);
	lfpt = tempt + SDBSZ;

	/* Delete text of call if in source text */

	if(levl == 0) {
		INT size = ffpt - sdb.spt - 1;

		sdb.spt++;
		if(size != 0) fmove(size,sdb.spt,sdb.inffpt);
		sdb.spt = sdb.inffpt - 1;
		ffpt = sdb.inffpt + size;
	}
}


#if	ANSI
INT xisalnum(INT ch)
#else
INT xisalnum(ch)
INT	ch;
#endif
/* Same as 'isalnum', but checks for characters outside the range 0 to 255
and handles conversion of the argument if required. */
{	if((ch >= 0) && (ch <= 255)) return((INT) isalnum((int) ch));
	return(FALSE);
}


#if	ANSI
INT xisalpha(INT ch)
#else
INT xisalpha(ch)
INT	ch;
#endif
/* Same as 'isalpha', but checks for characters outside the range 0 to 255
and handles conversion of the argument if required. */
{	if((ch >= 0) && (ch <= 255)) return((INT) isalpha((int) ch));
	return(FALSE);
}


#if	ANSI
INT xisdigit(INT ch)
#else
INT xisdigit(ch)
INT	ch;
#endif
/* Same as 'isdigit', but checks for characters outside the range 0 to 255
and handles conversion of the argument if required. */
{	if((ch >= 0) && (ch <= 255)) return((INT) isdigit((int) ch));
	return(FALSE);
}


#if	ANSI
INT xisupper(INT ch)
#else
INT xisupper(ch)
INT	ch;
#endif
/* Same as 'isupper', but checks for characters outside the range 0 to 255
and handles conversion of the argument if required. */
{	if((ch >= 0) && (ch <= 255)) return((INT) isupper((int) ch));
	return(FALSE);
}

/*
 ***********************
 *                     *
 *   End of module 3   *
 *                     *
 ***********************
 */

