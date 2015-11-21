#ifndef MPSERVER_COMMON_DEF_H_
#define MPSERVER_COMMON_DEF_H_

#include <string>

enum enAstroType {
    AstroType_Aries = 0, //白羊座
    AstroType_Taurus = 1, //金牛座
    AstroType_Gemini = 2, //双子座
    AstroType_Cancer = 3, //巨蟹座
    AstroType_Leo = 4,//狮子座
    AstroType_Virgo = 5,  //处女座
    AstroType_Libra = 6,//天秤座
    AstroType_Scorpio = 7,//天蝎座
    AstroType_Sagittarius = 8, //射手座
    AstroType_Capricorn = 9,//摩羯座
    AstroType_Aquarius = 10,//水瓶座
    AstroType_Pisces = 11,  //双鱼座
    AstroType_UnknownHoroscope = 12,     //出错
};

enum enHoroscopeType
{
	HoroscopeType_Aquarius = 1,		//水瓶座
	HoroscopeType_Pisces = 2,  		//双鱼座
	HoroscopeType_Aries	 = 3,		//白羊座
	HoroscopeType_Taurus	 = 4, 		//金牛座
	HoroscopeType_Gemini	 = 5, 		//双子座
	HoroscopeType_Cancer	 = 6, 		//巨蟹座
	HoroscopeType_Leo		 = 7,		//狮子座
	HoroscopeType_Virgo	 = 8,  		//处女座
	HoroscopeType_Libra	 = 9,		//天秤座
	HoroscopeType_Scorpio	 = 10,		//天蝎座
	HoroscopeType_Sagittarius = 11, 	//射手座
	HoroscopeType_Capricorn = 12,		//摩羯座
    HoroscopeType_UnknownHoroscope = 13,     //出错
};

enum enHoroscopeDate
{
    Today       = 1,
    Tomorrow    = 2,
    ThisWeek    = 3,
    ThisMonth   = 4,
    ThisYear    = 5,
    ChineseYangYear = 6,
    UnknownDate = 100,
};

enum ActionType {
    kActionType_Click = 1,
    kActionType_Message = 2,

}; // enum ActionType

enum dbDataType {
    dbDataType_Day = 0,
    dbDataType_Week = 1,
    dbDataType_Year = 9,
    dbDataType_DayStat = 8,    //每日星座指数
    dbDataType_DailyReport = 101,   //日报消息
};

#define SYSTEM_ERROR_WORDING "系统错误，请稍后重试 :-("
#define NOT_IMPLEMENT_WORDING "Coming soon"

//#define INPUT_BIRTHDAY_WITH_BIND_WORDING "请绑定你的出生日期，比如：绑定0622"
#define INPUT_HOROSCOPE_WITH_BIND_WORDING "请输入指令绑定你的出生日期或星座。比如：绑定11月9日 或 绑定1109 或 绑定天蝎"
#define INPUT_OTHER_HOROSCOPE_WORDING "请输入你要查询的星座或者对应的日期。比如：天蝎座 或 11月9日"
#define INPUT_MODIFY_HOROSCOPE_WORDING "若想修改你的星座，请回复绑定指令。比如：绑定11月9日 或 绑定1109 或 绑定天蝎"
#define BIND_KEY_WORDING "绑定"
#define BIND_SUCCESS_WORDING "绑定成功啦 :-)"
#define SUBSCRIBE_WORDING "你已经成功添加闹闹女巫店为好友啦!\n\n你可以使用屏幕下方的菜单按钮便捷地获取十二星座运势。\n\n你还可以直接回复消息获取关心的星座运势：比如“双子 本周”可以查询本周运势；直接输入日期如“1月12日”或者“0112“可以查询该日期所属星座的今日运势。\n\n喜欢就记得推荐给朋友吧，不相信星座的同志不是好同志mo-亲亲"
#define INPUT_UNKNOWN "不理解输入的指令，可能星座输入不对，或者试试菜单功能吧"
#define USER_COUNT_PREFIX "最近五分钟有"
#define USER_COUNT_SUFFIX "人与你同时使用了小巫~"
#define INPUT_HOROSCOPE_WITH_YEAR_WORDING "请输入你要查询的星座。比如天蝎年度 或者 天蝎2015"
#define INPUT_HOROSCOPE_WITH_YANG_YEAR_WORDING "请输入你要查询的星座和性别。比如 双鱼男 羊年签"

int Astro2Horoscope(int astro_type);
int Horoscope2Astro(int horoscope_type);

std::string GetUtf8String(const std::string& gbk_str);
std::string GetGbkString(const std::string& utf8_str);

int GetHoroscopeTypeByText(const std::string& utf8_origin);

int GetDateByText(const std::string& utf8_origin);
int GetSexByText(const std::string& utf_origin);

bool GetMonthDay(const std::string& gbk_input, std::string& month, std::string& day);

#endif // MPSERVER_COMMON_DEF_H_



