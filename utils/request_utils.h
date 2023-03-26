#ifndef REQUEST_UTILS_H
#define REQUEST_UTILS_H

#include <string>
#include <iostream>
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <Poco/JSON/Parser.h>

static bool hasSubstr(const std::string &str, const std::string &substr) {
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i) {
        bool ok{true};
        for (size_t j = 0; ok && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}

std::string extractBody(std::istream &reqstream, int len) {
    char* buffer = new char[len];
    reqstream.read(buffer, len);
    return buffer;
} 

void unauthorizedResponse(HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/unauthorized");
    root->set("title", "Unauthorized");
    root->set("status", "401");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

void badRequestResponse(HTTPServerResponse &response, std::string &message) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/validation");
    root->set("message", message);
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

void notFoundResponse(HTTPServerResponse &response) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    response.setChunkedTransferEncoding(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/not_found");
    root->set("title", "Internal exception");
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
    root->set("detail", "request ot found");
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

void serverExceptionResponse(HTTPServerResponse &response, const std::exception &ex) {
    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
    response.setChunkedTransferEncoding(true);
    response.setKeepAlive(true);
    response.setContentType("application/json");
    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
    root->set("type", "/errors/server_error");
    root->set("title", "Internal exception");
    root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
    root->set("detail", ex.what());
    std::ostream &ostr = response.send();
    Poco::JSON::Stringifier::stringify(root, ostr);
}

Poco::JSON::Object::Ptr parseResponseBody(std::string response_body) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(response_body);
    return result.extract<Poco::JSON::Object::Ptr>();
}

#endif