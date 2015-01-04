#include "storage_mysql_client.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/format.h"
#include "common/system/time/datetime.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/storage/common_def.h"

DEFINE_string(storage_mysql_name, "eyrie_farm", "db name");
DEFINE_string(storage_mysql_host, "114.113.155.201", "mysql host");
DEFINE_int32(storage_mysql_port, 3306, "mysql port");
DEFINE_string(storage_mysql_user, "public", "mysql user");
DEFINE_string(storage_mysql_pass, "forconnect", "mysql pass");
DEFINE_int32(storage_mysql_timezone, 0, "time zone");
DEFINE_bool(storage_mysql_use_cache, true, "use cache or not");
DEFINE_int32(storage_mysql_cache_capacity, 100, "cache capacity");
DEFINE_int32(storage_mysql_cache_expired, 60, "cache expired seconds");

StorageMysqlClientOptions& StorageMysqlClientOptions::GetDefaultOptions()
{
    static StorageMysqlClientOptions obj(
        FLAGS_storage_mysql_name, FLAGS_storage_mysql_host,
        FLAGS_storage_mysql_port, FLAGS_storage_mysql_user,
        FLAGS_storage_mysql_pass, FLAGS_storage_mysql_use_cache,
        FLAGS_storage_mysql_cache_capacity, FLAGS_storage_mysql_cache_expired);
    return obj;
}

StorageMysqlClient::StorageMysqlClient()
{
    //m_query = NULL;
    Connect(StorageMysqlClientOptions::GetDefaultOptions());
}

StorageMysqlClient::StorageMysqlClient(
    const StorageMysqlClientOptions& options)
{
    //m_query = NULL;
    Connect(options);
}

StorageMysqlClient::~StorageMysqlClient()
{}

int StorageMysqlClient::Connect(const StorageMysqlClientOptions& options)
{
    ScopedLocker<Mutex> locker(&m_mutex);

    m_options = options;

    if (m_options.m_use_cache) {
        m_cache.reset(new MyMemoryCache(m_options.m_cache_capacity));
    }

    return ConnectWithLock();
}


int StorageMysqlClient::GetForture(
    const int type,
    const int astro,
    const std::string& day,
    std::string* content)
{
    // get from cache firstly.
    std::string key;
    MakeFortureKey(type, astro, day, &key);
    int ret = GetFromCache(key, content);
    if (ret == 0) {
        return 0;
    }

    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select content from fortune where type=%d and "
        "astro=%d and day=\"%s\";", type, astro, day.c_str());
    mysqlpp::Query query = m_connection->query();
    //mysqlpp::StoreQueryResult result = m_query->store(sql);
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();

    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }
    *content = ReplaceAll(*content, "\r\n", "\n");

    // save to cache.
    SetToCache(key, *content);

    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::GetTodayForture(
    const int astro,
    std::string* content)
{
    TimeSpan span(0, FLAGS_storage_mysql_timezone, 0, 0);
    DateTime now = DateTime::UTCNow() - span;
    std::string day = now.ToString("yyyy-MM-dd 00:00:00");

    int type = 0;
    return GetForture(type, astro, day, content);
}

int StorageMysqlClient::GetTomorrowForture(
    const int astro,
    std::string* content)
{
    TimeSpan span(1, 0, 0, 0);
    DateTime now = DateTime::Now() + span - TimeSpan(0, FLAGS_storage_mysql_timezone, 0, 0);
    std::string day = now.ToString("yyyy-MM-dd 00:00:00");

    int type = 0;
    return GetForture(type, astro, day, content);
}

