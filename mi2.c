/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Module 2 - Main scanning routine                                 *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */


#include "ml1.h"


/*** Arguments to 'longjmp' for exit from 'stkarg' ***/

#define	STKARG01	1
#define	STKARG02	2
#define	STKARG03	3
#define	STKARG04	4
#define	STKARG05	5
#define	STKARG06	6
#define	STKARG07	7
#define	STKARG08	8
#define	STKARG09	9
#define	STKARG10	10
#define	STKARG11	11
#define	STKARG12	12
#if	CVARS
#define	STKARG13	13
#define	STKARG14	14
#endif
#define	STKARG15	15
#define	STKARG16	16
#define	STKARG17	17
#define	STKARG18	18
#define	STKARG19	19
#define	STKARG20	20
#define	STKARG21	21
#define	STKARG22	22
#define	STKARG23	23
#define	STKARG24	24
#define	STKARG25	25

/*** Forward references ***/

#if	ANSI
static	void	ckclass(void);
static	void	saexit(void);
static	void	stkarg(int);
#else
static	VOID	ckclass();
static	VOID	saexit();
static	VOID	stkarg();
#endif

/*** Local storage ***/

static	jmp_buf	sasave;			/* Transfers control to 'stkarg' return switch */
static	int	jmp_sw;			/* Used by 'stkarg' return switch */

