CXX=g++
CXXFLAGS=-Wall --pedantic-errors -O2

stdr: stdr.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f ./stdr
