
#include <signal.h>

#include "common/base/module.h"
#include "common/base/scoped_ptr.h"
#include "common/base/string/format.h"
#include "thirdparty/gflags/gflags.h"
#include "poppy/rpc_server.h"

#include "horoscope/rpcserver/wxmsgserver/wxmsgserver_impl.h"

DEFINE_string(wxmsgserver_ip, "127.0.0.1", "");
DEFINE_int32(wxmsgserver_port, 1234, "");
DEFINE_int32(wxmsgserver_net_threads_num, 1, "");
DEFINE_int32(wxmsgserver_work_threads_num, 4, "");
DEFINE_string(wxmsgserver_basic_host, "api.weixin.qq.com", "");
DEFINE_string(wxmsgserver_media_host, "file.api.weixin.qq.com", "");

bool g_quit_flag = false;

void SignalHandler(int signal_tag)
{
    VLOG(2)
        << "Receive Signal = " << signal_tag
        << ", stop providing wxmsg server.";
    g_quit_flag = true;
}

int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    InitAllModules(&argc, &argv);

    WxMsgServerOptions options;
    options.m_threads_num = FLAGS_wxmsgserver_work_threads_num;
    options.m_basic_host = FLAGS_wxmsgserver_basic_host;
    options.m_media_host = FLAGS_wxmsgserver_media_host;

    scoped_ptr<WxMsgServerImpl> impl(new WxMsgServerImpl());
    if (!impl->Initialize(options)) {
        LOG(ERROR) << "initialize impl failed. exit...";
        return EXIT_FAILURE;
    }

    scoped_ptr<poppy::RpcServer> rpc_server(
        new poppy::RpcServer(FLAGS_wxmsgserver_net_threads_num));
    rpc_server->RegisterService(impl.get());

    std::string address;
    StringFormatTo(
        &address, "%s:%d",
        FLAGS_wxmsgserver_ip.c_str(),
        FLAGS_wxmsgserver_port);
    if (!rpc_server->Start(address)) {
        LOG(ERROR) << "start rpc server failed. address " << address;
        return EXIT_FAILURE;
    }

    signal(SIGUSR1, SignalHandler);

    while (!g_quit_flag) {
        ThisThread::Sleep(1000);
    }

    return EXIT_SUCCESS;
}

