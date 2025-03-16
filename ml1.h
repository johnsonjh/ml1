/*
 **********************************************************************
 *                                                                    *
 *   ML/I macro processor -- C version                                *
 *                                                                    *
 *   Common header file                                               *
 *                                                                    *
 *   Copyright (C) R.D. Eager                        MMXVIII          *
 *                 P.J. Brown   University of Kent   MCMLXVII         *
 *                                                                    *
 **********************************************************************
 */

/*
 * Edit History (machine independent logic):
 *
 * CJA  --  Pre-release version.
 * CJB  --  First release of C version.
 * CKA  --  Addition of optional character string variables.
 * CKB  --  Addition of function prototypes and revision of types.
 * CKC  --  Addition of ANSI C support.
 * CKD  --  Addition of logical and (&) and logical or (|) operators in
 *          macro expressions.
 * CKE  --  Use of 'const' made conditional on ANSI.
 *      --  Include of 'stdarg.h' made conditional on ANSI.
 * CKF  --  Fixed bug in 'joinch' that resulted in loop if more than
 *          one element in chain.
 * CKG  --  Changed 'jmp_sw' in basic scanner to be an int, and removed
 *          INT cast when calling 'setjmp' to set it. This is to conform
 *          with ANSI.
 *          Added support for IBM C Set/2.
 * CKH  --  Further fixes for IBM C Set/2.
 * CKI  --  Added support for Zortech C extended model.
 * CKJ  --  Changed EOFCH and SLREP and EOFCH to values greater than 255,
 *          to allow MD-logic to use all character code values between 0
 *          and 255.
 * CKL  --  Added support for a single non-alphanumeric character to be
 *          treated as alphanumeric (via S6 setting).
 * CKM  --  Added support for Microsoft Windows (32 bit).
 * CKN  --  Added support for gcc on FreeBSD.
 * CKO  --  Added support for clang on FreeBSD.
 * CKP  --  Added support for 64 bit clang on FreeBSD.
 *
 */

#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>

/* Define EXACTLY ONE of the following symbols non-zero to enable the
   appropriate conditional compilation:

   ATT3B        - AT&T 3B series (UNIX)
   BSD4         - Berkeley UNIX version 4.x
   FBSD32	- GCC/CLANG on FreeBSD (32 bit)
   FBSD64	- CLANG on FreeBSD (64 bit)
   IBMC         - IBM C Set/2 on OS/2
   L1           - MOS-X on Olivetti Line 1
   MSC          - Microsoft C on the IBM PC and PS/2 under OS/2 and DOS
		  (small model)
   VMS          - VAXC on VMS
   WIN          - Visual C++ 5.0 on Microsoft Windows (32 bit)
   ZTC          - Zortech C on the IBM PC and PS/2
   ZTCX         - Zortech C on the IBM PC and PS/2 under DOS (extended model)
*/

#define ATT3B           0
#define BSD4            0
#define IBMC            0
#define L1              0
#define MSC             0
#define VMS             0
#define	WIN		0
#define ZTC             0
#define ZTCX            0
#define	FBSD32		1
#define	FBSD64		0

#if     IBMC | FBSD32 | FBSD64 | MSC | ZTCX | VMS | WIN | ZTC
#define ANSI            1
#else
#define ANSI            0
#endif

#if     ANSI
#include <stdarg.h>
#define CONST   const
#else
#define CONST
#endif

/*** Machine dependent constants ***/

#if     ATT3B | BSD4 | FBSD32 | IBMC | VMS | WIN
#define MAXINT  0x7fffffff              /* Largest positive number */
#endif
#if     FBSD64 | L1 | ZTCX
#define MAXINT  0x7fffffffffffffffL     /* Largest positive number */
#endif
#if     MSC | ZTC
#define MAXINT  32767                   /* Largest positive number */
#endif


/*** Machine dependent type definitions ***/

#if     ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
typedef int     INT;
#endif
#if     FBSD64 | L1
typedef long    INT;
#endif

#if     ANSI
#define VOID    void
#else
#define VOID
#endif

/*** Truth values ***/

#define FALSE           ((INT) 0)       /* Truth value */
#define TRUE            ((INT) 1)       /* Truth value */


