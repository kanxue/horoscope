#include "pb_to_xml.h"

#include <vector>

#include "common/base/string/concat.h"
#include "thirdparty/protobuf/descriptor.h"
#include "thirdparty/protobuf/message.h"
#include "thirdparty/protobuf/stubs/strutil.h"

void AppendAsXml(const std::string& key, const std::string& value, std::string* xml, bool as_binary = false)
{
    if (!as_binary)
        StringAppend(xml, '<', key, '>', value, "</", key, ">\n");
    else
        StringAppend(xml, '<', key, "><![CDATA[", value, "]]></", key, ">\n");
}

const std::string& SimpleStoa(const std::string& str)
{
    return str;
}

bool ProtoMessageToXmlWithoutRoot(
    const google::protobuf::Message& message,
    std::string* xml_string,
    std::string* error)
{
    using namespace std;
    using namespace google::protobuf;

    const Reflection* reflection = message.GetReflection();
    const Descriptor* descriptor = message.GetDescriptor();

    vector<const FieldDescriptor*> fields;
    for (int i = 0; i < descriptor->extension_range_count(); i++) {
        const Descriptor::ExtensionRange* ext_range = descriptor->extension_range(i);
        for (int tag_number = ext_range->start; tag_number < ext_range->end; tag_number++) {
            const FieldDescriptor* field = reflection->FindKnownExtensionByNumber(tag_number);
            if (field) {
                fields.push_back(field);
            }
        }
    }
    for (int i = 0; i < descriptor->field_count(); i++) {
        fields.push_back(descriptor->field(i));
    }

    for (size_t i = 0; i < fields.size(); i++) {
        const FieldDescriptor* field = fields[i];
        if (!field->is_repeated() && !reflection->HasField(message, field)) {
            if (field->is_required()) {
                if (error)
                    error->assign("missed required field " + field->full_name() + ".");
                return false;
            }
            continue;
        }

        switch (field->cpp_type()) {
#define CASE_FIELD_TYPE(cpptype, method, ToString, as_binary)           \
            case FieldDescriptor::CPPTYPE_##cpptype:  {                 \
                if (field->is_repeated()) {                             \
                    int field_size = reflection->FieldSize(message, field); \
                    for (int index = 0; index < field_size; ++index) {  \
                        AppendAsXml(                                    \
                            field->name(), ToString(                    \
                                reflection->GetRepeated##method(message, field, index)), \
                            xml_string, as_binary);                     \
                    }                                                   \
                } else {                                                \
                    AppendAsXml(                                        \
                        field->name(), ToString(                        \
                            reflection->Get##method(message, field)),   \
                        xml_string, as_binary);                         \
                }                                                       \
                break;                                                  \
            }

            CASE_FIELD_TYPE(INT32,  Int32,  SimpleItoa, false);
            CASE_FIELD_TYPE(INT64,  Int64,  SimpleItoa, false);
            CASE_FIELD_TYPE(UINT32, UInt32, SimpleItoa, false);
            CASE_FIELD_TYPE(UINT64, UInt64, SimpleItoa, false);
            CASE_FIELD_TYPE(FLOAT,  Float,  SimpleFtoa, false);
            CASE_FIELD_TYPE(DOUBLE, Double, SimpleDtoa, false);
            CASE_FIELD_TYPE(STRING, String, SimpleStoa, true);

#undef CASE_FIELD_TYPE

        case FieldDescriptor::CPPTYPE_ENUM: {
            if (field->is_repeated()) {
                int field_size = reflection->FieldSize(message, field);
                for (int index = 0; index < field_size; index++) {
                    AppendAsXml(
                        field->name(),
                        SimpleItoa(reflection->GetRepeatedEnum(message, field, index)->number()),
                        xml_string);
                }
            } else {
                AppendAsXml(
                    field->name(),
                    SimpleItoa(reflection->GetEnum(message, field)->number()),
                    xml_string);
            }
            break;
        }

        case FieldDescriptor::CPPTYPE_BOOL: {
            if (field->is_repeated()) {
                int field_size = reflection->FieldSize(message, field);
                for (int index = 0; index < field_size; index++) {
                    AppendAsXml(
                        field->name(),
                        reflection->GetRepeatedBool(message, field, index) ? "true" : "false",
                        xml_string);
                }
            } else {
                AppendAsXml(
                    field->name(),
                    reflection->GetBool(message, field) ? "true" : "false",
                    xml_string);
            }
            break;
        }

        case FieldDescriptor::CPPTYPE_MESSAGE: {
            if (field->is_repeated()) {
                int field_size = reflection->FieldSize(message, field);
                for (int index = 0; index < field_size; index++) {
                    StringAppend(xml_string, '<', field->name(), ">\n");
                    if (!ProtoMessageToXmlWithoutRoot(
                            reflection->GetRepeatedMessage(message, field, index), xml_string, error)) {
                        return false;
                    }
                    StringAppend(xml_string, "</", field->name(), ">\n");
                }
            } else {
                StringAppend(xml_string, '<', field->name(), ">\n");
                if (!ProtoMessageToXmlWithoutRoot(
                        reflection->GetMessage(message, field), xml_string, error)) {
                    return false;
                }
                StringAppend(xml_string, "</", field->name(), ">\n");
            }
            break;
        }

        } ///< switch
    } ///< for

    return true;
}

bool ProtoMessageToXmlWithRoot(
    const google::protobuf::Message& message,
    std::string* xml_string,
    std::string* error)
{
    StringAppend(xml_string, "<xml>");

    if (!ProtoMessageToXmlWithoutRoot(message, xml_string, error)) {
        return false;
    }

    StringAppend(xml_string, "</xml>");

    return true;
}

