cc_library(
    name = 'mpserver',
    srcs = [
        'base_handler.cpp',
        'base_processor.cpp',
        'mp_handler.cpp',
        'verify_token_processor.cpp',
    ],
    deps = [
        '//common/net/http:http',
        '//common/web:web',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog'
    ]
)

cc_binary(
    name = 'mpserver_main',
    srcs = 'mpserver_main.cpp',
    deps = ':mpserver'
)

