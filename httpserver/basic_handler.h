#ifndef HTTPSERVER_BASIC_HANDLER_H_
#define HTTPSERVER_BASIC_HANDLER_H_

#include "common/net/http/http_handler.h"

class BasicHandler {
public:
    BasicHandler(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

    virtual ~BasicHandler();

public:
    void Run();

private:
    virtual void BeforeHandle();

    virtual void AfterHanld();

    virtual void RealHandle(
        const std::string& request_data,
        std::string* response_data) = 0;

    void MakeErrorResponse(int error_code);

private:
    const HttpRequest* m_request;
    HttpResponse*      m_response;
    Closure<void>*     m_done;

    std::string        m_request_uri;
    std::string        m_request_data;
    std::string        m_response_data;
}; // class BasicHandler

class BasicSecureHandler : public BasicHandler {
public:
    BasicSecureHandler(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done);

    virtual ~BasicSecureHandler();

private:
    virtual void BrforeHandle();

    virtual void AfterHandle();

    virtual void RealHandle(
        const std::string& request_data,
        std::string* response_data) = 0;
}; // class BasicSecureHandler

#endif // HTTPSERVER_BASIC_HANDLER_H_

