
proto_library(
    name = 'hes_proto',
    srcs = 'hes.proto'
)

cc_library(
    name = 'hes',
    srcs = [
        'hes_handler.cpp',
        'hes_token_handler.cpp',
        'hes_verify_token_handler.cpp',
    ],
    deps = [
        ':hes_proto',

        '//common/encoding:encoding',
        '//common/net/http:http',
        '//common/web:web',

        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',

        '//horoscope/storage:storage'
    ]
)

cc_binary(
    name = 'hes_main',
    srcs = 'hes_main.cpp',
    deps = ':hes'
)

