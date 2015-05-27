xed_test: xed_test.o
	g++ -g -o xed_test xed_test.o /home/user/Documents/pin-2.14-71313-gcc.4.4.7-linux/extras/xed-ia32/lib/libxed.a -std=c++11
xed_test.o: xed_test.cpp
	g++ -g -c xed_test.cpp -I /home/user/Documents/pin-2.14-71313-gcc.4.4.7-linux/extras/xed-ia32/include/ -std=c++11

clean:
	rm xed_test xed_test.o
