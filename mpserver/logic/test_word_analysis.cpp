#include <iostream>
#include <string>

#include "word_analysis_mgr.h"

using namespace std;

int main()
{
	while(1)
	{
		string input;
        getline(cin, input);
        
		enHoroscopeType type1 = wordAnalysisMgr::GetHoroscopeType(input);
        enHoroscopeDate date = wordAnalysisMgr::GetHorosopeData(input);
        
        
		string showName = wordAnalysisMgr::GetHoroscopeName(type1);
		string showDate = wordAnalysisMgr::GetHoroscopeDate(date);
        
        cout<<showName<< "   " << showDate << endl;
	}
	return 0;
}
