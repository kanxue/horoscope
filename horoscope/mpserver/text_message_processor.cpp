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
#include "horoscope/wxapi/wx_api_manager.h"

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

    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());

    int date_type = GetDateByText(content);
    mpserver::NewsMessage news_message;
    mpserver::TextMessage text_message;
    bool use_text = true;
    if (date_type == ChineseYangYear) {
        use_text = false;
        ProcessChineseYangYear(&news_message, use_text, &text_message);
    }else if (date_type == ThisYear) {
        use_text = false;
        ProcessYear(&news_message, use_text, &text_message);
    }else if (content.find("日报") != std::string::npos) {
        use_text = false;
        ProcessDailyReport(&news_message, use_text, &text_message);
    }else {
        ProcessText(&text_message);
    }

    // serialize output.
    m_output->clear();
    bool succ = true;
    if (use_text) {
        succ = ProtoMessageToXmlWithRoot(text_message, m_output, &error);
    } else {
        succ = ProtoMessageToXmlWithRoot(news_message, m_output, &error);
    }

    if (!succ) {
        LOG(ERROR)
            << "message to xml failed. text message ["
            << text_message.ShortDebugString()
            << "] news message ["
            << news_message.ShortDebugString()
            << "] error [" << error << "]";
        return;
    }
}

bool getMysqlFortuneContent(int horoscope_type, int date_type, std::string& content)
{
    horoscope::HoroscopeAttr horoscope_attr;
    //StorageRedisClient redis_client;
    //StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    StorageMysqlClient mysql_client;
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

    case ThisYear:
        content.clear();
        ret = mysql_client.GetYearForture(astro, &mysql_content);
        break;

    case ThisMonth:
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

bool getNewyearDayContent(std::string& content)
{
    // content.clear();
    // ret = mysql_client.GetYearForture(astro, &mysql_content);
    // if (ret == 0) 
    // {
    //     content.append(mysql_content);
    //     return true;
    // }
    // LOG(ERROR)<< "getNewyearDayContent failed.";
    return false; 
}

void TextMessageProcessor::ProcessDailyReport(
    mpserver::NewsMessage* output_message,
    bool& use_error_message,
    mpserver::TextMessage* error_message)
{
    StorageMysqlClient mysql_client;
    const std::string& openid = m_input_message.fromusername();
    mysql_client.AddUserMessageAction(openid, m_input_message.content());
    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());
    // swap src/dst.
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_message.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("news");

    error_message->set_tousername(openid);
    error_message->set_fromusername(m_input_message.tousername());
    error_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    error_message->set_msgtype("text");

    bool all_success = false;
    std::string msyql_content, error_msg;
    mpserver::Articles* articles = output_message->mutable_articles();
    if (!mysql_client.GetDailyReport(&msyql_content)
        && XmlToProtoMessage(msyql_content, articles, &error_msg)) {
        all_success = true;
        output_message->set_articlecount(articles->item_size());
    }
    if (!all_success) {
        LOG(ERROR)
            << " error_msg [" << error_msg
            << "] msyql_content [" << msyql_content << "]";
        use_error_message = true;
        error_message->set_content(GetUtf8String(INPUT_HOROSCOPE_WITH_YEAR_WORDING));
    }
}

