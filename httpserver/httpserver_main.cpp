
#include "common/base/string/concat.h"
#include "common/net/http/http_server.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/httpserver/echo_handler.h"

DEFINE_string(server_ip, "127.0.0.1", "server ip address");
DEFINE_int32(server_port, 1234, "server port");
DEFINE_int32(threads_num, 4, "threads num");

int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    HttpServer server(FLAGS_threads_num);
    SimpleEchoHandler handler;
    HttpClosure* task = NewPermanentClosure(
        &handler, &SimpleEchoHandler::HandleGet);
    server.RegisterPrefixSimpleHandler("/echo", task);

    //HttpHandler* echo_handler = new EchoHandler();
    //server.RegisterHandler("/echo", echo_handler);

    std::string address = StringConcat(
        FLAGS_server_ip, ":", FLAGS_server_port);
    if (!server.Start(address)) {
        LOG(ERROR) << "start failed. address " << address;
        return 1;
    }

    LOG(INFO) << "listen on " << address;

    int ret = server.Run();
    LOG(INFO) << "server finish. ret " << ret;

    return ret;
}


