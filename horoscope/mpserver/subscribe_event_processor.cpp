#include "subscribe_event_processor.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/wxapi/wx_api_manager.h"
#include "horoscope/storage/storage_mysql_client.h"

SubscribeEventProcessor::SubscribeEventProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
    : BaseProcessor(uri, input, output)
{}

SubscribeEventProcessor::~SubscribeEventProcessor()
{}

void SubscribeEventProcessor::Run()
{
    // parse inout.
    std::string error;
    if (!XmlToProtoMessage(m_input, &m_input_event, &error)) {
        LOG(ERROR)
            << " xml to view event failed. input [" << m_input
            << "] error [" << error << "]";
        return;
    }

    // TODO(kanxue) record subscribe action.

    // just log it.
    LOG(INFO) << "[SUBSCRIBE_EVENT] " << m_input_event.ShortDebugString();

    // process
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

void SubscribeEventProcessor::Process(mpserver::TextMessage* output_message)
{
    // swap src/dst.
    const std::string openid = m_input_event.fromusername();
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_event.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("text");

    UpdateUserInfo(openid);

    std::string resp_content = GetUtf8String(SUBSCRIBE_WORDING);
    output_message->set_content(resp_content);
}

int SubscribeEventProcessor::UpdateUserInfo(const std::string& open_id)
{
    int result;
    std::string nickname;
    int sex;
    result = WxApiManager::getInstance()->getUserInfo(open_id, nickname, sex);
    if (result == 0)
    {
        LOG(INFO)
        << "success get user info [nickname=" << nickname
        << "] [sex=" << sex << "]";
    
        StorageMysqlClient mysql_client;

        horoscope::UserAttr userattr;
        result = mysql_client.GetUserAttr(open_id, &userattr);

        userattr.set_openid(open_id);
        userattr.set_sex(sex);
        userattr.set_nickname(nickname);
        result = mysql_client.SetUserAttr(open_id, userattr);

        if(result == 0)
        {
            LOG(INFO)
            << "write to mysql success";
        }
        else
        {
            LOG(ERROR)
            << "fail to write to mysql";
        }
    }
    else
    {
        LOG(ERROR)
	    << "failed get user info [openid=" << open_id
	    << "] [errorcode=" << result << "]";
    }
    return result; 
}
