
#include "horoscope/hes/hes_handler.h"

#include "common/encoding/pb_to_json.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/format.h"
#include "common/base/string/string_number.h"
#include "common/net/http/http_server.h"
#include "common/web/url.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/hes/hes.pb.h"
#include "horoscope/storage/hesapierrno.h"

HesHandler::HesHandler(const std::string& path, HttpServer* server)
{
    HttpClosure* task = NewPermanentClosure(this, &HesHandler::Handler);
    server->RegisterPrefixSimpleHandler(path, task);
    LOG(INFO)
        << __func__ << " this " << this
        << " task " << (void*)task
        << " path [" << path << "]";
}

HesHandler::~HesHandler()
{}

void HesHandler::Handler(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
{
    int64_t start = GetTimeStampInUs();
    std::string uri = request->uri();
    std::string input = request->http_body();
    std::string output, error;

    LOG(INFO)
        << "uri [" << uri
        << "] request " << (void*)request
        << " response " << (void*)response
        << " done " << (void*)done;

    // call virtual function.
    int ret = Process(request, &output);

    std::string real_output, base_output;
    hes::BaseResponse base_resp;
    base_resp.set_code(ret);
    base_resp.set_errmsg(HesApiErrMsg(ret));
    ProtoMessageToJson(base_resp, &base_output, &error, false);
    if ((ret != 0) || output.empty()) {
        real_output = base_output;
    } else {
        real_output = StringFormat(
            "{\"base\":%s,\"result\":%s}",
            base_output.c_str(), output.c_str());
    }

    real_output = web::url::Url::DecodeUrl(
        real_output,  web::url::Url::E_DECODE_PERCENT_SIGN_CHAR);

    response->set_body(real_output);
    response->SetHeader("Content-Type", "text/html");
    response->SetHeader("Content-Length", NumberToString(real_output.size()));

    done->Run();

    ReplaceAllChars(&input, "\n", ' ');
    ReplaceAllChars(&real_output, "\n", ' ');

    int64_t end = GetTimeStampInUs();

    LOG(INFO)
        << "uri [" << uri
        << "] input [" << input
        << "] output [" << output
        << "] cost " << (end - start) << " us";
}

