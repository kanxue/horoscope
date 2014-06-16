#ifndef MPSERVER_PROTO_XML_TO_PB_H_
#define MPSERVER_PROTO_XML_TO_PB_H_

#include <string>

namespace google {
namespace protobuf {
class Message;
} // namespace protobuf
} // namespace google

bool ProtoMessageToXmlWithoutRoot(
    const google::protobuf::Message& message,
    std::string* xml_string,
    std::string* error = NULL);

bool ProtoMessageToXmlWithRoot(
    const google::protobuf::Message& message,
    std::string* xml_string,
    std::string* error = NULL);

#endif // MPSERVER_PROTO_XML_TO_PB_H_

