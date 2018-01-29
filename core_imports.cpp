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
#include <libdevcore/Log.h>
#include <libdevcore/RLP.cpp>
#include <libdevcore/TrieDB.h>
#include <libdevcore/TrieDB.cpp>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/TrieHash.h>
#include <libethcore/Common.h>
#include <libdevcore/Exceptions.h>
#include <libethcore/BlockHeader.h>
#include <libethcore/BlockHeader.cpp>
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
#include <libethashseal/Ethash.h>
#include <libethashseal/Ethash.cpp>
#include <libethcore/SealEngine.h>
#include <libethcore/SealEngine.cpp>
#include <libethashseal/EthashAux.h>
#include <libethashseal/EthashAux.cpp>
#include <libethashseal/EthashCPUMiner.h>
#include <libethashseal/EthashCPUMiner.cpp>
#include <libethcore/Transaction.h>
#include <libethcore/Transaction.cpp>
#include <libethashseal/EthashProofOfWork.h>
#include <libethashseal/EthashProofOfWork.cpp>
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



u256 calculateDifficultyCustom(BlockHeader const& _bi, BlockHeader const& _parent)
{
	const unsigned c_expDiffPeriod = 100000;

	if (!_bi.number())
		throw GenesisBlockCannotBeCalculated();
	auto const& minimumDifficulty = MINIMUM_DIFFICULTY;
	auto const& difficultyBoundDivisor = DIFFICULTY_BOUND_DIVISOR;
	auto const& durationLimit = DURATION_LIMIT;

	bigint target;	// stick to a bigint for the target. Don't want to risk going negative.
	if (_bi.number() < HOMESTEAD_BLOCK)
		// Frontier-era difficulty adjustment
		target = _bi.timestamp() >= _parent.timestamp() + durationLimit ? _parent.difficulty() - (_parent.difficulty() / difficultyBoundDivisor) : (_parent.difficulty() + (_parent.difficulty() / difficultyBoundDivisor));
	else
	{
		bigint const timestampDiff = bigint(_bi.timestamp()) - _parent.timestamp();
		bigint const adjFactor = _bi.number() < BYZANTIUM_BLOCK ?
			max<bigint>(1 - timestampDiff / 10, -99) : // Homestead-era difficulty adjustment
			max<bigint>((_parent.hasUncles() ? 2 : 1) - timestampDiff / 9, -99); // Byzantium-era difficulty adjustment

		target = _parent.difficulty() + _parent.difficulty() / 2048 * adjFactor;
	}

	bigint o = target;
	unsigned exponentialIceAgeBlockNumber = unsigned(_parent.number() + 1);

	// EIP-649 modifies exponentialIceAgeBlockNumber
	if (_bi.number() >= BYZANTIUM_BLOCK)
	{
		if (exponentialIceAgeBlockNumber >= 3000000)
			exponentialIceAgeBlockNumber -= 3000000;
		else
			exponentialIceAgeBlockNumber = 0;
	}

	unsigned periodCount = exponentialIceAgeBlockNumber / c_expDiffPeriod;
	if (periodCount > 1)
		o += (bigint(1) << (periodCount - 2));	// latter will eventually become huge, so ensure it's a bigint.

	o = max<bigint>(minimumDifficulty, o);
	return u256(min<bigint>(o, std::numeric_limits<u256>::max()));
}
