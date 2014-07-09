#include "click_event_processor.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/storage/storage_mysql_client.h"
#include "horoscope/storage/storage_redis_client.h"

ClickEventProcessor::ClickEventProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
    : BaseProcessor(uri, input, output)
{}

ClickEventProcessor::~ClickEventProcessor()
{}

void ClickEventProcessor::Run()
{
    // parse inout.
    std::string error;
    if (!XmlToProtoMessage(m_input, &m_input_event, &error)) {
        LOG(ERROR)
            << " xml to click event failed. input [" << m_input
            << "] error [" << error << "]";
        return;
    }

    // record click action.
    StorageRedisClient storage_client;
    horoscope::UserMessages::Item item;
    item.set_stamp(static_cast<uint32_t>(time(NULL)));
    item.set_result_flag(0);
    storage_client.AddUserMessages(m_input_event.fromusername(), item);

    const std::string& event_key = m_input_event.eventkey();
    item.set_content(event_key);

    if(event_key == "ARTICLES_MOST_RECENT"){

        mpserver::NewsMessage output_message;
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
    else{
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
    

    
}

void ClickEventProcessor::Process(mpserver::NewsMessage* output_message)
{
// swap src/dst.
    const std::string& openid = m_input_event.fromusername();
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_event.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("news");

    mpserver::Articles* articles = output_message->mutable_articles();
    mpserver::ArticleItem* article_item = articles->add_item();
    article_item->set_title(GetUtf8String("女巫店12星座7月9日运势"));
    article_item->set_picurl("http://mmbiz.qpic.cn/mmbiz/jGPzXFib2qibTGPZp8ccpFHVHicjZjuRZ9JVsed2cViaKRQkQN3VdMNuwOWRt7EomURiaZhsBHRyZuibCJ9Mt7Z49mog/0");
    article_item->set_url("http://mp.weixin.qq.com/s?__biz=MjM5NzAzMzkyMA==&mid=202659744&idx=1&sn=e0dd03108f4d36ca9b62ca4b6fd8b047#rd");

    article_item = articles->add_item();
    article_item->set_title(GetUtf8String("女巫店测试：你的五根手指注定你的爱情，真挺准的！"));
    article_item->set_picurl("http://mmbiz.qpic.cn/mmbiz/jGPzXFib2qibTGPZp8ccpFHVHicjZjuRZ9J0MEKOBfFOoPdQ3TnJE37cmlbmMZZhpclgxGR1EnCNeOkiceTlRiaRscg/0");
    article_item->set_url("http://mp.weixin.qq.com/s?__biz=MjM5NzAzMzkyMA==&mid=202659744&idx=2&sn=2dd91b99e9d653b0e729ceac90cd5d7c#rd");

    output_message->set_articlecount(articles->item_size());
}


void ClickEventProcessor::Process(mpserver::TextMessage* output_message)
{
    // swap src/dst.
    const std::string& openid = m_input_event.fromusername();
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_event.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("text");

    std::string resp_content;
    StorageRedisClient redis_client;
    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();

    bool has_horoscope = false;
    std::string head = GetUtf8String("");
    horoscope::UserAttr userattr;
    int ret = redis_client.GetUserAttr(openid, &userattr);
    if (ret == 0) {
        horoscope::HoroscopeAttr horoscope_attr;
        ret = redis_client.GetHoroscopeAttr(
            userattr.horoscope_type(), &horoscope_attr);
        if (ret == 0) {
            has_horoscope = true;
            head.append(horoscope_attr.zh_cn_name());
	    // head.append("\n");
        }
    }

    std::string mysql_content;
    const std::string& event_key = m_input_event.eventkey();
    int astro = Horoscope2Astro(userattr.horoscope_type());

    if (event_key == "FORTUNE_HOROSCOPE_TODAY") {
        if (has_horoscope) {
            head.append(GetUtf8String("今日运势："));
	        head.append("\n\n"); 
	        ret = mysql_client.GetTodayForture(astro, &mysql_content);
            if (ret == 0) resp_content = head + mysql_content;
        } else {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    } else if (event_key == "FORTUNE_HOROSCOPE_TOMORROW") {
        if (has_horoscope) {
	  head.append(GetUtf8String("明日运势："));
	  head.append("\n\n");
            ret = mysql_client.GetTomorrowForture(astro, &mysql_content);
            if (ret == 0) resp_content = head + mysql_content;
        } else {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    } else if (event_key == "FORTUNE_HOROSCOPE_WEEK") {
        if (has_horoscope) {
	  head.append(GetUtf8String("本周运势："));
	  head.append("\n\n");
            ret = mysql_client.GetTswkForture(astro, &mysql_content);
            if (ret == 0) resp_content = head + mysql_content;
        } else {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    } else if (event_key == "FORTUNE_HOROSCOPE_OTHERS") {
        resp_content = GetUtf8String(INPUT_OTHER_HOROSCOPE_WORDING);
    } else if (event_key == "PLUGIN_HOROSCOPE_DETAIL") {
        ret = redis_client.GetAllHoroscopeAttr(&resp_content);
    } else if (event_key == "PLUGIN_HOROSCOPE_MATCH") {
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    } else if (event_key == "ME_HOROSCOPE_DETAIL") {
        if (has_horoscope) {
            resp_content = head;
            resp_content.append("\n");
            resp_content.append(GetUtf8String(INPUT_MODIFY_HOROSCOPE_WORDING));
        } else {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    } 
    else {
        LOG(ERROR) << "unkown event type [" << event_key << "]";
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    if (resp_content.empty()) {
        LOG(ERROR)
            << "never come here. " << m_input_event.ShortDebugString();
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    output_message->set_content(resp_content);
}

