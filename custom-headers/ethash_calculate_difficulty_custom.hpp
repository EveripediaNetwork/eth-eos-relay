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
	else{
			std::cout << "FAILED: FINISHED WITHOUT A PROOF OF WORK CHECK" << std::endl;
	}

}
