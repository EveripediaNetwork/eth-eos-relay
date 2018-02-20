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
    EthereumAddress(Public pubkey) {
        h256 hash = sha3(pubkey.ref());
        std::copy(hash.data() + 12, hash.data() + 32, m_data);
        cout << "pubkey hash " << hash << endl;
    }
    
    byte m_data[20];

    std::string hex()
    {
        std::stringstream ss;
        ss << std::hex;
        for(int i=0;i<20;++i)
            ss << std::setw(2) << std::setfill('0') << (int)m_data[i];
        return ss.str();
    }
};

std::string hexStrB(byte * data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

class EthereumTx {
public:
    EthereumTx(RLP tx_rlp) {
        id = sha3(tx_rlp.data());
        nonce = uint64_t(tx_rlp[0]);
        gasPrice = uint64_t(tx_rlp[1]);
        gasLimit = uint64_t(tx_rlp[2]);
        to = EthereumAddress(tx_rlp[3].data());
        value = u256(tx_rlp[4]);
        data = tx_rlp[5].data();

        v = tx_rlp[6].data()[0];
        r = tx_rlp[7].data();
        s = tx_rlp[8].data();

        RLPStream rstream;
        rstream.appendList(9);
        for (int i=0; i<6; i++)
            rstream.appendRaw(tx_rlp[i].data());
        rstream.append(1);
        rstream.append("");
        rstream.append("");
        RLP sig_rlp = RLP(rstream.out());
        bytes b = sig_rlp.data().toBytes();
        cout << "signing rlp " << hexStrB(&b[0], b.size()) << endl;

        cout << "TX RLP " << tx_rlp << endl;
        cout << "SIG RLP " << sig_rlp << endl;

        Signature sig = signature();
        h256 signing_hash = sha3(sig_rlp.data());
        sig.data()[64] = 0; // v must be set to 0 just for the recovery
        Public pubkey = recover(sig, signing_hash);

        cout << "pubkey " << pubkey << endl;
        from = EthereumAddress(pubkey);
    }

    Signature signature () {
        h520 sig;
        std::copy(r.begin() + 1, r.end(), sig.data());
        std::copy(s.begin() + 1, s.end(), sig.data() + 32);
        *(sig.data() + 64) = v;
        return sig;
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
    os << "  from " << tx.from.hex() << endl;
    os << "  to " << tx.to.hex() << endl;
    os << "  nonce " << tx.nonce << endl;
    os << "  gas price " << tx.gasPrice << endl;
    os << "  gas limit " << tx.gasLimit << endl;
    os << "  value " << tx.value << endl;
    os << "  signature " << tx.signature() << endl;
};