int StorageMysqlClient::GetTswkForture(
    const int astro,
    std::string* content)
{
    int type = 1;
    // get from cache firstly.
    std::string key;
    MakeTswkFortureKey(type, astro, &key);
    int ret = GetFromCache(key, content);
    if (ret == 0) {
        return 0;
    }

    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select content from fortune where type=%d and astro=%d "
        "order by day desc limit 1;", type, astro);
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();
    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }
	
    *content = ReplaceAll(*content, "\r\n", "\n");

    // save to cache.
    SetToCache(key, *content);

    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::GetYearForture(
    const int astro,
    std::string* content)
{
    int type = 9;
    std::string key;
    MakeYearFortureKey(type, astro, &key);
    int ret = GetFromCache(key, content);
    if (ret == 0) {
        return 0;
    }

    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select content from mp_articles where type=%d and astro=%d "
        "order by day desc limit 1;", type, astro);
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();
    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }
	
    *content = ReplaceAll(*content, "\r\n", "\n");

    // save to cache.
    SetToCache(key, *content);

    return (content->empty()) ? 1 : 0;
    
}

int StorageMysqlClient::GetTodayFortuneReport(
    std::string* content)
{
	TimeSpan span(0, FLAGS_storage_mysql_timezone, 0, 0);
    DateTime now = DateTime::Now() - span;
    std::string day = now.ToString("yyyy-MM-dd 00:00:00");

    int type = 8;
    return GetForture(type, 0, day, content);
}	

int StorageMysqlClient::GetDailyReport(
    std::string* content)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    int type = dbDataType_DailyReport;
    std::string sql = StringFormat(
        "select content from mp_articles where type=%d "
        "order by day desc limit 1;", type);
    //mysqlpp::StoreQueryResult result = m_query->store(sql);
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    content->clear();
    if (num_rows > 0) {
        result[0]["content"].to_string(*content);
    }

    ReplaceAllChars(content, "\r\n", '\n');

    return (content->empty()) ? 1 : 0;
}

int StorageMysqlClient::GetUserAttr(
    const std::string& openid,
    horoscope::UserAttr* userattr)
{
    LOG(INFO) << "begin " << __func__;
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select birth_year, birth_month, birth_day, sex, "
        "horoscope_type, rule_id from storage_user_attr "
        "where openid='%s';", openid.c_str());
    LOG(INFO) << "ready run mysql [" << sql << "]";
    //mysqlpp::StoreQueryResult result = m_query->store(sql);
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    if (num_rows > 0) {
#define SET_INT_FROM_MYSQL(item) userattr->set_##item(result[0][#item]);
#define SET_STR_FROM_MYSQL(item) result[0][#item].to_string(*userattr->mutable_##item());
        SET_INT_FROM_MYSQL(birth_year);
        SET_INT_FROM_MYSQL(birth_month);
        SET_INT_FROM_MYSQL(birth_day);
        SET_INT_FROM_MYSQL(sex);
        SET_INT_FROM_MYSQL(horoscope_type);
        SET_INT_FROM_MYSQL(rule_id);
#undef SET_STR_FROM_MYSQL
#undef SET_INT_FROM_MYSQL
    }

    return (num_rows > 0) ? 0 : 1;
}

int StorageMysqlClient::SetUserAttr(
    const std::string& openid,
    const horoscope::UserAttr& user_attr)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select openid from storage_user_attr where openid='%s';",
        openid.c_str());
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    bool succ = false;
    if (num_rows <= 0) {
        sql = StringFormat(
            "insert into storage_user_attr(openid, birth_year, birth_month, "
            "birth_day, sex, horoscope_type, rule_id) values('%s', %d, %d, "
            " %d, %d, %d, %d);",
            openid.c_str(), user_attr.birth_year(),
            user_attr.birth_month(), user_attr.birth_day(), user_attr.sex(),
            user_attr.horoscope_type(), user_attr.rule_id());
        //bool succ = m_query->exec(sql);
        query = m_connection->query();
        succ = query.exec(sql);
        LOG(INFO) << "run [" << sql << "] succ " << succ;
    } else {
        sql = StringFormat(
            "update storage_user_attr set birth_year=%d, birth_month=%d, "
            "birth_day=%d, sex=%d, horoscope_type=%d, rule_id=%d "
            "where openid='%s';",
            user_attr.birth_year(), user_attr.birth_month(),
            user_attr.birth_day(), user_attr.sex(),
            user_attr.horoscope_type(), user_attr.rule_id(), openid.c_str());
        query = m_connection->query();
        succ = query.exec(sql);
        LOG(INFO) << "run [" << sql << "] succ " << succ;
    }

    return succ ? 0 : 1;
}

