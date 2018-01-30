/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Ethash.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "Ethash.h"
#include <libethash/ethash.h>
#include <libethash/internal.h>
#include <libethereum/Interface.h>
#include <libethashseal/EthashProofOfWork.h>
#include <libethcore/ChainOperationParams.h>
#include <libethcore/CommonJS.h>
#include "EthashCPUMiner.h"
using namespace std;
using namespace dev;
using namespace eth;
using namespace dev::eth;

void Ethash::init()
{
	ETH_REGISTER_SEAL_ENGINE(Ethash);
}

Ethash::Ethash()
{
	map<string, GenericFarm<EthashProofOfWork>::SealerDescriptor> sealers;
	sealers["cpu"] = GenericFarm<EthashProofOfWork>::SealerDescriptor{&EthashCPUMiner::instances, [](GenericMiner<EthashProofOfWork>::ConstructionInfo ci){ return new EthashCPUMiner(ci); }};
	m_farm.setSealers(sealers);
	m_farm.onSolutionFound([=](EthashProofOfWork::Solution const& sol)
	{
//		cdebug << m_farm.work().seedHash << m_farm.work().headerHash << sol.nonce << EthashAux::eval(m_farm.work().seedHash, m_farm.work().headerHash, sol.nonce).value;
		setMixHash(m_sealing, sol.mixHash);
		setNonce(m_sealing, sol.nonce);
		if (!quickVerifySeal(m_sealing))
			return false;

		if (m_onSealGenerated)
		{
			RLPStream ret;
			m_sealing.streamRLP(ret);
			m_onSealGenerated(ret.out());
		}
		return true;
	});
}

strings Ethash::sealers() const
{
	return {"cpu"};
}

h256 Ethash::seedHash(BlockHeader const& _bi)
{
	return EthashAux::seedHash((unsigned)_bi.number());
}

StringHashMap Ethash::jsInfo(BlockHeader const& _bi) const
{
	return { { "nonce", toJS(nonce(_bi)) }, { "seedHash", toJS(seedHash(_bi)) }, { "mixHash", toJS(mixHash(_bi)) }, { "boundary", toJS(boundary(_bi)) }, { "difficulty", toJS(_bi.difficulty()) } };
}

