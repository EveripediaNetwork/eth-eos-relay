#include <iostream>
#include <string>
#include <typeinfo>
#include "custom-headers/json.hpp"
#include "custom-headers/constants.hpp"
#include "custom-headers/verify_blocks.hpp"
#include <list>
#include <vector>



using json = nlohmann::json;

// g++ -I . -c test.cpp -o out.o && g++ out.o -L/usr/lib/x86_64-linux-gnu/libboost_system.so -L/usr/lib/x86_64-linux-gnu/libboost_thread.so -o runner && ./runner

// g++ -I . -c test.cpp -o out.o && g++ out.o -pthread -lboost_filesystem -o runner && ./runner


bool getTransactionProof (const std::string& txHash){
        std::cout << "Received tx hash: " <<  txHash << '\n';

        // Need code here to get the raw transaction data from the the Ethereum blockchain
        // ...
        // ...

        //  Convert the tx data into JSON
        std::cout << "Parsing now..." << "\n-----------------------------\n";
        auto txHashJSON = json::parse(BLOCK_ONE_TX_STRING);
        long long txBlockNum = txHashJSON["blockNumber"];
        // std::cout << txHashJSON.dump(4) << "\n-----------------------------\n"<< std::endl;
        std::cout << "The block # for this transaction is: " <<  std::to_string(txBlockNum) << '\n';
        std::cout << "Fetching the block data..." << '\n';

        // Need code here to get the block data from the the Ethereum blockchain
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

        dev::RLPStream* txRLPStream = new dev::RLPStream();

        std::vector<std::string> txHashVector {};


        for ( json::iterator index = blockTransactions.begin(); index != blockTransactions.end() ; ++index ) {
                json txJSON = *index;
                std::string txHash = txJSON["hash"];
                int txIndex = txJSON["transactionIndex"];
                std::cout << txHash << " | Index: " << txIndex << std::endl;

                txHashVector.push_back (txHash);


                // std::string txIndexRLP = rlp_encode(txIndex);
                // std::cout << txJSON["hash"] << " | Index RLP: " << txIndexRLP << std::endl;
        }

        txRLPStream->appendVector(txHashVector);
        bytes bytedTxRLPStream = txRLPStream->out();
        // dev::RLP* txRLPList = new dev::RLP(bytedTxRLPStream, dev::RLP::LaissezFaire);
        dev::RLP txList = dev::RLP(bytedTxRLPStream);



        std::cout << "---------------------------\n" << toHex(bytedTxRLPStream) << "---------------------------\n" << std::endl;
        // std::cout << "------------Item-Count: " << txList.itemCount() << "---------------------------\n" << std::endl;
        auto expectedRoot = trieRootOver(txList.itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return txList[i].data().toBytes(); });
        std::string rootString = toString(expectedRoot);

        std::cout << "---------------------------\n" << rootString << "---------------------------\n" << std::endl;

	MemoryDB tm;
	GenericTrieDB<MemoryDB> transactionsTrie(&tm);
	transactionsTrie.init();

	vector<bytesConstRef> txs;

	for (unsigned i = 0; i < txList.itemCount(); ++i)
	{
		RLPStream k;
		k << i;

		transactionsTrie.insert(&k.out(), txList[i].data());

		txs.push_back(txList[i].data());
		std::cout << toHex(k.out()) << toHex(txList[i].data());
	}
	std::cout << "trieRootOver" << expectedRoot  << std::endl;
	std::cout << "orderedTrieRoot" << orderedTrieRoot(txs)  << std::endl;
	std::cout << "TrieDB" << transactionsTrie.root()  << std::endl;
	std::cout << "Contents:"  << std::endl;
	for (auto const& t: txs)
		std::cout << toHex(t)  << std::endl;

        delete txRLPStream;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
