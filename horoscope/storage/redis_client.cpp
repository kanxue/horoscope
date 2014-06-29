#include "horoscope/storage/redis_client.h"

#include "thirdparty/glog/logging.h"

RedisClient::RedisClient()
{}

RedisClient::RedisClient(
    const std::string& ip,
    uint16_t port,
    int32_t db_index)
{
    Connect(ip, port, db_index);
}

RedisClient::~RedisClient()
{}

#define COMMON_TRY_CATCH_WITHOUT_KEY                                    \
    catch (redis::connection_error& ex) {                               \
        LOG(ERROR) << "connection error. what [" << ex.what() << "]";   \
        ret = kRedisConnectError;                                       \
    } catch (redis::protocol_error& ex) {                               \
        LOG(ERROR) << "protocol error. what [" << ex.what() << "]";     \
        ret = kRedisProtocolError;                                      \
    } catch (redis::key_error& ex) {                                    \
        LOG(ERROR) << "key error. what [" << ex.what() << "]";          \
        ret = kRedisKeyError;                                           \
    } catch (redis::value_error& ex) {                                  \
        LOG(ERROR) << "value error. what [" << ex.what() << "]";        \
        ret = kRedisValueError;                                         \
    } catch (redis::timeout_error& ex) {                                \
        LOG(ERROR) << "timeout error. what [" << ex.what() << "]";      \
        ret = kRedisTimeoutError;                                       \
    } catch (redis::redis_error& ex) {                                  \
        LOG(ERROR) << "redis error. what [" << ex.what() << "]";        \
        ret = kRedisBasicError;                                         \
    } catch (...) {                                                     \
        LOG(ERROR) << "unkown exception.";                              \
        ret = kRedisOtherError;                                         \
    }

#define COMMON_TRY_CATCH                                            \
    catch (redis::connection_error& ex) {                           \
        LOG(ERROR) << "connection error. key [" << key              \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisConnectError;                                   \
    } catch (redis::protocol_error& ex) {                           \
        LOG(ERROR) << "protocol error. key [" << key                \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisProtocolError;                                  \
    } catch (redis::key_error& ex) {                                \
        LOG(ERROR) << "key error. key [" << key                     \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisKeyError;                                       \
    } catch (redis::value_error& ex) {                              \
        LOG(ERROR) << "value error. key [" << key                   \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisValueError;                                     \
    } catch (redis::timeout_error& ex) {                            \
        LOG(ERROR) << "timeout error. key [" << key                 \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisTimeoutError;                                   \
    } catch (redis::redis_error& ex) {                              \
        LOG(ERROR) << "redis error. key [" << key                   \
                   << "] what [" << ex.what() << "]";               \
        ret = kRedisBasicError;                                     \
    } catch (...) {                                                 \
        LOG(ERROR) << "unkown exception. key [" << key << "]";      \
        ret = kRedisOtherError;                                     \
    }

int RedisClient::Connect(
    const std::string& ip,
    uint16_t port,
    int32_t db_index)
{
    int ret = kRedisSuccess;

    try {
        m_redis_client.reset(new redis::client(ip, port, db_index));
    } COMMON_TRY_CATCH_WITHOUT_KEY;

    LOG(INFO) << "connect done. ret " << ret;

    return ret;
}

int RedisClient::Set(
    const std::string& key,
    const std::string& value)
{
    int ret = kRedisSuccess;

    try {
        m_redis_client->set(key, value);
    } COMMON_TRY_CATCH;

    if  (ret == 0) {
        m_redis_client->save();
    }

    return ret;
}

int RedisClient::Set(
    const std::string& key,
    const std::string& new_value,
    std::string* old_value)
{
    // TODO
    return 0;
}

int RedisClient::Get(
    const std::string& key,
    std::string* value)
{
    int ret = kRedisSuccess;

    try {
        *value = m_redis_client->get(key);
    } COMMON_TRY_CATCH;

    if ((ret == 0) && *value == redis::client::missing_value()) {
        return kRedisKeyNotExists;
    }

    return ret;
}

int RedisClient::IsExist(
    const std::string& key,
    bool* is_exist)
{
    int ret = kRedisSuccess;
    try {
        *is_exist = m_redis_client->exists(key);
    } COMMON_TRY_CATCH;

    return ret;
}

int RedisClient::Delete(const std::string& key)
{
    int ret = kRedisSuccess;
    bool done = false;

    try {
        done = m_redis_client->del(key);
    } COMMON_TRY_CATCH;

    if (ret == 0) {
        m_redis_client->save();
    }

    return ret;
}

int RedisClient::Append(
    const std::string& key,
    const std::string& value,
    size_t* new_length)
{
    int ret = kRedisSuccess;

    try {
        size_t length = m_redis_client->append(key, value);
        if (new_length) *new_length = length;
    } COMMON_TRY_CATCH;

    if (ret == 0) {
        m_redis_client->save();
    }

    return ret;
}

int RedisClient::Substr(
    const std::string& key,
    int begin,
    int end,
    std::string* value)
{
    int ret = kRedisSuccess;

    try {
        *value = m_redis_client->substr(key, begin, end);
    } COMMON_TRY_CATCH;

    return ret;
}

