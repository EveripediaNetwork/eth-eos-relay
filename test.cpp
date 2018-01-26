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

bool getTransactionProof (const std::string& txHash){
        std::cout << "Received tx hash: " <<  txHash << '\n';

        // Need code here to get the raw transaction data from the the Ethereum blockchain
        // ...
        // ...

        //  Convert the block data into JSON
        auto txBlockJSON = json::parse(BLOCK_ONE_BLOCK_STRING);
        // std::cout << txBlockJSON.dump(4) << std::endl;
        std::cout << "\n-----------------------------\n"<< std::endl;

        // radix_tree<std::string, int> tree;

        json blockTransactions = txBlockJSON["transactions"];

        // for (auto txID : txBlockJSON["transactions"]){
        //         auto innerTxJSON = json::parse(txID);
        //         std::cout << innerTxJSON["hash"] << '\n';
        // }




        // needs to be of form (index, rawtransaction)
        // https://etherscan.io/getRawTx?tx=0x5da0298e46e949f863f0873b6f8c102e150dc85d31f4d7c6d7c82cb69c8a672e

        dev::RLPStream* txRLPStream = new dev::RLPStream();
        // std::vector<std::string> txHashVector {};

        //
        // for ( json::iterator index = blockTransactions.begin(); index != blockTransactions.end() ; ++index ) {
        //         json txJSON = *index;
        //         std::string txHash = txJSON["hash"];
        //         int txIndex = txJSON["transactionIndex"];
        //         std::cout << txHash << " | Index: " << txIndex << std::endl;
        //
        //         txHashVector.push_back (txHash);
        //
        //
        //         // std::string txIndexRLP = rlp_encode(txIndex);
        //         // std::cout << txJSON["hash"] << " | Index RLP: " << txIndexRLP << std::endl;
        // }

        //REMOVE THE 0x FOR THE RAW TRANSACTION
        std::vector<byte> byteString = HexToBytes("f88b821a418504a817c800830249f09418a672e11d637fffadccc99b152f4895da06960180a4e56c85520000000000000000000000008c6d052fc566b50aa6ecf5fccc300e2f42b0105d25a09f820dabb270f2e62c376677e33d3aed0b7f6063f5865828baed2729196da6c7a04cc1f88a8fd948c31aeb75d93efdd6f8e22a9bbf2dc1523061d5760b5d76143b");

        std::string raw = "0xf88b821a418504a817c800830249f09418a672e11d637fffadccc99b152f4895da06960180a4e56c85520000000000000000000000008c6d052fc566b50aa6ecf5fccc300e2f42b0105d25a09f820dabb270f2e62c376677e33d3aed0b7f6063f5865828baed2729196da6c7a04cc1f88a8fd948c31aeb75d93efdd6f8e22a9bbf2dc1523061d5760b5d76143b";


        std::vector<std::string> txHashVector {("0", raw)};
        txRLPStream->appendVector(txHashVector);
        bytes bytedTxRLPStream = txRLPStream->out();
        // dev::RLP* txRLPList = new dev::RLP(bytedTxRLPStream, dev::RLP::LaissezFaire);
        dev::RLP txList = dev::RLP(bytedTxRLPStream);


        // std::cout << "---------------------------\n" << toHex(bytedTxRLPStream) << "---------------------------\n" << std::endl;
        // std::cout << "------------Item-Count: " << txList.itemCount() << "---------------------------\n" << std::endl;
        // auto expectedRoot = trieRootOver(txList.itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return txList[i].data().toBytes(); });
        auto expectedRoot = trieRootOver(txList.itemCount(), [&](unsigned i){ return rlp(i); }, byteString);
        std::string rootString = toString(expectedRoot);
        std::cout << "---------------------------\nExpected Root: " << rootString << "---------------------------\n" << std::endl;



	// MemoryDB tm;
	// GenericTrieDB<MemoryDB> transactionsTrie(&tm);
	// transactionsTrie.init();
        //
	// vector<bytesConstRef> txs;
        //
	// for (unsigned i = 0; i < txList.itemCount(); ++i)
	// {
	// 	RLPStream k;
	// 	k << i;
        //
	// 	transactionsTrie.insert(&k.out(), txList[i].data());
        //
	// 	txs.push_back(txList[i].data());
	// 	std::cout << toHex(k.out()) << toHex(txList[i].data());
	// }
	// std::cout << "trieRootOver" << expectedRoot  << std::endl;
	// std::cout << "orderedTrieRoot" << orderedTrieRoot(txs)  << std::endl;
	// std::cout << "TrieDB" << transactionsTrie.root()  << std::endl;
	// std::cout << "Contents:"  << std::endl;
	// for (auto const& t: txs)
	// 	std::cout << toHex(t)  << std::endl;

        delete txRLPStream;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