int StorageMysqlClient::DelUserAttr(const std::string& openid)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "delete from storage_user_attr where openid='%s';",
        openid.c_str());
    //bool succ = m_query->exec(sql);
    mysqlpp::Query query = m_connection->query();
    bool succ = query.exec(sql);
    LOG(INFO) << "run [" << sql << "] succ " << succ;

    return succ ? 0 : 1;
}

int StorageMysqlClient::GetHoroscopeAttr(
    const int32_t horoscope_type,
    horoscope::HoroscopeAttr* horoscope_attr)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "select start_month, start_day, end_month, end_day, en_name, "
        "zh_cn_name, zh_tw_name from storage_horoscope_attr where type=%d;",
        horoscope_type);
    //mysqlpp::StoreQueryResult result = m_query->store(sql);
    mysqlpp::Query query = m_connection->query();
    mysqlpp::StoreQueryResult result = query.store(sql);
    int num_rows = result.num_rows();
    LOG(INFO) << "run [" << sql << "] num_rows " << num_rows;
    if (num_rows > 0) {
#define SET_INT_FROM_MYSQL(item) horoscope_attr->set_##item(result[0][#item]);
#define SET_STR_FROM_MYSQL(item) result[0][#item].to_string(*horoscope_attr->mutable_##item());
        SET_INT_FROM_MYSQL(start_month);
        SET_INT_FROM_MYSQL(start_day);
        SET_INT_FROM_MYSQL(end_month);
        SET_INT_FROM_MYSQL(end_day);
        SET_STR_FROM_MYSQL(en_name);
        SET_STR_FROM_MYSQL(zh_cn_name);
        SET_STR_FROM_MYSQL(zh_tw_name);
#undef SET_STR_FROM_MYSQL
#undef SET_INT_FROM_MYSQL
    }

    return (num_rows > 0) ? 0 : 1;
}

int StorageMysqlClient::SetHoroscopeAttr(
    const int32_t horoscope_type,
    const horoscope::HoroscopeAttr& horoscope_attr)
{
    ScopedLocker<Mutex> locker(&m_mutex);
    ConnectWithLock();

    std::string sql = StringFormat(
        "replace into storage_horoscope_attr(type, start_month, start_day, "
        "end_month, end_day, en_name, zh_cn_name, zh_tw_name) values("
        "%d, %d, %d, %d, %d, '%s', '%s', '%s');", horoscope_type, horoscope_attr.start_month(),
        horoscope_attr.start_day(), horoscope_attr.end_month(), horoscope_attr.end_day(),
        horoscope_attr.en_name().c_str(), horoscope_attr.zh_cn_name().c_str(),
        horoscope_attr.zh_tw_name().c_str());
    //bool succ = m_query->exec(sql);
    mysqlpp::Query query = m_connection->query();
    bool succ = query.exec(sql);
    LOG(INFO) << "run [" << sql << "] succ " << succ;

    return succ ? 0 : 1;
}

int StorageMysqlClient::GetAllHoroscopeAttr(std::string* content)
{
    int ret = 0;
    
    horoscope::HoroscopeAttr horoscope_attr;

    content->clear();
    for (int type = 0; type < HoroscopeType_UnknownHoroscope; ++type) {
        ret = GetHoroscopeAttr(type, &horoscope_attr);
        if (ret == 0) {
            content->append(horoscope_attr.zh_cn_name());
            content->append(" : ");
            std::string start_end;
            StringFormatTo(
                &start_end, "%d月%d日 - %d月%d日\n",
                horoscope_attr.start_month(), horoscope_attr.start_day(),
                horoscope_attr.end_month(), horoscope_attr.end_day());
            content->append(GetUtf8String(start_end));
        }
    } ///< for

    StringTrim(content);

    return 0;
}

