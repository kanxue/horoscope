

#include <string>

#include "common_def.h"

class word_analysis_mgr
{
public:
	/* data */
	static bool is4DigitData(
		const std::string& input);

	static enHoroscopeType GetHoroscopeType(
		const std::string& input);
	
	static std::string GetHoroscopeName(
		const enHoroscopeType& type);

	static void printInfo();

};