VOID basic_scan()
/* The main scanning routine. This routine is NOT recursive in the
normal C sense; however, recursion is effected by stacking the
scanning description block (SDB) and returning to this routine at a
fixed closure point. */
{	INT i;					/* For miscellaneous loops, etc. */
	INT *wnspt = (INT *) NULL;		/* Temporary storage for 'sdb.spt' while processing warning markers */
	INT *wnidpt = (INT *) NULL;		/* Temporary storage for 'idpt' while processing warning markers */
	INT *cinfpt = (INT *) NULL;		/* Points at information block in primary construction */
	INT extra;				/* Used in code for MC-PVAR and MC-CVAR operation macros */
	INT opmtype;				/* Switch used in processing operation macro calls */

	/* Set up all the closures necessary for abnormal function returns */

	if(setjmp(bstsave) != 0) goto bstrex;
	if(setjmp(entsave) != 0) goto entext;
	if(setjmp(evpsave) != 0) goto evopt;
	if(setjmp(evrsave) != 0) goto evor;
	if(setjmp(evxsave) != 0) goto evexit;
	if(setjmp(bssave) != 0) goto bsnext;

	/* Set up special closure needed for 'stkarg'. The value passed
	by the 'longjmp' call indicates the point to which control is to
	be returned. */

	jmp_sw = setjmp(sasave);
	if(jmp_sw != 0) {
		switch(jmp_sw) {
			case STKARG01:	goto sa_01;
			case STKARG02:	goto sa_02;
			case STKARG03:	goto sa_03;
			case STKARG04:	goto sa_04;
			case STKARG05:	goto sa_05;
			case STKARG06:	goto sa_06;
			case STKARG07:	goto sa_07;
			case STKARG08:	goto sa_08;
			case STKARG09:	goto sa_09;
			case STKARG10:	goto sa_10;
			case STKARG11:	goto sa_11;
			case STKARG12:	goto sa_12;
#if	CVARS
			case STKARG13:	goto sa_13;
			case STKARG14:	goto sa_14;
#endif
			case STKARG15:	goto sa_15;
			case STKARG16:	goto sa_16;
			case STKARG17:	goto sa_17;
			case STKARG18:	goto sa_18;
			case STKARG19:	goto sa_19;
			case STKARG20:	goto sa_20;
			case STKARG21:	goto sa_21;
			case STKARG22:	goto sa_22;
			case STKARG23:	goto sa_23;
			case STKARG24:	goto sa_24;
			case STKARG25:	goto sa_25;

#if	DEBUGGING
			default      :	macerr((INT) 0);
#endif
		}
	}

	/* Start of main scanning loop */

	for(;;) {
		masksw = sdb.hashpt[LHV + 4] & glbwsw;

		sbstpl();	/* Decide whether to copy, stack or discard text */
		bespt = ZEROPT;

	bsloop:
		gtatom();
		if(*idpt == EOFCH) {
			ermtst();
			mihalt();
		}

		if(nestlv != 0) {		/* Search delimiter chain */
			htabpt = NULLPT;
			chanpt = delpt;

#if	DEBUGGING
			if(chanpt == NULLPT) macerr((INT) 1);
#endif

			for(;;) {
				chlink = *chanpt;
				if(cmpare(chanpt)) tebest();
				chanpt += chlink;
				if(chlink == ENDCHN) break;
			}

			if(masksw == 5) goto enexcl;
						/* Looking for exclusive delimiter */
		}

		/* Search chain of names */

	bsname:
		htabpt = mdfind();
		chanpt = (INT *) *htabpt;

		while(chanpt != NULLPT) {	/* Find the best match */
			if(cmpare(chanpt + 1)) tebest();
			chanpt = (INT *) *chanpt;
		}

	bsswit:
		switch(bestpl) {
			case BP_STACK:		/* Place atom on forwards stack */
			sastak:
				{	INT *lv4 = ffpt;

					bumpff(idlen);
					fmove(idlen,idpt,lv4);
					opdb.arglen += idlen;
					goto bsloop;
				}

			case BP_SKIP:		/* Action at skip name */
				encall();
				masksw = *binfpt & 4;
						/* Extract "matched" option */
				if(skiplv == 0) {
					skiplv = nestlv;
					copdsw = *binfpt & 1;
						/* Extract "delimiter" option */
					coptsw = *binfpt & 2;
						/* Extract "text" option */
				}

				/* Skip delimiter */

			skdel:
				sbstpl();
				if(coptsw == 0) bestpl = BP_NULL;
						/* Arrange to ignore text of skip */
				idlen = bespt - idpt + 1;
				if(!ressp()) goto skd1;

				/* End of skip */

				if(decalv() && (skiplv <= nestlv)) {
					/* End of nested skip */
					masksw = 4;
				} else {
					/* End of outermost skip */

					skiplv = 0;
					if(copdsw == 0) continue;

					/* Copy closing skip delimiter to output text */

				bstrex:
					masksw = sdb.hashpt[LHV + 4] & glbwsw;
					sbstpl();
					bespt = ZEROPT;
				}

				if(idlen == 0) goto bsloop;
				goto bsswit;

				/* Non-closing skip delimiter */

			skd1:
				if(skiplv != nestlv) goto bsswit;
				if((copdsw == 0) || (sdb.skval != 0)) goto bsloop;
				if(oplev != 0) goto sastak;

#if	IBMC
#pragma	checkout(suspend)
#endif
			case BP_COPY:;		/* Add atom to output text */
#if	IBMC
#pragma	checkout(resume)
#endif
				for(i = 0; i < idlen; i++) {
					INT c = idpt[i];

					if(c == SLREP) continue;
					mdouch(c);
				}

#if	IBMC
#pragma	checkout(suspend)
#endif
			case BP_NULL:		/* Skip over text - do not add to output text */
				goto bsloop;

			case BP_WARN:		/* Action at warning marker */
#if	IBMC
#pragma	checkout(resume)
#endif
				i = ressp();	/* Result is ignored */
				wnspt = sdb.spt;
				wnidpt = idpt;
				idlen = 0;
				if(gsatom()) {
					bestpl = BP_AWARN;
					masksw = 1;
					goto bsname;
				}

#if	IBMC
#pragma	checkout(suspend)
#endif
			case BP_AWARN:;
#if	IBMC
#pragma	checkout(resume)
#endif
				if(svarpt[-3] == 0) ersnw();
						/* Give error message, unless user has suppressed them */
				sdb.spt = wnspt;
				idpt = wnidpt;	/* Restore pointers, and insert warning marker in output text */
				idlen = sdb.spt - idpt + 1;
				goto bstrex;

			case BP_STOP:		/* Action at stop marker */
				sdb.spt = idpt - 1;
				if(*idpt == '\n') sdb.linect--;
				ermtst();
				continue;

			case BP_MACRO:		/* Action at macro or insert name */
			case BP_INSERT:
				masksw = sdb.hashpt[LHV + 4] & glbwsw;
				tempt = binfpt;
				if(*tempt == STRMK) {
						/* Straight-scan macro */
					masksw = 0;
					tempt++;
				}
				if(nestlv == 0) {
					cinfpt = tempt + 1;
					caltyp = *tempt;
					if((caltyp == LOCMK) && !sdb.ohsw) {
						/* Stack new hash table, if not already done at this level */
						declf((INT) 1);
						*lfpt = (INT) sdb.hashpt;
						stkhsh();
					}
					declf((INT) SDBSZ);
						/* Make space to stack the SDB later on */
					nargpt = lfpt;
					sdb.argct = 0;
					declf((INT) 2);
					lfpt[0] = (INT) (bespt + 1);
					lfpt[1] = (INT) idpt;
					sdb.skval = -sdb.skval - 1;
				}
				encall();
				bestpl = BP_NULL;
				if(!ressp()) goto bsloop;
				break;

			case BP_SDELIM:		/* Action at secondary delimiter */
			dello:
				if(skiplv != 0) {
					delpt = binfpt + bindic;
					goto skdel;
				}
				if(nestlv == 1) {
					sdb.argct++;
					declf((INT) 2);
					lfpt[0] = (INT) (bespt + 1);
					lfpt[1] = (INT) idpt;
				}
				delpt = binfpt + bindic;
				bestpl = BP_NULL;
				if(!ressp()) goto bsloop;
				break;

			default:
				macerr((INT) 2);
		}

		/* Perform call */

		if(decalv()) continue;
		sdb.skval = -sdb.skval - 1;
		declf((INT) 1);			/* Stack terminator to argument vector */
		*lfpt = (INT) NULLPT;
		fmove((INT) SDBSZ,at_sdb,nargpt);

		if(caltyp < UINSMK) {		/* Test for insert or straight-scan macro */
			if(sdb.skval != 0) {	/* Still searching for label - forget the call */
				lfpt = nargpt + (INT) SDBSZ;
				continue;
			}
			invoct++;

			/* Test for user macro or operation macro */

			if(caltyp < OPMK) {	/* Call of user macro */
				levl++;
				sdb.stoppt = caltyp + cinfpt - 1;
						/* Pointer to end of replacement text */
				sdb.spt = *cinfpt + cinfpt - 1;
						/* Pointer to start of replacement text */
				sdb.dbugpt = sdb.mtchpt;
				sdb.dbugsw = DB_REPL;
				sdb.stakpt = sdb.argpt = nargpt;
				prscan();

				/* Set up temporary variables */

				{	INT capacity = cinfpt[1];
						/* Get capacity of macro from information block */

					sdb.tvarpt = lfpt - 1;
					declf((INT) 4);

					/* Initialise T1, T2 and T3 */

					lfpt[0] = levl - oplev;
					lfpt[1] = invoct;
					lfpt[2] = sdb.argct;
					lfpt[3] = capacity;

					declf(capacity - 3);
				}
				continue;	/* Continue the scan (now of the replacement text) */
			}
		}

		/* Perform operation macro call or insert */

		levl++;
		if(oplev != 0) {
			declf((INT) OPDBSZ);
			fmove((INT) OPDBSZ,at_opdb,lfpt);
		}
		oplev++;
		opdb.mhshpt = sdb.hashpt;
		opdb.topspt = lfpt;
		opdb.optyp = caltyp;
		sdb.dbugsw = DB_EVAL;
		sdb.argno = 1;
		sdb.inffpt = ffpt;
		sdb.stakpt = sdb.dbugpt = nargpt;

		/* Test for insert */

		if(caltyp >= UINSMK) {		/* Perform insert */
			opdb.sqnum = sdb.skval;
			stkarg(STKARG01); sa_01:
			if(!gargch()) erlia();
			if(*sdb.spt != 'L') {
				INT wrsw = FALSE;
				INT insw = DB_EVAL;

				if(opdb.sqnum != 0) {
						/* Searching for label - ignore insert */
					opexit();
					continue;
				}
				if(*sdb.spt == 'W') {
						/* 'Written' argument */
					wrsw = TRUE;
					if(!gargch()) erlia();
				}

				switch(*sdb.spt) {
					default:
						/* Case of macro expression */

						macexp(wrsw);
						goto bstrex;

					case 'D':
						insw = DB_DELIM;
						getexp();
						if((meval == 0) && (sdb.argpt != NULLPT)) break;
						varpt = sdb.argpt;

						/* Check that this delimiter actually exists */

						er1tst();
						break;

					case 'B':
						insw = DB_SUBARG;

#if	IBMC
#pragma	checkout(suspend)
#endif
					case 'A':;
#if	IBMC
#pragma	checkout(resume)
#endif
						getexp();
						varpt = sdb.argpt;

						/* Check that this argument actually exists */

						er1tst();
				}

				sdb.dbugpt = sdb.argpt;
				sdb.argno = meval;
				prarg(insw);
				if(wrsw) {	/* Written argument - do no further evaluation */
					idpt = sdb.spt + 1;
					idlen = sdb.stoppt - idpt;
					opexit();
					goto bstrex;
				}

				if(opdb.optyp == PINSMK) {
#if	IBMC
#pragma	checkout(suspend)
#endif
					sdb.hashpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->hashpt);
				}
						/* Protected insert - use value of 'sdb.hashpt' from previous level */
				unopdb();
				lfpt = sdb.stakpt;	/* Partially collapse stack, while evaluating inserted text */
				sdb.tvarpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->tvarpt);
						/* Previous value of 'sdb.tvarpt' */
				sdb.argpt = (INT *) (((struct sdbf *)(sdb.dbugpt))->argpt);
						/* Previous value of 'sdb.argpt' */
#if	IBMC
#pragma	checkout(resume)
#endif
			} else {
				/* Case of label */

				getexp();
				if(meval <= 0) erlme();
						/* Zero and negative labels may not be placed */
				opexit();
				if(meval == sdb.skval) sdb.skval = 0;
						/* Label was being searched for - search is now over */

				/* Remember label if necessary */

				if(levl == 0) continue;
						/* Labels in source text are not remembered */
				if(!sklab(meval)) {
						/* New label being defined - add to chain of label entries */
					declf((INT) 4);
					lfpt[0] = (INT) sdb.labpt;
					lfpt[1] = meval;
					lfpt[2] = sdb.spt - sdb.stoppt;
					lfpt[3] = sdb.linect;
					sdb.labpt = lfpt;
					continue;
				}
				if((*chanpt + sdb.stoppt) != sdb.spt) erlmd();
						/* Already seen at different offset in current text - thus multiply-defined */
			}
			continue;

			/* Action at end of piece of text */

		entext:
			if((nestlv == 0) || ((sdb.dbugsw != DB_OPARG) && (sdb.dbugsw != DB_SUBARG))) goto entx1;

			/* End of argument - look ahead for exclusive delimiter... */

			setpts(DB_DELIM);
			sdb.stoppt = idpt + idlen;
			sdb.spt = idpt - 1;
			masksw = 5;
			goto bsloop;

			/* ...subsequently returning here */

		enexcl:
			sdb.stoppt = idpt;
			if(bindic == EXCLMK) goto dello;

		entx1:
			ermtst();
			if(sdb.dbugsw == DB_OPARG) saexit();
			unsdb();
			continue;
		}

		/***************************************************
		*                                                  *
		*      Code for each individual operation macro    *
		*                                                  *
		***************************************************/

		opmtype = *cinfpt;
		switch(opmtype) {

			/*************************
			* Code for M C A L T E R *
			*************************/

			case MC_ALTER:
				sdb.argno = 2;
				stkarg(STKARG02); sa_02:
				chatom();
				opdb.sqnum = idlen;
				opdb.sqpt = idpt;
				sdb.argno = 1;
				stkarg(STKARG03); sa_03:
				chatom();
				if(!ludel(delchn)) erlia();
						/* Specified delimiter does not exist */
				opdb.arglen = opdb.sqnum;
				eriapt = opdb.sqpt;
				sdb.argno = 2;

				for(;;) {
					if(chlink - opdb.arglen <= 3) erlia();
						/* New delimiter is too long */
					sdb.spt = chanpt + 3;
						/* Point to old delimiter */

					/* Test for case of N (the node marker)
					or whatever has replaced it */

					if(sdb.spt == knpt) {
						INT c = *opdb.sqpt;
							/* New node marker */

						if(!(xisupper(c) || xisdigit(c))) erlia();
						/* New node marker is not an upper case letter or a digit */
					}

					chanpt[2] = opdb.arglen;
						/* Set up length of new delimiter */
					tempt = sdb.spt + opdb.arglen;
						/* Point to where nextlink will have to go */

					{	INT nlink = *infopt;	/* Old nextlink */

						/* Relocate nextlink */

						if(nlink != ENDCHN) nlink += (infopt - tempt);
						*tempt = (INT) nlink;

	                                        /* Move in the new delimiter */

						fmove(opdb.arglen,opdb.sqpt,sdb.spt);

	                                        /* Move past this instance of the delimiter */

						chanpt += chlink;
					}
					if(!ludel(chanpt)) break;
				}

				opexit();
				continue;

				/*******************
				* Code for M C G O *
				*******************/

			case MC_GO:
				if(sdb.argct != 1) {

					/* Conditional goto */

					opdb.optyp = tesdel(dif);	/* 0 for IF, 1 for UNLESS */

					sdb.argno = 2;
					meval = tesdel(dge);
					opdb.sqnum = 0;
					opdb.sqsw = FALSE;

					/* Switch on type of condition */

					switch(meval) {

						case 4:		/* Character comparison   (=) */

							stkarg(STKARG04); sa_04:
							opdb.sqnum = opdb.arglen;
							sdb.argno = 3;
							stkarg(STKARG05); sa_05:
							if(opdb.arglen != opdb.sqnum) break;
							/* Conditional goto fails */
							tempt = sdb.inffpt;

							for(;;) {
								if(!advnce()) {

									/* Conditional goto succeeds */

									opdb.optyp++;
									break;
								}
								if(*sdb.spt != *tempt) break;

								/* Conditional goto fails */

								tempt++;
							}
							break;

						case 3:		/* Belongs to class   (BC) */

							stkarg(STKARG06); sa_06:
							sdb.argno = 3;
							stkarg(STKARG07); sa_07:
							chekid();

							/* Class must be a single character */

							if(idlen != 1) erlia();
							ckclass();
							break;

						case 0:		/* Numerical comparison   (GE) */

							opdb.sqnum = 1;

#if	IBMC
#pragma	checkout(suspend)
#endif
						case 1:;		/* Numerical comparison   (GR) */

							opdb.sqsw = TRUE;

						case 2:;		/* Numerical comparison   (EN) */
#if	IBMC
#pragma	checkout(resume)
#endif
							stkarg(STKARG08); sa_08:
							getexp();
							opdb.sqnum += meval;
							sdb.argno = 3;
							stkarg(STKARG09); sa_09:
							getexp();

							if(opdb.sqsw) {

								if(meval < opdb.sqnum) opdb.optyp++;

								/* Conditional goto succeeds */

							} else {
								if(meval == opdb.sqnum) opdb.optyp++;

								/* Conditional goto succeeds */
							}
							break;

						default:
							macerr((INT) 3);
					}

					if(opdb.optyp != 1) {
						opexit();
						continue;
					}
					sdb.argno = 1;
				}

				/* Perform goto */

				stkarg(STKARG10); sa_10:
				if(!(gargch() && (*sdb.spt == 'L'))) erlia();
				getexp();
				if(meval == 0) {

					/* Label L0 - return from current level */

					if(levl == 1) erlme();
					opexit();
					if(sdb.dbugsw == DB_OPARG) saexit();
					unsdb();
					continue;
				}
				if(meval < 0) erlme();
				opexit();
				if(sklab(meval)) {

					/* Label already encountered */

					sdb.spt = chanpt[0] + sdb.stoppt;
					sdb.linect = chanpt[1];
				} else {
					sdb.skval = meval;
					sdb.sklin = sdb.mchlin;
				}
				continue;

				/***********************
				* Code for M C L E N G *
				***********************/

			case MC_LENG:
				stkarg(STKARG11); sa_11:
				mdconv(opdb.arglen);	/* Convert argument length to a string */
				opexit();
				goto bstrex;

				/***********************
				* Code for M C N O T E *
				***********************/

			case MC_NOTE:
				stkarg(STKARG12); sa_12:
				idpt = eriapt;
				idlen = opdb.arglen;
				mderpr("\n");
				mderid();
				opexit();
				if(svarpt[-4] == 0) prctxt(); else mderpr("\n");
				continue;

				/***********************
				* Code for M C C V A R *
				***********************/

#if	CVARS
			case MC_CVAR:
				stkarg(STKARG13); sa_13:
				getexp();
				opdb.sqnum = meval;
				if(sdb.argct == 2) {   /* Setting variable size */
					sdb.argno = 2;
					stkarg(STKARG14); sa_14:
					getexp();
					if(meval <= 0) erlia();
					if(cvsize != 0 && cvsize != meval)
						erlia();  /* Once only unless the same*/
					cvsize = meval;
				};
				if(cvsize == 0) {
					sdb.argno = 1;
					erlia();	/* Not set up yet */
				}
				extra = opdb.sqnum - cvnum;
				if(extra > 0) {
					INT *p,*q;
#if	DEBUGGING
					int j;
#endif

					mkcroom(extra);
					p = cvarpt - opdb.sqnum;
						/* Point to highest variable pointer */
					q = p - (cvsize+1);
						/* Point to space for highest new variable */
					for(i = 0; i < extra; i++) {
						*p++ = (INT) q;	/* Fill in area pointer */
						*q = 0;	/* Clear variable */
#if	DEBUGGING
						for(j = 1; j <= (int) cvsize; j++)
							q[j] = 0x81;
#endif
						q -= cvsize + 1;
					}
					cvnum = opdb.sqnum;
					*cvarpt = cvnum;
				}
				opexit();
				continue;
#endif

				/***********************
				* Code for M C P V A R *
				***********************/

			case MC_PVAR:
				stkarg(STKARG15); sa_15:
				getexp();
				extra = meval - pvnum;
				if(extra > 0) {
					mkroom(extra);
					for(i = 0; i < extra; i++) ndefpt[i] = 0;
						/* Zeroise new permanent variables */
					*pvarpt = pvnum = meval;
				}
				opexit();
				continue;

				/*********************
				* Code for M C S E T *
				*********************/

			case MC_SET:
				stkarg(STKARG16); sa_16:
				chekid();
				sdb.spt = idpt + idlen - 1;
				opdb.sqpt = gmeadd();
						/* Get address of macro variable on LHS */
#if	CVARS
				opdb.sqnum = exprsw;
#endif
						/* Get type of macro expression */
				sdb.argno = 2;
				stkarg(STKARG17); sa_17:
#if	CVARS
				if(opdb.sqnum == 0) {
#endif
					getexp();
					*opdb.sqpt = meval;
						/* Update the variable */
#if	CVARS
				} else {
					if(opdb.arglen > cvsize) erlia();
					opdb.sqpt = (INT *) *opdb.sqpt;
					*opdb.sqpt = opdb.arglen;
					fmove(opdb.arglen,eriapt,opdb.sqpt+1);
				}
#endif
				opexit();
				continue;

				/*********************
				* Code for M C S U B *
				*********************/

			case MC_SUB:
				stkarg(STKARG18); sa_18:
				opdb.optyp = opdb.arglen;
				sdb.argno = 2;
				stkarg(STKARG19); sa_19:
				getexp();
				opdb.sqnum = 0;
				subchk();
				opdb.sqnum = meval - 1;
				sdb.argno = 3;
				stkarg(STKARG20); sa_20:
				getexp();
				subchk();
				idlen = meval - opdb.sqnum;
				idpt = sdb.inffpt + opdb.sqnum;
				opexit();
				goto bstrex;

				/**********************************
				* Code for M C N O - - - - macros *
				**********************************/

			case MC_NOWARN:
				sdb.hashpt[LHV + 4] = 7;

#if	IBMC
#pragma	checkout(suspend)
#endif
			case MC_NODEF:
			case MC_NOINS:
			case MC_NOSKIP:;
#if	IBMC
#pragma	checkout(resume)
#endif

				/* Set new limit on appropriate local constructions */

				sdb.hashpt[LHV + opmtype] = (INT) (sdb.dbugpt + (INT) SDBSZ);
				opexit();
				continue;

				/***********************
				* Code for M C W A R N *
				***********************/

			case MC_WARN:
				opdb.ntypsw = TY_WARN;
				break;

				/***********************
				* Code for M C S T O P *
				***********************/

			case MC_STOP:
				opdb.ntypsw = TY_STOP;
				break;

				/*********************
				* Code for M C D E F *
				*********************/

			case MC_DEF:
				sdb.argno = sdb.argct - 1;
				opdb.sqsw = (tesdel(das) != 0);
						/* Indicates whether macro is to be straight-scan */
				opdb.sqnum = 3;	/* Default capacity of macro */
				if(sdb.argct != 2) {
						/* The VARS part is present */
					sdb.argno = 1;
					stkarg(STKARG21); sa_21:
					getexp();
					if(meval > 3) opdb.sqnum = meval;
						/* User-specified capacity for macro */
					ffpt = sdb.inffpt;
				}
				sdb.argno = *sdb.dbugpt;
				stkarg(STKARG22); sa_22:
				sdb.argno--;
				opdb.ntypsw = TY_MACRO;
				break;

				/*********************
				* Code for M C I N S *
				*********************/

			case MC_INS:
				opdb.sqnum = PINSMK;
				opdb.ntypsw = TY_INSERT;
				if(sdb.argct != 1) {	/* Insert type has been specified */
					stkarg(STKARG23); sa_23:
					chekid();
					if(idlen != 1) erlia();
					{	INT c = *idpt;

						if(c == 'P') {
							sdb.argno = 2;
							break;
						}
						if(c != 'U') erlia();
					}
					opdb.sqnum = UINSMK;
					sdb.argno = 2;
				}
				break;

				/***********************
				* Code for M C S K I P *
				***********************/

			case MC_SKIP:
				opdb.sqsw = 0;
				opdb.ntypsw = TY_SKIP;
				if(sdb.argct == 1) break;
				stkarg(STKARG24); sa_24:
				while(gargch()) {
					switch(*sdb.spt) {
						default:
							erlia();

#if	IBMC
#pragma	checkout(suspend)
#endif
						case 'D':;
#if	IBMC
#pragma	checkout(resume)
#endif
							opdb.sqsw |= 1;
							continue;

						case 'T':
							opdb.sqsw |= 2;
							continue;

						case 'M':
							opdb.sqsw |= 4;
							continue;
					}
				}
				sdb.argno = 2;
				break;

			default:
				macerr((INT) 4);
		}

		/* Deal with structure representation */

		stkarg(STKARG25); sa_25:
		nodept = NULLPT;
		delct = optlev = 0;
		bumpff((INT) 1);
		exitsw = FALSE;
		consw = nodesw = keysw = TRUE;
		lnodpt = lfpt;
		getdel();

		/* Start building up the information block for the new
		construction, on the forwards stack. */

	ev1:
		*ffpt = opdb.ntypsw;
		bumpff((INT) 1);
		switch(opdb.ntypsw) {
			case TY_MACRO:
				if(opdb.sqsw) {
					*ffpt = STRMK;
					bumpff((INT) 1);
				}

				/* Stack offsets of end and start of replacement text */

				ffpt[0] = sdb.stoppt - ffpt;
				ffpt[1] = sdb.inffpt + opdb.arglen - ffpt - 1;
				bumpff((INT) 2);

#if	IBMC
#pragma	checkout(suspend)
#endif
			case TY_INSERT:;
#if	IBMC
#pragma	checkout(resume)
#endif
				*ffpt = opdb.sqnum;
				bumpff((INT) 1);
				break;

			case TY_SKIP:
				*ffpt = opdb.sqsw;
				bumpff((INT) 1);

#if	IBMC
#pragma	checkout(suspend)
#endif
			case TY_STOP:
			case TY_WARN:;
#if	IBMC
#pragma	checkout(resume)
#endif
				break;

			default:
				macerr((INT) 5);
		}

		/* The following, apparently endless, loop terminates at the
		end of the structure representation - see below */

		for(;;) getdel();

		/* Action at OPT */

	evopt:
		optlev++;
		declf((INT) ALLSZ);		/* Stack the ALL block */
		fmove((INT) ALLSZ,at_all,lfpt);
		oab.opthpt = oab.optpt = ffpt;
		oab.allpt = NULLPT;
		nodesw = FALSE;
		getdel();

	evcont:
		if(delct == 0) goto ev1;

		/* The following, apparently endless, loop jumps to 'evexit'
		at the end of the structure representation. */

		for (;;) getdel();

		/* Action at OR */

	evor:
		joinch();
		if(delct == 0) bumpff((INT) 1);
		nodept = oab.optpt;
		oab.optpt = ffpt;
		nodesw = TRUE;
		getdel();

		/* Check that name of branch is unique */

		chanpt = oab.opthpt;

