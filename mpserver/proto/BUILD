proto_library(
    name = 'message_proto',
    srcs = 'message.proto'
)

proto_library(
    name = 'event_proto',
    srcs = 'event.proto'
)

cc_library(
    name = 'pb_to_xml',
    srcs = 'pb_to_xml.cpp',
    deps = [
        '//common/base/string:string',
        '//thirdparty/protobuf:protobuf'
    ]
)

cc_library(
    name = 'xml_to_pb',
    srcs = 'xml_to_pb.cpp',
    deps = [
        '//common/base/string:string',
        '//thirdparty/protobuf:protobuf',
        '//thirdparty/tinyxml:tinyxml',
    ]
)

proto_library(
    name = 'test_proto',
    srcs = 'test.proto'
)

cc_binary(
    name = 'pb_to_xml_test',
    srcs = 'pb_to_xml_test.cpp',
    deps = [
        ':test_proto',
        ':pb_to_xml',
    ]
)

cc_binary(
    name = 'xml_to_pb_test',
    srcs = 'xml_to_pb_test.cpp',
    deps = [
        ':test_proto',
        ':xml_to_pb',
    ]
)

