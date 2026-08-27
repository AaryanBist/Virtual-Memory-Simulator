CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
# -static-lib{gcc,stdc++}: on Windows, avoids crashes caused by picking up a
# mismatched libstdc++-6.dll when multiple MinGW toolchains are on PATH.
LDFLAGS := -static-libgcc -static-libstdc++

SRC := src/main.cpp src/Simulator.cpp src/Trace.cpp
TARGET := vmsim

.PHONY: all clean trace run

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

trace:
	python generate_trace.py

run: all
	./$(TARGET) trace.txt

clean:
	rm -f $(TARGET) $(TARGET).exe