/*** Version number ***/

#define MIVERSION       "CKP"           /* Version number of machine independent logic */


/*** Tunable values ***/

#define CVARS           1               /* Set non-zero to enable character string variables */
#define	SPECAN		1		/* Set non-zero to enable special alphanumeric character */

#define DEBUGGING       1               /* Set non-zero during development and testing */
#define LHV             32              /* Size of hash table */
#define TEXMAX          64              /* Maximum length of construction in an error message */
#define HTMAX           (TEXMAX/2 - 4)
#if     CVARS
#define CVSIZE          20              /* Default character string variable size */
#endif
#define PVARNUM         10              /* Initial number of permanent variables */
#define SVARNUM         24              /* Number of system variables */
#define EOFCH           0x100           /* End of file character */
#define SLREP           0x101           /* For SL keyword */


/*** Miscellaneous constants ***/

#if     ATT3B | BSD4 | FBSD32 | IBMC | MSC | VMS | WIN | ZTC | ZTCX
#define ZEROPT          ((INT *) 0)     /* Must be less than or equal to any pointer value */
#define NULLPT          ((INT *) 0)     /* Must be different from any pointer value */
#endif
#if     FBSD64 | L1
#define ZEROPT          ((INT *) 0L)    /* Must be less than or equal to any pointer value */
#define NULLPT          ((INT *) 0L)    /* Must be different from any pointer value */
#endif
#define NODESZ          3               /* Size of a node entry on backwards stack */


/*** Types of constructions ***/

#define OPMK            0               /* Operation macro */
#define LOCMK           1               /* Local NEC macro */
#define UINSMK          2               /* Unprotected insert */
#define PINSMK          3               /* Protected insert */
#define STRMK           4               /* Straight-scan macro */

#define ENDCHN          (MAXINT)        /* Closing delimiter/end of chain marker */
#define EXCLMK          (MAXINT - 1)    /* Exclusive delimiter marker */
#define WITHMK          (MAXINT - 2)    /* For WITH keyword */
#define WTHSMK          (MAXINT - 3)    /* For WITHS keyword */
#define SPCSMK          (MAXINT - 4)    /* For SPACES keyword */


/*** Codes for operation macros ***/

#define MC_NODEF        0               /* Suppress local macro definitions */
#define MC_NOWARN       1               /* Suppress local warning marker definitions */
#define MC_NOINS        2               /* Suppress local insert definitions */
#define MC_NOSKIP       3               /* Suppress local skip definitions */
#define MC_DEF          4               /* Define a macro (local or global) */
#define MC_WARN         5               /* Define a warning marker (local or global) */
#define MC_INS          6               /* Define an insert (local or global) */
#define MC_SKIP         7               /* Define a skip (local or global) */
#define MC_SET          8               /* Update a macro-time variable */
#define MC_GO           9               /* Perform a macro-time goto */
#define MC_ALTER        10              /* Alter name of secondary delimiter, etc. */
#define MC_NOTE         11              /* Macro-time debugging aid */
#define MC_PVAR         12              /* Alter number of permanent variables */
#define MC_LENG         13              /* Find length of macro-time string */
#define MC_SUB          14              /* Extract macro-time substring */
#define MC_STOP         15              /* Define a stop marker */
#if     CVARS
#define MC_CVAR         16              /* Alter number of character string variables */
#endif


/*** Values for 'bestpl' ***/

#define BP_COPY         0               /* Copy atom to output text */
#define BP_MACRO        1               /* Macro name encountered */
#define BP_WARN         2               /* Warning marker encountered */
#define BP_INSERT       3               /* Insert name encountered */
#define BP_SKIP         4               /* Skip name encountered */
#define BP_SDELIM       5               /* Scanning for secondary delimiter */
#define BP_NULL         6               /* Produce no output text */
#define BP_AWARN        7               /* After warning marker encountered */
#define BP_STACK        8               /* Place atom on forwards stack */
#define BP_STOP         9               /* Stop marker encountered */


/*** Types of construction ***/

#define TY_STOP         0               /* Stop marker */
#define TY_MACRO        1               /* Macro */
#define TY_WARN         2               /* Warning marker */
#define TY_INSERT       3               /* Insert */
#define TY_SKIP         4               /* Skip */


