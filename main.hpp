#include "core_imports.hpp"
#include "core_functions.cpp"

typedef unsigned char byte;
typedef byte EthereumTxId[32];
typedef byte EthereumAddress[20];

class EthereumTx {
	EthereumAddress from;
	EthereumAddress to;
	uint64_t value;
	uint64_t gasLimit;
	uint64_t gasPrice;
	uint64_t nonce;
	byte v;
	byte r[32];
	byte s[32];
	std::vector<byte> init;
	std::vector<byte> data;
};
