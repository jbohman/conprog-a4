options = -g -O2 -lgecodesearch -lgecodeint -lgecodekernel -lgecodesupport -lgecodeminimodel -lgecodegist -lgecodedriver -lQtCore -lQtGui  -I/usr/include/qt4 

squarepacking: squarepacking.cpp interval.cpp
	g++ $(options) squarepacking.cpp -o squarepacking

gameoflife: gameoflife.cpp
	g++ $(options) gameoflife.cpp -o gameoflife
