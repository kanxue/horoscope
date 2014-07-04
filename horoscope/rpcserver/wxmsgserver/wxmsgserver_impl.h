#ifndef RPCSERVER_WXMSGSERVER_IMPL_H_
#define RPCSERVER_MSGSERVER_IMPL_H_

#include <string>
#include "common/base/inttypes.h"
#include "common/base/scoped_ptr.h"
#include "horoscope/rpcserver/wxmsgserver/wxmsgserver.pb.h"

class ThreadPool;

struct WxMsgServerOptions {
    int32_t       m_threads_num;
    std::string   m_basic_host;
    std::string   m_media_host;
}; // struct WxMsgServerOptions

class WxMsgServerImpl : public wxmsgserver::WxMsgServer {
public:
    WxMsgServerImpl();

    virtual ~WxMsgServerImpl();

public:
    bool Initialize(const WxMsgServerOptions& options_value);

    void AccessToken(
        google::protobuf::RpcController* controller,
        const wxmsgserver::AccessTokenRequest* request,
        wxmsgserver::AccessTokenResponse* response,
        google::protobuf::Closure* done);

    // TODO(kanxue) impl more...

private:
    void DoAccessToken(
        google::protobuf::RpcController* controller,
        const wxmsgserver::AccessTokenRequest* request,
        wxmsgserver::AccessTokenResponse* response,
        google::protobuf::Closure* done);

private:
    WxMsgServerOptions      m_options;
    scoped_ptr<ThreadPool>  m_thread_pool;
}; // class WxMsgServerImpl

#endif // RPCSERVER_MSGSERVER_IMPL_H_

