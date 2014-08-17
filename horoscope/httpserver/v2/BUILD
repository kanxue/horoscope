
proto_library(
    name = 'horoscope_routine_proto',
    srcs = 'horoscope_routine.proto'
)

cc_library(
    name = 'httpserver',
    srcs = [
        'httpserver_filter.cpp',
        'horoscope_routine_handler.cpp',
        'horoscope_routine_logic.cpp',
    ],
    deps = [
        ':horoscope_routine_proto',

#        '//common/file:local_file',
        '//common/net/http:http',
        '//common/system/concurrency:concurrency',
        '//common/system/io:io',

        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',

        '//horoscope/mpserver/proto:pb_to_xml',
        '//horoscope/storage:common_memory_cache',
        '//horoscope/storage:storage',
    ]
)

cc_binary(
    name = 'httpserver_main',
    srcs = 'httpserver_main.cpp',
    deps = ':httpserver'
)

