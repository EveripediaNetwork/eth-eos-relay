#pragma once
#define ETH_PROJECT_VERSION "1"
#define ETH_COMMIT_HASH "1"

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
// #include <libethereum/ChainParams.cpp>


// #include <libethcore/ChainOperationParams.cpp>
// #include <libethashseal/EthashClient.cpp>
// #include <libethashseal/Ethash.cpp>
// #include <libethash/internal.h>
// #include <libethashseal/EthashAux.h>
// #include <libethashseal/EthashAux.cpp>
// #include <libethashseal/Ethash.h>
// #include <libethashseal/Ethash.cpp>
// #include <libethcore/SealEngine.h>
// #include <libethcore/SealEngine.cpp>
// #include <libethashseal/EthashProofOfWork.h>
// #include <libethashseal/EthashProofOfWork.cpp>
using namespace std;
using namespace dev;
using namespace dev::eth;

u256 HOMESTEAD_BLOCK = 0x118c30;
u256 BYZANTIUM_BLOCK = 0x42ae50;
u256 MINIMUM_DIFFICULTY = 0x020000;
u256 DIFFICULTY_BOUND_DIVISOR = 0x0800;
u256 DURATION_LIMIT = 0x0d;


u256 calculateDifficulty(BlockHeader const& _bi, BlockHeader const& _parent)
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


//
// void BlockHeader::verify(Strictness _s, BlockHeader const& _parent, bytesConstRef _block) const
// {
// 	if (m_number > ~(unsigned)0)
// 		BOOST_THROW_EXCEPTION(InvalidNumber());
//
// 	if (_s != CheckNothingNew && m_gasUsed > m_gasLimit)
// 		BOOST_THROW_EXCEPTION(TooMuchGasUsed() << RequirementError(bigint(m_gasLimit), bigint(m_gasUsed)));
//
// 	if (_parent)
// 	{
// 		if (m_parentHash && _parent.hash() != m_parentHash)
// 			BOOST_THROW_EXCEPTION(InvalidParentHash());
//
// 		if (m_timestamp <= _parent.m_timestamp)
// 			BOOST_THROW_EXCEPTION(InvalidTimestamp());
//
// 		if (m_number != _parent.m_number + 1)
// 			BOOST_THROW_EXCEPTION(InvalidNumber());
// 	}
//
// 	if (_block)
// 	{
// 		RLP root(_block);
//
//                 //  this needs to be a list of all the RLPs  of all the transactions
// 		auto txList = root[1];
//
//
// 		auto expectedRoot = trieRootOver(txList.itemCount(), [&](unsigned i){ return rlp(i); }, [&](unsigned i){ return txList[i].data().toBytes(); });
//
// 		clog(BlockInfoDiagnosticsChannel) << "Expected trie root:" << toString(expectedRoot);
// 		if (m_transactionsRoot != expectedRoot)
// 		{
// 			MemoryDB tm;
// 			GenericTrieDB<MemoryDB> transactionsTrie(&tm);
// 			transactionsTrie.init();
//
// 			vector<bytesConstRef> txs;
//
// 			for (unsigned i = 0; i < txList.itemCount(); ++i)
// 			{
// 				RLPStream k;
// 				k << i;
//
// 				transactionsTrie.insert(&k.out(), txList[i].data());
//
// 				txs.push_back(txList[i].data());
// 				cdebug << toHex(k.out()) << toHex(txList[i].data());
// 			}
// 			cdebug << "trieRootOver" << expectedRoot;
// 			cdebug << "orderedTrieRoot" << orderedTrieRoot(txs);
// 			cdebug << "TrieDB" << transactionsTrie.root();
// 			cdebug << "Contents:";
// 			for (auto const& t: txs)
// 				cdebug << toHex(t);
//
// 			BOOST_THROW_EXCEPTION(InvalidTransactionsRoot() << Hash256RequirementError(expectedRoot, m_transactionsRoot));
// 		}
// 		clog(BlockInfoDiagnosticsChannel) << "Expected uncle hash:" << toString(sha3(root[2].data()));
// 		if (m_sha3Uncles != sha3(root[2].data()))
// 			BOOST_THROW_EXCEPTION(InvalidUnclesHash() << Hash256RequirementError(sha3(root[2].data()), m_sha3Uncles));
// 	}
// }
