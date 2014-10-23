#ifndef STORAGE_STORAGE_MYSQL_CLIENT_H_
#define STORAGE_STORAGE_MYSQL_CLIENT_H_

#include <string>

#include "common/base/inttypes.h"
#include "common/base/scoped_ptr.h"
#include "common/base/singleton.h"
#include "common/base/string/concat.h"
#include "common/collection/memory_cache.h"
#include "common/system/concurrency/atomic/atomic.h"
#include "common/system/concurrency/mutex.h"
#include "thirdparty/mysql++/mysql++.h"

#include "horoscope/storage/storage.pb.h"

struct StorageMysqlClientOptions {
    std::string m_db_name;
    std::string m_host;
    int16_t     m_port;
    std::string m_user;
    std::string m_pass;
    bool        m_use_cache;
    uint32_t    m_cache_capacity;
    uint32_t    m_cache_expired;

    StorageMysqlClientOptions() {}

    StorageMysqlClientOptions(
        const std::string& db_name,
        const std::string& host,
        const int16_t port,
        const std::string& user,
        const std::string& pass,
        const bool use_cache,
        const uint32_t cache_capacity,
        const uint32_t cache_expired)
    : m_db_name(db_name),
        m_host(host),
        m_port(port),
        m_user(user),
        m_pass(pass),
        m_use_cache(use_cache),
        m_cache_capacity(cache_capacity),
        m_cache_expired(cache_expired) {}

    std::string ToString() const {
        return StringConcat(
            "name: ", m_db_name, "host: ", m_host, " port: ", m_port,
            " user: ", m_user, " pass: ", m_pass);
    }

    static StorageMysqlClientOptions& GetDefaultOptions();

}; // struct StorageMysqlClientOptions

class StorageMysqlClient {
public:
    StorageMysqlClient();

    StorageMysqlClient(const StorageMysqlClientOptions& options);
 
    virtual ~StorageMysqlClient();

public:
    int Connect(const StorageMysqlClientOptions& options);

    int GetForture(
        const int type, 
        const int astro,
        const std::string& day,
        std::string* content);

    int GetTodayForture(
        const int astro,
        std::string* content);

    int GetTomorrowForture(
        const int astro,
        std::string* content);

    int GetTswkForture(
        const int astro,
        std::string* content);
		
	int GetTodayFortuneReport(
		std::string* content);
		
    int GetMostRecentArticles(
        std::string* content);

public:
    // 从redis中迁移的数据接口
    int GetUserAttr(
        const std::string& openid,
        horoscope::UserAttr* userattr);

    int SetUserAttr(
        const std::string& openid,
        const horoscope::UserAttr& user_attr);

    int DelUserAttr(const std::string& openid);

    int GetHoroscopeAttr(
        const int32_t horoscope_type,
        horoscope::HoroscopeAttr* horoscope_attr);

    int SetHoroscopeAttr(
        const int32_t horoscope_type,
        const horoscope::HoroscopeAttr& horoscope_attr);

    int GetAllHoroscopeAttr(std::string* content);

private:
    int ConnectWithLock();

private:
    int GetFromCache(const std::string& key, std::string* value);

    int SetToCache(const std::string& key, const std::string& value);

    void MakeFortureKey(
        const int type,
        const int astro,
        const std::string& day,
        std::string* key);

    void MakeTswkFortureKey(
        const int type,
        const int astro,
        std::string* key);

private:
    typedef MemoryCache<std::string, std::string> MyMemoryCache;

    StorageMysqlClientOptions         m_options;
    Atomic<bool>                      m_has_connected;
    Mutex                             m_mutex;
    scoped_ptr<mysqlpp::Connection>   m_connection;
    scoped_ptr<mysqlpp::Query>        m_query;
    scoped_ptr<MyMemoryCache>         m_cache;
}; // class StorageMysqlClient

typedef Singleton<StorageMysqlClient> StorageMysqlClientSingleton;

#endif // STORAGE_STORAGE_MYSQL_CLIENT_H_

