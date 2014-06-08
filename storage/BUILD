cc_library(
    name = 'redis_client',
    srcs = 'redis_client.cpp',
    deps = [
        '//thirdparty/glog:glog',
        '//thirdparty/redis:redisclient',
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

