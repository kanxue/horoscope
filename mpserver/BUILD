cc_library(
    name = 'mpserver',
    srcs = [
        'base_handler.cpp',
        'base_processor.cpp',
        'mp_handler.cpp',
        'text_message_processor.cpp',
    ],
    deps = [
        '//common/net/http:http',
        '//common/web:web',
        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',

        '//horoscope/mpserver/proto:event_proto',
        '//horoscope/mpserver/proto:message_proto',
        '//horoscope/mpserver/proto:pb_to_xml',
        '//horoscope/mpserver/proto:xml_to_pb',
    ]
)

cc_binary(
    name = 'mpserver_main',
    srcs = 'mpserver_main.cpp',
    deps = ':mpserver'
)

