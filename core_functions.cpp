
static std::vector<unsigned char> HexToBytes(const std::string& hex) {
        std::vector<unsigned char> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 2) {
                std::string byteString = hex.substr(i, 2);
                unsigned char byte = (unsigned char) strtol(byteString.c_str(), NULL, 16);
                bytes.push_back(byte);
        }

        return bytes;
}

static void RLPtoStringStream(std::stringstream& _out, dev::RLP const& _d, unsigned _depth = 0) {
        if (_depth > 64)
		_out << "<max-depth-reached>";
	else if (_d.isNull())
		_out << "null";
	else if (_d.isInt())
		_out << std::showbase << std::hex << std::nouppercase << _d.toInt<bigint>(RLP::LaissezFaire) << dec;
	else if (_d.isData())
		_out << escaped(_d.toString(), false);
	else if (_d.isList())
	{
		_out << "[";
		int j = 0;
		for (auto i: _d)
		{
			_out << (j++ ? ", " : " ");
			RLPtoStringStream(_out, i, _depth + 1);
		}
		_out << " ]";
	}
}

static std::string RLPtoString(dev::RLP const& _d, unsigned _depth = 0) {
        std::stringstream _out;
        RLPtoStringStream(_out, _d, 0);
        return _out.str();
}

ethash_light_t getLightCacheNodes(uint64_t block_number){
        ethash_light_t lightCache = ethash_light_new(0);

        // struct ethash_light {
        // 	void* cache;
        // 	uint64_t cache_size;
        // 	uint64_t block_number;
        // };

        uint32_t theBlockNumber = lightCache->block_number;
        uint32_t theCacheSize = lightCache->cache_size;
        node* nodes = (node*)lightCache->cache;

        std::cout << "For block number " << theBlockNumber << " the cache size is " << theCacheSize << " " << std::endl;

        uint32_t num_nodes = (uint32_t) (theCacheSize / sizeof(node));
        std::cout << "The number of nodes is: " << num_nodes << std::endl;

        // typedef union node {
        // 	uint8_t bytes[NODE_WORDS * 4];
        // 	uint32_t words[NODE_WORDS];
        // 	uint64_t double_words[NODE_WORDS / 2];
        // }

        return lightCache;
}
