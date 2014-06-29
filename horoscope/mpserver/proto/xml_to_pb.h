#ifndef MPSERVER_PROTO_PB_TO_XML_H_
#define MPSERVER_PROTO_PB_TO_XML_H_

#include <string>

class TiXmlNode;

namespace google {
namespace protobuf {
class FieldDescriptor;
class Message;
} // namespace protobuf
} // namespace google

bool XmlToProtoMessage(
    const std::string& xml_string,
    google::protobuf::Message* message,
    std::string* error = NULL);

bool XmlValueToProtoMessage(
    const TiXmlNode* root,
    const std::string& list_name,
    const std::string& item_name,
    const google::protobuf::FieldDescriptor* file_descriptor,
    google::protobuf::Message* message,
    std::string* error = NULL);

#endif // MPSERVER_PROTO_PB_TO_XML_H_

