#include "horoscope/storage/common_def.h"

#include "common/encoding/charset_converter.h"
#include "common/text/regex/regex.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

int Astro2Horoscope(int astro_type)
{
#define ADD_CASE_FIELD(type) \
    case AstroType_##type: return HoroscopeType_##type; break;

    switch (astro_type) {
        ADD_CASE_FIELD(Aquarius);
        ADD_CASE_FIELD(Pisces);
        ADD_CASE_FIELD(Aries);
        ADD_CASE_FIELD(Taurus);
        ADD_CASE_FIELD(Gemini);
        ADD_CASE_FIELD(Cancer);
        ADD_CASE_FIELD(Leo);
        ADD_CASE_FIELD(Virgo);
        ADD_CASE_FIELD(Libra);
        ADD_CASE_FIELD(Scorpio);
        ADD_CASE_FIELD(Sagittarius);
        ADD_CASE_FIELD(Capricorn);
    default:
        return HoroscopeType_UnknownHoroscope;
        break;
    }
#undef ADD_CASE_FIELD
}

int Horoscope2Astro(int horoscope_type)
{
    #define ADD_CASE_FIELD(type) \
    case HoroscopeType_##type: return AstroType_##type; break;

    switch (horoscope_type) {
        ADD_CASE_FIELD(Aquarius);
        ADD_CASE_FIELD(Pisces);
        ADD_CASE_FIELD(Aries);
        ADD_CASE_FIELD(Taurus);
        ADD_CASE_FIELD(Gemini);
        ADD_CASE_FIELD(Cancer);
        ADD_CASE_FIELD(Leo);
        ADD_CASE_FIELD(Virgo);
        ADD_CASE_FIELD(Libra);
        ADD_CASE_FIELD(Scorpio);
        ADD_CASE_FIELD(Sagittarius);
        ADD_CASE_FIELD(Capricorn);
    default:
        return AstroType_UnknownHoroscope;
        break;
    }
#undef ADD_CASE_FIELD
}

std::string GetUtf8String(const std::string& gbk_str)
{
    std::string out;
    if (!ConvertGbkToUtf8(gbk_str, &out)) out.assign(gbk_str);

    return out;
}

std::string GetGbkString(const std::string& utf8_str)
{
    std::string out;
    if (!ConvertUtf8ToGbk(utf8_str, &out)) out.assign(utf8_str);

    return out;
}



bool contains4DigitMonthDay(
		const std::string& origin,
        std::string& month,
        std::string& day)
{
    Regex date_regex("(\\d{4})");
    std::string input;

    if(date_regex.PartialMatch(origin, &input))
    {
        month = input.substr(0, 2);
        day = input.substr(2);
        return true;
    }
    return false;
    
}

bool containsSeperateMonthDay(const std::string& input,
                                     std::string& month,
                                     std::string& day)
{
    std::string format("((\\d{1,2})(月|-| |/|(\\.))(\\d{1,2}))");
    Regex date_regex(format.c_str());
    std::string date;
    
    if(date_regex.PartialMatch(input, &date))
    {
        Regex detailDate_regex("(\\d{1,2})");

        if(detailDate_regex.PartialMatch(date, &month))
        {           
        	std::string strPartDate = date.substr(month.size());

            if(month.size() == 1)
                month = "0" + month;
                    
            if(detailDate_regex.PartialMatch(strPartDate, &day))
            {
                if(day.size() == 1)
                    day = "0" + day;
                
                return true;
            }
            return false;
        }
        LOG(ERROR) << "second INPUT match error. " << input;
        return false;
    }
    else
    {
        LOG(WARNING) << "INPUT Regex match error. " << input;
        return false;
    }
}

bool GetMonthDay(const std::string& input, std::string& month, std::string& day)
{
    std::string digitDate;
    
    if(containsSeperateMonthDay(input, month, day))
    {
        return true;
    }
    if(contains4DigitMonthDay(input, month, day))
    {
        return true;
    }
    return false;
}

