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

        // Need code here to fetch the transaction's entire block
        // ...
        // ...

        //  Convert the block data into JSON
        auto txBlockJSON = json::parse(BLOCK_ONE_BLOCK_STRING);

        // declaring vector of pairs

        // Would probably need a for loop here
        // Need to create tuples of the form (txIndex, HexToBytes(rawTransaction without the leading 0x))
        //  1 transaction
        // https://etherscan.io/getRawTx?tx=0x5da0298e46e949f863f0873b6f8c102e150dc85d31f4d7c6d7c82cb69c8a672e
        //  2 transactions
        // https://www.etherchain.org/block/4052768

        std::vector<std::string> rawTransactionStrings {"0xf86c218504e3b292008252089483854e6e4d49489b0b25d803069e6dea98b3aad68819ce7b0ee1e170008026a0e37ea36cb26a41771b8e30d126ecaf69312c9cd1a0ede5f5c16ddcc1235ac896a07f7ba3c0127a6b707971e294846fd8c9e0f7603454fbd89b63f61d5c2b9e621e", "0xf86c0e8504e3b2920082520894963d7e8cb029b764acd839e62d733038dc463c17881bc16d674ec800008026a045f5a2f49ba9d911ebf6e7eef8271e3c43c03d2784454b30072f4e0bdd1706b1a01a5729422eb46b80b5eb93407aa4c777fc8ee845b0f88fba982c84553d326a50",
        "0xf86f0d84ee6b28008288b894fa52274dd61e1643d2205169732f29114bc240b388019e0ee3d1e4a80084f76541761ba01f4ed2ce15fc64e98d90bc8e53870e3cf2fe79d717c98e64c4a572897c64c165a0393b0e3bc257dd257404ac61cc69dc1e6a289f2dae37247b38062c53756d1298"};

        std::vector<bytes> byteVector = {};
        std::vector<std::string> quickVector = {};

        int index = 0;
        for(std::vector<std::string>::iterator it = rawTransactionStrings.begin(); it != rawTransactionStrings.end(); ++it) {
                it->erase(0,2);
                byteVector.push_back(HexToBytes(*it));
                quickVector.push_back((std::to_string(index), rawTransactionStrings[index] ));
                index++;
        }

        dev::RLPStream* txRLPStream = new dev::RLPStream();
        txRLPStream->appendVector(quickVector);
        bytes bytedTxRLPStream = txRLPStream->out();
        dev::RLP txList = dev::RLP(bytedTxRLPStream);

        std::cout << "---------------------------\n" << toHex(bytedTxRLPStream) << "---------------------------\n" << std::endl;
        auto expectedRoot = trieRootOver(txList.itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return byteVector[i]; });
        std::string rootString = "0x" + toString(expectedRoot);
        std::cout << "---------------------------\nExpected Root: " << rootString << "---------------------------\n" << std::endl;
        delete txRLPStream;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
