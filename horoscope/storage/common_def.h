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
    UnknownDate = 6,
};

#define NOT_IMPLEMENT_WORDING "开发中，敬请期待 :-)"

#define INPUT_BIRTHDAY_WORDING "请输入你的出生日期，比如：0622"
#define INPUT_BIRTHDAY_WITH_BIND_WORDING "请绑定你的出生日期，比如：绑定0622"
#define INPUT_HOROSCOPE_WORDING "请输入你的星座，比如：天蝎"
#define INPUT_HOROSCOPE_WITH_BIND_WORDING "请绑定你的星座，比如：绑定天蝎"
#define INPUT_OTHER_HOROSCOPE_WORDING "请输入你要查询的星座，比如：天蝎座"
#define INPUT_MODIFY_HOROSCOPE_WORDING "若想修改你的星座，请回复绑定指令。比如：绑定天蝎"
#define BIND_SUCCESS_WORDING "绑定成功啦 :-)"
#define SUBSCRIBE_WORDING "你已经成功添加女巫店十二星座运势为好友啦. 首先输入“绑定 你的生日”来关联自己的星座，比如有双子座输入“绑定 0112”，然后就可以获取你的专属星座运势啦。玩转自定义回复获取关心的星座运势：比如“双子 本周”可以查询本周运势；直接输入生日如“0112“可以查询该日期所属星座的今日运势。喜欢就记得推荐给朋友吧，不相信星座的同志不是好同志mo-亲亲"

int Astro2Horoscope(int astro_type);
int Horoscope2Astro(int horoscope_type);

std::string GetUtf8String(const std::string& gbk_str);

int GetHoroscopeTypeByText(const std::string& origin);

int GetDateByText(const std::string& origin);

#endif // MPSERVER_COMMON_DEF_H_

