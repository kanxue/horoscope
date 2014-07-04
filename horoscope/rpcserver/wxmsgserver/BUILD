proto_library(
    name = 'wxmsgserver_proto',
    srcs = 'wxmsgserver.proto'
)

cc_library(
    name = 'wxmsgserver',
    srcs = 'wxmsgserver_impl.cpp',
    deps = [
        ':wxmsgserver_proto',
        '//poppy:poppy'
    ]
)

cc_binary(
    name = 'wxmsgserver_main',
    srcs = 'wxmsgserver_main.cpp',
    deps = [
        ':wxmsgserver',
        '//common/base:module'
    ]
)

