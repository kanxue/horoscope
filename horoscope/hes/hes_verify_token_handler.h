
#ifndef HES_HES_VERIFY_TOKEN_HANDLER_H_
#define HES_HES_VERIFY_TOKEN_HANDLER_H_

#include "horoscope/hes/hes_handler.h"

class HesVerifyTokenHandler : public HesHandler {
public:
    HesVerifyTokenHandler(HttpServer* server);

    virtual ~HesVerifyTokenHandler();

public:
    virtual int Process(const HttpRequest* request, std::string* output);
}; // class HesVerifyTokenHandler

#endif // HES_HES_VERIFY_TOKEN_HANDLER_H_

