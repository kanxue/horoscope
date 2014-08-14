#include "wxmsgserver_impl.h"

#include "common/system/concurrency/thread_pool.h"
#include "thirdparty/glog/logging.h"

WxMsgServerImpl::WxMsgServerImpl()
{}

WxMsgServerImpl::~WxMsgServerImpl()
{}

bool WxMsgServerImpl::Initialize(const WxMsgServerOptions& options_value)
{
    m_options = options_value;
    m_thread_pool.reset(
        new ThreadPool(m_options.m_threads_num, m_options.m_threads_num));
    return true;
}

void WxMsgServerImpl::AccessToken(
    google::protobuf::RpcController* controller,
    const wxmsgserver::AccessTokenRequest* request,
    wxmsgserver::AccessTokenResponse* response,
    google::protobuf::Closure* done)
{
    Closure<void>* task = NewClosure(
        this, &WxMsgServerImpl::DoAccessToken,
        controller, request, response, done);
    m_thread_pool->AddTask(task);
}

void WxMsgServerImpl::DoAccessToken(
    google::protobuf::RpcController* controller,
    const wxmsgserver::AccessTokenRequest* request,
    wxmsgserver::AccessTokenResponse* response,
    google::protobuf::Closure* done)
{
    LOG(INFO) << __func__ << " request " << request->ShortDebugString();

    // TODO(kanxue) impl it.
    response->set_access_token("mock access-token from kanxue.");
    response->set_expired(1024);
    LOG(INFO) << __func__ << " response " << response->ShortDebugString();

    done->Run();
}

