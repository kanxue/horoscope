
#include "horoscope_routine_logic.h"

#include "common/base/string/concat.h"
#include "common/base/string/format.h"
#include "common/net/http/http_request.h"
#include "common/net/http/http_response.h"

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

    // get from cache
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
    std::string content;
    int ret = client.GetTodayForture(astro, &content);
    if (ret != 0) {
        LOG(ERROR)
            << "call GetTodayForture failed. astro " << astro
            << " ret " << ret;
        return ret;
    }

    // TODO(kanxue) parse xml.

    // set result.
    routine->set_lucky_color("lucky_color");
    routine->set_lucky_number(100 + astro);
    routine->set_lucky_astro("lucky_astro");
    routine->set_lucky_object("lucky_object");
    routine->set_today_index(10 + astro);
    routine->set_today_forture("today_forture, very long text.");

    return 0;
}

std::string HoroscopeRoutineLogic::GetNameByAstro(const int astro)
{
    std::string gbk_name;
    switch (astro) {
    case AstroType_Aries:
        gbk_name.assign("°×Ñò×ù");
        break;

    case AstroType_Taurus:
        gbk_name.assign("½ðÅ£×ù");
        break;

    case AstroType_Gemini:
        gbk_name.assign("Ë«×Ó×ù");
        break;

    case AstroType_Cancer:
        gbk_name.assign("¾ÞÐ·×ù");
        break;

    case AstroType_Leo:
        gbk_name.assign("Ê¨×Ó×ù");
        break;

    case AstroType_Virgo:
        gbk_name.assign("´¦Å®×ù");
        break;

    case AstroType_Libra:
        gbk_name.assign("Ìì³Ó×ù");
        break;

    case AstroType_Scorpio:
        gbk_name.assign("ÌìÐ«×ù");
        break;

    case AstroType_Sagittarius:
        gbk_name.assign("ÉäÊÖ×ù");
        break;

    case AstroType_Capricorn:
        gbk_name.assign("Ä¦ôÉ×ù");
        break;

    case AstroType_Aquarius:
        gbk_name.assign("Ë®Æ¿×ù");
        break;

    case AstroType_Pisces:
        gbk_name.assign("Ë«Óã×ù");
        break;

    default:
        break;
    } ///< switch

    return gbk_name;
}