void Ethash::verify(Strictness _s, BlockHeader const& _bi, BlockHeader const& _parent, bytesConstRef _block) const
{
	SealEngineFace::verify(_s, _bi, _parent, _block);

	if (_s != CheckNothingNew)
	{
		if (_bi.difficulty() < MINIMUM_DIFFICULTY)
			BOOST_THROW_EXCEPTION(InvalidDifficulty() << RequirementError(bigint(MINIMUM_DIFFICULTY), bigint(_bi.difficulty())) );

		if (_bi.gasLimit() < MIN_GAS_LIMIT)
			BOOST_THROW_EXCEPTION(InvalidGasLimit() << RequirementError(bigint(MIN_GAS_LIMIT), bigint(_bi.gasLimit())) );

		if (_bi.gasLimit() > MAX_GAS_LIMIT)
			BOOST_THROW_EXCEPTION(InvalidGasLimit() << RequirementError(bigint(MAX_GAS_LIMIT), bigint(_bi.gasLimit())) );

		if (_bi.number() && _bi.extraData().size() > MAXIMUM_EXTRA_DATA_SIZE)
			BOOST_THROW_EXCEPTION(ExtraDataTooBig() << RequirementError(bigint(MAXIMUM_EXTRA_DATA_SIZE), bigint(_bi.extraData().size())) << errinfo_extraData(_bi.extraData()));

		u256 const& daoHardfork = DAO_BLOCK;
		if (daoHardfork != 0 && daoHardfork + 9 >= daoHardfork && _bi.number() >= daoHardfork && _bi.number() <= daoHardfork + 9)
			if (_bi.extraData() != fromHex("0x64616f2d686172642d666f726b"))
				BOOST_THROW_EXCEPTION(ExtraDataIncorrect() << errinfo_comment("Received block from the wrong fork (invalid extradata)."));
	}

	if (_parent)
	{
		// Check difficulty is correct given the two timestamps.
		auto expected = calculateDifficulty(_bi, _parent);
		auto difficulty = _bi.difficulty();
		if (difficulty != expected)
			BOOST_THROW_EXCEPTION(InvalidDifficulty() << RequirementError((bigint)expected, (bigint)difficulty));

		auto gasLimit = _bi.gasLimit();
		auto parentGasLimit = _parent.gasLimit();
		if (
			gasLimit < MIN_GAS_LIMIT ||
			gasLimit > MAX_GAS_LIMIT ||
			gasLimit <= parentGasLimit - parentGasLimit / GAS_LIMIT_BOUND_DIVISOR ||
			gasLimit >= parentGasLimit + parentGasLimit / GAS_LIMIT_BOUND_DIVISOR)
			BOOST_THROW_EXCEPTION(
				InvalidGasLimit()
				<< errinfo_min((bigint)((bigint)parentGasLimit - (bigint)(parentGasLimit / GAS_LIMIT_BOUND_DIVISOR)))
				<< errinfo_got((bigint)gasLimit)
				<< errinfo_max((bigint)((bigint)parentGasLimit + parentGasLimit / GAS_LIMIT_BOUND_DIVISOR))
			);
	}

	// check it hashes according to proof of work or that it's the genesis block.
	if (_s == CheckEverything && _bi.parentHash() && !verifySeal(_bi))
	{
		std::cout << "STARTING PROOF OF WORK HASHING (CheckEverything)" << std::endl;
		InvalidBlockNonce ex;
		ex << errinfo_nonce(nonce(_bi));
		ex << errinfo_nonce(nonce(_bi));
		ex << errinfo_mixHash(mixHash(_bi));
		ex << errinfo_seedHash(seedHash(_bi));
		EthashProofOfWork::Result er = EthashAux::eval(seedHash(_bi), _bi.hash(WithoutSeal), nonce(_bi));
		ex << errinfo_ethashResult(make_tuple(er.value, er.mixHash));
		ex << errinfo_hash256(_bi.hash(WithoutSeal));
		ex << errinfo_difficulty(_bi.difficulty());
		ex << errinfo_target(boundary(_bi));
		BOOST_THROW_EXCEPTION(ex);
	}
	else if (_s == QuickNonce && _bi.parentHash() && !quickVerifySeal(_bi))
	{
		std::cout << "STARTING PROOF OF WORK (QuickNonce)..." << std::endl;
		InvalidBlockNonce ex;
		auto quickHash = _bi.hash(WithoutSeal);
		std::cout << quickHash << std::endl;
		ex << errinfo_hash256(quickHash);
		ex << errinfo_difficulty(_bi.difficulty());
		ex << errinfo_nonce(nonce(_bi));
		BOOST_THROW_EXCEPTION(ex);
	}
	std::cout << "FINISHED" << std::endl;
}

void Ethash::verifyTransaction(ImportRequirements::value _ir, TransactionBase const& _t, BlockHeader const& _bi) const
{
	SealEngineFace::verifyTransaction(_ir, _t, _bi);

	if (_ir & ImportRequirements::TransactionSignatures)
	{
		if (_bi.number() >= chainParams().u256Param("EIP158ForkBlock"))
		{
			int chainID(chainParams().u256Param("chainID"));
			_t.checkChainId(chainID);
		}
		else
			_t.checkChainId(-4);
	}
	// Unneeded as it's checked again in Executive. Keep it here since tests assume it's checked.
	if (_ir & ImportRequirements::TransactionBasic && _t.baseGasRequired(evmSchedule(EnvInfo(_bi))) > _t.gas())
		BOOST_THROW_EXCEPTION(OutOfGasIntrinsic());
}

