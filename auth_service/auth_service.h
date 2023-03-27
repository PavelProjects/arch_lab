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
#include "../utils/request_utils.h"

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
                        if (id > 0) {
                            std::cout << "Found user with id " << id << std::endl;
                            response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                            root->set("token", generate_token(login));
                            std::ostream &ostr = response.send();
                            Poco::JSON::Stringifier::stringify(root, ostr);
                            return;
                        } else if (id == 0) {
                            unauthorizedResponse(response);
                        }
                    }
                    return;
                } else if (hasSubstr(request.getURI(), "/sign/up") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                    std::string body = extractBody(request.stream(), request.getContentLength());

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
                    badRequestResponse(response, validation_exception);
                    return;
                } else if (hasSubstr(request.getURI(), "/validate") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    std::string scheme;
                    std::string token;

                    request.getCredentials(scheme, token);

                    std::cout << "Scheme/token: " << scheme << "/" << token << std::endl;
                    if (scheme == "Bearer") {
                        std::string login = validate_token(token);
                        if (login.length() > 0) {
                            response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_ACCEPTED);
                            response.setChunkedTransferEncoding(true);
                            response.setContentType("application/json");
                            Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                            root->set("login", login);
                            std::ostream &ostr = response.send();
                            Poco::JSON::Stringifier::stringify(root, ostr);
                            return;  
                        }
                    }
                    unauthorizedResponse(response);
                    return;
                }
                notFoundResponse(response);
            } catch (const std::exception &ex) {
                std::cout << "Server exception: " << ex.what() << std::endl;
                serverExceptionResponse(response, ex);
            }
        }
    private:
        std::string _format;
};


class HTTPAuthRequestFactory : public HTTPRequestHandlerFactory {
    public:
        HTTPAuthRequestFactory(const std::string &format) : _format(format){}
        HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request){
            std::cout << "request [" << request.getMethod() << "] " << request.getURI()<< std::endl;

            if (request.getURI().rfind("/auth") == 0 || 
                (request.getURI().rfind("http") == 0 && hasSubstr(request.getURI(), "/auth"))) {
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
            char * portValue = "8080";
            if (std::getenv("AUTH_SERVICE_PORT") != nullptr) {
                portValue = std::getenv("AUTH_SERVICE_PORT");
            }
            if (strlen(portValue) == 0) {
                std::cout << "Port value is missing" << std::endl;
                return Application::EXIT_DATAERR;
            }

            if (!_helpRequested) {
                ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", atoi(portValue)));
                HTTPServer srv(new HTTPAuthRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
                srv.start();
                std::cout << "auth server started on port " << portValue << std::endl;
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
