
#include "common/base/string/concat.h"
//#include "common/file/file.h"
#include "common/net/http/http_server.h"
#include "thirdparty/gflags/gflags.h"
#include "horoscope/httpserver/v2/horoscope_routine_handler.h"
#include "horoscope/storage/common_memory_cache.h"

DEFINE_string(server_ip, "127.0.0.1", "server ip address");
DEFINE_int32(server_port, 1234, "server port");
DEFINE_int32(threads_num, 4, "threads num");
DEFINE_string(routine_prefix, "/cgi-bin/horoscope/routine", "routine prefix url");

int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    // if (!common::File::Init()) {
    //     LOG(ERROR) << "init file failed. exiting...";
    //     return -1;
    // }

    CommonMemoryCacheGroupSingleton::Instance(
        ).CreateCache(HoroscopeRoutineHandler::CacheName());

    HttpServer server(FLAGS_threads_num);
    HoroscopeRoutineHandler handler(FLAGS_routine_prefix, &server);

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

