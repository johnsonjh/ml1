/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 4 - Routines used in setting up delimiter structures      *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


/*** Forward references ***/

#if	ANSI
static	INT	gsratm(void);
static	INT	keysrc(INT *);
static	INT	plnode(void);
static	void	snodch(INT *);
#else
static	INT	gsratm();
static	INT	keysrc();
static	INT	plnode();
static	VOID	snodch();
#endif


#if	ANSI
static VOID ertest(INT sw)
#else
static VOID ertest(sw)
INT	sw;
#endif
{	if(sw && ((opdb.ntypsw == 1) || (opdb.ntypsw == 4))) return;

	erlia();
}


VOID getdel()
{	INT *exidpt;
	INT *oldspt;
	INT type;

gdnext:
	type = KW_OPT;
	if(!gsratm()) longjmp(evxsave,1);	/* End of structure representation */

gd1:
	oldspt = sdb.spt;
	if(!keysrc(&type)) {
		if((*idpt == *knpt) && (idlen != 1)) {
			idpt++;
			if(mdnum()) goto node;
						/* Node marker was followed by a number */
			idpt--;			/* Node is false alarm */
		}

		/* Case of delimiter */

		if(type != KW_OPT) goto gdw1;

		/* Place LID on forwards stack and link it up */

		snodch(ffpt);
		*ffpt = ENDCHN;			/* Orlink */
		bumpff((INT) 1);

	gtloop:
		*ffpt = idlen;
		bumpff((INT) 1);

		{	INT *lv4 = ffpt;

			bumpff(idlen);
			fmove(idlen,idpt,lv4);
		}

		/* Look ahead for WITH or WITHS */

		exidpt = idpt;
		if(!(gsratm() && keysrc(&type))) {
						/* WITH or WITHS found */
		gtexit:
			if(exidpt == spcsrp) {
				*ffpt = SPCSMK;
				bumpff((INT) 1);
			}
			sdb.spt = oldspt;
			nodept = ffpt;
			*ffpt = ENDCHN;		/* Nextlink */
			bumpff((INT) 1);

			if(optlev == 0) delct++;

			nodesw = keysw = TRUE;
			return;
		}

		/* Test for WITHS or WITH */

		if((type != KW_WITH) && (type != KW_WITHS)) goto gtexit;
		*ffpt = (type == KW_WITH) && (exidpt != spcsrp) ? WITHMK: WTHSMK;
		bumpff((INT) 1);
		nodesw = keysw = FALSE;
		if(!gsratm()) erlia();		/* WITH or WITHS not followed by another atom */
		goto gd1;

		/* Subsequently returning here to perform checking */

	gdw1:
		if(type == KW_WITHS) {
			if(*idpt != ' ') goto gtloop;

			/* Convert WITHS SPACE to with SPACES */

			ffpt[-1] = WITHMK;
			idpt = spcsrp;
			goto gtloop;
		} else {
			INT c = *idpt;

			/* WITH may not separate atoms which are identifiers */

#if	SPECAN
			if(!(xisalnum(c) || (c == *at_s6))) goto gtloop;
#else
			if(!xisalnum(c)) goto gtloop;
#endif
			c = ffpt[-2];		/* or 'exidpt' */
#if	SPECAN
			if(!(xisalnum(c) || (c == *at_s6))) goto gtloop;
#else
			if(!xisalnum(c)) goto gtloop;
#endif
			erlia();
		}

		/* Node has been found */
	node:
		ertest(nodesw);
		nodesw = FALSE;

		/* Test whether placing or goto */

		if(!gsratm()) goto ndgo;
		if(keysrc(&type)) goto ndkey;

		/* Case of placing */

	ndplac:
		if(meval == 0) erlia();		/* Trying to place node zero */
		consw = TRUE;
		if(plnode()) {
			if(*nnodpt == 1) erlia();
						/* Node already placed */
			snodch(ffpt);
			nodept = tempt;
		}
		nnodpt[0] = 1;
		nnodpt[1] = (INT) ffpt;
		sdb.spt = oldspt;
		goto gdnext;

	ndkey:

		switch(type) {
			case KW_WITH:
			case KW_WITHS:
				erlia();	/* WITH or WITHS encountered immediately after a node */

#if	IBMC
#pragma	checkout(suspend)
#endif
			case KW_OPT:;
#if	IBMC
#pragma	checkout(resume)
#endif
				goto ndplac;
						/* Node is placed */

			case KW_OR:
			case KW_ALL:		/* Node is gone to */
			ndgo:
				if(meval == 0) goto ndn0;
				if(nodept == NULLPT) erlia();
				if(!plnode()) break;
				if(*nnodpt == 1) goto oldnod;
				chanpt = tempt;
				if(chanpt != NULLPT) {
					for(;;) {
						chlink = *chanpt;
						if(chlink == ENDCHN) break;
						chanpt += chlink;
					}
				} else goto nund1;

				*chanpt = nodept - chanpt;
				nodept = NULLPT;
				sdb.spt = oldspt;
				goto gdnext;

			default:
				macerr((INT) 14);
		}
		*nnodpt = 0;

	nund1:
		nnodpt[1] = (INT) nodept;

	gnod3:
		nodept = NULLPT;
		sdb.spt = oldspt;
		goto gdnext;

		/* Going to old node */

	oldnod:
		snodch(tempt);
		goto gnod3;

		/* Node N0 */

	ndn0:
		chanpt = nodept;
		if(chanpt == NULLPT) erlia();
		for(;;) {
			chlink = *chanpt;
			*chanpt = EXCLMK;
			if(chlink == ENDCHN) break;
			chanpt += chlink;
		}
		exitsw = TRUE;
		goto gnod3;
	}

	/* Case of keyword */

	ertest(keysw);
	keysw = FALSE;

	switch(type) {
		case KW_WITH:
		case KW_WITHS:
			erlia();		/* WITH or WITHS encountered out of context */

#if	IBMC
#pragma	checkout(suspend)
#endif
		case KW_OPT:;
			longjmp(evpsave,1);

		case KW_OR:;
			longjmp(evrsave,1);

		case KW_ALL:;			/* Action at ALL */
#if	IBMC
#pragma	checkout(resume)
#endif
			joinch();
			nodept = oab.allpt;
			consw = FALSE;
			fmove((INT) ALLSZ,lfpt,at_all);
			lfpt += ALLSZ;
			if(--optlev == 0) delct++;
			nodesw = keysw = TRUE;
			return;

		default:
			macerr((INT) 15);
	}
}


