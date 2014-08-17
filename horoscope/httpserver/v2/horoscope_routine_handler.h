#ifndef HTTPSERVER_HOROSCOPE_ROUTINE_HANDLER_H_
#define HTTPSERVER_HOROSCOPE_ROUTINE_HANDLER_H_

#include <string>

#include "common/base/closure.h"
#include "common/base/scoped_ptr.h"

class HttpRequest;
class HttpResponse;
class HttpServer;

class HoroscopeRoutineHandler {
public:
    HoroscopeRoutineHandler(const std::string& prefix, HttpServer* server);

    virtual ~HoroscopeRoutineHandler();

public:
    void Process(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

    static std::string CacheName();

}; // class HoroscopeRoutineHandler

#endif // HTTPSERVER_HOROSCOPE_ROUTINE_HANDLER_H_
