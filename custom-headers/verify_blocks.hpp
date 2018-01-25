#pragma once
#define ETH_PROJECT_VERSION "1"
#define ETH_COMMIT_HASH "1"


// #include <libdevcore/Common.h>
// #include <libdevcore/Common.cpp>
// #include <libethcore/Common.h>
// #include <libethcore/Common.cpp>
// #include <libdevcore/CommonData.h>
// #include <libdevcore/CommonData.cpp>

// #include <libdevcrypto/Common.h>
// #include <libdevcrypto/Common.cpp>
// #include <libethcore/ICAP.h>
// #include <libethcore/ICAP.cpp>

// #include <libdevcore/Log.h>
// #include <libdevcore/Log.cpp>
// #include <libdevcore/RLP.h>
// #include <libdevcore/RLP.cpp>
// #include <libdevcore/SHA3.h>
// #include <libdevcore/SHA3.cpp>

// #include <libdevcore/FixedHash.h>
// #include <libdevcore/FixedHash.cpp>

// #include <libdevcore/TrieDB.h>
// #include <libdevcore/TrieDB.cpp>
// #include <libdevcore/TrieCommon.h>
// #include <libdevcore/TrieCommon.cpp>
// #include <libdevcore/MemoryDB.h>
// #include <libdevcore/MemoryDB.cpp>
// #include <libdevcore/TrieHash.h>
// #include <libdevcore/TrieHash.cpp>
// #include <libdevcore/Exceptions.h>

// #include <libethcore/BlockHeader.h>
// #include <libethcore/BlockHeader.cpp>

// using namespace std;
// using namespace dev;
// using namespace dev::eth;

#include <libdevcore/Common.h>
#include <libdevcore/Log.h>
#include <libdevcore/RLP.cpp>
#include <libdevcore/TrieDB.h>
#include <libdevcore/TrieDB.cpp>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/TrieHash.h>
#include <libethcore/Common.h>
#include <libdevcore/Exceptions.h>
#include <libethcore/BlockHeader.h>
#include <libdevcore/CommonData.cpp>
#include <libdevcore/TrieHash.cpp>
#include <libdevcore/TrieCommon.cpp>
#include <libdevcore/SHA3.h>
#include <libdevcore/SHA3.cpp>
#include <libdevcore/Log.h>
#include <libdevcore/Log.cpp>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/MemoryDB.cpp>
using namespace std;
using namespace dev;
using namespace dev::eth;


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
