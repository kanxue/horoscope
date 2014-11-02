
#include "user_count_stat.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/format.h"
#include "thirdparty/gflags/gflags.h"

#include "horoscope/storage/common_def.h"

DEFINE_bool(enable_user_count_stat, false, "");

UserCountStat::UserCountStat()
{
    m_whitelist.insert("owNXfjnoorf8a_eo4GUf9TqMmiS0");
    m_whitelist.insert("owNXfjmEoZoStLb_SSvSR2T_jLQ8");
}

UserCountStat::~UserCountStat()
{}

void UserCountStat::AddUserCount(const std::string& openid)
{
    if (!StringStartsWith(openid, "TEST_OPENID_")) {
        m_stats.Increase();
    }
}

uint32_t UserCountStat::GetRecentUserCount(const uint32_t seconds)
{
    return m_stats.GetRecentCount(seconds);
}

std::string UserCountStat::GetAndReportUserCount(
    const std::string& openid, const uint32_t seconds)
{
    uint32_t count = m_stats.GetRecentCount(seconds);

    if (!StringStartsWith(openid, "TEST_OPENID_")) {
        m_stats.Increase();
    }

    if (!FLAGS_enable_user_count_stat
        && (m_whitelist.find(openid) == m_whitelist.end()))
        return "";

    return StringFormat("\n%s%u%s", USER_COUNT_PREFIX, count, USER_COUNT_SUFFIX);
}

