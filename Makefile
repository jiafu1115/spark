# petter wahlman <petter@wahlman.no>

CXX     = clang++

OBJDIR  = ./obj
OUTDIR  = ./bin
VPATH   = ./src

OUTPUT  = $(OUTDIR)/spark

INCLUDE = -I ./include \
	  -I ../libspark/include

CXXFLAGS= -std=c++11 \
	  -MMD -MP \
	  -g \
	  -Wall \
	  -Wno-deprecated-declarations \
	  $(INCLUDE)

LIBRARIES = ../libspark/lib/libspark.a

LDFLAGS = -lcurl \
	  $(LIBRARIES)

$(OBJDIR)/%.o: %.cpp Makefile
	$(CXX) -c $< -o $@ $(CXXFLAGS)

OBJECTS = \
	$(OBJDIR)/main.o \
	$(OBJDIR)/param.o \

.PHONY:
all:    make_dirs $(OUTPUT)

$(OUTPUT): $(OBJECTS) $(LIBRARIES)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)

.PHONY:
clean:
	@rm -rvf \
		$(OUTPUT) \
		$(OBJDIR)/*.o

.PHONY:
make_dirs:
	@mkdir -p $(OBJDIR) \
	          $(OUTDIR)

.PHONY:
install: all
	@cp -v $(OUTPUT) ~/local/bin


-include $(OBJECTS:.o=.d)
