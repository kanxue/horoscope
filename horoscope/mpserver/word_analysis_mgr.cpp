#include <string>
#include <regex>
#include "word_analysis_mgr.h"

wordAnalysisMgr::wordAnalysisMgr()
{
}


bool wordAnalysisMgr::getMonthDay(const std::string& input, std::string& month, std::string& day)
{
    std::string digitDate;
    
    if(wordAnalysisMgr::containsSeperateMonthDay(input, month, day))
    {
        return true;
    }
    if(wordAnalysisMgr::contains4DigitMonthDay(input, month, day))
    {
        return true;
    }
    return false;
}


bool wordAnalysisMgr::contains4DigitMonthDay(
		const std::string& input,
        std::string& month,
        std::string& day)
{
    std::smatch match;
    
    std::regex_constants::syntax_option_type fl = std::regex_constants::syntax_option_type::icase;
    // 编译一个正则表达式语句
    std::regex regExpress("(\\d{4})", fl);
    
    
    if(regex_search(input, match, regExpress))
    {
        std::string result = input.substr(match.position(0), match.length(0));
        month = result.substr(0, 2);
        day = result.substr(2);
        return true;
    }
    return false;
    
}


bool wordAnalysisMgr::containsSeperateMonthDay(
                                     const std::string& input,
                                     std::string& month,
                                     std::string& day)
{
    std::smatch match;
    std::regex_constants::syntax_option_type fl = std::regex_constants::syntax_option_type::icase;
    // 编译一个正则表达式语句
    std::regex regExpress("((\\d{1,2})(月|-| |/|(\\.))(\\d{1,2}))", fl);
    
    if(regex_search(input, match, regExpress))
    {
        std::string strDate = input.substr(match.position(0), match.length(0));
        std::smatch match2, match3;
        std::regex regExpress2("(\\d{1,2})", fl);
        
        if(regex_search(strDate, match2, regExpress2))
        {
            month = strDate.substr(match2.position(0), match2.length(0));
            
            if(month.size() == 1)
                month = "0" + month;
            
            strDate = strDate.substr(match2.length(0));
            
            if(regex_search(strDate.substr(), match3, regExpress2))
            {
                day = strDate.substr(match3.position(0), match3.length(0));
                
                if(day.size() == 1)
                    day = "0" + day;
                
                return true;
            }
            return false;
        }
        return false;
    }
    else
    {
        return false;
    }
}


enHoroscopeType wordAnalysisMgr::GetHoroscopeType(const std::string& month, const std::string& day)
{
    if(month == "01")
    {
        //摩羯座
        if(day >= "01" && day <= "19")
            return Capricorn;
        //水瓶
        if(day >= "20" && day <= "31")
            return Aquarius;
    }
    if(month == "02")
    {
        //水瓶
        if(day >= "01" && day <= "18")
            return  Aquarius;
        //双鱼
        if(day >= "19" && day <= "29")
            return Pisces;
    }
    if(month == "03")
    {
        //双鱼
        if(day >= "01" && day <= "20")
            return Pisces;
        //白羊
        if(day >= "21" && day <= "31")
            return Aries;
    }
    if(month == "04")
    {
        //白羊
        if(day >= "01" && day <= "19")
            return Aries;
        //金牛座
        if(day >= "20" && day <= "30")
            return Taurus;
    }
    if(month == "05")
    {
        //金牛座
        if(day >= "01" && day <= "21")
            return Taurus;
        //双子座
        if(day >= "22" && day <= "31")
            return Gemini;
    }
    if(month == "06")
    {
        //双子座
        if(day >= "01" && day <= "20")
            return Gemini;
        //巨蟹座
        if(day >= "21" && day <= "30")
            return Cancer;
    }
    if(month == "07")
    {
        //巨蟹座
        if(day >= "01" && day <= "21")
            return Taurus;
        //狮子座
        if(day >= "22" && day <= "31")
            return Leo;
    }
    if(month == "08")
    {
        //狮子座
        if(day >= "01" && day <= "22")
            return Leo;
        //处女座
        if(day >= "23" && day <= "31")
            return Virgo;
    }
    if(month == "09")
    {
        //处女座
        if(day >= "01" && day <= "22")
            return Virgo;
        //天秤座
        if(day >= "23" && day <= "30")
            return Libra;
    }
    if(month == "10")
    {
        //天秤座
        if(day >= "01" && day <= "22")
            return Libra;
        //天蝎座
        if(day >= "23" && day <= "31")
            return Scorpio;
    }
    if(month == "11")
    {
        //天蝎座
        if(day >= "01" && day <= "21")
            return Scorpio;
        //射手座
        if(day >= "22" && day <= "30")
            return Sagittarius;
    }
    if(month == "12")
    {
        //射手座
        if(day >= "01" && day <= "21")
            return Sagittarius;
        //摩羯座
        if(day >= "22" && day <= "31")
            return Capricorn;
    }
    return UnknownHoroscope;
}


