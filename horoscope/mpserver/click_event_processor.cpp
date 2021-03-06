#include "click_event_processor.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/tinyxml/tinyxml.h"
#include "common/base/string/string_number.h"

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/mpserver/user_count_stat.h"
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

bool XmlToArticles(
    const std::string& xml_string,
    mpserver::Articles* articles,
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
    for (TiXmlNode* child = root->FirstChild(); child; child = child->NextSibling()) {
        mpserver::ArticleItem* article_item = articles->add_item();
            
        TiXmlElement* element;

        for(TiXmlNode* itemchild = child->FirstChild(); itemchild; itemchild = itemchild->NextSibling())
        {
            switch(itemchild->Type())
            {
                case TiXmlNode::ELEMENT:
                {
                    element = itemchild->ToElement();

                    if(element->Value()==GetUtf8String("Title"))
                        article_item->set_title(element->FirstChild()->Value());
                    else if(element->Value()==GetUtf8String("Url"))
                        article_item->set_url(element->FirstChild()->Value());
                    else if(element->Value()==GetUtf8String("PicUrl"))
                        article_item->set_picurl(element->FirstChild()->Value());
                    else if(element->Value()==GetUtf8String("Description"))
                        article_item->set_description(element->FirstChild()->Value());
                    break;
                }
                default:
                    break;
            }
        }
    }
    return true;
}


