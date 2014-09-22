#ifndef HTTPSERVER_HTTPSERVER_FILTER_H_
#define HTTPSERVER_HTTPSERVER_FILTER_H_

#include <set>
#include <string>

#include "common/base/inttypes.h"
#include "common/system/concurrency/rwlock.h"

class HttpServerFilter {
public:
    HttpServerFilter();

    virtual ~HttpServerFilter();

    static HttpServerFilter* GetDefault();

    static bool EasyCheckPass(const std::string& ip);

public:
    void LoadIfModify(const std::string& config_file);

    bool CheckPass(const std::string& ip);

private:
    void Load(const std::string& config_file, uint32_t modify_time);
	
	void GetIpSegement(const std::string& ip, std::string& ip_seg);

private:
    typedef std::set<std::string> WhiteIpSet;
    RWLock       m_rw_lock;
    uint32_t     m_last_load_time;
    WhiteIpSet   m_white_ip_set;
}; // class HttpServerFilter

#endif // HTTPSERVER_HTTPSERVER_FILTER_H_

