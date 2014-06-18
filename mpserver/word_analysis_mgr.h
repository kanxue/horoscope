

#include <string>

#include "common_def.h"


class wordAnalysisMgr{
public:
    wordAnalysisMgr();
    
    
	static enHoroscopeType GetHoroscopeType(
		const std::string& input);
	
	static std::string GetHoroscopeName(
		const enHoroscopeType& type);

	
private:
    static bool is4DigitDate(
		const std::string& input);

};
