SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DOCGEN_DLL = docgen.dll

DEBUG_FLAG = -g
PIC_FLAG = -fPIC
CXXFLAGS = $(DEBUG_FLAG) $(PIC_FLAG) -I$(HCC1_SRCDIR) -w

DLL_FLAG =  -shared

all:$(DOCGEN_DLL)

$(DOCGEN_DLL) : $(OBJS)
	$(CXX) $(DEBUG_FLAG) $(PROF_FLAG) $(DLL_FLAG) -o $@ $(OBJS)

RM = rm -r -f

clean:
	$(RM) $(DOCGEN_DLL) $(OBJS) *~
	$(RM) .vs Debug Release x64
