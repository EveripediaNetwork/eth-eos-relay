/** @file main.cpp
 * @author Travis Moore <travis@everipedia.com>
 * @author Kedar Iyer <kedar@everipedia.com>
 * @date 2018
 */

#include "main.hpp"

using json = nlohmann::json;

// FOR DEBUG PURPOSES ONLY
// WILL BE REMOVED IN PRODUCTION
std::string hexStr(uint8_t * data, int len)
{
    std::stringstream ss;
    ss << std::hex;
    for(int i=0;i<len;++i)
        ss << std::setw(2) << std::setfill('0') << (int)data[i];
    return ss.str();
}

bool verify_full_block(const std::string& BLOCK_RLP, const std::string& PARENT_BLOCK_RLP) {
        bytes* bytedParentBlock = new bytes(HexToBytes(PARENT_BLOCK_RLP));
        bytes* bytedBlock = new bytes(HexToBytes(BLOCK_RLP));
        dev::RLP* parentBlockRLP = new dev::RLP(*bytedParentBlock);
        dev::RLP* blockRLP = new dev::RLP(*bytedBlock);
        dev::RLP* headerRLPs = new dev::RLP(blockRLP[0][0]);
        dev::RLP* transactionRLPs = new dev::RLP(blockRLP[0][1]);

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

        dev::eth::BlockHeader* parentBlockHeader = new dev::eth::BlockHeader(parentBlockRLP[0].data().toBytes());
        dev::eth::BlockHeader* blockHeader = new dev::eth::BlockHeader(blockRLP[0].data().toBytes());

        cout << "Tx Root for parent from its BlockHeader Object: " << parentBlockHeader->transactionsRoot() << endl;
        cout << "Tx Root for block from its BlockHeader Object: " << blockHeader->transactionsRoot() << endl;

        bytesConstRef* bytesConstRefBlock = new bytesConstRef(blockRLP->toBytesConstRef());

        blockHeader->verify(dev::eth::Strictness::CheckEverything, *parentBlockHeader, *bytesConstRefBlock);
        std::cout << "BLOCK HEADER VERIFIED" << std::endl;

        dev::eth::Ethash quickEthHash;
        quickEthHash.init();
        u256 result2 = quickEthHash.calculateDifficulty( *blockHeader, *parentBlockHeader);
        std::cout << "|||||||||||| DIFFICULTY SHOULD BE: " << result2 << " |||||||||||| " << std::endl;
        quickEthHash.verify(dev::eth::Strictness::CheckEverything, *blockHeader, *parentBlockHeader, *bytesConstRefBlock);
        std::cout << "BLOCK HEADER VERIFIED (non-custom method)" << std::endl;

        delete bytesConstRefBlock;
        delete bytedParentBlock;
        delete bytedBlock;
        delete parentBlockRLP;
        delete blockRLP;
        delete headerRLPs;
        delete transactionRLPs;

}

// check if the cache has already been computed for this epoch
// if it has, read it into memory
// if it doesn't, compute and save it to file so it can be fetched
// for future blocks
ethash_light_t compute_cache(uint64_t blockno) {
    const uint64_t EPOCH_LENGTH = 30000;
    uint64_t epoch = blockno / EPOCH_LENGTH;

    boost::filesystem::create_directory("cache");
    std::string cachefile = "./cache/" + std::to_string(epoch);
    bool exists = access( cachefile.c_str(), F_OK ) != -1;
    if (exists) {
        cout << "LOADING CACHE FROM FILESYSTEM" << endl;

        // read cache into buffer
        ifstream is (cachefile, ifstream::binary);

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
        light->block_number = blockno;
        light->cache_size = (uint64_t) cache_size;
        light->cache = (void*) buffer;
        cout << "POPULATED ETHASH CACHE STRUCT" << endl;

        return light;
    }
    else {
        cout << "COMPUTING FRESH CACHE " << endl;
        ethash_light_t light = ethash_light_new(blockno);
        ofstream cache_file;
        cache_file.open(cachefile, ios::out | ios::binary);
        cache_file.write(light->cache, light->cache_size);
        return light;
    }
}

void verify_block_header(uint64_t blockno, std::string BLOCK_HEADER_RLP, std::string PARENT_HEADER_RLP) {
    RLP blockRLP = RLP(new bytes(HexToBytes(BLOCK_HEADER_RLP)));
    RLP parentRLP = RLP(new bytes(HexToBytes(PARENT_HEADER_RLP)));
    BlockHeader header = BlockHeader();
    BlockHeader parentHeader = BlockHeader();
    header.populate(blockRLP);
    parentHeader.populate(parentRLP);

    // get header info
    Ethash ethash;
    h256 _headerHash = header.hash(WithoutSeal);
	h64 _nonce = ethash.nonce(header);

    // compute answer
    ethash_light_t light = compute_cache(blockno);
    ethash_return_value_t r = ethash_light_compute(light, *(ethash_h256_t*)_headerHash.data(), (uint64_t)(u64)_nonce);
    if (!r.success) {
        cout << "ETHASH HASHING FAILED" << endl;
        exit(3);
    }

    // TODO: verify answer is below difficulty

    cout << "RESULT: " << hexStr(r.result.b, 32) << endl;
    cout << "MIXHASH: " << hexStr(r.mix_hash.b, 32) << endl;
    cout << "EXPECTED MIXHASH: 5247691ab0953fa5c5c2c84b0b142b6d62e9dc5f35a865ed197b9cd3736af6f1" << endl;
}



EthereumTx merkle_proof(const std::string& TX_RLP, std::string path, h256 merkle_root) {
    bytes* tx_bytes = new bytes(HexToBytes(TX_RLP));
    dev::RLP* tx_rlp = new dev::RLP(*tx_bytes);

    for (int i=0; i < tx_rlp[0].itemCount(); i++) {
        dev::RLP node = tx_rlp[0][i];

        // verify merkle root
        if (i == 0) {
            h256 nodeHash = sha3(node.data().toBytes());
            assert(nodeHash == merkle_root);
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
	std::clock_t start;

    cout << "RUNNING END TO END TEST" << endl;

    cout << "TESTING FULL BLOCK VERIFICATION" << endl;

    std::string PARENT_BLOCK_RLP = BLOCK_RLP_STRING_4699999;
    std::string BLOCK_RLP = BLOCK_RLP_STRING_4700000;

    start = std::clock();
    verify_full_block(BLOCK_RLP, PARENT_BLOCK_RLP);
    double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    cout << "duration "<< duration << endl << endl;

    cout << "TESTING BLOCK HEADER VERIFICATION" << endl;

    uint64_t blockno = 4700000;
    std::string BLOCK_HEADER_RLP = BLOCK_HEADER_RLP_4700000;
    std::string PARENT_HEADER_RLP = BLOCK_HEADER_RLP_4699999;

    start = std::clock();
    verify_block_header(blockno, BLOCK_HEADER_RLP, PARENT_HEADER_RLP);
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    cout << "duration "<< duration << endl << endl;

    cout << "TESTING MERKLE PROOFS" << endl;

    h256 merkle_root_4700000 = h256("0x1e93385be250047bd0b5fcad904c42e4bf4ced18dacc729fc68cd4e3871727f0");
    string path = "81CB";
    std::string* TX_RLP = &TX_IN_4700000_RLP_HEX;

    start = std::clock();
    EthereumTx tx = merkle_proof(*TX_RLP, path, merkle_root_4700000);
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;

    cout << tx << endl;
    cout << "duration "<< duration << endl;

    return 0;
}