/*** Types of keyword in structure representations ***/

#define KW_WITH         0               /* WITH */
#define KW_WITHS        1               /* WITHS */
#define KW_OPT          2               /* OPT */
#define KW_OR           3               /* OR */
#define KW_ALL          4               /* ALL */


/*** Values for 'dbugsw'***/

#define DB_SOURCE       ((INT) 0)       /* Scanning source text */
#define DB_REPL         ((INT) 1)       /* Scanning replacement text */
#define DB_OPARG        ((INT) 2)       /* Scanning for operation macro argument */
#define DB_ROPARG       ((INT) 3)       /* Scanning text of operation macro argument or insert */
#define DB_SUBARG       ((INT) 4)       /* Scanning for substitution macro argument */
#define DB_EVAL         ((INT) 5)       /* Scanning operation macro or first insert */
#define DB_DELIM        ((INT) 6)       /* Scanning for delimiter */


/*** Scanning description block (SDB) ***/

/***
   * The variables in the SDB are copied as a block, and hence they 
   * are stored in a structure.
***/

struct sdbf {
INT     argct;                          /* Counts number of arguments when nested construction is scanned */
INT     *stakpt;                        /* Points at latest SDB on stack ('nullpt' whilst scanning source text) */
INT     *argpt;                         /* Points at argument vector (scanning replacement text) */
INT     *dbugpt;                        /* Points at orlink preceding macro name */
INT     *spt;                           /* Points at last scanned character */
INT     mchlin;                         /* Set to current value of 'linect' when a nested construction is encountered */
INT     linect;                         /* Line count of current text */
INT     argno;                          /* Number of argument or delimiter */
INT     dbugsw;                         /* Indicates state of scan */
INT     *tvarpt;                        /* Points at temporary variables */
INT     *hashpt;                        /* Points at local hash table */
INT     *mtchpt;                        /* Points at orlink when a nested construction is encountered */
INT     *stoppt;                        /* Points one beyond last character of current text */
INT     *labpt;                         /* Head of chain of labels */
INT     *inffpt;                        /* Points at start of source text on forwards stack */
INT     skval;                          /* Number of designated label for forward MC-GO */
INT     sklin;                          /* Line number in which MC-GO occurred */
INT     ohsw;                           /* TRUE if a new hash table has been stacked for this level, otherwise FALSE */
};
extern  struct  sdbf    sdb;
#define SDBSZ           18              /* Size of scanning description block */
#define EDBSZ           8               /* Size of the error description block  ('stakpt' to 'dbugsw') */


/*** Operation macro description block (OPDB) ***/

/***
   * The variables in the OPDB are copied as a block, and hence they
   * are stored in a structure.
***/

struct opdbf {
INT     *topspt;                        /* Points at latest OPDB on backwards stack */
INT     *mhshpt;                        /* Value of 'hashpt' when macro was called */
INT     *sqpt;                          /* Safe variable, miscellaneous uses */
INT     linkpt;                         /* Link for 'stkarg' */
INT     arglen;                         /* Length of value of current argument */
INT     optyp;                          /* Type, e.g. global, local, insert. Also other uses */
INT     sqnum;                          /* Safe variable, miscellaneous uses */
INT     sqsw;                           /* Safe variable, miscellaneous uses */
INT     ntypsw;                         /* Type of construction being defined */
};
extern  struct  opdbf   opdb;
#define OPDBSZ          9               /* Size of operation macro description block */


/*** Variables used in processing OPT - ALL brackets ***/

/***
   * The variables in the OPT-ALL area are copied as a block, and hence they 
   * are stored in a structure.
***/

struct oabf {
INT     *allpt;                         /* Head of chain of nextlinks to be attached to delimiter following ALL */
INT     *opthpt;                        /* Head of orlink chain */
INT     *optpt;                         /* Last entry on orlink chain */
};
extern  struct  oabf    oab;
#define ALLSZ           3               /* Size of the OPT-ALL block */


/*** Variables requiring dynamic initialisation ***/