#if	DEBUGGING
		if(chanpt == NULLPT) macerr((INT) 6);
#endif

		for(;;) {
			chlink = *chanpt;
			if(chanpt == oab.optpt) goto evcont;
			idpt = oab.optpt;
			infopt = chanpt;

			for(;;) {
				idlen = idpt[1];
				idpt += 2;
				if(!cmpare(infopt)) break;
				idpt += idlen;

				/* Check WITH, WITHS and SPACES */

				tespac();
				tempt = infopt;
				infopt = idpt;
				idpt = tempt;
				tespac();
				if(!tewith(infopt)) {
					if(tewith(idpt)) break;
					erlia();
				}
				if(!tewith(idpt)) break;
			}

			if(chlink == ENDCHN) break;
			chanpt += chlink;
		}

		/* Action at end of structure representation */

	evexit:
		if((delct == 0) || (optlev != 0)) erlia();
		chanpt = nodept;
		if(chanpt != NULLPT) {
			for(;;) {
				chlink = *chanpt;
				*chanpt = ENDCHN;
				if(chlink == ENDCHN) break;
				chanpt += chlink;
			}
		} else goto evex1;

		/* Check that all nodes have been placed */

		while(lfpt != opdb.topspt) {
			if(lfpt[1] == 0) erlia();
			lfpt += NODESZ;
			continue;

		evex1:
			if(!exitsw) erlia();
		}

		/* Action after processing of structure representation */

		offset = 1;
		idlen = ffpt - sdb.stoppt;

		switch(opdb.ntypsw) {
			case TY_MACRO:		/* Macro case - make delimiter structure adjacent to replacement text */
				fmove(idlen,sdb.stoppt,eriapt);
				sdb.stoppt = sdb.inffpt;
				ffpt -= opdb.arglen;
				offset += (eriapt - sdb.stoppt);
				idlen = ffpt - sdb.stoppt;
				break;

			case TY_WARN:
				if(delct != 1) erlia();
				if(opdb.optyp != OPMK) {
					sdb.hashpt[LHV + 4] = 6;
				} else glbwsw = 6;
				break;

			case TY_STOP:
				if(delct != 1) erlia();
				break;

			case TY_INSERT:
				if(delct != 2) erlia();

#if	IBMC
#pragma	checkout(suspend)
#endif
			case TY_SKIP:
				break;
#if	IBMC
#pragma	checkout(resume)
#endif

			default:
				macerr((INT) 7);
		}

		/* Add to environment */

		idpt = sdb.stoppt;
		if(opdb.optyp == LOCMK) {		/* Local case - make space on backwards stack */
			opexit();
			declf(idlen);
			ndefpt = lfpt;
		} else {
			mkroom(idlen);		/* Global case - make space on forwards stack */
			idpt += idlen;		/* Relocate pointer after text has been shuffled up */
			opexit();
		}
		bespt = sdb.spt;
		bmove(idlen,idpt,ndefpt);	/* Move new definition into place */

		chanpt = ndefpt + offset;

