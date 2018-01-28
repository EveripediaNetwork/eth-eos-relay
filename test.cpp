#include <iostream>
#include <string>
#include <typeinfo>
#include "custom-headers/json.hpp"
#include "custom-headers/constants.hpp"
#include "custom-headers/verify_blocks.hpp"
#include <list>
#include <vector>

using json = nlohmann::json;

static std::vector<unsigned char> HexToBytes(const std::string& hex) {
        std::vector<unsigned char> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                unsigned char byte = (unsigned char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
        }

        return bytes;
}

static void RLPtoStringStream(std::stringstream& _out, dev::RLP const& _d, unsigned _depth = 0) {
        if (_depth > 64)
		_out << "<max-depth-reached>";
	else if (_d.isNull())
		_out << "null";
	else if (_d.isInt())
		_out << std::showbase << std::hex << std::nouppercase << _d.toInt<bigint>(RLP::LaissezFaire) << dec;
	else if (_d.isData())
		_out << escaped(_d.toString(), false);
	else if (_d.isList())
	{
		_out << "[";
		int j = 0;
		for (auto i: _d)
		{
			_out << (j++ ? ", " : " ");
			RLPtoStringStream(_out, i, _depth + 1);
		}
		_out << " ]";
	}
}

static std::string RLPtoString(dev::RLP const& _d, unsigned _depth = 0) {
        std::stringstream _out;
        RLPtoStringStream(_out, _d, 0);
        return _out.str();
}

bool getTransactionProof (const std::string& txHash){
        std::cout << "Received tx hash: " <<  txHash << '\n';

        // Need code here to fetch the transaction's entire block given the txID
        // ...
        // ...

        // Would probably need a for loop here
        // Need to create tuples of the form (txIndex, HexToBytes(rawTransaction without the leading 0x))
        //  1 transaction
        // https://etherscan.io/getRawTx?tx=0x5da0298e46e949f863f0873b6f8c102e150dc85d31f4d7c6d7c82cb69c8a672e
        //  2 transactions
        // https://www.etherchain.org/block/4052768

        bytes* bytedEntireBlock_4699999 = new bytes(HexToBytes(BLOCK_RLP_STRING_4699999));
        bytes* bytedEntireBlock_4700000 = new bytes(HexToBytes(BLOCK_RLP_STRING_4700000));
        // dev::RLP* entireBlockRLP_4699999 = new dev::RLP(*bytedEntireBlock_4699999);
        dev::RLP* entireBlockRLP_4700000 = new dev::RLP(*bytedEntireBlock_4700000);
        dev::RLP* headerRLPs = new dev::RLP(entireBlockRLP_4700000[0][0]);
        // std::cout << "---------------------------\nheaderRLPs: " << headerRLPs[0] << "\n---------------------------\n" << std::endl;

        dev::RLP* transactionRLPs = new dev::RLP(entireBlockRLP_4700000[0][1]);
        // std::cout << "---------------------------\ntransactionRLPs: " << transactionRLPs[0] << "\n---------------------------\n" << std::endl;

        // Print out the raw bytes
        for (int i = 0; i < transactionRLPs[0].itemCount(); i++ ){
                // std::cout << transactionRLPs[0][i].data().toBytes() << std::endl;
        }

        auto expectedRootHex = trieRootOver(transactionRLPs[0].itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return transactionRLPs[0][i].data().toBytes(); });
        std::string expectedRoot = toString(expectedRootHex);

        std::string txRootString = RLPtoString(headerRLPs[0][4]).erase(0,2);
        transform(txRootString.begin(), txRootString.end(), txRootString.begin(), ::tolower);
        std::cout << "Tx Root From Header: " << txRootString << std::endl;
        std::cout << "Expected Root: " << expectedRoot << std::endl;

        bool txRootsMatch = false;
        if((txRootString.compare(expectedRoot)) == 0){
                txRootsMatch = true;
                cout << "Calculated tx root matches the one in the header." << std::endl;
        }
        else{
                cout << "Calculated tx root does not match the one given in the header. " << std::endl;
                txRootsMatch = false;
        }

        // dev::eth::BlockHeader* blockHeaderObj_4699999 = new dev::eth::BlockHeader(entireBlockRLP_4699999[0].data().toBytes());
        dev::eth::BlockHeader* blockHeaderObj_4700000 = new dev::eth::BlockHeader(entireBlockRLP_4700000[0].data().toBytes());
        // cout << "BYTES_4699999: " << blockHeaderObj_4699999->transactionsRoot() << endl;
        cout << "Tx Root for 4700000 from its BlockHeader Object: " << blockHeaderObj_4700000->transactionsRoot() << endl;

        // cout << "BYTES: " << blockHeaderObj->transactionsRoot() << endl;




        // delete bytedEntireBlock_4699999;
        delete bytedEntireBlock_4700000;
        // delete entireBlockRLP_4699999;
        delete entireBlockRLP_4700000;
        delete headerRLPs;
        delete transactionRLPs;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
