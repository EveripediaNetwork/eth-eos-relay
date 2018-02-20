# eth-eos-relay

A block header relay supporting the ability to run ETH transactions on EOS

Much of the code taken from cpp-ethereum
http://www.ethdocs.org/en/latest/ethereum-clients/cpp-ethereum/building-from-source/linux.html#building-linux-from-source

To run on Ubuntu 16.04
1) sudo apt-get install libboost-all-dev libleveldb-dev libcurl4-openssl-dev libmicrohttpd-dev libminiupnpc-dev libgmp-dev scrypt libssl-dev
2) make
3) ./main

The provided code takes the RLP encoded blocks 4699999 4700000 and calculates the difficulty and the MixHash for the latter.

The first run will generate a verification cache and save it to the cache/ folder for future runs. 
The second run and onward will use the pre-generated cache to verify new headers
The cache must be re-computed every 30000 blocks per Ethereum rules.