void TextMessageProcessor::ProcessChineseYangYear(
    mpserver::NewsMessage* output_message,
    bool& use_error_message,
    mpserver::TextMessage* error_message)
{
    int ret = 0;
    StorageMysqlClient mysql_client;
    const std::string& openid = m_input_message.fromusername();
    mysql_client.AddUserMessageAction(openid, m_input_message.content());
    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());
    // swap src/dst.
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_message.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("news");

    error_message->set_tousername(openid);
    error_message->set_fromusername(m_input_message.tousername());
    error_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    error_message->set_msgtype("text");

    int horoscope_type = GetHoroscopeTypeByText(content);
    int sex_type = GetSexByText(content);
    horoscope::UserAttr userattr;
    ret = mysql_client.GetUserAttr(
            m_input_message.fromusername(), &userattr);
    LOG(INFO)<< userattr.nickname() << " and " << userattr.sex()<<" end";

    bool show_nickname = false;
    if(horoscope_type == HoroscopeType_UnknownHoroscope || sex_type == 0){
        if(ret != 0)
        {
            LOG(ERROR)<< "HoroscopeType_UnknownHoroscope failed. ";
            use_error_message = true;
            error_message->set_content(GetUtf8String(INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING));
            return;
        }
        else
        {
            show_nickname = true;
        }
    }
    else
    {
        if (ret == 0 && horoscope_type == userattr.horoscope_type() && sex_type == userattr.sex()) {
            show_nickname = true;
        }
        else
        {
            show_nickname = false;
        }
    }

    std::string msyql_news_url, mysql_news_pic_url, error_msg;
    mpserver::Articles* articles = output_message->mutable_articles();


    if(show_nickname)
        ret = mysql_client.GetYangNewYearKeyword(userattr.horoscope_type(), userattr.sex(), &msyql_news_url, &mysql_news_pic_url);
    else
        ret = mysql_client.GetYangNewYearKeyword(horoscope_type, sex_type, &msyql_news_url, &mysql_news_pic_url);

    if (ret != 0) {
        LOG(ERROR)<< "ProcessChineseYangYear GetYangNewYearKeyword failed. ";
        use_error_message = true;
        error_message->set_content(GetUtf8String(INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING));
        return;
    }

    mpserver::ArticleItem *articleItem = articles->add_item();
    std::string title = "";
    if(show_nickname)
    {
        title = GetUtf8String("闹闹女巫店送给");
        title.append(userattr.nickname());
        title.append(GetUtf8String("的羊年签"));
    }
    else
    {   horoscope::HoroscopeAttr horoscope_attr;
        int horoscope_ret = mysql_client.GetHoroscopeAttr(
            horoscope_type, &horoscope_attr);
        if (horoscope_ret != 0) {
            LOG(ERROR)<< "ProcessChineseYangYear GetHoroscopeAttr failed. ";
            use_error_message = true;
            error_message->set_content(GetUtf8String(INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING));
            return;
        }
        title = horoscope_attr.zh_cn_name();
        if(sex_type == 1)
            title.append(GetUtf8String("男的羊年签"));
        else
            title.append(GetUtf8String("女的羊年签"));
    }
    articleItem->set_title(title);
    // articleItem->set_description("description");
    articleItem->set_picurl(mysql_news_pic_url);

    std::string linkUrl = "http://121.42.45.75:8888/yang?nickname=";
    linkUrl.append(userattr.nickname());
    linkUrl.append("&resultpic=");
    linkUrl.append(mysql_news_pic_url);
    articleItem->set_url(linkUrl);

    output_message->set_articlecount(articles->item_size());
}

void TextMessageProcessor::ProcessYear(
    mpserver::NewsMessage* output_message,
    bool& use_error_message,
    mpserver::TextMessage* error_message)
{
    int ret = 0;
    StorageMysqlClient mysql_client;
    const std::string& openid = m_input_message.fromusername();
    mysql_client.AddUserMessageAction(openid, m_input_message.content());
    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());
    // swap src/dst.
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_message.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("news");

    error_message->set_tousername(openid);
    error_message->set_fromusername(m_input_message.tousername());
    error_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    error_message->set_msgtype("text");

    int horoscope_type = GetHoroscopeTypeByText(content);
    if (horoscope_type == HoroscopeType_UnknownHoroscope) {
        horoscope::UserAttr userattr;
        ret = mysql_client.GetUserAttr(
            m_input_message.fromusername(), &userattr);
        if (ret == 0) {
            horoscope_type = userattr.horoscope_type();
        }
    }

    bool all_success = false;
    std::string msyql_content, error_msg;
    mpserver::Articles* articles = output_message->mutable_articles();
    if ((horoscope_type != HoroscopeType_UnknownHoroscope)
        && getMysqlFortuneContent(horoscope_type, ThisYear, msyql_content)
        && XmlToProtoMessage(msyql_content, articles, &error_msg)) {
        all_success = true;
        output_message->set_articlecount(articles->item_size());
    }
    if (!all_success) {
        LOG(ERROR)
            << __func__ << " failed. horoscope_type " << horoscope_type
            << " error_msg [" << error_msg
            << "] msyql_content [" << msyql_content << "]";
        use_error_message = true;
        error_message->set_content(GetUtf8String(INPUT_HOROSCOPE_WITH_YEAR_WORDING));
    }
}

void TextMessageProcessor::ProcessText(mpserver::TextMessage* output_message)
{
    StorageMysqlClient mysql_client;
    const std::string& openid = m_input_message.fromusername();
    mysql_client.AddUserMessageAction(openid, m_input_message.content());
    std::string content;
    if (!ConvertUtf8ToGbk(m_input_message.content(), &content))
        content.assign(m_input_message.content());
    // swap src/dst.
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_message.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("text");

    std::string resp_content;

    int ret = 0;
    int horoscope_type = GetHoroscopeTypeByText(content);
    int date_type = GetDateByText(content);

    if (horoscope_type != HoroscopeType_UnknownHoroscope) {
        
        if (content.find("绑定") != std::string::npos) {
            UpdateUserInfo(openid);
            std::string head = GetUtf8String("");
            horoscope::HoroscopeAttr horoscope_attr;
            ret = mysql_client.GetHoroscopeAttr(horoscope_type, &horoscope_attr);
            if (ret == 0) {
                head.append(horoscope_attr.zh_cn_name());
            }
            resp_content = head;

            horoscope::UserAttr userattr;

            ret = mysql_client.GetUserAttr(
                m_input_message.fromusername(), &userattr);

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

int TextMessageProcessor::UpdateUserInfo(const std::string& open_id)
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

