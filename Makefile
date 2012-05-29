# qrivers@ucsc.edu
# Based in part on a Makefile by Prof. W. Mackey

CSOURCE   = slab.c freelist.c buddy.c allocation.c
CHEADER   = space.h

TESTSRC   = test1.c

LIBRARY   = libmem.a
AROPT     = rvs

GCCOPT    = -Wall -Wextra
OBJECTS   = ${CSOURCE:.c=.o}
#EXECBIN   =
# TESTOPT   = 

FILES     = ${CSOURCE} ${CHEADER} Makefile README design.pdf report.pdf
ASGNAME   = asg3
TAROPT    = -czf
TAREXT    = .tar.gz

RSYNCPATH = ~/private/cs111/${ASGNAME}/
RSYNCOPT  = -r

all : ${LIBRARY} clean

${LIBRARY} : ${OBJECTS}
	ar ${AROPT} $@ ${OBJECTS}

%.o : %.c
	gcc ${GCCOPT} -c $<

clean : 
	- rm ${OBJECTS}
	
spotless : clean
	- rm ${LIBRARY}

sync :
	rsync ${RSYNCOPT} qrivers@unix.ic.ucsc.edu:${RSYNCPATH} ./

testrun : all
	gcc test1.c libmem.a -o test1
	./test1

tar : spotless
	- rm ${ASGNAME}.${TAREXT}
	tar ${TAROPT} ${ASGNAME}${TAREXT} ./
