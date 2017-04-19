SYSTEM     = x86-64_linux
LIBFORMAT  = static_pic
CPP      = g++

CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Studio1271/cplex
CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Studio1271/concert
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCLNFLAGS = -lconcert -lilocplex -lcplex -lm -lpthread

mst: main.cpp
	$(CPP) -o mst main.cpp -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)  -L$(CPLEXLIBDIR) -L$(CONCERTLIBDIR)  $(CCLNFLAGS) -std=c++11 -m64 -DIL_STD -lemon
