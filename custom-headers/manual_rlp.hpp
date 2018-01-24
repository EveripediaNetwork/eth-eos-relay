


//  from https://github.com/ethereum/wiki/wiki/%5BEnglish%5D-RLP

static std::string rlp_to_binary( long x ) {
        if (x == 0){
                return "";
        }
        else {
                return rlp_to_binary(static_cast<int>(x / 256)) + static_cast<char>(x % 256);
        }
}

static std::string rlp_encode_length( long L, long offset ) {
        if (L < 56){
                std::string returnString(1, L + offset);
                return returnString;
        }
        else if ( L < pow (256, 8) ){
                std::string BL = rlp_to_binary(L);
                std::string returnString(1,  BL.length() + offset + 55);
                return returnString + BL;
        }
        else{
                std::cout << "Input too long" << std::endl;
        }
}

static std::string rlp_encode( std::string &inputString ) {
        const char* charredString = inputString.c_str();
        if (inputString.length() == 1 && *charredString < 128){
                return inputString;
        }
        else{
                return rlp_encode_length(inputString.length(), 128) + inputString;
        }
}
