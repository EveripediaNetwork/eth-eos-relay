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

        // Need code here to fetch the transaction's entire block given the txID
        // ...
        // ...

        // Would probably need a for loop here
        // Need to create tuples of the form (txIndex, HexToBytes(rawTransaction without the leading 0x))
        //  1 transaction
        // https://etherscan.io/getRawTx?tx=0x5da0298e46e949f863f0873b6f8c102e150dc85d31f4d7c6d7c82cb69c8a672e
        //  2 transactions
        // https://www.etherchain.org/block/4052768

        bytes* bytedEntireBlock= new bytes(HexToBytes(BLOCK_RLP_STRING));
        dev::RLP* entireBlockRLP = new dev::RLP(bytedEntireBlock);
        dev::RLP* transactionRLPs = new dev::RLP(entireBlockRLP[0][1]);
        std::cout << "---------------------------\ntransactionRLPs: " << transactionRLPs[0] << "---------------------------\n" << std::endl;

        // Print out the raw bytes
        for (int i = 0; i < transactionRLPs[0].itemCount(); i++ ){
                std::cout << transactionRLPs[0][i].data().toBytes() << std::endl;
        }

        auto expectedRoot = trieRootOver(transactionRLPs[0].itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return transactionRLPs[0][i].data().toBytes(); });
        std::cout << "---------------------------\nExpected Root: " << expectedRoot << "---------------------------\n" << std::endl;

        delete entireBlockRLP;
        delete transactionRLPs;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
