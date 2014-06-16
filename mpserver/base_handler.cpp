#include "horoscope/mpserver/base_handler.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/string_number.h"
#include "common/net/http/http_server.h"

#include "thirdparty/glog/logging.h"

BaseHandler::BaseHandler(
    const std::string& path,
    HttpServer* server)
{
    HttpClosure* task = NewPermanentClosure(this, &BaseHandler::Handler);
    server->RegisterPrefixSimpleHandler(path, task);

    LOG(INFO)
        << "this " << this
        << " task " << (void*)task
        << " path [" << path << "]";
}

BaseHandler::~BaseHandler()
{}

void BaseHandler::Handler(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
{
    int64_t start = GetTimeStampInUs();

    LOG(INFO)
        << "request " << (void*)request
        << " response " << (void*)response
        << " done " << (void*)done;

    std::string uri = request->uri();
    std::string input = request->http_body();
    std::string output;

    // call virtual function.
    Process(uri, input, &output);

    response->SetHeader("Content-Type", "text/html");
    response->SetHeader("Content-Length", NumberToString(output.size()));
    response->set_body(output);

    done->Run();

    ReplaceAll(&input, "\n", "");
    ReplaceAll(&output, "\n", "");

    int64_t end = GetTimeStampInUs();

    LOG(INFO)
        << "uri [" << uri
        << "] input [" << input
        << "] output [" << output
        << "] cost " << (end - start) << " us";
}

