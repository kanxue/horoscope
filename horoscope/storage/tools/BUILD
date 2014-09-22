
cc_binary(
    name = 'init_redis_horoscope_attr',
    srcs = 'init_redis_horoscope_attr.cpp',
    deps = '//horoscope/storage:storage'
)

cc_binary(
    name = 'rebuild_userattr',
    srcs = 'rebuild_userattr.cpp',
    deps = [
        '//horoscope/storage:storage',
        '//horoscope/mpserver/proto:message_proto',
        '//horoscope/mpserver/proto:xml_to_pb',
    ]
)

