#ifndef HTTPSERVER_BASIC_HANDLER_H_
#define HTTPSERVER_BASIC_HANDLER_H_

#include "common/net/http/http_handler.h"
#include "common/system/uuid/uuid.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/proto/basic.pb.h"
#include "horoscope/proto/error_code.h"

template<typename RequestType, typename ResponseType, uint32_t CMD_ID>
class BaseAppCgi {
public:
    BaseAppCgi(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done)
        : m_request(request),
        m_response(response),
        m_done(done)
        {}

    virtual ~BaseAppCgi() {};

public:
    virtual bool Accept(int cmd_id) {
        return cmd_id == CMD_ID;
    }

    virtual void Run() {
        int ret = 0;

        do {
            ret = BeforeHandle();
            if (ret != 0) break;

            ret = RealHandle(m_request_message, &m_response_message);
            if (ret != 0) break;

            ret = AfterHandler();
            if (ret != 0) break;
        } while (false);

        if (ret != 0) {
            MakeResponse(ret);
        }

        m_done->Run();
    }

private:
    virtual int BeforeHandle() {
        LOG(ERROR) << __func__ << " not implemented.";
        return System_Error;
    }

    virtual int AfterHandler() {
        LOG(ERROR) << __func__ << " not implemented.";
        return System_Error;
    }

    virtual int RealHandle(
        const horoscope::CommonHeader& header,
        const RequestType& request,
        ResponseType* response)  = 0;

    virtual void MakeResponse(int error_code) {
        horoscope::CommonHeader* header = m_common_response.mutable_header();
        header->CopyFrom(m_common_request.header());
        header->set_error_code(error_code);
        header->set_error_msg(ErrorMsg(error_code));
        LOG(ERROR) << __func__ << " error " << error_code
                   << " msg [" << ErrorMsg(error_code) << "]";
    }

    virtual uint32_t GetCmdId() const {
        return CMD_ID;
    }

    virtual bool Accept(int cmd_id) const {
        return cmd_id == GetCmdId();
    }

private:
    const HttpRequest*                  m_request;
    HttpResponse*                       m_response;
    Closure<void>*                      m_done;

    horoscope::CommonRequest            m_common_request;
    RequestType                         m_request_message;

    horoscope::CommonResponse           m_common_response;
    ResponseType                        m_response_message;

}; // class BaseAppCgi

template<typename RequestType, typename ResponseType, uint32_t CMD_ID>
class BaseSecureAppCgi {
public:
     BaseSecureAppCgi(
        const HttpRequest* request,
        HttpResponse* response,
        Closure<void>* done)
         : BaseAppCgi(request, response, done)
    {}

    virtual ~BaseSecureAppCgi() {}

private:
    virtual int BeforeHandle() {
        m_request_uri = m_request->uri();

        const std::string& request_data = m_request->http_body();

        LOG(DEBUG) << __func__ << " uri [" << m_request_uri
                   << "] request_size " << request_data.size();

        // 1. un-base64 request buffer.
        std::string buffer;
        m_secure_func.reset(new Base64SecureFunction());
        if (!m_secure_func->Decode(request_data, &buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] base64.decode failed.";
            return Invalid_Request_Format;
        }

        // 2. parse request message.
        if (!m_common_request.ParseFromString(buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] parse common request failed.";
            return Invalid_Request_Format;
        }

        // 3. aes-decode request body buffer.
        m_secure_func.reset(new AesSecureFunction(m_common_request.header().aes_key()));
        if (!m_secure_func->Decode(m_common_request.body(), &buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] aes.decode failed. key ["
                       << m_common_request.header().aes_key() << "]";
            return Invalid_Request_Format;
        }

        // 4. parse request body message.
        if (!m_request_message.ParseFromString(buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] parse body message failed.";
            return Invalid_Request_Format;
        }
    }

    virtual int AfterHandler() {
        // 1. serialize response body message.
        std::string buffer;
        if (!m_response_message.SerializeToString(&buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] serialize body message failed.";
            return System_Error;
        }

        // 2. aes-encode response body buffer.
        Uuid uuid;
        uuid.Generate();
        std::string aes_key = uuid.ToString();
        m_secure_func.reset(new AesSecureFunction(aes_key));
        if (!m_secure_func->Encode(buffer, m_common_response.mutable_body())) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] aes.encode failed.";
            return System_Error;
        }

        // 3. serialize response message.
        horoscope::CommonHeader* header = m_common_response.mutable_header();
        header->CopyFrom(m_common_request.header());
        header->set_error_code(0);
        header->set_error_msg(ErrorMsg(0));
        if (!m_common_response.SerializeToString(&buffer)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] serialize common response failed.";
            return System_Error;
        }

        // 4. base64-encode response buffer.
        std::string response_data;
        m_secure_func.reset(new Base64SecureFunction());
        if (!m_secure_func->Encode(buffer, &response_data)) {
            LOG(ERROR) << __func__ << " uri [" << m_request_uri
                       << "] base64.encode failed.";
            return System_Error;
        }

        // 5. set http response.
        std::string length = NumberToString(response_data.size());
        m_response->SetHeader("Content-Type", "text/plain");
        m_response->SetHeader("Content-Length", length);
        m_response->set_body(response_data);

        LOG(DEBUG) << __func__ << " uri [" << m_request_uri
                   << "] response_size " << response_data.size();
    }

    virtual int RealHandle(
        const horoscope::CommonHeader& header,
        const RequestType& request,
        ResponseType* response) = 0;

private:
    std::string                         m_request_uri;

    scoped_ptr<BasicSecureFunction>     m_secure_func;

}; // class BaseSecureAppCgi

class AppCgiFactory {
public:
    AppCgiFactory();

    virtual ~AppCgiFactory();

public:
    void AddHandler(int cmd_id, BaseSecureAppCgi* handler);

    void 
}; // class AppCgiFactory

#endif // HTTPSERVER_BASIC_HANDLER_H_

