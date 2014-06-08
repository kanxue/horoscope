#include "horoscope/httpserver/basic_handler.h"

#include "common/base/string/format.h"
#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"

#include "horoscope/httpserver/secure_functions.h"
#include "horoscope/proto/error_code.h"
#include "horoscope/proto/horoscope_def.h"

BasicHandler::BasicHandler(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
    : m_request(request),
      m_response(response),
      m_done(done)
{}

BasicHandler::~BasicHandler()
{}

void BasicHandler::Run()
{
    BeforeHandle();
    RealHandle(m_request_data, &m_response_data);
    AfterHandler();

    m_done->Run();
}

void BasicHandler::BeforeHandle()
{
    m_request_uri = m_request->uri();
    m_request_data = m_request->http_body();
    m_response_data.clear();
}

void BasicHandler::AfterHandler()
{
    std::string length = NumberToString(m_response_data.size());
    m_response->SetHeader("Content-Type", "text/plain");
    m_response->SetHeader("Content-Length", length);
    m_response->set_body(m_response_data);
}

void BasicHandler::MakeErrorResponse(int error_code)
{
    m_response_data = StringFormat(
        "error: %d[%s]", error_code, ErrorMsg(error_code).c_str());
}

BasicSecureHandler::BasicSecureHandler(
    const HttpRequest* request,
    HttpResponse* response,
    Closure<void>* done)
    : BasicHandler(request, response, done)
{}

BasicSecureHandler::~BasicSecureHandler()
{}

void BasicSecureHandler::BeforeHandle()
{
    BasicHandler::BeforeHandle();

    std::string buffer;
    Base64AndAesSecureFunction func(HOROSCOPE_HTTPSERVER_AES_KEY);
    if (!func.Decode(m_request_data, &buffer)) {
        LOG(ERROR) << "decode failed.";
        MakeErrorResponse(Invalid_Request_Format);
        return ;
    }

    std::swap(m_request_data, buffer);
}

void BasicSecureHandler::AfterHandle()
{
    std::string buffer;
    Base64AndAesSecureFunction func(HOROSCOPE_HTTPSERVER_AES_KEY);
    if (!func.Encode(m_response_data, &buffer)) {
        LOG(ERROR) << "encode failed.";
        MakeErrorResponse(System_Error);
        return ;
    }

    std::swap(m_response_data, buffer);

    BasicHandler::AfterHandle();
}

