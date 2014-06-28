#ifndef STORAGE_STORAGE_REDIS_CLIENT_H_
#define STORAGE_STORAGE_REDIS_CLIENT_H_

#include <algorithm>

#include "common/base/scoped_ptr.h"
#include "horoscope/storage/redis_client.h"
#include "horoscope/storage/storage_client.h"

class StorageRedisClient : public StorageClient {
public:
    StorageRedisClient();

    virtual ~StorageRedisClient();

public:
    // user_attr function cluster.
    virtual int GetUserAttr(
        const std::string& openid,
        horoscope::UserAttr* userattr);

    virtual int SetUserAttr(
        const std::string& openid,
        const horoscope::UserAttr& user_attr);

    virtual int MergeUserAttr(
        const std::string& openid,
        const horoscope::UserAttr& user_attr);

    virtual int DelUserAttr(const std::string& openid);

public:
    // user messages function cluster.
    virtual int GetUserMessages(
        const std::string& openid,
        horoscope::UserMessages* user_messages);

    virtual int AddUserMessages(
        const std::string& openid,
        const horoscope::UserMessages::Item& item);

public:
    // user locations function cluster.
    virtual int GetUserLocations(
        const std::string& openid,
        horoscope::UserLocations* user_locations);

    virtual int AddUserLocation(
        const std::string& openid,
        const horoscope::UserLocations::Item& item);

public:
    // lucky message function cluster.
    virtual int GetLuckyMessage(
        const std::string& date,
        int32_t horoscope_type,
        horoscope::LuckyMessage* lucky_message);

    virtual int SetLuckyMessage(
        const std::string& date,
        int32_t horoscope_type,
        const horoscope::LuckyMessage& lucky_message);

public:
    virtual int GetHoroscopeAttr(
        const std::string& date,
        horoscope::HoroscopeAttr* horoscope_attr);

    virtual int GetHoroscopeAttr(
        int32_t horoscope_type,
        horoscope::HoroscopeAttr* horoscope_attr);

    virtual int SetHoroscopeAttr(
        int32_t horoscope_type,
        const horoscope::HoroscopeAttr& horoscope_attr);

public:
    int GetAllHoroscopeAttr(std::string* content);

private:
    void ResetRedisClient();

    void MakeUserAttrKey(const std::string& openid, std::string* key);

    void MakeUserMessagesKey(const std::string& openid, std::string* key);

    void MakeUserLocationsKey(const std::string& openid, std::string* key);

    void MakeLuckyMessageKey(
        const std::string& date,
        int32_t horoscope_type,
        std::string* key);

    void MakeHoroscopeAttrKey(int32_t horoscope_type, std::string* key);

    int GetPbFromRedis(
        const std::string& key,
        google::protobuf::Message* message);

    int SetPbToRedis(
        const std::string& key,
        const google::protobuf::Message& message);

    template<typename MessageType>
    void AdjustListCount(MessageType* message, int32_t max_count);

private:
    scoped_ptr<RedisClient>    m_redis_client;
}; // class StorageRedisClient


template<typename ItemType>
bool ItemCompare(const ItemType& lhs, const ItemType& rhs)
{
    return (lhs.stamp() >rhs.stamp());
}

template<typename MessageType>
void StorageRedisClient::AdjustListCount(
    MessageType* message, int32_t max_count)
{
    if (!message) return;

    std::vector<typename MessageType::Item> item_list;
    item_list.resize(message->item_list_size());
    for (int index = 0; index < message->item_list_size(); ++index) {
        item_list[index].CopyFrom(message->item_list(index));
    }

    std::sort(item_list.begin(), item_list.end(),
              ItemCompare<typename MessageType::Item>);
    if (message->item_list_size() > max_count) {
        item_list.resize(max_count);
    }

    message->clear_item_list();
    for (size_t index = 0; index < item_list.size(); ++index) {
        message->add_item_list()->CopyFrom(item_list[index]);
    }
}

#endif // STORAGE_STORAGE_REDIS_CLIENT_H_

