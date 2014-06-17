#include <string>
#include "word_analysis_mgr.h"


static bool s4DigitData(
		const std::string& input)
{
	if（input.size != 4)
		return false;

	for（int i = 0；i < input.size; i++)
		if(input[i] < '0' || input[i] > 9)
			return false;

	return true;
}

static enHoroscopeType GetHoroscopeType(
	const std::string& input)
{
	if（is4DigitData(input)）
	{
		//双鱼
		if((input >= "0219" && input <= "0229") || 
		   (input >= "0301" && input <= "0320"))
			return enHoroscopeType.Pisces;

		//水瓶座
		if((input >= "0120" && input <= "0131") ||
		   (input >= "0201" && input <= "0218"))
			return enHoroscopeType.Aquarius;

		//摩羯座
		if((input >= "1222" && input <= "1231") ||
		   (input >= "0101" && input <= "0119"))
			return enHoroscopeType.Capricorn;	

		//射手座
		if((input >= "1122" && input <= "1130") ||
		   (input >= "1201" && input <= "1221"))
			return enHoroscopeType.Sagittarius;	

		//天蝎座
		if((input >= "1023" && input <= "1031") ||
		   (input >= "1101" && input <= "1121"))
			return enHoroscopeType.Scorpio;	

		//天秤座
		if((input >= "0923" && input <= "0930") ||
		   (input >= "1001" && input <= "1022"))
			return enHoroscopeType.Libra;	

		//处女座
		if((input >= "0823" && input <= "0831") ||
		   (input >= "0901" && input <= "0922"))
			return enHoroscopeType.Virgo;

		//狮子座
		if((input >= "0722" && input <= "0731") ||
		   (input >= "0801" && input <= "0822"))
			return enHoroscopeType.Leo;

		//巨蟹座
		if((input >= "0621" && input <= "0630") ||
		   (input >= "1001" && input <= "1022"))
			return enHoroscopeType.Cancer;

		//双子座
		if((input >= "0522" && input <= "0531") ||
		   (input >= "0601" && input <= "0620"))
			return enHoroscopeType.Gemini;

		//金牛座
		if((input >= "0420" && input <= "0530") ||
		   (input >= "0501" && input <= "0521"))
			return enHoroscopeType.Taurus;

		//白羊座
		if((input >= "0321" && input <= "0331") ||
		   (input >= "0401" && input <= "0419"))
			return enHoroscopeType.Aries;
	}
	else
	{
		if(input.find("双鱼") != std::string::npos)
			return enHoroscopeType.Pisces;

		if(input.find("水瓶") != std::string::npos)
			return enHoroscopeType.Aquarius;

		if(input.find("摩羯") != std::string::npos ||
		   input.find("魔羯") != std::string::npos ||
		   input.find("山羊") != std::string::npos)
			return enHoroscopeType.Capricorn;

		if(input.find("射手") != std::string::npos)
			return enHoroscopeType.Sagittarius;

		if(input.find("天蝎") != std::string::npos)
			return enHoroscopeType.Scorpio;

		if(input.find("天秤") != std::string::npos ||
		   input.find("天平") != std::string::npos ||
		   input.find("天枰") != std::string::npos)
			return enHoroscopeType.Libra;

		if(input.find("处女") != std::string::npos)
			return enHoroscopeType.Virgo;

		if(input.find("狮子") != std::string::npos)
			return enHoroscopeType.Leo;

		if(input.find("巨蟹") != std::string::npos)
			return enHoroscopeType.Cancer;

		if(input.find("双子") != std::string::npos)
			return enHoroscopeType.Gemini;

		if(input.find("金牛") != std::string::npos)
			return enHoroscopeType.Taurus;

		if(input.find("白羊") != std::string::npos)
			return enHoroscopeType.Aries;
	}

}


static std::string GetHoroscopeName(
		const enHoroscopeType& type)
{
	switch(type)
	{
		case enHoroscopeType.Pisces:
			return "双鱼座";

		case enHoroscopeType.Aquarius:
			return "水瓶座";

		case enHoroscopeType.Capricorn:
			return "摩羯座";

		case enHoroscopeType.Sagittarius:
			return "射手座";

		case enHoroscopeType.Scorpio:
			return "天蝎座";

		case enHoroscopeType.Libra:
			return "天秤座";

		case enHoroscopeType.Virgo:
			return "处女座";

		case enHoroscopeType.Leo:
			return "狮子座";

		case enHoroscopeType.Cancer:
			return "巨蟹座";

		case enHoroscopeType.Gemini:
			return "双子座";

		case enHoroscopeType.Taurus:
			return "金牛座";

		case enHoroscopeType.Aries:
			return "白羊座";

	}

}

static void printInfo()
	{
		int i = 0;
	}









