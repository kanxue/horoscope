#include <iostream>
#include <string>

#include "word_analysis_mgr.h"

using namespace std;

int main()
{
	while(1)
	{
		string input;
		cin>>input;
		enHoroscopeType type1 = wordAnalysisMgr::GetHoroscopeType(input);
		string showName = wordAnalysisMgr::GetHoroscopeName(type1);
		cout<<"This is "<<showName<<endl;
	}
	return 0;
}
