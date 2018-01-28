# 	g++ test.o -pthread -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_thread -lboost_random -lsecp256k1 -lcrypto -lscrypt -o runner


runner: test.o
	g++ test.o -pthread -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_thread -o runner
	rm *.o

test.o: test.cpp
	g++ -I . -c test.cpp

clean:
	rm *.o runner
