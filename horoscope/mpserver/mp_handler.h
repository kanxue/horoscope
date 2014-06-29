#ifndef MPSERVER_MP_HANDLER_H_
#define MPSERVER_MP_HANDLER_H_

#include <string>

#include "horoscope/mpserver/base_handler.h"

class HttpServer;

class MpHandler : public BaseHandler {
public:
    MpHandler(HttpServer* server);

    virtual ~MpHandler();

public:
    virtual void Process(
        const std::string& uri,
        const std::string& input,
        std::string* output);

private:
    void ProcessVerifyToken();
}; // class MpHandler

#endif // MPSERVER_MP_HANDLER_H_

