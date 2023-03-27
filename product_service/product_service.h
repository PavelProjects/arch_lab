#ifndef LOT_SERVICE_H
#define LOT_SERVICE_H


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
#include "../database/product.h"
#include "../utils/request_utils.h"
#include "../utils/validation_utils.h"

class ProductRequesthandler: public HTTPRequestHandler {
    public:
        ProductRequesthandler(const std::string &format): _format(format){};
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

                if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_GET) {
                    const Poco::URI uri(request.getURI());
                    const Poco::URI::QueryParameters params = uri.getQueryParameters();
                    database::ProductSearch search;

                    for(std::pair<std::string, std::string> key_value: params) {
                        // TODO REWORK BAD CODE!
                        if (key_value.first == "id") {
                            search.id = atoi(key_value.second.c_str());
                        } else if (key_value.first == "cost_min") {
                            search.cost_min = atoi(key_value.second.c_str());
                        } else if (key_value.first == "cost_max") {
                            search.cost_max = atoi(key_value.second.c_str());
                        } else if (key_value.first == "seller_id") {
                            search.seller_id = atoi(key_value.second.c_str());
                        } else if (key_value.first == "name") {
                            search.name = key_value.second;
                        } else if (key_value.first == "creation_date_start") {
                            search.creation_date_start = parse_time(key_value.second);
                        } else if (key_value.first == "creation_date_end") {
                            search.creation_date_end = parse_time(key_value.second);
                        }
                    }
                    //todo add search by id
                    if (hasSubstr(request.getURI(), "/search")) {
                        std::vector<database::Product> result = database::Product::search(search);
                        std::cout << "Found total " << result.size() << std::endl;
                        Poco::JSON::Array arr;
                        for (database::Product user: result) {
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
                } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_POST) {
                    //todo create
                } else if (request.getMethod() == Poco::Net::HTTPRequest::HTTP_PUT) {
                    //todo edit
                }
            
            } catch (std::exception &ex) {
                serverExceptionResponse(response, ex);
                std::cout << "Server exception: " << ex.what() << std::endl;
                return;
            }
            notFoundResponse(response);
        }
    private:
        std::string _format;
};

class HTTPProductRequestFactory : public HTTPRequestHandlerFactory {
    public:
        HTTPProductRequestFactory(const std::string &format) : _format(format){}
        HTTPRequestHandler *createRequestHandler([[maybe_unused]] const HTTPServerRequest &request){
            std::cout << "request [" << request.getMethod() << "] " << request.getURI()<< std::endl;

            if (request.getURI().rfind("/product") == 0) {
                return new ProductRequesthandler(_format);
            }
            return 0;
        }
    private:
        std::string _format;
};

class HTTPProductWebServer : public Poco::Util::ServerApplication {
    public:
        HTTPProductWebServer() : _helpRequested(false){}
        ~HTTPProductWebServer() {}
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
            if (std::getenv("PRODUCT_SERVICE_PORT") != nullptr) {
                portValue = std::getenv("PRODUCT_SERVICE_PORT");
            }
            if (strlen(portValue) == 0) {
                std::cout << "Port value is missing" << std::endl;
                return Application::EXIT_DATAERR;
            }

            if (!_helpRequested) {
                ServerSocket svs(Poco::Net::SocketAddress("0.0.0.0", atoi(portValue)));
                HTTPServer srv(new HTTPProductRequestFactory(DateTimeFormat::SORTABLE_FORMAT), svs, new HTTPServerParams);
                srv.start();
                std::cout << "service lot server started on port " << portValue << std::endl;
                waitForTerminationRequest();
                srv.stop();
                std::cout << "service lot server stoped" << std::endl;
            }
            return Application::EXIT_OK;
        }
    private:
        bool _helpRequested;
};


#endif