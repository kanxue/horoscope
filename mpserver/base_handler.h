#ifndef MPSERVER_BASE_HANDLER_H_
#define MPSERVER_BASE_HANDLER_H_

#include <string>
#include "common/base/closure.h"

class HttpRequest;
class HttpResponse;
class HttpServer;

class BaseHandler {
public:
    BaseHandler(
        const std::string& path,
        HttpServer* server);

    virtual ~BaseHandler();

public:
    void Handler(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

    virtual void Process(
        const std::string& uri,
        const std::string& input,
        std::string* output) = 0;
}; // class BaseHandler

#endif // MPSERVER_BASE_HANDLER_H_

