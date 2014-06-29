#include <iostream>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "horoscope/storage/redis_client.h"

DEFINE_string(redis_ip, "127.0.0.1", "redis ip");
DEFINE_int32(redis_port, 6379, "redis port");
DEFINE_int32(redis_db_index, 0, "redis db index");

DEFINE_string(redis_op, "", "get/set/del/exist/substr");

DEFINE_string(redis_key, "", "redis key");
DEFINE_string(redis_value, "", "redis value");
DEFINE_int32(redis_begin, 0, "redis begin");
DEFINE_int32(redis_end, 0, "redis end");

int main(int argc, char** argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    RedisClient client;
    int ret = client.Connect(
        FLAGS_redis_ip, FLAGS_redis_port, FLAGS_redis_db_index);
    if (ret != 0) {
        std::cout << "connect failed.\n";
        return 1;
    }

    std::cout << "op " << FLAGS_redis_op << std::endl
              << "key [" << FLAGS_redis_key << "]\n";

    if (FLAGS_redis_op == "get") {
        std::string value;
        ret = client.Get(FLAGS_redis_key, &value);
        std::cout << "value [" << value << "]\n";
    } else if (FLAGS_redis_op == "set") {
        ret = client.Set(FLAGS_redis_key, FLAGS_redis_value);
    } else if (FLAGS_redis_op == "del") {
        ret = client.Delete(FLAGS_redis_key);
    } else if (FLAGS_redis_op == "exist") {
        bool is_exist = false;
        ret = client.IsExist(FLAGS_redis_key, &is_exist);
        std::cout << "is_exist " << (int)is_exist << std::endl;
    } else if (FLAGS_redis_op == "substr") {
        std::string value;
        ret = client.Substr(FLAGS_redis_key, FLAGS_redis_begin, FLAGS_redis_end, &value);
        std::cout << "value [" << value << "]\n";
    } else {
        std::cout << "plz use correct op.\n";
        return 1;
    }
    
    std::cout << "ret " << ret << std::endl;

    return 0;
}