#if	DEBUGGING
		if(chanpt == NULLPT) macerr((INT) 8);
#endif
		for(;;) {
			chlink = *chanpt;
			idpt = chanpt + 1;
			idlen = *idpt;
			idpt++;
			sdb.spt = idpt + idlen - 1;
			htabpt = mdfind();
			if(ndefpt != lfpt) {	/* Global case - find end of locals on hash chain */
				for(;;) {
					if(lfpt > ((INT *) *htabpt)) break;
					if(((INT *) *htabpt) > endpt) break;
					htabpt = (INT *) *htabpt;
				}
			}

			tempt = chanpt - 1;
			*tempt = *htabpt;
			*htabpt = (INT) tempt;
			if(ndefpt != lfpt) {
				INT *tpt = sdb.hashpt;

				/* Add globals to all hash tables */

				for(;;) {
					tpt = (INT *) (tpt[LHV + 5]);
					if(tpt == NULLPT) break;
					htabpt = tpt + offset;
					if(*htabpt == *tempt) *htabpt = (INT) tempt;
				}
			}
			if(chlink == ENDCHN) break;
			chanpt += chlink;
		}

		sdb.spt = bespt;
	bsnext:;
	}					/* End of main scanning loop */
}


static VOID ckclass()
/* Part of the code for MC-GO. Checks the class of the atom which is
argument 2; used in evaluating a BC ('belongs to class') condition.
If the atom does indeed belong to 'class', the global variable 'opdb.optyp'
is incremented by one. */
{	INT class = *idpt;

	/* Get length of argument 2 */

	idlen = eriapt - sdb.inffpt;
	if(idlen == 0) return;			/* Conditional goto fails */
	idpt = sdb.inffpt;
	switch(class) {
		case 'N':   			/* Class of numbers */
		{	INT c = *idpt;

			while((c == '+') || (c == '-')) {
				idpt++;
				idlen--;
				if(idlen == 0) return;
						/* Conditional goto fails */
				c = *idpt;
			}

			while(xisdigit(c)) {
				idpt++;
				idlen--;

				if(idlen == 0) {
						/* Conditional goto succeeds */
					opdb.optyp++;
					return;
				}
				c = *idpt;
			}
			break;			/* Conditional goto fails */
		}

		case 'L':			/* Class of letters */
		case 'I':			/* Class of identifiers */

			while(idlen > 0) {
				INT c = *idpt;

				if(!(xisalpha((int) c) ||
					(xisdigit(c) && (class == 'I')))) return;
						/* Conditional goto fails */
				idpt++;
				idlen--;
			}
			opdb.optyp++;		/* Conditional goto succeeds */
			return;

		default:
			erlia();		/* Illegal class */
	}
}


