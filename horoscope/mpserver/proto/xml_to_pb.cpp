#include "xml_to_pb.h"

#include "common/base/string/compare.h"
#include "common/base/string/string_number.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"
#include "thirdparty/tinyxml/tinyxml.h"

bool XmlToProtoMessage(
    const std::string& xml_string,
    google::protobuf::Message* message,
    std::string* error)
{
    TiXmlDocument document;
    if (!document.Parse((xml_string + "\n").c_str())) {
        if (error) {
            error->assign("parse xml failed. msg [" + std::string(document.ErrorDesc()) + "]");
            return false;
        }
    }

    TiXmlNode* root = document.RootElement();

    return XmlValueToProtoMessage(root, "", "", NULL, message, error);
}

bool XmlValueToProtoMessage(
    const TiXmlNode* root,
    const std::string& list_name,
    const std::string& item_name,
    const google::protobuf::FieldDescriptor* file_descriptor,
    google::protobuf::Message* message,
    std::string* error)
{
    if (!root || !message) return true;

    using namespace std;
    using namespace google::protobuf;

    std::string tmp_list_name = list_name;
    const Reflection* reflection = message->GetReflection();
    const Descriptor* descriptor = message->GetDescriptor();

    for (const TiXmlNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        switch (child->Type()) {
        case TiXmlNode::ELEMENT: {
            if (!list_name.empty() && !item_name.empty() && (item_name == child->ValueStr())) {
                Message* sub_message = reflection->AddMessage(message, file_descriptor);
                bool ret = XmlValueToProtoMessage(
                    child, tmp_list_name, item_name, file_descriptor, sub_message, error);
                if (!ret) {
                    return false;
                }
                break;
            }

            file_descriptor = descriptor->FindFieldByName(child->ValueStr());
            if (!file_descriptor) {
                if (error) {
                    error->append("FindFieldByName failed. name [" + child->ValueStr() + "]");
                }
                break;
            }

            Message* sub_message = NULL;
            if (file_descriptor->cpp_type( ) == FieldDescriptor::CPPTYPE_MESSAGE) {
                if (!item_name.empty() && file_descriptor->is_repeated()) {
                    tmp_list_name = child->ValueStr();
                } else {
                    if (file_descriptor->is_repeated()) {
                        sub_message = reflection->AddMessage(message, file_descriptor);
                    } else {
                        sub_message = reflection->MutableMessage(message, file_descriptor);
                    }

                    if (!sub_message) {
                        if (error) {
                            error->append(
                                "can't add message for " + descriptor->name() + "." + child->ValueStr());
                        }
                        return false;
                    }
                }
            }

            bool ret = XmlValueToProtoMessage(
                child, tmp_list_name, item_name, file_descriptor,
                (sub_message ? sub_message : message), error);
            if (!ret) {
                return false;
            }

            tmp_list_name = list_name;
            break;
        }

        case TiXmlNode::TEXT: {
            if (!file_descriptor) {
                if (error) {
                    error->append("file_descriptor in null");
                }
                return false;
            }

            switch (file_descriptor->cpp_type()) {
#define CASE_FILED_TYPE(cpptype, method, realtype)                      \
                case FieldDescriptor::CPPTYPE_##cpptype: {              \
                    realtype tmp;                                       \
                    if (!StringToNumber(child->ValueStr(), &tmp)) {     \
                        if (error) {                                    \
                            error->append("string2number failed. value ["); \
                            error->append(child->ValueStr());           \
                            error->append("]");                         \
                        }                                               \
                        return false;                                   \
                    }                                                   \
                    if (file_descriptor->is_repeated()) {               \
                        reflection->Add##method(message, file_descriptor, tmp); \
                    } else {                                            \
                        reflection->Set##method(message, file_descriptor, tmp); \
                    }                                                   \
                    break;                                              \
                }

                CASE_FILED_TYPE(INT32,  Int32,  int32_t);
                CASE_FILED_TYPE(UINT32, UInt32, uint32_t);
                CASE_FILED_TYPE(INT64,  Int64,  int64_t);
                CASE_FILED_TYPE(UINT64, UInt64, uint64_t);
                CASE_FILED_TYPE(FLOAT,  Float,  float);
                CASE_FILED_TYPE(DOUBLE, Double, double);

#undef CASE_FILED_TYPE

            case FieldDescriptor::CPPTYPE_STRING: {
                if (file_descriptor->is_repeated()) {
                    reflection->AddString(message, file_descriptor, child->ValueStr());
                } else {
                    reflection->SetString(message, file_descriptor, child->ValueStr());
                }

                break;
            }

            case FieldDescriptor::CPPTYPE_BOOL: {
                bool is_true = StringEqualsIgnoreCase("true", child->ValueStr());
                if (file_descriptor->is_repeated()) {
                    reflection->AddBool(message, file_descriptor, is_true);
                } else {
                    reflection->SetBool(message, file_descriptor, is_true);
                }

                break;
            }

            case FieldDescriptor::CPPTYPE_ENUM: {
                const EnumValueDescriptor* enum_value = descriptor->FindEnumValueByName(child->ValueStr());
                if (!enum_value) {
                    if (error) {
                        error->append("find enum failed. value [" + child->ValueStr() + "]");
                    }
                    return false;
                }
                if (file_descriptor->is_repeated()) {
                    reflection->AddEnum(message, file_descriptor, enum_value);
                } else {
                    reflection->SetEnum(message, file_descriptor, enum_value);
                }

                break;
            }

            default: {
                if (error) {
                    error->append(descriptor->name() + "." + file_descriptor->name() + " unkown type.");
                }
                return false;
            }

            } ///< switch

            break;
        }

        default: break;

        } ///< switch
    } ///< for

    return true;
}