extern  INT     *endpt;                 /* Points at end of backwards stack */
extern  INT     glbwsw;                 /* Global warning switch */
extern  INT     *lfpt;                  /* Points at last used location on backwards stack */
extern  INT     *pvarpt;                /* Points at permanent variables */
extern  INT     pvnum;                  /* Number of permanent variables */
#if     CVARS
extern  INT     *cvarpt;                /* Points at character string variables */
extern  INT     cvnum;                  /* Number of character string variables */
extern  INT     cvsize;                 /* Size of each character string variable */
extern  INT     exprsw;                 /* Returns expression type from 'gmeadd' */
#endif


/*** Miscellaneous variables ***/

extern  INT     beslin;                 /* Best-so-far value of 'linect' */
extern  INT     *bespt;                 /* Best-so-far value of 'spt' */
extern  INT     bestpl;                 /* Switch value used in basic scan routine */
extern  INT     *bfndpt;                /* Best-so-far value of 'fndpt' */
extern  INT     bindic;                 /* Best-so-far value of 'indic' */
extern  INT     *binfpt;                /* Best-so-far value of 'infopt' */
extern  INT     caltyp;                 /* First number in information block */
extern  INT     *chanpt;                /* Used in chaining */
extern  INT     chlink;                 /* Used in chaining */
extern  INT     *cllfpt;                /* Points at top entry after scanning information is stacked */
extern  INT     copdsw;                 /* For skips; reflects setting of delimiter option */
extern  INT     coptsw;                 /* For skips; reflects setting of text option */
extern  INT     *delpt;                 /* Head of chain of delimiters searching for */
extern  INT     *eriapt;                /* Points at value of operation macro argument */
extern  INT     *ffpt;                  /* Points to first free location on forwards stack */
extern  INT     *htabpt;                /* Points at current hash table */
extern  INT     idlen;                  /* Length of current identifier */
extern  INT     *idpt;                  /* Points at current identifier */
extern  INT     indic;                  /* Contents of nextlink */
extern  INT     *infopt;                /* Points beyond currently matched LID */
extern  INT     invoct;                 /* Count of macro calls */
extern  INT     *knpt;                  /* Points to node marker entry in delimiter chain */
extern  INT     levl;                   /* Level of macros and inserts */
extern  INT     masksw;                 /* Used to indicate which types of construction are recognised */
extern  INT     meval;                  /* Miscellaneous; used for numerical values calculated at macro time */
extern  INT     *nargpt;                /* Points at argument vector + 1 */
extern  INT     nestlv;                 /* Nesting level of calls and skips during scanning */
extern  INT     offset;                 /* Offset in hash table */
extern  INT     oplev;                  /* Level of operation macros and inserts */
extern  INT     skiplv;                 /* Level of skip nesting */
extern  INT     *stffpt;                /* Points at start of global macros */
extern  INT     *svarpt;                /* Points at system variables */
extern  INT     *tempt;                 /* Temporary */
extern  INT     tlinct;                 /* Temporary storage for 'linect' */


/*** Variables used in processing structure representations ***/

extern  INT     consw;                  /* For syntax checking ('evtree' and 'getdel') */
extern  INT     delct;                  /* Count of delimiters */
extern  INT     exitsw;                 /* For syntax checking ('evtree') */
extern  INT     keysw;                  /* For syntax checking ('evtree' and 'getdel') */
extern  INT     *lnodpt;                /* Points at topmost node entry on backwards stack */
extern  INT     *ndefpt;                /* Destination of newly defined construction */
extern  INT     *nnodpt;                /* Points at current node entry on backwards stack */
extern  INT     *nodept;                /* Head of chain of links to be attached to next delimiter */
extern  INT     nodesw;                 /* For syntax checking ('evtree' and 'getdel') */
extern  INT     *ollfpt;                /* Previous value of 'lfpt' */
extern  INT     optlev;                 /* Level of OPT-ALL brackets */


/*** Variables used in processing macro expressions or inserts ***/

extern  INT     *varpt;                 /* Points at vector of variables */


/*** Names of table items - dynamically initialised ***/

