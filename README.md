# eth-eos-relay

A block header relay supporting the ability to run ETH transactions on EOS

Much of the code taken from cpp-ethereum
http://www.ethdocs.org/en/latest/ethereum-clients/cpp-ethereum/building-from-source/linux.html#building-linux-from-source

git clone --recursive https://github.com/ethereum/cpp-ethereum.git
cd cpp-ethereum

To run on Ubuntu 16.04
1) sudo apt-get install libboost-all-dev libleveldb-dev libcurl4-openssl-dev libmicrohttpd-dev libminiupnpc-dev libgmp-dev scrypt libssl-dev
2) make
3) ./main

The provided code takes the RLP encoded blocks 4699999 4700000 and calculates the difficulty and the MixHash.

TODO: Pre-generate the caches