int StorageMysqlClient::AddUserClickAction(
    const std::string& openid, const std::string& event_key)
{
    if (openid == "TEST_OPENID_FOR_AUTOTEST") return 0;

    int action_type = kActionType_Click;
    uint32_t create_time = static_cast<uint32_t>(time(NULL));

    mysqlpp::SQLStream sql_stream(m_connection.get(), event_key.c_str());
    std::string escape_string;
    sql_stream.escape_string(&escape_string, event_key.data(), event_key.size());

    std::string sql = StringFormat(
        "insert into storage_user_action_history("
        "openid, type, content, create_time) values('%s', %d, '%s', %u);",
        openid.c_str(), action_type, escape_string.c_str(), create_time);

    mysqlpp::Query query = m_connection->query();
    bool succ = query.exec(sql);
    LOG(INFO) << "run [" << sql << "] succ " << succ;

    return succ ? 0 : -1;
}

int StorageMysqlClient::AddUserMessageAction(
    const std::string& openid, const std::string& content)
{
    if (openid == "TEST_OPENID_FOR_AUTOTEST") return 0;

    int action_type = kActionType_Message;
    uint32_t create_time = static_cast<uint32_t>(time(NULL));

    mysqlpp::SQLStream sql_stream(m_connection.get(), content.c_str());
    std::string escape_string;
    sql_stream.escape_string(&escape_string, content.data(), content.size());
    if (escape_string.size() > 126u) {
        escape_string.resize(126u);
    }

    std::string sql = StringFormat(
        "insert into storage_user_action_history("
        "openid, type, content, create_time) values('%s', %d, '%s', %u);",
        openid.c_str(), action_type, escape_string.c_str(), create_time);
    mysqlpp::Query query = m_connection->query();
    bool succ = query.exec(sql);
    LOG(INFO) << "run [" << sql << "] succ " << succ;

    return succ ? 0 : -1;
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
        //m_query->reset(new mysqlpp::Query(m_connection.get(), false));
        if (!done) {
            LOG(ERROR) << "try connect failed. " << m_options.ToString();
            return -1;
        }

        LOG(INFO)
            << "build connection done. connection " << (void*)m_connection.get()
            << " " << m_options.ToString();
        //m_query = &(m_connection->query());
        mysqlpp::Query query = m_connection->query();
        //if (!m_query->exec("set names utf8;")) {
        if (!query.exec("set names utf8;")) {
            LOG(ERROR) << "set names utf8 failed.";
        }

        m_has_connected = true;
    }

    return 0;
}

int StorageMysqlClient::GetFromCache(
    const std::string& key,
    std::string* value)
{
    if (!m_options.m_use_cache) {
        return 1;
    }

    if (m_cache->Get(key, value)) {
        LOG(INFO)
            << __func__ << " hit cache. key [" << key
            << "] value_size " << value->size();
        return 0;
    } else {
        LOG(INFO)
            << __func__ << " miss cache. key [" << key
            << "] cache_size " << m_cache->Size();
        return 1;
    }
}

int StorageMysqlClient::SetToCache(
    const std::string& key,
    const std::string& value)
{
    if (m_options.m_use_cache) {
        m_cache->PutWithExpiry(key, value, m_options.m_cache_expired * 1000);
    }

    return 0;
}

void StorageMysqlClient::MakeFortureKey(
    const int type,
    const int astro,
    const std::string& day,
    std::string* key)
{
    StringFormatTo(key, "day_%d_%d_%s", type, astro, day.c_str());
}

void StorageMysqlClient::MakeTswkFortureKey(
    const int type,
    const int astro,
    std::string* key)
{
    StringFormatTo(key, "tswk_%d_%d", type, astro);
}

void StorageMysqlClient::MakeYearFortureKey(
    const int type,
    const int astro,
    std::string* key)
{
    StringFormatTo(key, "year_%d_%d", type, astro);
}