extern  INT     *das;                   /* Head of AS/SSAS chain for MC-DEF */
extern  INT     *delchn;                /* Chain of secondary delimiters */
extern  INT     *dge;                   /* Head of relation chain for MC-GO */
extern  INT     *dif;                   /* Head of IF/UNLESS chain for MC-GO */
extern  INT     *ghshtb[];              /* Global hash table */
extern  INT     *keychn;                /* Head of keyword chain */
extern  INT     *knrep;                 /* Points at current node marker */
extern  INT     *kspacs;                /* Points at SPACES keyword */
extern  INT     *laychn;                /* Chain of names of layout characters */
extern  INT     *spcsrp;                /* Points at representation of SPACE */


/*** More miscellaneous variables ***/

extern  INT     *at_edb;                /* \                                                  */
extern  INT     *at_sdb;                /* |__ Addresses of blocks of scanning data           */
extern  INT     *at_opdb;               /* |                                                  */
extern  INT     *at_all;                /* /                                                  */
extern  INT     *at_s1;                 /* \                                                  */
extern  INT     *at_s2;                 /* |__ Addresses of S-variables; for efficient access */
extern  INT     *at_s5;                 /* |                                                  */
#if	SPECAN
extern  INT     *at_s6;                 /* /                                                  */
#endif
extern  INT     *convarea;              /* Pointer to number conversion area for 'mdconv' */
extern  INT     nlsw;                   /* TRUE if last character read was a newline, otherwise FALSE */


/*** Machine-dependent variables ***/

extern  INT     *at_s10;                /* \                                                  */
extern  INT     *at_s12;                /* |                                                  */
extern  INT     *at_s16;                /* |                                                  */
extern  INT     *at_s17;                /* |                                                  */
extern  INT     *at_s19;                /* |-- Addresses of S-variables; for efficient access */
extern  INT     *at_s20;                /* |                                                  */
extern  INT     *at_s21;                /* |                                                  */
extern  INT     *at_s22;                /* |                                                  */
extern  INT     *at_s23;                /* |                                                  */
extern  INT     *at_s24;                /*/                                                   */

/*** Save areas for 'setjmp' calls ***/

extern  jmp_buf bssave;                 /* Transfers control to 'bsnext' */
extern  jmp_buf bstsave;                /* Transfers control to 'bstrex' */
extern  jmp_buf entsave;                /* Transfers control to 'entext' */
extern  jmp_buf evpsave;                /* Transfers control to 'evopt' */
extern  jmp_buf evrsave;                /* Transfers control to 'evor' */
extern  jmp_buf evxsave;                /* Transfers control to 'evexit' */


/*** External function definitions ***/

#if     ANSI

/* Function prototypes */

extern  INT     advnce(void);
extern  void    basic_scan(void);
extern  void    bumpff(INT);
extern  void    chatom(void);
extern  void    chekid(void);
extern  INT     ckvaly(INT *,INT);
extern  INT     cmpare(INT *);
extern  INT     decalv(void);
extern  void    declf(INT);
extern  void    encall(void);
extern  void    er1tst(void);
extern  void    erlia(void);
extern  void    erlmd(void);
extern  void    erlme(void);
extern  void    erlovf(void);
extern  void    erlso(void);
extern  void    ermtst(void);
extern  void    ersic(void);
extern  void    ersnw(void);
extern  INT     gargch(void);
extern  void    getdel(void);
extern  void    getexp(void);
extern  INT     *gmeadd(void);
extern  INT     gsatom(void);
extern  void    gtatom(void);
extern  void    init_tables(void);
extern  void    joinch(void);
extern  INT     ludel(INT *);
extern  INT     *lulayk(INT);
extern  void    macerr(INT);
extern  void    macexp(INT);
extern  void    mihalt(void);
extern  INT     milogic(INT *,INT);
#if     CVARS
extern  void    mkcroom(INT);
#endif
extern  void    mkroom(INT);
extern  void    opexit(void);
extern  void    prarg(INT);
extern  void    prctxt(void);
extern  void    prenv(void);
extern  void    prerr(void);
extern  void    prscan(void);
extern  void    prviz(void);
extern  INT     ressp(void);
extern  void    sbstpl(void);
extern  void    setpts(INT);
extern  INT     sklab(INT);
extern  void    stkhsh(void);
extern  void    subchk(void);
extern  void    tebest(void);
extern  INT     tesdel(INT *);
extern  void    tespac(void);
extern  INT     tewith(INT *);
extern  void    unopdb(void);
extern  void    unsdb(void);
extern  INT     xisalnum(INT);
extern	INT	xisalpha(INT);
extern  INT     xisdigit(INT);
extern  INT     xisupper(INT);

