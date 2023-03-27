#ifndef USER_SERVICE_H
#define USER_SERVICE_H

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

#include "../database/user.h"
#include "../utils/request_utils.h"
#include "../utils/validation_utils.h"

class UserRequestHandler : public HTTPRequestHandler {
    public:
        UserRequestHandler(const std::string &format): _format(format){};

        void handleRequest(HTTPServerRequest &request, HTTPServerResponse &response) {
            try {
                std::string scheme;
                std::string token;
                request.getCredentials(scheme, token);

                std::string login = validateToken(scheme, token);
                if (login.length() == 0) {
                    std::cout << "Failed to authorize user" << std::endl;
                    unauthorizedResponse(response);
                    return;
                }
                std::cout << "Authorized user " << login << std::endl;

                // TODO bad code, need refactor
                if (hasSubstr(request.getURI(), "/edit") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
                    std::string body = extractBody(request.stream(), request.getContentLength());
                    std::string validation_exception;
                    if (body.length() > 0) {
                        Poco::JSON::Object::Ptr obj = parseJson(body);
                        if (!obj->has("id")) {
                            validation_exception = "Can't edit without user id!";
                        } else {
                            database::User user = database::User::get_by_id(obj->getValue<long>("id"));
                            if (user.get_id() <= 0) {
                                validation_exception = "Can't find user";
                            } else if (user.get_login() != login) { // todo add user role check
                                validation_exception = "Current user can't edit " + user.get_login();
                            } else {
                                bool canUpdate = true;
                                if (obj->has("name")) {
                                    std::string value = obj->getValue<std::string>("name");
                                    canUpdate &= check_name(value, validation_exception);
                                    user.name() = obj->getValue<std::string>("name");
                                }
                                if (obj->has("email")) {
                                    std::string value = obj->getValue<std::string>("email");
                                    canUpdate &= check_email(value, validation_exception);
                                    user.email() = obj->getValue<std::string>("email");
                                }
                                if (canUpdate) {
                                    user.save_to_db();
                                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                                    response.setChunkedTransferEncoding(true);
                                    response.setContentType("application/json");
                                    std::ostream &ostr = response.send();
                                    ostr << user.get_id();
                                    return;
                                }
                            }
                        }
                        
                    } else {
                        validation_exception = "Body is missing!";
                    }
                    badRequestResponse(response, validation_exception);
                    return;
                } else if (hasSubstr(request.getURI(), "/search") && request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    // TODO add get by id
                    const Poco::URI uri(request.getURI());
                    const Poco::URI::QueryParameters params = uri.getQueryParameters();
                    database::User likeUser;

                    for(std::pair<std::string, std::string> key_value: params) {
                        std::string key = key_value.first;
                        std::string value = key_value.second;
                        if (key == "login") {
                            likeUser.login() = value;
                        } else if (key == "name") {
                            likeUser.name() = value;
                        } else if (key == "email") {
                            likeUser.email() = value;
                        } else {
                            std::cout << "Param " << key << " :: " << value << " ignored" << std::endl;
                        }
                    }
                    
                    std::vector<database::User> result = database::User::search(likeUser);
                    std::cout << "Found total " << result.size() << std::endl;
                    Poco::JSON::Array arr;
                    for (database::User user: result) {
                        // without toJson don't work, dunno why
                        arr.add(user.toJSON());
                    }
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(arr, ostr);
                    return;
                }
            } catch (const std::exception &ex) {
                serverExceptionResponse(response, ex);
                std::cout << "Server exception: " << ex.what() << std::endl;
                return;
            }
            notFoundResponse(response);
        }
    private:
        std::string _format;
};

class HTTPUserRequestFactory : public HTTPRequestHandlerFactory {
    public:
        HTTPUserRequestFactory(const std::string &format) : _format(format){}
        HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request){
            std::cout << "request [" << request.getMethod() << "] " << request.getURI()<< std::endl;

            if (request.getURI().rfind("/user") == 0) {
                return new UserRequestHandler(_format);
            }
            return 0;
        }
    private:
        std::string _format;
};

class HTTPUserWebServer : public Poco::Util::ServerApplication {
    public:
        HTTPUserWebServer() : _helpRequested(false){}
        ~HTTPUserWebServer() {}
    protected:
        void initialize(Application &self) {
            loadConfiguration();
            ServerApplication::initialize(self);
        }
        void uninitialize() {
            ServerApplication::uninitialize();
        }
        int main([[maybe_unused]] const std::vector<std::string> &args) {
            const char * portValue = "8080";
            if (std::getenv("USER_SERVICE_PORT") != nullptr) {
                portValue = std::getenv("USER_SERVICE_PORT");
            }

            if (!_helpRequested) {
                ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", atoi(portValue)));
                HTTPServer srv(new HTTPUserRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
                srv.start();
                std::cout << "user server started on port " << portValue << std::endl;
                waitForTerminationRequest();
                srv.stop();
                std::cout << "user server stoped" << std::endl;
            }
            return Application::EXIT_OK;
        }
    private:
        bool _helpRequested;
};

#endif