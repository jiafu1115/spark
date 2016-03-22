# petter wahlman <pwahlman@cisco.com>

CC      = gcc
CXX     = clang++

SRCDIR  = ./src
OBJDIR  = ./obj
BINDIR  = ./bin
INCLUDE = -I ./include -I ../libspark/include

BINARY  = $(BINDIR)/spark

CFLAGS  = $(DEBUG) $(INCLUDE) -g -Wall -Wno-deprecated-declarations -std=gnu99
CXXFLAGS= $(DEBUG) $(INCLUDE) -g -Wall -std=c++11 -Wno-deprecated-declarations
LDFLAGS = -lcurl ../libspark/lib/libspark.a

VPATH   = $(SRCDIR)

$(OBJDIR)/%.o: %.c Makefile
	$(CC) -c $< -o $@ $(CFLAGS)

$(OBJDIR)/%.o: %.cpp Makefile
	$(CXX) -c $< -o $@ $(CXXFLAGS)

BINARY_OBJ = \
	$(OBJDIR)/main.o \
	$(OBJDIR)/param.o \

.PHONY:
all:    make_dirs $(BINARY) 

$(BINARY): $(BINARY_OBJ) ../libspark/lib/libspark.a
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

.PHONY:
clean:
	@/bin/rm -rvf \
		$(BINARY) \
		$(OBJDIR)/*.o

.PHONY:
make_dirs:
	@mkdir -p $(OBJDIR) $(BINDIR)

.PHONY:
install: all
	@cp -v $(BINARY) ~/local/bin