#else

/* These are provided mainly for documentation and for a small amount
of additional type checking. Functions with no effective result are
represented as such in order to make clear which they are. */

extern  INT     advnce();
extern  VOID    basic_scan();
extern  VOID    bumpff();
extern  VOID    chatom();
extern  VOID    chekid();
extern  INT     ckvaly();
extern  INT     cmpare();
extern  INT     decalv();
extern  VOID    declf();
extern  VOID    encall();
extern  VOID    er1tst();
extern  VOID    erlia();
extern  VOID    erlmd();
extern  VOID    erlme();
extern  VOID    erlovf();
extern  VOID    erlso();
extern  VOID    ermtst();
extern  VOID    ersic();
extern  VOID    ersnw();
extern  INT     gargch();
extern  VOID    getdel();
extern  VOID    getexp();
extern  INT     *gmeadd();
extern  INT     gsatom();
extern  VOID    gtatom();
extern  VOID    init_tables();
extern  VOID    joinch();
extern  INT     ludel();
extern  INT     *lulayk();
extern  VOID    macerr();
extern  VOID    macexp();
extern  VOID    mihalt();
extern  INT     milogic();
#if     CVARS
extern  VOID    mkcroom();
#endif
extern  VOID    mkroom();
extern  VOID    opexit();
extern  VOID    prarg();
extern  VOID    prctxt();
extern  VOID    prenv();
extern  VOID    prerr();
extern  VOID    prscan();
extern  VOID    prviz();
extern  INT     ressp();
extern  VOID    sbstpl();
extern  VOID    setpts();
extern  INT     sklab();
extern  VOID    stkhsh();
extern  VOID    subchk();
extern  VOID    tebest();
extern  INT     tesdel();
extern  VOID    tespac();
extern  INT     tewith();
extern  VOID    unopdb();
extern  VOID    unsdb();
extern  INT     xisalnum();
extern  INT     xisdigit();
extern  INT     xisupper();
#endif

/*** Machine-dependent routines ***/

#if     ANSI
extern  VOID    bmove(INT,INT *,INT *); /* Moves blocks of data, starting at their end */
extern  VOID    fmove(INT,INT *,INT *); /* Moves blocks of data, starting at their start */
extern  void    mdconv(INT);            /* Converts numbers to strings */
extern  INT     mddiv(INT,INT);         /* Performs division */
extern  void    mderid(void);           /* Outputs atom to the debugging file */
extern  void    mderpr(char *,...);     /* Outputs messages to the debugging file */
extern  void    mdfinal(void);          /* Performs machine-dependent finalisation */
extern  INT     *mdfind(void);          /* The hashing function */
extern  void    mdinit(void);           /* Machine-independent initialisation */
extern  INT     mdnum(void);            /* Converts strings to numbers */
extern  void    mdouch(INT);            /* Main output routine */
extern  INT     mdread(void);           /* Main input routine */

#else

extern  VOID    bmove();                /* Moves blocks of data, starting at their end */
extern  VOID    fmove();                /* Moves blocks of data, starting at their start */
extern  VOID    mdconv();               /* Converts numbers to strings */
extern  INT     mddiv();                /* Performs division */
extern  VOID    mderid();               /* Outputs atom to the debugging file */
extern  VOID    mderpr();               /* Outputs messages to the debugging file */
extern  VOID    mdfinal();              /* Performs machine-dependent finalisation */
extern  INT     *mdfind();              /* The hashing function */
extern  VOID    mdinit();               /* Machine-independent initialisation */
extern  INT     mdnum();                /* Converts strings to numbers */
extern  VOID    mdouch();               /* Main output routine */
extern  INT     mdread();               /* Main input routine */
#endif

/*
 ************************************
 *                                  *
 *   End of common header file      *
 *                                  *
 ************************************
 */

