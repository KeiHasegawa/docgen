SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DOCGEN_DLL = docgen.dll

XX_OBJS = $(SRCS:.cpp=.obj)
DOCGEN_XX_DLL = docgenxx.dll

DEBUG_FLAG = -g
PIC_FLAG = -fPIC
CXXFLAGS = $(DEBUG_FLAG) $(PIC_FLAG) -I$(HCC1_SRCDIR) -w
CXXFLAGS_FOR_XX = $(DEBUG_FLAG) $(PIC_FLAG) -I$(HCXX1_SRCDIR) -w \
-DCXX_GENERATOR

DLL_FLAG =  -shared

all:$(DOCGEN_DLL) $(DOCGEN_XX_DLL)

$(DOCGEN_DLL) : $(OBJS)
	$(CXX) $(DEBUG_FLAG) $(PROF_FLAG) $(DLL_FLAG) -o $@ $(OBJS)

$(DOCGEN_XX_DLL) : $(XX_OBJS)
	$(CXX) $(DEBUG_FLAG) $(PROF_FLAG) $(DLL_FLAG) -o $@ $(XX_OBJS)

%.obj : %.cpp
	$(CXX) $(CXXFLAGS_FOR_XX) $< -o $@ -c

RM = rm -r -f

clean:
	$(RM) $(DOCGEN_DLL) $(OBJS) *~
	$(RM) .vs Debug Release x64 DebugCXX ReleaseCXX
