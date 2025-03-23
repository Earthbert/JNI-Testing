CXX = g++
CXX_FLAGS = -g
INCLUDE = -I${JAVA_HOME}/include -I${JAVA_HOME}/include/linux
LIB = -L${JAVA_HOME}/lib/server -ljvm

build: Test.cpp
	$(CXX) $(CXX_FLAGS) $(INCLUDE) $^ $(LIB) -o Test

run: build
	LD_PRELOAD=${JAVA_HOME}/lib/server/libjvm.so ./Test
