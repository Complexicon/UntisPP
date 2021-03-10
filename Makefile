LIBS=ssl crypto crypt32 ws2_32
DEBUG=y
NAME=untis
SHARED=y
IDIR=include
LDIR=lib
CC=g++
64BIT=n
OPTIMIZELEVEL=2
CFLAGS=--std=c++17
LFLAGS=
SRC=untis.cpp rpc.cpp httplib.cpp

ODIR=obj
SRCDIR=src
OUTFILE=

ifeq ($(DEBUG),y)
	CFLAGS+=-g
	LFLAGS+=-g
else
	CFLAGS+=-O$(OPTIMIZELEVEL)
	LFLAGS+=-O$(OPTIMIZELEVEL)
endif

ifeq ($(64BIT),n) 
	64BIT:=-m32
	NAME:=$(NAME)32
endif

ifeq ($(SHARED),y) 
	LFLAGS+=-shared
	OUTFILE:=$(NAME).dll
else
	OUTFILE:=$(NAME).exe
endif

LDIR?=.
IDIR?=.

$(ODIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) -c -o $@ $< $(64BIT) $(CFLAGS) -I$(IDIR)

build: $(patsubst %.cpp,$(ODIR)/%.o,$(SRC))
	$(CC) -o $(OUTFILE) $^ $(64BIT) $(LFLAGS) -L$(LDIR) $(patsubst %,-l%,$(LIBS))

test: build $(patsubst %.cpp,$(ODIR)/%.o,test.cpp)
	$(CC) -o test.exe $(64BIT) $(filter-out $<,$^) -L. -l$(NAME)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(OUTFILE)