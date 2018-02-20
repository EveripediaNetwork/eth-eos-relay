/** @file main.cpp
 * @author Travis Moore <travis@everipedia.com>
 * @date 2018
 */

#include "main.hpp"

using json = nlohmann::json;

std::string hexStr(uint8_t * data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

bool prove(const std::string& txHash){
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
        dev::RLP* entireBlockRLP_4699999 = new dev::RLP(*bytedEntireBlock_4699999);
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

        dev::eth::BlockHeader* blockHeaderObj_4699999 = new dev::eth::BlockHeader(entireBlockRLP_4699999[0].data().toBytes());
        dev::eth::BlockHeader* blockHeaderObj_4700000 = new dev::eth::BlockHeader(entireBlockRLP_4700000[0].data().toBytes());

        cout << "Tx Root for 4699999 from its BlockHeader Object: " << blockHeaderObj_4699999->transactionsRoot() << endl;
        cout << "Tx Root for 4700000 from its BlockHeader Object: " << blockHeaderObj_4700000->transactionsRoot() << endl;

        // u256 result = calculateDifficultyCustom(*blockHeaderObj_4700000, *blockHeaderObj_4699999);
        // std::cout << "|||||||||||| DIFFICULTY SHOULD BE: " << result << " |||||||||||| " << std::endl;

        bytesConstRef* bytesConstRef4700000 = new bytesConstRef(entireBlockRLP_4700000->toBytesConstRef());

        blockHeaderObj_4700000->verify(dev::eth::Strictness::CheckEverything, *blockHeaderObj_4699999, *bytesConstRef4700000);
        std::cout << "BLOCK HEADER VERIFIED" << std::endl;

        dev::eth::Ethash quickEthHash;
        quickEthHash.init();
        u256 result2 = quickEthHash.calculateDifficulty( *blockHeaderObj_4700000, *blockHeaderObj_4699999);
        std::cout << "|||||||||||| DIFFICULTY SHOULD BE: " << result2 << " |||||||||||| " << std::endl;
        std::cout << "BLOCK HEADER VERIFIED (non-custom method)" << std::endl;
        quickEthHash.verify(dev::eth::Strictness::CheckEverything, *blockHeaderObj_4700000, *blockHeaderObj_4699999, *bytesConstRef4700000);
    


        delete bytesConstRef4700000;
        delete bytedEntireBlock_4699999;
        delete bytedEntireBlock_4700000;
        delete entireBlockRLP_4699999;
        delete entireBlockRLP_4700000;
        delete headerRLPs;
        delete transactionRLPs;

}

EthereumTx merkle (const std::string & merkleProof) {
    std::cout << "Received RLP encoded merkle proof: " <<  merkleProof << '\n';
    
}

// check if the cache has already been computed for this epoch
// if it has, read it into memory
// if it doesn't, compute and save it to file so it can be fetched
// for future blocks
ethash_light_t compute_cache() {

    bool exists = access( "cache_4700000", F_OK ) != -1;
    if (exists) {
        cout << "LOADING CACHE FROM FILESYSTEM" << endl;

        // read cache into buffer
        ifstream is ("cache_4700000", ifstream::binary);

        is.seekg(0, is.end);
        size_t cache_size = (size_t) is.tellg();
        is.seekg(0, is.beg);

        char * buffer = new char[cache_size];
        is.read(buffer, cache_size);

        cout << "FILE READ COMPLETE" << endl;
        cout << "BUFFER ITEM: " <<  hexStr(buffer, 64) << endl;

        // create ethash cache struct 
        struct ethash_light * light;
        light = calloc(sizeof(*light), 1);
        if (!light) {
            cout << "ETHASH_LIGHT_T STRUCT CREATION FAILED" << endl;
            free(buffer);
            exit(2);
        }

        // populate cache struct
        light->block_number = (uint64_t) 4700000;
        light->cache_size = (uint64_t) cache_size;
        light->cache = (void*) buffer;
        cout << "POPULATED ETHASH CACHE STRUCT" << endl;
        
        return light;
    }
    else {
        cout << "COMPUTING FRESH CACHE " << endl;
        ethash_light_t light = ethash_light_new(4700000);
        ofstream cache_file;
        cache_file.open("cache_4700000", ios::out | ios::binary);
        cache_file.write(light->cache, light->cache_size);
        return light;
    }
}

void verify_block() {
    ethash_light_t light = compute_cache();
    cout << "CACHE LENGTH: " << light->cache_size << endl;
    char * cacheChar = (uint8_t *) light->cache;
    cout << "CACHE ITEM: " <<  hexStr(cacheChar, 64) << endl;

    // read in block 4700000
    bytes* bytedEntireBlock_4700000 = new bytes(HexToBytes(BLOCK_RLP_STRING_4700000));
    dev::RLP* entireBlockRLP_4700000 = new dev::RLP(*bytedEntireBlock_4700000);
    BlockHeader* blockHeaderObj_4700000 = new BlockHeader(entireBlockRLP_4700000[0].data().toBytes());

    // get header info
    Ethash ethash;
	BlockHeader bi = *blockHeaderObj_4700000;
    h256 _headerHash = bi.hash(WithoutSeal);
	h64 _nonce = ethash.nonce(bi);

    // compute answer
    ethash_return_value_t r = ethash_light_compute(light, *(ethash_h256_t*)_headerHash.data(), (uint64_t)(u64)_nonce);
    if (!r.success) {
        cout << "ETHASH HASHING FAILED" << endl;
        exit(3);
    }
    
    cout << "RESULT: " << hexStr(r.result.b, 32) << endl;
    cout << "MIXHASH: " << hexStr(r.mix_hash.b, 32) << endl;
    cout << "EXPECTED MIXHASH: 5247691ab0953fa5c5c2c84b0b142b6d62e9dc5f35a865ed197b9cd3736af6f1" << endl;

    //free(buffer);
    cout << "MEMORY FREED" << endl;
}

EthereumTx merkle_proof() {
    h256 block_merkle = h256("0x1e93385be250047bd0b5fcad904c42e4bf4ced18dacc729fc68cd4e3871727f0");
    bytes* tx_bytes = new bytes(HexToBytes(TX_IN_4700000_RLP_HEX));
    dev::RLP* tx_rlp = new dev::RLP(*tx_bytes);
    string path = "81CB";

    for (int i=0; i < tx_rlp[0].itemCount(); i++) {
        dev::RLP node = tx_rlp[0][i];

        // verify merkle root
        if (i == 0) {
            h256 nodeHash = sha3(node.data().toBytes());
            assert(nodeHash == block_merkle);
            cout << "MERKLE ROOT MATCHES" << endl;
        }

        if (node.itemCount() == 17) {
            cout << "Branch Node" << endl; 

            // convert path nibble hex to index (0-15)
            char nibble = (char) path[i];
            int nibbleIndex = (nibble >= 'A') ? (nibble - 'A' + 10) : (nibble - '0');

            // determine branch path
            bytesConstRef branchPathBytes = node[nibbleIndex].data();
            size_t branchPathSize = branchPathBytes.size() - 1;
            uint8_t branchPathArr[branchPathSize];
            copy(branchPathBytes.begin() + 1, branchPathBytes.end(), branchPathArr);
            string branchPath = hexStr(branchPathArr, branchPathSize);
            h256 branchPathHash = h256(branchPath);

            // hash next node
            dev::RLP nextNode = tx_rlp[0][i+1];
            h256 nextNodeHash = sha3(nextNode.data().toBytes());

            // compare branch path and next node
            assert(branchPathHash == nextNodeHash);
            cout << "  Verified current node path" << endl;
        }
        else if (node.itemCount() == 2) {
            u256 LEAF_FLAG = u256(0x20);

            if (node[0][0] == LEAF_FLAG) {
                cout << "MERKLE PROOF VERIFIED" << endl;
                cout << "Leaf Node" << endl;
                EthereumTx* tx = new EthereumTx(node[1][0]);
                return *tx;
            }
            else {
                cout << "Extension Node" << endl; 
            }
        }
    }
}

int main()
{
    std::string inputTxHash = "0xcbc84cc7337bc15867e46a892955ea3d3c5270b5f31a6741abb6fe91ad11132b";
    //prove(inputTxHash);
    //verify_block();
    
    EthereumTx tx = merkle_proof();
    cout << tx << endl;

    return 0;
}