u256 Ethash::childGasLimit(BlockHeader const& _bi, u256 const& _gasFloorTarget) const
{
	u256 gasFloorTarget = _gasFloorTarget == Invalid256 ? 3141562 : _gasFloorTarget;
	u256 gasLimit = _bi.gasLimit();
	u256 boundDivisor = chainParams().u256Param("gasLimitBoundDivisor");
	if (gasLimit < gasFloorTarget)
		return min<u256>(gasFloorTarget, gasLimit + gasLimit / boundDivisor - 1);
	else
		return max<u256>(gasFloorTarget, gasLimit - gasLimit / boundDivisor + 1 + (_bi.gasUsed() * 6 / 5) / boundDivisor);
}

void Ethash::manuallySubmitWork(const h256& _mixHash, Nonce _nonce)
{
	m_farm.submitProof(EthashProofOfWork::Solution{_nonce, _mixHash}, nullptr);
}

u256 Ethash::calculateDifficulty(BlockHeader const& _bi, BlockHeader const& _parent) const
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

void Ethash::populateFromParent(BlockHeader& _bi, BlockHeader const& _parent) const
{
	SealEngineFace::populateFromParent(_bi, _parent);
	_bi.setDifficulty(calculateDifficulty(_bi, _parent));
	_bi.setGasLimit(childGasLimit(_parent));
}

bool Ethash::quickVerifySeal(BlockHeader const& _bi) const
{
	std::cout << "QUICK VERIFY SEAL REACHED" << std::endl;
	if (_bi.number() >= ETHASH_EPOCH_LENGTH * 2048)
		return false;

	auto h = _bi.hash(WithoutSeal);
	auto m = mixHash(_bi);
	auto n = nonce(_bi);
	auto b = boundary(_bi);
	bool ret = !!ethash_quick_check_difficulty(
		(ethash_h256_t const*)h.data(),
		(uint64_t)(u64)n,
		(ethash_h256_t const*)m.data(),
		(ethash_h256_t const*)b.data());
	std::cout << "QUICK VERIFY SEAL DONE: " << ret << std::endl;
	return ret;
}

bool Ethash::verifySeal(BlockHeader const& _bi) const
{
	std::cout << "VERIFY SEAL REACHED" << std::endl;
	bool pre = quickVerifySeal(_bi);



#if !ETH_DEBUG
	if (!pre)
	{
		cwarn << "Fail on preVerify";
		return false;
	}
#endif

	auto result = EthashAux::eval(seedHash(_bi), _bi.hash(WithoutSeal), nonce(_bi));
	bool slow = result.value <= boundary(_bi) && result.mixHash == mixHash(_bi);

#if ETH_DEBUG
	if (!pre && slow)
	{
		cwarn << "WARNING: evaluated result gives true whereas ethash_quick_check_difficulty gives false.";
		cwarn << "headerHash:" << _bi.hash(WithoutSeal);
		cwarn << "nonce:" << nonce(_bi);
		cwarn << "mixHash:" << mixHash(_bi);
		cwarn << "difficulty:" << _bi.difficulty();
		cwarn << "boundary:" << boundary(_bi);
		cwarn << "result.value:" << result.value;
		cwarn << "result.mixHash:" << result.mixHash;
	}
#endif // ETH_DEBUG

	return slow;
}

void Ethash::generateSeal(BlockHeader const& _bi)
{
	m_sealing = _bi;
	m_farm.setWork(m_sealing);
	m_farm.start(m_sealer);
	m_farm.setWork(m_sealing);		// TODO: take out one before or one after...
	bytes shouldPrecompute = option("precomputeDAG");
	if (!shouldPrecompute.empty() && shouldPrecompute[0] == 1)
		ensurePrecomputed((unsigned)_bi.number());
}

bool Ethash::shouldSeal(Interface*)
{
	return true;
}

void Ethash::ensurePrecomputed(unsigned _number)
{
	if (_number % ETHASH_EPOCH_LENGTH > ETHASH_EPOCH_LENGTH * 9 / 10)
		// 90% of the way to the new epoch
		EthashAux::computeFull(EthashAux::seedHash(_number + ETHASH_EPOCH_LENGTH), true);
}
