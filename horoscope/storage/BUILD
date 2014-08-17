
proto_library(
    name = 'storage_proto',
    srcs = 'storage.proto'
)

cc_library(
    name = 'common_memory_cache',
    srcs = 'common_memory_cache.cpp',
    deps = [
        '//common/system/concurrency:concurrency',
        '//thirdparty/glog:glog',
        '//thirdparty/protobuf:protobuf',
    ]
)

cc_library(
    name = 'redis_client',
    srcs = 'redis_client.cpp',
    deps = [
        '//thirdparty/glog:glog',
        '//thirdparty/redis:redisclient',
    ]
)

cc_library(
    name = 'storage',
    srcs = [
        'common_def.cpp',
        'storage_mysql_client.cpp',
        'storage_redis_client.cpp',
    ],
    deps = [
        ':redis_client',
        ':storage_proto',

        '//common/encoding:encoding',
        '//common/text/regex:regex',
        '//common/system/concurrency:concurrency',
        '//thirdparty/mysql++:mysqlpp',
    ]
)

cc_binary(
    name = 'redis_client_tools',
    srcs = 'redis_client_tools.cpp',
    deps = [
        ':redis_client',
        '//thirdparty/gflags:gflags',
    ]
)

cc_test(
    name = 'storage_mysql_client_test',
    srcs = 'storage_mysql_client_test.cpp',
    deps = ':storage'
)

