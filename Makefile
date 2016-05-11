CXXFLAGS = -Wall -std=c++11 -I/usr/local/opt/readline/include
ifdef DEBUG
	CXXFLAGS += -g
else
	CXXFLAGS += -O2
endif
LDFLAGS = -L/usr/local/opt/readline/lib -lreadline
CXX = g++

BINARIES = kaash
CXX_FILES = $(shell echo *.cpp)
H_FILES = *.h
OBJ_FILES = $(CXX_FILES:.cpp=.o)

.PHONY: all clean remake

all: $(BINARIES)

clean:
	rm -f $(BINARIES) $(OBJ_FILES)

remake: clean all

kaash: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ_FILES)
ifndef DEBUG
	strip $@
endif

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c -o $*.o $*.cpp