bool XMLToFortuneDailyReport(
	const std::string& xml_string,
    mpserver::FortuneDailyReport* report,
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
	for(TiXmlNode* itemchild = root->FirstChild(); itemchild; itemchild = itemchild->NextSibling())
    {
		switch(itemchild->Type())
        {
			case TiXmlNode::ELEMENT:
            {
				TiXmlElement* element = itemchild->ToElement();

				if(element->Value()==GetUtf8String("lucky_color"))
					report->set_luckycolor(element->FirstChild()->Value());
                else if(element->Value()==GetUtf8String("lucky_goods"))
                    report->set_luckygoods(element->FirstChild()->Value());
                else if(element->Value()==GetUtf8String("lucky_astro"))
                    report->set_luckyastro(element->FirstChild()->Value());
                else if(element->Value()==GetUtf8String("lucky_number"))
                    report->set_luckynumber(element->FirstChild()->Value());
				else if(element->Value()==GetUtf8String("lucky_star"))
                    report->set_luckystars(element->FirstChild()->Value());
                break;
            }
            default:
                break;
		}
    }
	return true;
}

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
    // StorageRedisClient storage_client;
    // horoscope::UserMessages::Item item;
    // item.set_stamp(static_cast<uint32_t>(time(NULL)));
    // item.set_result_flag(0);
    // storage_client.AddUserMessages(m_input_event.fromusername(), item);

    const std::string& event_key = m_input_event.eventkey();
    //item.set_content(event_key);

    if(event_key == "ARTICLES_MOST_RECENT"){
	//can not be reached now, not show articles anymore
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
        mpserver::TextMessage text_message;
        mpserver::NewsMessage news_message;
        bool use_news_message = false;
        Process(&text_message, use_news_message, &news_message);

            // serialize output.
        m_output->clear();
        bool succ = true;
        if (use_news_message) {
            succ = ProtoMessageToXmlWithRoot(news_message, m_output, &error);
        } else {
            succ = ProtoMessageToXmlWithRoot(text_message, m_output, &error);
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
}


void ClickEventProcessor::Process(mpserver::NewsMessage* output_message)
{
// swap src/dst.
    const std::string& openid = m_input_event.fromusername();
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_event.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("news");

    std::string str_articles;
    StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    int ret = mysql_client.GetDailyReport(&str_articles);
    if(ret == 0)
    {    
        std::string error;

        mpserver::Articles* articles = output_message->mutable_articles();
        if(XmlToArticles(str_articles, articles, &error))
        {
            output_message->set_articlecount(articles->item_size());
        }
        else
        {
            LOG(ERROR) << error;
        }
    }
}


void ClickEventProcessor::Process(
    mpserver::TextMessage* output_message,
    bool& use_news_message,
    mpserver::NewsMessage* news_message)
{
    // swap src/dst.
    const std::string& openid = m_input_event.fromusername();
    output_message->set_tousername(openid);
    output_message->set_fromusername(m_input_event.tousername());
    output_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    output_message->set_msgtype("text");

    news_message->set_tousername(openid);
    news_message->set_fromusername(m_input_event.tousername());
    news_message->set_createtime(static_cast<uint32_t>(time(NULL)));
    news_message->set_msgtype("news");

    std::string resp_content;
    StorageRedisClient redis_client;
    //StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    StorageMysqlClient mysql_client;

    bool has_horoscope = false;
    std::string head = GetUtf8String("");
    horoscope::UserAttr userattr;
    int ret = mysql_client.GetUserAttr(openid, &userattr);
    //LOG(INFO) << "call GetUserAttr done. openid " << openid << " ret " << ret;
    if (ret == 0) {
        horoscope::HoroscopeAttr horoscope_attr;
        ret = mysql_client.GetHoroscopeAttr(
            userattr.horoscope_type(), &horoscope_attr);
        if (ret == 0) {
            has_horoscope = true;
            head.append(horoscope_attr.zh_cn_name());
        }
    }

    std::string mysql_content;
    const std::string& event_key = m_input_event.eventkey();
    int astro = Horoscope2Astro(userattr.horoscope_type());

    mysql_client.AddUserClickAction(openid, event_key);

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
    } else if (event_key == "FORTUNE_HOROSCOPE_YEAR") {
        ret = mysql_client.GetYearForture(astro, &mysql_content);
        use_news_message = false;
        if (ret == 0) {
            mpserver::Articles* articles = news_message->mutable_articles();
            std::string error_msg;
            if (XmlToProtoMessage(mysql_content, articles, &error_msg)) {
                use_news_message = true;
                news_message->set_articlecount(articles->item_size());
            }
        }
        if (!use_news_message) {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    }else if(event_key == "YANG_NEW_YEAR_RESULT") {
        use_news_message = false;

        if(userattr.horoscope_type() == HoroscopeType_UnknownHoroscope || userattr.sex() == 0){
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING);
        }else{
            std::string msyql_news_url;
            std::string mysql_news_pic_url;
            std::string title = GetUtf8String("闹闹女巫店送给");
            ret = mysql_client.GetYangNewYearKeyword(userattr.horoscope_type(), userattr.sex(), &msyql_news_url, &mysql_news_pic_url);
            if (ret != 0) {
                LOG(ERROR)<< "ProcessChineseYangYear GetYangNewYearKeyword failed. ";
                resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING);
            }else{
                use_news_message = true;

                mpserver::Articles* articles = news_message->mutable_articles();
                mpserver::ArticleItem *articleItem = articles->add_item();
                if(userattr.nickname().length() > 0)
                    title.append(userattr.nickname());
                else
                    title.append(GetUtf8String("我"));
                title.append(GetUtf8String("的羊年签"));
                articleItem->set_title(title);
                articleItem->set_picurl(mysql_news_pic_url);
                articleItem->set_url(msyql_news_url);
                news_message->set_articlecount(articles->item_size());

            }
        }

    } else if (event_key == "FORTUNE_HOROSCOPE_OTHERS") {
        resp_content = GetUtf8String(INPUT_OTHER_HOROSCOPE_WORDING);
    } else if (event_key == "PLUGIN_HOROSCOPE_DETAIL") {
        ret = mysql_client.GetAllHoroscopeAttr(&resp_content);
    } else if (event_key == "ME_HOROSCOPE_DETAIL") {
        if (has_horoscope) {
            if(userattr.birth_month() != 0 && userattr.birth_day() != 0){
                head.append("(");
                head.append(NumberToString(userattr.birth_month()));
                head.append(GetUtf8String("月"));
                head.append(NumberToString(userattr.birth_day()));
                head.append(GetUtf8String("日"));
                head.append(")");
            }

            resp_content = head;

            resp_content.append("\n\n");
            resp_content.append(GetUtf8String(INPUT_MODIFY_HOROSCOPE_WORDING));
        } else {
            resp_content = GetUtf8String(INPUT_HOROSCOPE_WITH_BIND_WORDING);
        }
    } else if (event_key == "TODAY_LUCKY_COLOR") {
		ret = mysql_client.GetTodayFortuneReport(&mysql_content);
		if(ret == 0) {
			mpserver::FortuneDailyReport daily_report;
			std::string error;
			if(XMLToFortuneDailyReport(mysql_content, &daily_report, &error)) {
				resp_content = daily_report.luckycolor();
			} else {
				LOG(ERROR) << error;
			}
		}
	} else if (event_key == "TODAY_LUCKY_GOODS") {
		ret = mysql_client.GetTodayFortuneReport(&mysql_content);
		if(ret == 0) {
			mpserver::FortuneDailyReport daily_report;
			std::string error;
			if(XMLToFortuneDailyReport(mysql_content, &daily_report, &error)) {
				resp_content = daily_report.luckygoods();
			} else {
				LOG(ERROR) << error;
			}
		}
	} else if (event_key == "TODAY_LUCKY_ASTRO") {
		ret = mysql_client.GetTodayFortuneReport(&mysql_content);
		if(ret == 0) {
			mpserver::FortuneDailyReport daily_report;
			std::string error;
			if(XMLToFortuneDailyReport(mysql_content, &daily_report, &error)) {
				resp_content = daily_report.luckyastro();
			} else {
				LOG(ERROR) << error;
			}
		} 
	}else if (event_key == "TODAY_LUCKY_NUMBER") {
		ret = mysql_client.GetTodayFortuneReport(&mysql_content);
		if(ret == 0) {
			mpserver::FortuneDailyReport daily_report;
			std::string error;
			if(XMLToFortuneDailyReport(mysql_content, &daily_report, &error)) {
				resp_content = daily_report.luckynumber();
			} else {
				LOG(ERROR) << error;
			}
		}
	}else if (event_key == "TODAY_LUCKY_STARS") {
		ret = mysql_client.GetTodayFortuneReport(&mysql_content);
		if(ret == 0) {
			mpserver::FortuneDailyReport daily_report;
			std::string error;
			if(XMLToFortuneDailyReport(mysql_content, &daily_report, &error)) {
				resp_content = daily_report.luckystars();
			} else {
				LOG(ERROR) << error;
			}
		} 
	}
    else {
        LOG(ERROR) << "unkown event type [" << event_key << "]";
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    std::string user_count = UserCountStatSingleton::Instance(
        ).GetAndReportUserCount(openid);
    if (!user_count.empty()) {
        resp_content.append("\n");
        resp_content.append(GetUtf8String(user_count));
    }

    if (resp_content.empty()) {
        LOG(ERROR)
            << "never come here. " << m_input_event.ShortDebugString();
        resp_content = GetUtf8String(NOT_IMPLEMENT_WORDING);
    }

    output_message->set_content(resp_content);
}



