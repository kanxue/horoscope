#include "storage_mysql_client.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/format.h"
#include "common/system/time/datetime.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(storage_mysql_name, "eyrie_farm", "db name");
DEFINE_string(storage_mysql_host, "114.113.155.201", "mysql host");
DEFINE_int32(storage_mysql_port, 3306, "mysql port");
DEFINE_string(storage_mysql_user, "public", "mysql user");
DEFINE_string(storage_mysql_pass, "forconnect", "mysql pass");

StorageMysqlClientOptions& StorageMysqlClientOptions::GetDefaultOptions()
{
    static StorageMysqlClientOptions obj(
        FLAGS_storage_mysql_name, FLAGS_storage_mysql_host,
        FLAGS_storage_mysql_port, FLAGS_storage_mysql_user,
        FLAGS_storage_mysql_pass);
    return obj;
}

StorageMysqlClient::StorageMysqlClient()
{
    Connect(StorageMysqlClientOptions::GetDefaultOptions());
}

StorageMysqlClient::StorageMysqlClient(
    const StorageMysqlClientOptions& options)
{
    Connect(options);
}

StorageMysqlClient::~StorageMysqlClient()
{}

int StorageMysqlClient::Connect(const StorageMysqlClientOptions& options)
{
    ScopedLocker<Mutex> locker(&m_mutex);

    m_options = options;

    return ConnectWithLock();
}


int StorageMysqlClient::GetForture(
    const int type,
    const int astro,
    const std::string& day,
    std::string* content)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select content from fortune where type=%d and "
        "astro=%d and day=\"%s\";", type, astro, day.c_str());
    mysqlpp::StoreQueryResult result = m_query->store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();

    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }

    *content = ReplaceAll(*content, "\r\n", "\n");
    
    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::GetTodayForture(
    const int astro,
    std::string* content)
{
    TimeSpan span(0, 0, 0, 0);
    DateTime now = DateTime::Now() - span;
    std::string day = now.ToString("yyyy-MM-dd 00:00:00");

    int type = 0;
    return GetForture(type, astro, day, content);
}

int StorageMysqlClient::GetTomorrowForture(
    const int astro,
    std::string* content)
{
    TimeSpan span(1, 0, 0, 0);
    DateTime now = DateTime::Now() - span;
    std::string day = now.ToString("yyyy-MM-dd 00:00:00");

    int type = 0;
    return GetForture(type, astro, day, content);
}

int StorageMysqlClient::GetTswkForture(
    const int astro,
    std::string* content)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    int type = 1;
    std::string sql = StringFormat(
        "select content from fortune where type=%d and astro=%d "
        "order by day desc limit 1;", type, astro);
    mysqlpp::StoreQueryResult result = m_query->store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();
    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }

    *content = ReplaceAll(*content, "\r\n", "\n");

    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::GetMostRecentArticles(
    std::string* content)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    int type = 0;
    std::string sql = StringFormat(
        "select content from mp_articles where type=%d "
        "order by day desc limit 1;", type);
    mysqlpp::StoreQueryResult result = m_query->store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();
    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }

    ReplaceAllChars(content, "\r\n", '\n');

    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::ConnectWithLock()
{
    m_has_connected = (m_connection.get() && m_connection->connected());
    if (!m_has_connected) {
        m_connection.reset(new mysqlpp::Connection(false));
        bool done = m_connection->connect(
            m_options.m_db_name.c_str(), m_options.m_host.c_str(),
            m_options.m_user.c_str(), m_options.m_pass.c_str(),
            m_options.m_port);
        m_query.reset(new mysqlpp::Query(m_connection.get(), false));
        if (!done) {
            LOG(ERROR) << "try connect failed. " << m_options.ToString();
            return -1;
        }

        m_has_connected = true;
    }

    return 0;
}

