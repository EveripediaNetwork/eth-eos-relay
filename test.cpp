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

        std::vector<std::string> rawTransactionStrings {"0xf86f830128fa850df847580082520894819c0af8a979af4472cc54f60100f2aeec1aa70b8801aa535d3d0c00008026a0be0612e0204c75726f95b5c1b70f7daff440e330a4f16e96ad5e32d58ea92661a02d5c7d18022a322383887fd969bc70c79c68160a3eee978d7da65a3c6677ca4f", "0xf86e8274618507cfbc5f8082520994ac05122f4f6441d5b307fb8a4b13d57f91161f7888066942e7c57760008025a07f5ab43a02383bb892bdf6b006363e4b5ebd194a8b9ee7defa01f185fa06b412a01513f38abc9f7e0b85884af45bf1f6306cf076b4976e1bd122c16db26c289021" };

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
