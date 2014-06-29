cc_library(
    name = 'httpserver',
    srcs = 'echo_handler.cpp',
    deps = [
        '//common/net/http:http',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog'
    ]
)

cc_binary(
    name = 'httpserver_main',
    srcs = 'httpserver_main.cpp',
    deps = ':httpserver'
)

cc_binary(
    name = 'test_main',
    srcs = 'test_main.cpp',
    deps = [
        '//common/system/concurrency:concurrency',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog'
    ]
)
