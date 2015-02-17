cc_library(
    name = 'simpletemplate',
    srcs = 'simple_template.cpp',
    deps = []
)
cc_library(
    name = 'httpserver',
    srcs = 'chinese_yang_handler.cpp',
    deps = [
        '//common/net/http:http',
        '//common/web:web',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',
	':simpletemplate'
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
