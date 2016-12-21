CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++11 -fwrapv -I/usr/local/opt/readline/include
DEBUG=1
ifdef DEBUG
	CXXFLAGS += -g
else
	CXXFLAGS += -O2
endif
LDFLAGS = -L/usr/local/opt/readline/lib -lreadline

TARGETS = kaash
CXX_FILES = $(wildcard *.cpp)
H_FILES = *.h
OBJ_FILES = $(patsubst %.cpp,%.o,$(CXX_FILES))

.PHONY: all clean remake

all: $(TARGETS)

clean:
	rm -f $(TARGETS) $(OBJ_FILES)

remake: clean all

$(TARGETS): %: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<