enHoroscopeType wordAnalysisMgr::GetHoroscopeType(
	const std::string& input)
{
    std::string month;
    std::string day;
	if(wordAnalysisMgr::getMonthDay(input, month, day))
	{
        return wordAnalysisMgr::GetHoroscopeType(month, day);
	}
	else
	{
		if(input.find("双鱼") != std::string::npos ||
           input.find("雙魚") != std::string::npos)
			return Pisces;

		if(input.find("水瓶") != std::string::npos)
			return Aquarius;

		if(input.find("摩羯") != std::string::npos ||
		   input.find("魔羯") != std::string::npos ||
		   input.find("山羊") != std::string::npos)
			return Capricorn;

		if(input.find("射手") != std::string::npos ||
           input.find("人马") != std::string::npos)
			return Sagittarius;

		if(input.find("天蝎") != std::string::npos ||
           input.find("天蠍") != std::string::npos)
			return Scorpio;

		if(input.find("天秤") != std::string::npos ||
		   input.find("天平") != std::string::npos ||
		   input.find("天枰") != std::string::npos ||
           input.find("天称") != std::string::npos)
			return Libra;

		if(input.find("处女") != std::string::npos ||
           input.find("處女") != std::string::npos)
			return Virgo;

		if(input.find("狮子") != std::string::npos ||
           input.find("獅子") != std::string::npos)
			return Leo;

		if(input.find("巨蟹") != std::string::npos)
			return Cancer;

		if(input.find("双子") != std::string::npos ||
           input.find("雙子") != std::string::npos)
			return Gemini;

		if(input.find("金牛") != std::string::npos)
			return Taurus;

		if(input.find("白羊") != std::string::npos)
			return Aries;
	}
    return UnknownHoroscope;

}

enHoroscopeDate wordAnalysisMgr::GetHorosopeData(
    const std::string& input)
{
    if(input.find("今天") != std::string::npos ||
       input.find("今日") != std::string::npos)
        return Today;
    
    if(input.find("明天") != std::string::npos ||
       input.find("明日") != std::string::npos)
        return Tomorrow;
    
    if(input.find("本周") != std::string::npos ||
       input.find("本週") != std::string::npos ||
       input.find("一周") != std::string::npos)
        return ThisWeek;
    
    if(input.find("本月") != std::string::npos)
        return ThisMonth;

    if(input.find("今年") != std::string::npos ||
       input.find("年度") != std::string::npos)
        return ThisYear;

    return UnknownDate;
}

bool wordAnalysisMgr::IsBindCmd(const std::string& input)
{
    if(input.find("绑定") != std::string::npos)
    {
        return true;
    }
    return false;
}




//for test

std::string wordAnalysisMgr::GetHoroscopeName(
		const enHoroscopeType& type)
{
	switch(type)
	{
		case Pisces:
			return "双鱼座";

		case Aquarius:
			return "水瓶座";

		case Capricorn:
			return "摩羯座";

		case Sagittarius:
			return "射手座";

		case Scorpio:
			return "天蝎座";

		case Libra:
			return "天秤座";

		case Virgo:
			return "处女座";

		case Leo:
			return "狮子座";

		case Cancer:
			return "巨蟹座";

		case Gemini:
			return "双子座";

		case Taurus:
			return "金牛座";

		case Aries:
			return "白羊座";

        default:
            return "未知星座";
	}

}


std::string wordAnalysisMgr::GetHoroscopeDate(
        const enHoroscopeDate& date)
{
    switch (date) {
        case Today:
            return "今日";
            
        case Tomorrow:
            return "明日";
            
        case ThisWeek:
            return "本周";
        
        case ThisMonth:
            return "本月";
            
        case ThisYear:
            return "今年";
            
        default:
            return "未知日期";
    }
}








