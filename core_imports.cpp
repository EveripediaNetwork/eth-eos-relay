#pragma once
#define ETH_PROJECT_VERSION "1"
#define ETH_COMMIT_HASH "1"
#include <iostream>
#include <string>
#include <typeinfo>
#include "custom-headers/json.hpp"
#include "custom-headers/constants.hpp"

#include <libdevcore/Common.h>
#include <libdevcore/Common.cpp>
#include <libdevcore/Guards.h>
#include <libdevcore/Guards.cpp>
#include <libdevcore/FileSystem.h>
#include <libdevcore/FileSystem.cpp>
#include <libdevcore/RLP.cpp>
#include <libdevcore/TrieDB.h>
#include <libdevcore/TrieDB.cpp>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/TrieHash.h>
#include <libethcore/Common.h>
#include <libdevcore/Exceptions.h>
#include <libethcore/BlockHeader.h>
#include <libethcore/BlockHeader.cpp>
#include <libethashseal/EthashProofOfWork.h>
#include <libethashseal/EthashProofOfWork.cpp>
#include <libdevcore/CommonData.cpp>
#include <libdevcore/TrieHash.cpp>
#include <libdevcore/TrieCommon.cpp>
#include <libdevcore/SHA3.h>
#include <libdevcore/SHA3.cpp>
#include <libdevcore/Log.h>
#include <libdevcore/Log.cpp>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/MemoryDB.cpp>
#include <libethcore/ChainOperationParams.h>
#include <libethcore/ChainOperationParams.cpp>

u256 HOMESTEAD_BLOCK = 0x118c30;
u256 DAO_BLOCK = 0x1d4c00;
u256 BYZANTIUM_BLOCK = 0x42ae50;
u256 MINIMUM_DIFFICULTY = 0x020000;
u256 DIFFICULTY_BOUND_DIVISOR = 0x0800;
u256 DURATION_LIMIT = 0x0d;
u256 MIN_GAS_LIMIT = 0x1388;
u256 MAX_GAS_LIMIT = 0x7fffffffffffffff;
u256 GAS_LIMIT_BOUND_DIVISOR = 0x0400;
u256 MAXIMUM_EXTRA_DATA_SIZE = 0x20;



#include <libdevcore/Worker.h>
#include <libdevcore/Worker.cpp>
#include <libdevcore/FixedHash.cpp>
#include <libdevcrypto/Common.h>
#include <libdevcrypto/Common.cpp>
#include <libdevcrypto/CryptoPP.h>
#include <libdevcrypto/CryptoPP.cpp>
#include <libdevcrypto/AES.h>
#include <libdevcrypto/AES.cpp>
#include <libethash/Ethash.h>
#include <libethash/sha3.h>
#include <libethash/sha3.c>
#include <libethashseal/EthashAux.h>
#include <libethashseal/EthashAux.cpp>
#include <libethashseal/EthashCPUMiner.h>
#include <libethashseal/EthashCPUMiner.cpp>
#include <libethashseal/Ethash.h>
#include <libethashseal/Ethash.cpp>
#include <libethcore/SealEngine.h>
#include <libethcore/SealEngine.cpp>
#include <libethcore/Transaction.h>
#include <libethcore/Transaction.cpp>
#include <libethash/internal.h>
#include <libethash/internal.c>
#include <libethash/io.h>
#include <libethash/io.c>
#include <libethash/io_posix.c>

#include <list>
#include <vector>

using namespace std;
using namespace dev;
using namespace dev::eth;
