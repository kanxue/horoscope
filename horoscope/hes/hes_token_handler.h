
#ifndef HES_HES_TOKEN_HANDLER_H_
#define HES_HES_TOKEN_HANDLER_H_

#include "horoscope/hes/hes_handler.h"

class HesTokenHandler : public HesHandler {
public:
    HesTokenHandler(HttpServer* server);

    virtual ~HesTokenHandler();

public:
    virtual int Process(const HttpRequest* request, std::string* output);
}; // class HesTokenHandler

#endif // HES_HES_TOKEN_HANDLER_H_

