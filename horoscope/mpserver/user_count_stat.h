#ifndef MPSERVER_USER_COUNT_STAT_H_
#define MPSERVER_USER_COUNT_STAT_H_

#include <set>
#include <string>
#include "common/base/singleton.h"
#include "common/base/timed_stats.h"

class UserCountStat {
public:
    UserCountStat();

    virtual ~UserCountStat();

public:
    void AddUserCount();

    uint32_t GetRecentUserCount(const uint32_t seconds);

    std::string GetAndReportUserCount(
        const std::string& openid, const uint32_t seconds = 300u);

private:
    std::set<std::string>     m_whitelist;
    CountingStats<uint32_t>   m_stats;
}; // class UserCountStat

typedef Singleton<UserCountStat> UserCountStatSingleton;

#endif // MPSERVER_USER_COUNT_STAT_H_

