#
# Makefile for ML/I on FreeBSD
#
# Bob Eager   September 2018
#
# Macro definitions
#
PRODUCT=	ml1
TARGET=		/usr/local
#
SRC=		md.c mi1.c mi2.c mi3.c mi4.c mi5.c mi6.c
HDR=		ml1.h
OBJ=		$(SRC:.c=.o)
#
DOC=		doc/*
MANSRC=		$(PRODUCT).1
MAN=		$(MANSRC).gz
MISC=		Makefile distname
PRIVMISC=	TODO
TESTS=		tests/*
#
DISTNAME!=	./distname $(PRODUCT)
ARCH=		$(DISTNAME)-arch.zip
#
# Do NOT enable optimisation, as ML/I fools the optimiser!
CFLAGS=		-std=c99 -Wall
#
#----------------------------------------------------------------------
#
all:		$(PRODUCT) $(MAN)

$(PRODUCT):	$(OBJ)
		$(CC) -o $(PRODUCT) $(OBJ)
#
depend:
		mkdep $(CFLAGS) $(SRC)
#
$(MAN):		$(MANSRC)
		gzip < $(PRODUCT).1 > $(MAN)
#
install:	$(PRODUCT) $(MAN) $(DOC)
		install $(PRODUCT) $(TARGET)/bin
		install $(MAN) $(TARGET)/man/man1
		@mkdir -p $(TARGET)/share/doc/$(PRODUCT)
		install $(DOC) $(TARGET)/share/doc/$(PRODUCT)
#
distfile:	$(SRC) $(HDR) $(MANSRC) $(DOC) $(MISC)
		@rm -rf $(DISTNAME)
		@mkdir $(DISTNAME)
		cp $(SRC) $(HDR) $(MANSRC) $(MISC) $(DISTNAME)/
		@sed \
			-e '/#define	FBSD32/s/0/1/' \
			-e '/#define	FBSD64/s/1/0/' \
			-i '' \
			$(DISTNAME)/ml1.h
		@mkdir -p $(DISTNAME)/doc
		cp $(DOC) $(DISTNAME)/doc
		@rm -f $(DISTNAME).tar.gz
		tar acvf $(DISTNAME).tar.gz $(DISTNAME)
		@rm -rf $(DISTNAME)
#
arch:		$(SRC) $(HDR) $(MANSRC) $(DOC) $(MISC) $(PRIVMISC) $(TESTS)
		@rm -f $(ARCH)
		zip -9 -r $(ARCH) $(.ALLSRC)
#
clean:
		rm -f $(OBJ) *.core
#
clean-all:
		rm -f $(OBJ) *.core $(PRODUCT) $(MAN)
#
# End of makefile for ML/I
#