static INT gsratm()
/* Yields FALSE if at end of structure representation, otherwise
yields TRUE. */
{	for(;;) {
		if((sdb.stoppt - sdb.spt) == 1) return(FALSE);
		gtatom();
		if(lulayk(FALSE) == NULLPT) return(TRUE);
						/* Ignore layout characters */
	}
#if	IBMC
#pragma	checkout(suspend)
#endif
}
#if	IBMC
#pragma	checkout(resume)
#endif


VOID joinch()
{	if(optlev == 0) erlia();		/* ALL encountered without matching OPT */

	/* Add 'nodept' chain to head of 'oab.allpt' chain */

	chanpt = nodept;

	if(chanpt != NULLPT) {
		for(;;) {
			chlink = *chanpt;
			if(chlink == ENDCHN) break;
			chanpt += chlink;
		}

		if(oab.allpt != NULLPT) *chanpt = oab.allpt - chanpt;
		oab.allpt = nodept;
	}
}


#if	ANSI
static INT keysrc(INT *at_type)
#else
static INT keysrc(at_type)
INT	*at_type;
#endif
/* Function to search keyword chain for a keyword. Yields TRUE if
keyword found, the keyword type is returned via 'at_type'; otherwise
yields FALSE. */
{	if(!ludel(keychn)) return(FALSE);
	*at_type = chanpt[1];
	return(TRUE);
}


static INT plnode()
/* Yields FALSE if node is new, otherwise yields TRUE. */
{	nnodpt = lnodpt;
	while(nnodpt != opdb.topspt) {
		if(*nnodpt == meval) {
			nnodpt++;
			tempt = (INT *) (nnodpt[1]);
			return(TRUE);
		}
		nnodpt += NODESZ;
	} 

	/* Reserve new node */

	ollfpt = lfpt;
	declf((INT) NODESZ);
	if(lnodpt != ollfpt) fmove((INT) (lnodpt - ollfpt),ollfpt,lfpt);

	lnodpt -= NODESZ;
	*lnodpt = meval;
	nnodpt = lnodpt + 1;
	return(FALSE);
}


#if	ANSI
static VOID snodch(INT *pt)
#else
static VOID snodch(pt)
INT	*pt;
#endif
{	chanpt = nodept;
	if(chanpt != NULLPT) {
		for(;;) {
			chlink = *chanpt;
			*chanpt = pt - chanpt;
			if(chlink == ENDCHN) break;
			chanpt += chlink;
		}
	}
	else {
		if(!consw) erlia();
	}
}


VOID tespac()
{	if((*infopt == SPCSMK) || (*infopt == WTHSMK)) {
		while((*idpt == WITHMK) && (idpt[2] == ' ')) {
			idpt += 3;
		}
	}

}

/*
 ***********************
 *                     *
 *   End of module 4   *
 *                     *
 ***********************
 */
