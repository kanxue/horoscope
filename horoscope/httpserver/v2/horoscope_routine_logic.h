#ifndef HTTPSERVER_HOROSCOPE_ROUTINE_LOGIC_H_
#define HTTPSERVER_HOROSCOPE_ROUTINE_LOGIC_H_

#include <string>

class HttpRequest;
class HttpResponse;

namespace horoscope { namespace routine {
class HoroscopeRoutineItem;
}}

class HoroscopeRoutineLogic {
public:
    HoroscopeRoutineLogic(
        const HttpRequest* request,
        HttpResponse* response);

    virtual ~HoroscopeRoutineLogic();

public:
    void Run();

private:
    bool CheckRequest();

    void DoGetHoroscopeRoutine(
        const int astro,
        horoscope::routine::HoroscopeRoutineItem* routine);
    
    int DoGetHoroscopeRoutineFromDB(
        const int astro,
        horoscope::routine::HoroscopeRoutineItem* routine);

    std::string GetNameByAstro(const int astro);

private:
    const HttpRequest*   m_request;
    HttpResponse*        m_response;
    std::string          m_user_ip;
}; // class HoroscopeRoutineLogic

#endif // HTTPSERVER_HOROSCOPE_ROUTINE_LOGIC_H_

