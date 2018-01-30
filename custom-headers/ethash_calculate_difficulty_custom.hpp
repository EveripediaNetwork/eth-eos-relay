
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
