# 	g++ test.o -pthread -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_thread -lboost_random -lsecp256k1 -lcrypto -lscrypt -o runner


runner: main.o
	g++ main.o -pthread -L/usr/lib/x86_64-linux-gnu/ -lboost_system -lboost_thread -lboost_random -lboost_filesystem -lboost_context -lboost_fiber -lboost_program_options -lcryptopp -ljson_spirit -L/usr/local/lib/ -lscrypt -o runner
	rm *.o

main.o: main.cpp
	g++ -I . -c main.cpp -fpermissive -DHAVE_CONFIG_H

clean:
	rm *.o runner
