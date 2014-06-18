

#include <string>

#include "common_def.h"


class wordAnalysisMgr{
public:
    wordAnalysisMgr();
    
    
	static enHoroscopeType GetHoroscopeType(
		const std::string& input);
	
    static enHoroscopeDate GetHorosopeData(
        const std::string& input);
    
    
    
    //for test
	static std::string GetHoroscopeName(
		const enHoroscopeType& type);

    static std::string GetHoroscopeDate(
        const enHoroscopeDate& date);
    
private:

    static bool has4DigitDate(
		const std::string& input,
        std::string& result);

};
