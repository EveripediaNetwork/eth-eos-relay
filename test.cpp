#include <iostream>
#include <string>
#include "json.hpp"
#include "constants.hpp"

using json = nlohmann::json;

// g++ -c test.cpp -o out.o && g++ out.o -o runner && ./runner

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
        std::cout << txBlockJSON.dump(4) << std::endl;
        std::cout << "\n-----------------------------\n"<< std::endl;

}

int main()
{
        std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
        getTransactionProof(inputTxHash);
        return 0;
}
