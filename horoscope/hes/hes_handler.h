
#ifndef HES_HES_HANDLER_H_
#define HES_HES_HANDLER_H_

#include <string>
#include "common/base/closure.h"
#include "common/net/http/http_server.h"

//class HttpRequest;
//class HttpResponse;
//class HttpServer;

class HesHandler {
public:
    HesHandler(
        const std::string& path,
        HttpServer* server);

    virtual ~HesHandler();

public:
    void Handler(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

    virtual int Process(
        const HttpRequest* request,
        std::string* output) = 0;
}; // class HesHandler

#endif // HES_HES_HANDLER_H_

