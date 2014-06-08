#ifndef STORAGE_REDIS_CLIENT_H_
#define STORAGE_REDIS_CLIENT_H_

#include <string>

#include "common/base/scoped_ptr.h"
#include "common/base/stdint.h"
#include "thirdparty/redis/redisclient.h"

enum RedisStatusCode {
    kRedisSuccess = 0,
    kRedisBasicError = -99,
    kRedisConnectError = -100,
    kRedisProtocolError = -101,
    kRedisKeyError = -102,
    kRedisValueError = -103,
    kRedisTimeoutError = -104,

    kRedisOtherError = -199,
}; // enum RedisStatusCode

class RedisClient {
public:
    RedisClient();

    explicit RedisClient(const std::string& ip,
                uint16_t port = 6379,
                int32_t db_index = 0);

    virtual ~RedisClient();

public:
    int Connect(const std::string& ip,
                uint16_t port = 6379,
                int32_t db_index = 0);

    int Set(const std::string& key,
            const std::string& value);

    int Set(const std::string& key,
            const std::string& new_value,
            std::string* old_value);

    int Get(const std::string& key,
            std::string* value);

    int IsExist(const std::string& key, bool* is_exist);

    int Delete(const std::string& key);

    int Append(const std::string& key,
               const std::string& value,
               size_t* new_length = NULL);

    int Substr(const std::string& key,
               int begin,
               int end,
               std::string* value);

private:
    scoped_ptr<redis::client> m_redis_client;
}; // class RedisClient

#endif // STORAGE_REDIS_CLIENT_H_

