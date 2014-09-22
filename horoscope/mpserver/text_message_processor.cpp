#include "horoscope/mpserver/text_message_processor.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "common/base/string/string_number.h"
#include "common/encoding/charset_converter.h"
#include "common/encoding/pb_to_json.h"
#include "common/text/regex/regex.h"
#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/mpserver/user_count_stat.h"
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
    // StorageRedisClient storage_client;
    // horoscope::UserMessages::Item item;
    // item.set_stamp(static_cast<uint32_t>(time(NULL)));
    // item.set_content(m_input_message.content());
    // item.set_result_flag(0);
    // storage_client.AddUserMessages(m_input_message.fromusername(), item);

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

bool getMysqlFortuneContent(int horoscope_type, int date_type, std::string& content)
{
    horoscope::HoroscopeAttr horoscope_attr;
    //StorageRedisClient redis_client;
    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    int astro = Horoscope2Astro(horoscope_type);

    int ret = mysql_client.GetHoroscopeAttr(horoscope_type, &horoscope_attr);
    if (ret == 0) {
        content.append(horoscope_attr.zh_cn_name());
    }
    else
    {
        LOG(ERROR)
            << "call GetHoroscopeAttr failed. horoscope_type "
            << horoscope_type << " ret " << ret;
        return false;
    }

    std::string mysql_content;
    switch (date_type) {
            case Today:
                content.append(GetUtf8String("今日运势\n\n"));
                ret = mysql_client.GetTodayForture(astro, &mysql_content);
                break;

            case Tomorrow:
                content.append(GetUtf8String("明日运势\n\n"));
                ret = mysql_client.GetTomorrowForture(astro, &mysql_content);
                break;

            case ThisWeek:
                content.append(GetUtf8String("本周运势\n\n"));
                ret = mysql_client.GetTswkForture(astro, &mysql_content);
                break;

            case ThisMonth:
            case ThisYear:
            case UnknownDate:
            default:
                // TODO(kanxue) use today first.
                content.append(GetUtf8String("今日运势\n\n"));
                ret = mysql_client.GetTodayForture(astro, &mysql_content);
                break;
    }

    if (ret == 0) 
    {
        content.append(mysql_content);
        return true;
    }
    LOG(ERROR)
        << __func__ << " failed. horoscope_type " << horoscope_type
        << " date_type " << date_type << " ret " << ret;
    return false;

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

    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    //StorageRedisClient redis_client;
    std::string resp_content;

    int ret = 0;
    int horoscope_type = GetHoroscopeTypeByText(content);
    int date_type = GetDateByText(content);
    

    if (horoscope_type != HoroscopeType_UnknownHoroscope) {
        
        if (content.find("绑定") != std::string::npos) {

            std::string head = GetUtf8String("");
            horoscope::HoroscopeAttr horoscope_attr;
            ret = mysql_client.GetHoroscopeAttr(horoscope_type, &horoscope_attr);
            if (ret == 0) {
                head.append(horoscope_attr.zh_cn_name());
            }
            resp_content = head;

            horoscope::UserAttr userattr;
            userattr.set_horoscope_type(horoscope_type);
			
			std::string month = "";
			std::string day = "";
			if(GetMonthDay(content, month, day)){
				if(!month.empty() && !day.empty()) {
					if(month[0] == '0') month = month.substr(1);
					if(day[0] == '0') day = day.substr(1);
					int nMonth = 0;
					int nDay = 0;
					ParseNumber(month.c_str(), &nMonth);
					ParseNumber(day.c_str(), &nDay);
					userattr.set_birth_month(nMonth);
					userattr.set_birth_day(nDay);
				}
			} else {
				userattr.set_birth_month(0);
				userattr.set_birth_day(0);
			}
			
            //ret = redis_client.MergeUserAttr(
            //    m_input_message.fromusername(), userattr);
            ret = mysql_client.SetUserAttr(
                m_input_message.fromusername(), userattr);
            if (ret == 0) {
                resp_content.append(GetUtf8String(BIND_SUCCESS_WORDING));
            }

            // 双写到mysql
            // StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
            // int mysql_ret = mysql_client.SetUserAttr(
            //     m_input_message.fromusername(), userattr);
            // LOG(INFO)
            //     << "call StorageMysqlClient.SetUserAttr done. openid "
            //     << openid << " ret " << mysql_ret;

        } else {
            std::string mysql_content;

            if(getMysqlFortuneContent(horoscope_type, date_type, mysql_content))
            {
                resp_content = mysql_content;
            }
        }

        resp_content.append("\n\n");
        resp_content.append(GetUtf8String(INPUT_MODIFY_HOROSCOPE_WORDING));
    } 
    else {
        int uer_horoscope_type = HoroscopeType_UnknownHoroscope;

        horoscope::UserAttr userattr;
        //ret = redis_client.GetUserAttr(
        ret = mysql_client.GetUserAttr(
        m_input_message.fromusername(), &userattr);
        if (ret == 0) {
            uer_horoscope_type = userattr.horoscope_type();
        }

        if (uer_horoscope_type != HoroscopeType_UnknownHoroscope) {
            //已经绑定过星座
            if(date_type != UnknownDate)
            {            
                std::string mysql_content;
                if(getMysqlFortuneContent(uer_horoscope_type, date_type, mysql_content))
                {
                    resp_content = mysql_content;
                }
                else
                {
                    resp_content = GetUtf8String(INPUT_UNKNOWN);
                    LOG(ERROR) << "user input unknown. [" << content << "]";
                }
            }
            else
            {
                resp_content = GetUtf8String(INPUT_UNKNOWN);
                LOG(ERROR) << "user input unknown. [" << content << "]";
            }

        }
        else
        {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }


    }

    std::string user_count = UserCountStatSingleton::Instance(
        ).GetAndReportUserCount(openid);
    if (!user_count.empty()) {
        resp_content.append("\n");
        resp_content.append(GetUtf8String(user_count));
    }

    if (resp_content.empty()) {
        LOG(ERROR) << "never come here. " << m_input_message.ShortDebugString();
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    output_message->set_content(resp_content);
}

