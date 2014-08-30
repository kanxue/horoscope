
#include "horoscope_routine_logic.h"

#include "common/base/string/concat.h"
#include "common/base/string/format.h"
#include "common/base/string/string_number.h"
#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"
#include "common/base/string/algorithm.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/httpserver/v2/horoscope_routine.pb.h"
#include "horoscope/httpserver/v2/horoscope_routine_handler.h"
#include "horoscope/httpserver/v2/httpserver_filter.h"
#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/storage/common_memory_cache.h"
#include "horoscope/storage/storage_mysql_client.h"

HoroscopeRoutineLogic::HoroscopeRoutineLogic(
    const HttpRequest* request,
    HttpResponse* response)
    : m_request(request),
      m_response(response)
{}

HoroscopeRoutineLogic::~HoroscopeRoutineLogic()
{}

void HoroscopeRoutineLogic::Run()
{
    // check request.
    if (!CheckRequest()) {
        StringAppend(
            m_response->mutable_http_body(),
            "ip ",
            m_user_ip,
            " NOT in white list. plz contact admin.");
        return;
    }

    // get all routine
    horoscope::routine::HoroscopeRoutineList routine_list;
    for (int astro = AstroType_Aries;
         astro < AstroType_UnknownHoroscope; ++astro)
    {
        DoGetHoroscopeRoutine(astro, routine_list.add_list());
    }

    // serialize to xml.
    std::string error;
    bool succ = ProtoMessageToXmlWithRoot(
        routine_list, m_response->mutable_http_body(), &error);
    if (!succ) {
        LOG(ERROR)
            << "to xml failed. error [" << error
            << "] message [" << routine_list.ShortDebugString() << "]";
    }
    *m_response->mutable_http_body() = GetUtf8String(*m_response->mutable_http_body());
}

bool HoroscopeRoutineLogic::CheckRequest()
{
    const HttpHeaders& headers = m_request->headers();
    if (!headers.Get("user_real_ip", &m_user_ip)) {
        LOG(ERROR)
            << "get m_user_ip from header failed. headers ["
            << headers.ToString() << "]";
        return false;
    }

    if (!HttpServerFilter::EasyCheckPass(m_user_ip)) {
        LOG(ERROR)
            << "m_user_ip NOT in white-list. m_user_ip ["
            << m_user_ip << "]";
        return false;
    }

    LOG(INFO)
        << "check request succ. m_user_ip [" << m_user_ip << "]";

    return true;
}

void HoroscopeRoutineLogic::DoGetHoroscopeRoutine(
    const int astro,
    horoscope::routine::HoroscopeRoutineItem* routine)
{
    CommonMemoryCacheGroup& cache
        = CommonMemoryCacheGroupSingleton::Instance();
    std::string cache_name = HoroscopeRoutineHandler::CacheName();
    std::string cache_key = StringFormat("astro_%d", astro);

    //get from cache
    if (cache.Get(cache_name, cache_key, routine)) {
        return;
    }

    // get from db
    int ret = DoGetHoroscopeRoutineFromDB(astro, routine);
    if (ret != 0) {
        LOG(ERROR)
            << "call DoGetHoroscopeRoutineFromDB failed. astro" << astro
            << " ret " << ret;
        return;
    }

    // insert into cache
    cache.Set(cache_name, cache_key, *routine);
}

int HoroscopeRoutineLogic::DoGetHoroscopeRoutineFromDB(
    const int astro, 
    horoscope::routine::HoroscopeRoutineItem* routine)
{
    StorageMysqlClient client;

    routine->set_astro(astro);
    routine->set_name(GetNameByAstro(astro));

    // get today forture
    std::string utf8_content;
    int ret = client.GetTodayForture(astro, &utf8_content);
    if (ret != 0) {
        LOG(ERROR)
            << "call GetTodayForture failed. astro " << astro
            << " ret " << ret;
        return ret;
    }
    std::string content = GetGbkString(utf8_content);

    std::string str_color = "color";
    int n_number = 9999;
    std::string str_astro = "astro";
    std::string str_goods = "goods";
    std::string str_todaystar = "star";    
    std::string str_fortune = "today_forture, very long text.";

    if(!ParseFortuneContent(content, str_color, n_number, str_astro, str_goods, str_todaystar, str_fortune))
    {   
        LOG(ERROR)<< "ParseFortuneContent error" << astro << " content : " << content;
    }

    routine->set_lucky_color(str_color);
    routine->set_lucky_number(n_number);
    routine->set_lucky_astro(str_astro);
    routine->set_lucky_goods(str_goods);
    routine->set_today_star(str_todaystar);
    routine->set_today_fortune(str_fortune);

    return 0;
}

bool HoroscopeRoutineLogic::ParseFortuneContent(
        const std::string& content,
        std::string& color,
        int& number,
        std::string& astro,
        std::string& goods,
        std::string& star,
        std::string& fortune)
{
    std::vector<std::string> str_vector;
    const char* delim = "\r\n";
    SplitStringByAnyOf(content, delim, &str_vector);
    
    std::vector<std::string>::iterator it;
    for(it = str_vector.begin(); it != str_vector.end(); it++)
    {
        std::string str = *it;

        if(str.find("������ɫ") == 0)
        {
            color.assign(StringTrim(str.substr(str.find(':') + 1)));
        }
        else if(str.find("��������") == 0)
        {
            ParseNumber(str.substr(str.find(':')).c_str() + 1, &number);
        }
        else if(str.find("��������") == 0)
        {
            astro.assign(StringTrim(str.substr(str.find(':') + 1)));
        }
        else if(str.find("������Ʒ") == 0)
        {
            goods.assign(StringTrim(str.substr(str.find(':')+ 1)));
        }
        else if(str.find("��������") == 0)
        {
            star.assign(StringTrim(str.substr(14)));
        }
        else if(str.find("��������") == 0)
        {

        }
        else
        {
            fortune.assign(str);
        }
    }
   return true;
}

std::string HoroscopeRoutineLogic::GetNameByAstro(const int astro)
{
    std::string gbk_name;
    switch (astro) {
    case AstroType_Aries:
        gbk_name.assign("������");
        break;

    case AstroType_Taurus:
        gbk_name.assign("��ţ��");
        break;

    case AstroType_Gemini:
        gbk_name.assign("˫����");
        break;

    case AstroType_Cancer:
        gbk_name.assign("��з��");
        break;

    case AstroType_Leo:
        gbk_name.assign("ʨ����");
        break;

    case AstroType_Virgo:
        gbk_name.assign("��Ů��");
        break;

    case AstroType_Libra:
        gbk_name.assign("�����");
        break;

    case AstroType_Scorpio:
        gbk_name.assign("��Ы��");
        break;

    case AstroType_Sagittarius:
        gbk_name.assign("������");
        break;

    case AstroType_Capricorn:
        gbk_name.assign("Ħ����");
        break;

    case AstroType_Aquarius:
        gbk_name.assign("ˮƿ��");
        break;

    case AstroType_Pisces:
        gbk_name.assign("˫����");
        break;

    default:
        break;
    } ///< switch

    return gbk_name;
}
