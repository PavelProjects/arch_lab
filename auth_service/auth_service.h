#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H


#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/JSON/Object.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/SharedPtr.h"
#include "Poco/Net/SSLManager.h"
#include "Poco/Net/KeyConsoleHandler.h"
#include "Poco/Net/ConsoleCertificateHandler.h"
#include "Poco/Net/AcceptCertificateHandler.h"
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <Poco/URIStreamFactory.h>
#include <Poco/URIStreamOpener.h>
#include <Poco/Net/HTTPSStreamFactory.h>
#include <Poco/Net/HTTPStreamFactory.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::NameValueCollection;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include <iostream>
#include <string>
#include <fstream>
#include "../utils/token_utils.h"
#include "../utils/validation_utils.h"
#include "../database/user.h"

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

class AuthRequestHandler : public HTTPRequestHandler {
    public:
        AuthRequestHandler(const std::string &format): _format(format){};

        void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
            try {
                if (hasSubstr(request.getURI(), "/sign/in") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    HTMLForm form(request, request.stream());
                    std::string scheme;
                    std::string info;
                    request.getCredentials(scheme, info);

                    std::string login;
                    std::string password;

                    std::cout << "scheme " << scheme << " :: info " << info << std::endl;

                    if (scheme == "Basic" && decode_info(info, login, password)) {
                        long id = database::User::auth(login, password);
                        // todo token generation
                        std::cout << "id " << id << std::endl;
                        if (id > 0) {
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                            root->set("token", generate_token(login));
                            std::ostream &ostr = response.send();
                            Poco::JSON::Stringifier::stringify(root, ostr);
                            return;
                        } else if (id == 0) {
                            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                            root->set("type", "/errors/unauthorized");
                            root->set("title", "Unauthorized");
                            root->set("status", "401");
                            root->set("detail", "invalid login or password");
                            root->set("instance", "/auth");
                            std::ostream &ostr = response.send();
                            Poco::JSON::Stringifier::stringify(root, ostr);
                        }
                    }
                } else if (hasSubstr(request.getURI(), "/sign/up") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                    std::istream &i = request.stream();
                    int len = request.getContentLength();
                    char* buffer = new char[len];
                    i.read(buffer, len);
                    std::string body = buffer;

                    std::string validation_exception;
                    if (body.length() != 0) {
                        database::User user = database::User::fromJson(body);

                        if (validate_user(user, validation_exception)) {
                            std::cout << "Creating new user: " << body << std::endl;
                            user.save_to_db();
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_CREATED);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            std::ostream &ostr = response.send();
                            ostr << user.get_id();
                            return;
                        }
                    } else {
                        validation_exception = "Body is missing";
                    }

                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/validation");
                    root->set("title", "Validation exception");
                    root->set("message", validation_exception);
                    root->set("status", "500");
                    root->set("instance", "/auth");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                } else if (hasSubstr(request.getURI(), "/validate") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    const Poco::URI Uri(request.getURI());
                    const Poco::URI::QueryParameters params = Uri.getQueryParameters();
                    if (params.size() > 0) {
                        std::string token;
                        for(int i = 0; i < ((int)params.size()); i++) {
                            if (params[i].first == "token") {
                                token = params[i].second;
                            }
                        }
                        std::string login = validate_token(token);
                        if (login.length() > 0) {
                            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                            root->set("login", login);
                            std::ostream &ostr = response.send();
                            Poco::JSON::Stringifier::stringify(root, ostr);
                            return;  
                        }
                    }
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_UNAUTHORIZED);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/unauthorized");
                    root->set("title", "Unauthorized");
                    root->set("status", "401");
                    root->set("instance", "/auth");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                }
            } catch (...) { //todo
                response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_INTERNAL_SERVER_ERROR);
                response.setChunkedTransferEncoding(true);
                response.setContentType("application/json");
                Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                root->set("type", "/errors/server_error");
                root->set("title", "Internail server error");
                root->set("status", "500");
                root->set("instance", "/auth");
                std::ostream &ostr = response.send();
                Poco::JSON::Stringifier::stringify(root, ostr);
            }
            
        }
    private:
        std::string _format;
};


class HTTPAuthRequestFactory : public HTTPRequestHandlerFactory {
    public:
        HTTPAuthRequestFactory(const std::string &format) : _format(format){}
        HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request){
            std::cout << "request:" << request.getURI()<< std::endl;

            if (request.getURI().rfind("/auth") == 0) {
                    return new AuthRequestHandler(_format);
                }
            return 0;
        }
    private:
        std::string _format;
};

class HTTPAuthWebServer : public Poco::Util::ServerApplication {
    public:
        HTTPAuthWebServer() : _helpRequested(false){}
        ~HTTPAuthWebServer() {}
    protected:
        void initialize(Application &self) {
            loadConfiguration();
            ServerApplication::initialize(self);
        }
        void uninitialize() {
            ServerApplication::uninitialize();
        }
        int main([[maybe_unused]] const std::vector<std::string> &args) {
            if (!_helpRequested) {
                ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", 8081));
                HTTPServer srv(new HTTPAuthRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
                srv.start();
                std::cout << "auth server started" << std::endl;
                waitForTerminationRequest();
                srv.stop();
                std::cout << "auth server stoped" << std::endl;
            }
            return Application::EXIT_OK;
        }
    private:
        bool _helpRequested;
};

#endif
