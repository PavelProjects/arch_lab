#pragma once
#include <string>
#include "Poco/Base64Decoder.h"
#include "Poco/Base64Encoder.h"
#include "Poco/JWT/Token.h"
#include "Poco/JWT/Signer.h"
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

std::string generate_token(std::string &login) {
    Poco::JWT::Token token;
    token.setType("JWT");
    token.setSubject("login");
    token.payload().set("login", login);
    token.setIssuedAt(Poco::Timestamp());

    Poco::JWT::Signer signer("0123456789ABCDEF0123456789ABCDEF");
    return signer.sign(token, Poco::JWT::Signer::ALGO_HS256);
}

std::string validate_token(std::string &jwt_token) {
    if (jwt_token.length() == 0) {
        return "";
    }

    Poco::JWT::Signer signer("0123456789ABCDEF0123456789ABCDEF");
    try {
        Poco::JWT::Token token = signer.verify(jwt_token);
        if (token.payload().has("login")) {
            return token.payload().get("login");
        }
    } catch (...) {
        std::cout << "Token verification failed" << std::endl;
    }
    return "";
}