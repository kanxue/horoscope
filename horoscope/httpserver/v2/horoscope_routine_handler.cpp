#include "horoscope_routine_handler.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/format.h"
#include "common/base/string/string_number.h"
#include "common/net/http/http_server.h"
#include "common/system/time/timestamp.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/httpserver/v2/horoscope_routine_logic.h"

HoroscopeRoutineHandler::HoroscopeRoutineHandler(
    const std::string& prefix, HttpServer* server)
{
    HttpClosure* task = NewPermanentClosure(
        this, &HoroscopeRoutineHandler::Process);
    server->RegisterPrefixSimpleHandler(prefix, task);
    LOG(INFO)
        << "this " << this
        << " task " << (void*)task
        << " prefix [" << prefix << "]";
}

HoroscopeRoutineHandler::~HoroscopeRoutineHandler()
{}

void HoroscopeRoutineHandler::Process(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
{
    int64_t start = GetTimeStampInUs();

    LOG(INFO)
        << "request " << (void*)request
        << " response " << (void*)response
        << " done " << (void*)done
        << " header [" << request->headers().ToString() << "]";

    std::string uri = request->uri();
    HoroscopeRoutineLogic logic(request, response);
    logic.Run();
    const std::string& body = response->http_body();

    response->SetHeader("Content-Type", "text/html");
    response->SetHeader("Content-Length", NumberToString(body.size()));
    done->Run();

    int64_t end = GetTimeStampInUs();
    LOG(INFO)
        << "uri [" << uri
        << "] cost " << (end - start) << " us";
}

std::string HoroscopeRoutineHandler::CacheName()
{
    static std::string cache_name("HoroscopeRoutine");
    return cache_name;
}
