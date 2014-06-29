
cc_library(
    name = 'mpserver',
    srcs = [
        'base_handler.cpp',
        'base_processor.cpp',
        'mp_handler.cpp',

        'click_event_processor.cpp',
        'subscribe_event_processor.cpp',
        'text_message_processor.cpp',
        'unsubscribe_event_processor.cpp',
        'view_event_processor.cpp',
    ],
    deps = [
        '//common/encoding:encoding',
        '//common/net/http:http',
        '//common/web:web',

        '//thirdparty/gflags:gflags',
        '//thirdparty/glog:glog',

        '//horoscope/mpserver/proto:event_proto',
        '//horoscope/mpserver/proto:message_proto',
        '//horoscope/mpserver/proto:pb_to_xml',
        '//horoscope/mpserver/proto:xml_to_pb',

        '//horoscope/storage:storage',
    ]
)

cc_binary(
    name = 'mpserver_main',
    srcs = 'mpserver_main.cpp',
    deps = ':mpserver'
)

