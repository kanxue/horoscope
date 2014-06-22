

#include <string>

#include "common_def.h"


class wordAnalysisMgr{
public:
    wordAnalysisMgr();
    
    static enHoroscopeType GetHoroscopeType(const std::string& month, const std::string& day);
    
	static enHoroscopeType GetHoroscopeType(
		const std::string& input);
	
    static enHoroscopeDate GetHorosopeData(
        const std::string& input);
    
    static bool IsBindCmd(
        const std::string& input);
    
    
    
    //for test
	static std::string GetHoroscopeName(
		const enHoroscopeType& type);

    static std::string GetHoroscopeDate(
        const enHoroscopeDate& date);
    
private:

    static bool contains4DigitMonthDay(
		const std::string& input,
        std::string& month,
        std::string& day);
    
    static bool containsSeperateMonthDay(
        const std::string& input,
        std::string& month,
        std::string& day);
    
    static bool getMonthDay(
        const std::string& input,
        std::string& month,
        std::string& day);
};
