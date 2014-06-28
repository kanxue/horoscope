#include "horoscope/mpserver/text_message_processor.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "common/encoding/charset_converter.h"
#include "common/encoding/pb_to_json.h"
#include "common/text/regex/regex.h"
#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/storage/storage_mysql_client.h"
#include "horoscope/storage/storage_redis_client.h"

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

    // record upstream message.
    StorageRedisClient storage_client;
    horoscope::UserMessages::Item item;
    item.set_stamp(static_cast<uint32_t>(time(NULL)));
    item.set_content(m_input_message.content());
    item.set_result_flag(0);
    int ret = storage_client.AddUserMessages(
        m_input_message.fromusername(), item);

    mpserver::TextMessage output_message;
    Process(&output_message);

    // serialize output.
    m_output->clear();
    if (!ProtoMessageToXmlWithRoot(output_message, m_output, &error)) {
        LOG(ERROR)
            << "text message to xml failed. message ["
            << output_message.ShortDebugString()
            << "] error [" << error << "]";
        return;
    }
}

void TextMessageProcessor::Process(mpserver::TextMessage* output_message)
{
    // swap src/dst.
    const std::string& openid = m_input_message.fromusername();
    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_message.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("text");

    StorageRedisClient redis_client;
    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    std::string resp_content;

    int ret = 0;
    int horoscope_type = GetHoroscopeTypeByText(content);
    if (horoscope_type == HoroscopeType_UnknownHoroscope) {
        horoscope::UserAttr userattr;
        ret = redis_client.GetUserAttr(
            m_input_message.fromusername(), &userattr);
        if (ret == 0) {
            horoscope_type = userattr.horoscope_type();
        }
    }

    if (horoscope_type != HoroscopeType_UnknownHoroscope) {
        std::string head = GetUtf8String("ÐÇ×ù : ");
        horoscope::HoroscopeAttr horoscope_attr;
        ret = redis_client.GetHoroscopeAttr(horoscope_type, &horoscope_attr);
        if (ret == 0) {
            head.append(horoscope_attr.zh_cn_name());
            head.append("\n");
        }
        resp_content = head;
        resp_content.append("\n");

        if (content.find("°ó¶¨") != std::string::npos) {
            horoscope::UserAttr userattr;
            userattr.set_horoscope_type(horoscope_type);
            ret = redis_client.MergeUserAttr(
                m_input_message.fromusername(), userattr);
            if (ret == 0) {
                resp_content.append(GetUtf8String(BIND_SUCCESS_WORDINGF));
            }

        } else {
            std::string mysql_content;
            int astro = Horoscope2Astro(horoscope_type);
            ret = mysql_client.GetTodayForture(astro, &mysql_content);
            if (ret == 0) resp_content.append(mysql_content);
        }

        resp_content.append("\n");
        resp_content.append(GetUtf8String(INPUT_MODIFY_HOROSCOPE_WORDING));
    } else {
        resp_content = GetUtf8String(INPUT_BIRTHDAY_WORDING);
    }

    if (resp_content.empty()) {
        LOG(ERROR) << "never come here. " << m_input_message.ShortDebugString();
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    output_message->set_content(resp_content);
}

