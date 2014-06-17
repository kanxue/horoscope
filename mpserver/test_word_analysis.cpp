#include "word_analysis_mgr.h"

#include <iostream>
#include <string>

using namespace std;

int main(int argc, char const *argv[])
{
	while(1)
	{
		string input;
		cin>>input;
		word_analysis_mgr::printInfo();
		//enHoroscopeType type1 = word_analysis_mgr::GetHoroscopeType(input);
		//string showName = word_analysis_mgr::GetHoroscopeName(type1);
		//cout<<"This is "<<showName<<endl;
	}
	return 0;
}