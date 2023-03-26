#pragma once
#include <string>
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include <istream>
#include <ostream>
#include <sstream>

bool decode_info(const std::string info, std::string &login, std::string &password) {
    std::istringstream istr(info);
    std::ostringstream ostr;
    Poco::Base64Decoder b64in(istr);
    copy(std::istreambuf_iterator<char>(b64in),
         std::istreambuf_iterator<char>(),
         std::ostreambuf_iterator<char>(ostr));
    std::string decoded = ostr.str();

    size_t pos = decoded.find(':');
    login = decoded.substr(0, pos);
    password = decoded.substr(pos + 1);
    return true;
}