#if	ANSI
static VOID stkarg(int sw)
#else
static VOID stkarg(sw)
int	sw;
#endif
/*  The  entry  part of the linkroutine.  Evaluates argument 'sdb.argno' and
stacks the evaluated form; This is done by re-entering the main scanning
loop.  The parameter 'sw' specifies the point to which 'saexit' should
eventually transfer control, by means of a 'longjmp' to 'sasave'. */
{	opdb.linkpt = (INT) sw;			/* Save linkage switch in SDB */

	prarg(DB_ROPARG);
	opdb.arglen = 0;

	longjmp(bssave,1);			/* Resume scanning */
}


static VOID saexit()
/*  The exit part of the linkroutine.  This is called when the evaluated
form of the required argument has been stacked.   Resets  pointers,  and
resumes  the  scan at the point where the corresponding call of 'stkarg'
was made.  It is, however, important to realise that return is  made  to
the  closure 'sasave'; thus the code calling 'stkarg' should NOT have
any local variables extant in addition to those declared at the start of
'basic_scan', as they will be corrupted during argument evaluation.  */
{	lfpt = opdb.topspt;
	sdb.stoppt = ffpt;
	eriapt = sdb.stoppt - opdb.arglen;
	sdb.spt = eriapt - 1;
	sdb.dbugsw = DB_EVAL;

#if	ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
	longjmp(sasave,opdb.linkpt);		/* Resume processing of outer call */
#endif
#if	FBSD64 | L1
	longjmp(sasave,(int) opdb.linkpt);	/* Resume processing of outer call */
#endif
}

/*
 ***********************
 *                     *
 *   End of module 2   *
 *                     *
 ***********************
 */
