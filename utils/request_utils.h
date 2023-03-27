#ifndef REQUEST_UTILS_H
#define REQUEST_UTILS_H

#include <string>
#include <iostream>
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <Poco/JSON/Parser.h>
#include "ctime"

time_t parse_time(std::string time) {
    struct tm tm;
    strptime(time.c_str(), "%d-%m-%Y %H:%M:%S", &tm);
    return mktime(&tm);
}

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

Poco::JSON::Object::Ptr parseJson(std::string response_body) {
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var result = parser.parse(response_body);
    return result.extract<Poco::JSON::Object::Ptr>();
}

std::string validateToken(std::string scheme, std::string token) {
    if (scheme.length() == 0 || token.length() == 0) {
        return "";
    }

    std::string host = "localhost";
    std::string auth_port = "8081";
    if (std::getenv("AUTH_SERVICE_HOST") != nullptr)
        host = std::getenv("AUTH_SERVICE_HOST");
    if (std::getenv("AUTH_SERVICE_PORT") != nullptr) {
        auth_port = std::getenv("AUTH_SERVICE_PORT");
    }   
    std::string url = "http://" + host + ":" + auth_port + "/auth/validate";

    try {
        Poco::URI uri(url);
        Poco::Net::HTTPClientSession session(uri.getHost(), uri.getPort());
        Poco::Net::HTTPRequest request(Poco::Net::HTTPRequest::HTTP_GET, uri.toString());
        request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
        request.setKeepAlive(true);
        request.setCredentials(scheme, token);

        session.sendRequest(request);

        Poco::Net::HTTPResponse response;
        std::istream &rs = session.receiveResponse(response);
        std::string response_body;

        while (rs) {
            char c{};
            rs.read(&c, 1);
            if (rs)
                response_body += c;
        }

        if (response.getStatus() != Poco::Net::HTTPResponse::HTTPStatus::HTTP_ACCEPTED) {
            std::cout << "Failed to validate token [" << response.getStatus() << "] " << response_body << std::endl;
            return "";
        }

        Poco::JSON::Object::Ptr object = parseJson(response_body);
        if (object->has("login")) {
            return object->getValue<std::string>("login");
        }

    } catch (Poco::Exception &ex) {
        std::cout << "Failed to validate token " << ex.what() << " :: " << ex.message() << std::endl;
    }
    return "";
}

#endif