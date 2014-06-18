#include <string>
#include "word_analysis_mgr.h"

wordAnalysisMgr::wordAnalysisMgr()
{
}

void wordAnalysisMgr::testFunc()
{
    int i = 1;
}

bool wordAnalysisMgr::is4DigitData(
		const std::string& input)
{
	if(input.size() != 4)
		return false;

	for(int i = 0; i < input.size(); i++)
		if(input[i] < '0' || input[i] > 9)
			return false;

	return true;
}

enHoroscopeType wordAnalysisMgr::GetHoroscopeType(
	const std::string& input)
{
	if(wordAnalysisMgr::is4DigitData(input))
	{
		//双鱼
		if((input >= "0219" && input <= "0229") || 
		   (input >= "0301" && input <= "0320"))
			return Pisces;

		//水瓶座
		if((input >= "0120" && input <= "0131") ||
		   (input >= "0201" && input <= "0218"))
			return Aquarius;

		//摩羯座
		if((input >= "1222" && input <= "1231") ||
		   (input >= "0101" && input <= "0119"))
			return Capricorn;	

		//射手座
		if((input >= "1122" && input <= "1130") ||
		   (input >= "1201" && input <= "1221"))
			return Sagittarius;	

		//天蝎座
		if((input >= "1023" && input <= "1031") ||
		   (input >= "1101" && input <= "1121"))
			return Scorpio;	

		//天秤座
		if((input >= "0923" && input <= "0930") ||
		   (input >= "1001" && input <= "1022"))
			return Libra;	

		//处女座
		if((input >= "0823" && input <= "0831") ||
		   (input >= "0901" && input <= "0922"))
			return Virgo;

		//狮子座
		if((input >= "0722" && input <= "0731") ||
		   (input >= "0801" && input <= "0822"))
			return Leo;

		//巨蟹座
		if((input >= "0621" && input <= "0630") ||
		   (input >= "1001" && input <= "1022"))
			return Cancer;

		//双子座
		if((input >= "0522" && input <= "0531") ||
		   (input >= "0601" && input <= "0620"))
			return Gemini;

		//金牛座
		if((input >= "0420" && input <= "0530") ||
		   (input >= "0501" && input <= "0521"))
			return Taurus;

		//白羊座
		if((input >= "0321" && input <= "0331") ||
		   (input >= "0401" && input <= "0419"))
			return Aries;
	}
	else
	{
		if(input.find("双鱼") != std::string::npos)
			return Pisces;

		if(input.find("水瓶") != std::string::npos)
			return Aquarius;

		if(input.find("摩羯") != std::string::npos ||
		   input.find("魔羯") != std::string::npos ||
		   input.find("山羊") != std::string::npos)
			return Capricorn;

		if(input.find("射手") != std::string::npos)
			return Sagittarius;

		if(input.find("天蝎") != std::string::npos)
			return Scorpio;

		if(input.find("天秤") != std::string::npos ||
		   input.find("天平") != std::string::npos ||
		   input.find("天枰") != std::string::npos)
			return Libra;

		if(input.find("处女") != std::string::npos)
			return Virgo;

		if(input.find("狮子") != std::string::npos)
			return Leo;

		if(input.find("巨蟹") != std::string::npos)
			return Cancer;

		if(input.find("双子") != std::string::npos)
			return Gemini;

		if(input.find("金牛") != std::string::npos)
			return Taurus;

		if(input.find("白羊") != std::string::npos)
			return Aries;
	}

}


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

	}

}

void wordAnalysisMgr::printInfo()
	{
		int i = 0;
	}









