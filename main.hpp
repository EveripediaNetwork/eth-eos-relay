#include "core_imports.hpp"
#include "core_functions.cpp"

typedef unsigned char byte;
typedef h256 EthereumTxId;

class EthereumAddress {
public:
    EthereumAddress() {}
    EthereumAddress(bytesConstRef a) {
        assert(a.size() == 20 || a.size() == 21);
        if (a.size() == 20)
            copy(a.begin(), a.end(), m_data);
        else if (a.size() == 21)
            copy(a.begin() + 1, a.end(), m_data);
    }
    byte m_data[20];
};

class EthereumTx {
public:
    EthereumTx(RLP tx_rlp) {
        id = sha3(tx_rlp.data());
        nonce = uint64_t(tx_rlp[0]);
        gasPrice = uint64_t(tx_rlp[1]);
        gasLimit = uint64_t(tx_rlp[2]);
        to = EthereumAddress(tx_rlp[3].data());
        value = u256(tx_rlp[4]);
        v = tx_rlp[5].data()[0];
        r = tx_rlp[6].data();
        s = tx_rlp[7].data();
    }

    EthereumTxId id;
	EthereumAddress from;
	EthereumAddress to;
	u256 value;
	uint64_t gasLimit;
	uint64_t gasPrice;
	uint64_t nonce;
	byte v;
	bytesConstRef r; // size=32
	bytesConstRef s; // size=32
	bytesConstRef init;
	bytesConstRef data;
};

ostream& operator<<(ostream& os, const EthereumTx& tx)  
{
    os << "Ethereum Tx " << tx.id << endl;
    os << "  nonce " << tx.nonce << endl;
    os << "  gas price " << tx.gasPrice << endl;
    os << "  gas limit " << tx.gasLimit << endl;
    os << "  value " << tx.value << endl;
}

