#include "horoscope/mpserver/text_message_processor.h"

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"

TextMessageProcessor::TextMessageProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
  : BaseProcessor(uri, input, output)
{}

TextMessageProcessor::~TextMessageProcessor()
{}

void TextMessageProcessor::Run()
{
    // parse input.
    std::string error;
    if (!XmlToProtoMessage(m_input, &m_input_message, &error)) {
        LOG(ERROR)
            << "xml to text message failed. input [" << m_input
            << "] error [" << error << "]";
        return;
    }

    // TODO(boxerzhang) mock implement.
    mpserver::TextMessage output_message;
    // swap src/dst.
    output_message.set_tousername(m_input_message.fromusername());
    output_message.set_fromusername(m_input_message.tousername());
    output_message.set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message.set_msgtype("text");
    output_message.set_content("reply for: " + m_input_message.content());

    // serialize output.
    m_output->clear();
    if (!ProtoMessageToXmlWithRoot(output_message, m_output, &error)) {
        LOG(ERROR)
            << "text message to xml failed. message [" << output_message.ShortDebugString()
            << "] error [" << error << "]";
        return;
    }
}

