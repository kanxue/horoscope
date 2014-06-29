#ifndef STORAGE_STORAGE_MYSQL_CLIENT_H_
#define STORAGE_STORAGE_MYSQL_CLIENT_H_

#include <string>

#include "common/base/inttypes.h"
#include "common/base/scoped_ptr.h"
#include "common/base/singleton.h"
#include "common/base/string/concat.h"
#include "common/system/concurrency/atomic/atomic.h"
#include "common/system/concurrency/mutex.h"
#include "thirdparty/mysql++/mysql++.h"

struct StorageMysqlClientOptions {
    std::string m_db_name;
    std::string m_host;
    int16_t     m_port;
    std::string m_user;
    std::string m_pass;

    StorageMysqlClientOptions() {}

    StorageMysqlClientOptions(
        const std::string& db_name,
        const std::string& host,
        const int16_t port,
        const std::string& user,
        const std::string& pass)
    : m_db_name(db_name),
        m_host(host),
        m_port(port),
        m_user(user),
        m_pass(pass) {}

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

private:
    int ConnectWithLock();

private:
    StorageMysqlClientOptions         m_options;
    Atomic<bool>                      m_has_connected;
    Mutex                             m_mutex;
    scoped_ptr<mysqlpp::Connection>   m_connection;
    scoped_ptr<mysqlpp::Query>        m_query;
}; // class StorageMysqlClient

typedef Singleton<StorageMysqlClient> StorageMysqlClientSingleton;

#endif // STORAGE_STORAGE_MYSQL_CLIENT_H_

