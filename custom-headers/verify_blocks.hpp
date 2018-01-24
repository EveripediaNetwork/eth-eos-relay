#include <libdevcore/Common.h>
#include <libdevcore/Log.h>
#include <libdevcore/RLP.cpp>
#include <libdevcore/TrieDB.h>
#include <libdevcore/MemoryDB.h>
#include <libdevcore/TrieHash.h>
#include <libethcore/Common.h>
#include <libdevcore/Exceptions.h>
#include <libethcore/BlockHeader.h>
#include <libdevcore/CommonData.cpp>
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
