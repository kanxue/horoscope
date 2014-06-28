#include "horoscope/storage/common_def.h"

#include "common/encoding/charset_converter.h"
#include "common/text/regex/regex.h"

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

int GetHoroscopeTypeByText(const std::string& origin)
{
    Regex date_regex("(\\d{4})");
    std::string input;
    if (date_regex.FullMatch(origin, &input)) {
        //Ë®Æ¿×ù
		if((input >= "0120" && input <= "0131") ||
		   (input >= "0201" && input <= "0218"))
			return HoroscopeType_Aquarius;

		//Ë«Óã
		if((input >= "0219" && input <= "0229") || 
		   (input >= "0301" && input <= "0320"))
			return HoroscopeType_Pisces;

		//Ä¦ôÉ×ù
		if((input >= "1222" && input <= "1231") ||
		   (input >= "0101" && input <= "0119"))
			return HoroscopeType_Capricorn;	

		//ÉäÊÖ×ù
		if((input >= "1122" && input <= "1130") ||
		   (input >= "1201" && input <= "1221"))
			return HoroscopeType_Sagittarius;	

		//ÌìĞ«×ù
		if((input >= "1023" && input <= "1031") ||
		   (input >= "1101" && input <= "1121"))
			return HoroscopeType_Scorpio;	

		//Ìì³Ó×ù
		if((input >= "0923" && input <= "0930") ||
		   (input >= "1001" && input <= "1022"))
			return HoroscopeType_Libra;	

		//´¦Å®×ù
		if((input >= "0823" && input <= "0831") ||
		   (input >= "0901" && input <= "0922"))
			return HoroscopeType_Virgo;

		//Ê¨×Ó×ù
		if((input >= "0722" && input <= "0731") ||
		   (input >= "0801" && input <= "0822"))
			return HoroscopeType_Leo;

		//¾ŞĞ·×ù
		if((input >= "0621" && input <= "0630") ||
		   (input >= "1001" && input <= "1022"))
			return HoroscopeType_Cancer;

		//Ë«×Ó×ù
		if((input >= "0522" && input <= "0531") ||
		   (input >= "0601" && input <= "0620"))
			return HoroscopeType_Gemini;

		//½ğÅ£×ù
		if((input >= "0420" && input <= "0530") ||
		   (input >= "0501" && input <= "0521"))
			return HoroscopeType_Taurus;

		//°×Ñò×ù
		if((input >= "0321" && input <= "0331") ||
		   (input >= "0401" && input <= "0419"))
			return HoroscopeType_Aries;
    } else {
        input = origin;
		if(input.find("Ë«Óã") != std::string::npos)
			return HoroscopeType_Pisces;

		if(input.find("Ë®Æ¿") != std::string::npos)
			return HoroscopeType_Aquarius;

		if(input.find("Ä¦ôÉ") != std::string::npos ||
		   input.find("Ä§ôÉ") != std::string::npos ||
		   input.find("É½Ñò") != std::string::npos)
			return HoroscopeType_Capricorn;

		if(input.find("ÉäÊÖ") != std::string::npos)
			return HoroscopeType_Sagittarius;

		if(input.find("ÌìĞ«") != std::string::npos)
			return HoroscopeType_Scorpio;

		if(input.find("Ìì³Ó") != std::string::npos ||
		   input.find("ÌìÆ½") != std::string::npos ||
		   input.find("ÌìèÒ") != std::string::npos)
			return HoroscopeType_Libra;

		if(input.find("´¦Å®") != std::string::npos)
			return HoroscopeType_Virgo;

		if(input.find("Ê¨×Ó") != std::string::npos)
			return HoroscopeType_Leo;

		if(input.find("¾ŞĞ·") != std::string::npos)
			return HoroscopeType_Cancer;

		if(input.find("Ë«×Ó") != std::string::npos)
			return HoroscopeType_Gemini;

		if(input.find("½ğÅ£") != std::string::npos)
			return HoroscopeType_Taurus;

		if(input.find("°×Ñò") != std::string::npos)
			return HoroscopeType_Aries;
	}

    return HoroscopeType_UnknownHoroscope;
}