int GetHoroscopeType(const std::string& month, const std::string& day)
{
    if(month == "01")
    {
        // 摩羯座
        if(day >= "01" && day <= "19")
            return HoroscopeType_Capricorn;
        // 水瓶
        if(day >= "20" && day <= "31")
            return HoroscopeType_Aquarius;
    }
    if(month == "02")
    {
        // 水瓶
        if(day >= "01" && day <= "18")
            return  HoroscopeType_Aquarius;
        //双鱼
        if(day >= "19" && day <= "29")
            return HoroscopeType_Pisces;
    }
    if(month == "03")
    {
        //双鱼
        if(day >= "01" && day <= "20")
            return HoroscopeType_Pisces;
        //白羊
        if(day >= "21" && day <= "31")
            return HoroscopeType_Aries;
    }
    if(month == "04")
    {
        //白羊
        if(day >= "01" && day <= "19")
            return HoroscopeType_Aries;
        //金牛座
        if(day >= "20" && day <= "30")
            return HoroscopeType_Taurus;
    }
    if(month == "05")
    {
        //金牛座
        if(day >= "01" && day <= "21")
            return HoroscopeType_Taurus;
        //双子座
        if(day >= "22" && day <= "31")
            return HoroscopeType_Gemini;
    }
    if(month == "06")
    {
        //双子座
        if(day >= "01" && day <= "20")
            return HoroscopeType_Gemini;
        //巨蟹座
        if(day >= "21" && day <= "30")
            return HoroscopeType_Cancer;
    }
    if(month == "07")
    {
        //巨蟹座
        if(day >= "01" && day <= "21")
            return HoroscopeType_Cancer;
        //狮子座
        if(day >= "22" && day <= "31")
            return HoroscopeType_Leo;
    }
    if(month == "08")
    {
        // 狮子座
        if(day >= "01" && day <= "22")
            return HoroscopeType_Leo;
        //处女座
        if(day >= "23" && day <= "31")
            return HoroscopeType_Virgo;
    }
    if(month == "09")
    {
        //处女座
        if(day >= "01" && day <= "22")
            return HoroscopeType_Virgo;
        //天秤座
        if(day >= "23" && day <= "30")
            return HoroscopeType_Libra;
    }
    if(month == "10")
    {
        //天秤座
        if(day >= "01" && day <= "22")
            return HoroscopeType_Libra;
        //天蝎座
        if(day >= "23" && day <= "31")
            return HoroscopeType_Scorpio;
    }
    if(month == "11")
    {
        //天蝎座
        if(day >= "01" && day <= "21")
            return HoroscopeType_Scorpio;
        //射手座
        if(day >= "22" && day <= "30")
            return HoroscopeType_Sagittarius;
    }
    if(month == "12")
    {
        //射手座
        if(day >= "01" && day <= "21")
            return HoroscopeType_Sagittarius;
        //摩羯座
        if(day >= "22" && day <= "31")
            return HoroscopeType_Capricorn;
    }
    return HoroscopeType_UnknownHoroscope;
}

int GetHoroscopeTypeByText(const std::string& utf8_input)
{
    std::string input = GetGbkString(utf8_input);
    std::string month;
    std::string day;
	
    if(input.find("双鱼")!= std::string::npos)
        return HoroscopeType_Pisces;

    if(input.find("水瓶") != std::string::npos)
        return HoroscopeType_Aquarius;

    if(input.find("摩羯") != std::string::npos ||
       input.find("魔羯") != std::string::npos ||
       input.find("山羊") != std::string::npos)
        return HoroscopeType_Capricorn;

    if(input.find("射手") != std::string::npos ||
       input.find("人马") != std::string::npos)
        return HoroscopeType_Sagittarius;

    if(input.find("天蝎") != std::string::npos)
        return HoroscopeType_Scorpio;

    if(input.find("天秤") != std::string::npos ||
       input.find("天平") != std::string::npos ||
       input.find("天枰") != std::string::npos ||
       input.find("天称") != std::string::npos)
        return HoroscopeType_Libra;

    if(input.find("处女") != std::string::npos)
        return HoroscopeType_Virgo;

    if(input.find("狮子") != std::string::npos)
        return HoroscopeType_Leo;

    if(input.find("巨蟹") != std::string::npos)
        return HoroscopeType_Cancer;

    if(input.find("双子") != std::string::npos)
        return HoroscopeType_Gemini;

    if(input.find("金牛") != std::string::npos)
        return HoroscopeType_Taurus;

    if(input.find("白羊") != std::string::npos)
        return HoroscopeType_Aries;

    if(GetMonthDay(input, month, day))
        return GetHoroscopeType(month, day);

    return HoroscopeType_UnknownHoroscope;
}

int GetDateByText(const std::string& utf_origin)
{
    std::string origin = GetGbkString(utf_origin);
    if((origin.find("今天") != std::string::npos)
       || (origin.find("今日") != std::string::npos)
       || (origin.find("今儿") != std::string::npos))
        return Today;
    
    if((origin.find("明天") != std::string::npos)
       || (origin.find("明日") != std::string::npos)
       || (origin.find("明儿") != std::string::npos))
        return Tomorrow;
    
    if(origin.find("本周") != std::string::npos)
        return ThisWeek;
    
    if(origin.find("本月") != std::string::npos)
        return ThisMonth;

    if((origin.find("今年") != std::string::npos)
       || (origin.find("本年") != std::string::npos)
       || (origin.find("年度") != std::string::npos)
       || (origin.find("2015") != std::string::npos))
        return ThisYear;

    if(origin.find("羊羊羊羊羊羊") != std::string::npos)
        return ChineseYangYear;

    return UnknownDate;
}

int GetSexByText(const std::string& utf_origin)
{
    std::string origin = GetGbkString(utf_origin);
    if(origin.find("男") != std::string::npos)
        return 1;
    else if(origin.find("女") != std::string::npos)
        return 2;
    return 0;
}

