

#include <string>

#include "common_def.h"


class wordAnalysisMgr{
public:
    wordAnalysisMgr();
    
    void testFunc();
    
	/* data */
	
	static enHoroscopeType GetHoroscopeType(
		const std::string& input);
	
	static std::string GetHoroscopeName(
		const enHoroscopeType& type);

	static void printInfo();
	
private:
    static bool is4DigitData(
		const std::string& input);

};
