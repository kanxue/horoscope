#include "horoscope/storage/storage_redis_client.h"

#include "common/base/string/algorithm.h"
#include "common/base/string/concat.h"
#include "common/base/string/format.h"
#include "common/base/string/string_number.h"
#include "common/encoding/json_to_pb.h"
#include "common/encoding/pb_to_json.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/storage/common_def.h"

DEFINE_string(storage_redis_ip, "127.0.0.1", "");
DEFINE_int32(storage_redis_port, 6379, "");
DEFINE_int32(storage_redis_index, 0, "");
DEFINE_int32(storage_max_user_messages_count, 100, "");
DEFINE_int32(storage_max_user_locations_count, 100, "");

StorageRedisClient::StorageRedisClient()
{}

StorageRedisClient::~StorageRedisClient()
{}

int StorageRedisClient::GetUserAttr(
    const std::string& openid,
    horoscope::UserAttr* user_attr)
{
    std::string key;
    MakeUserAttrKey(openid, &key);
    return GetPbFromRedis(key, user_attr);
}

int StorageRedisClient::SetUserAttr(
    const std::string& openid,
    const horoscope::UserAttr& user_attr)
{
    std::string key;
    MakeUserAttrKey(openid, &key);
    return SetPbToRedis(key, user_attr);
}

int StorageRedisClient::MergeUserAttr(
    const std::string& openid,
    const horoscope::UserAttr& user_attr)
{
    std::string key;
    horoscope::UserAttr saved_attr;

    MakeUserAttrKey(openid, &key);
    int ret = GetPbFromRedis(key, &saved_attr);
    if ((ret != 0) && (ret != kRedisKeyNotExists)) {
        LOG(ERROR)
            << __func__ << " read old attr fail. key [" << key << "]";
        return ret;
    }

    saved_attr.set_openid(openid);
    saved_attr.MergeFrom(user_attr);
    ret = SetPbToRedis(key, saved_attr);
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " set new attr fail. key [" << key << "]";
        return ret;
    }

    return 0;
}

int StorageRedisClient::DelUserAttr(const std::string& openid)
{
    ResetRedisClient();

    std::string key;
    MakeUserAttrKey(openid, &key);
    int ret = m_redis_client->Delete(key);
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " del attr fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    return 0;
}

// user messages function cluster.
int StorageRedisClient::GetUserMessages(
    const std::string& openid,
    horoscope::UserMessages* user_messages)
{
    std::string key;
    MakeUserMessagesKey(openid, &key);
    return GetPbFromRedis(key, user_messages);
}

int StorageRedisClient::AddUserMessages(
    const std::string& openid,
    const horoscope::UserMessages::Item& item)
{
    std::string key;
    horoscope::UserMessages saved_message;
    MakeUserMessagesKey(openid, &key);
    int ret = GetPbFromRedis(key, &saved_message);
    if ((ret != 0) && (ret != kRedisKeyNotExists)) {
        LOG(ERROR)
            << __func__ << " read old messages fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    saved_message.set_openid(openid);
    saved_message.add_item_list()->CopyFrom(item);
    AdjustListCount(&saved_message, FLAGS_storage_max_user_messages_count);

    ret = SetPbToRedis(key, saved_message);
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " set new messages fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    return 0;
}

// user locations function cluster.
int StorageRedisClient::GetUserLocations(
    const std::string& openid,
    horoscope::UserLocations* user_locations)
{
    std::string key;
    MakeUserLocationsKey(openid, &key);
    return GetPbFromRedis(key, user_locations);
}

int StorageRedisClient::AddUserLocation(
    const std::string& openid,
    const horoscope::UserLocations::Item& item)
{
    std::string key;
    horoscope::UserLocations saved_locations;
    MakeUserLocationsKey(openid, &key);
    int ret = GetPbFromRedis(key, &saved_locations);
    if ((ret != 0) && (ret != kRedisKeyNotExists)) {
        LOG(ERROR)
            << __func__ << " read old locations fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    saved_locations.set_openid(openid);
    saved_locations.add_item_list()->CopyFrom(item);
    AdjustListCount(&saved_locations, FLAGS_storage_max_user_locations_count);

    ret = SetPbToRedis(key, saved_locations);
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " set new locations fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    return 0;
}

// lucky message function cluster.
int StorageRedisClient::GetLuckyMessage(
    const std::string& date,
    int32_t horoscope_type,
    horoscope::LuckyMessage* lucky_message)
{
    std::string key;
    MakeLuckyMessageKey(date, horoscope_type, &key);
    return GetPbFromRedis(key, lucky_message);
}

int StorageRedisClient::SetLuckyMessage(
    const std::string& date,
    int32_t horoscope_type,
    const horoscope::LuckyMessage& lucky_message)
{
    std::string key;
    MakeLuckyMessageKey(date, horoscope_type, &key);
    return SetPbToRedis(key, lucky_message);
}

int StorageRedisClient::GetHoroscopeAttr(
    const std::string& date,
    horoscope::HoroscopeAttr* horoscope_attr)
{
    int32_t month = 0, day = 0;
    if ((date.size() != 4u)
        || !StringToNumber(date.substr(0, 2), &month, 10)
        || !StringToNumber(date.substr(2), &day, 10)) {
        LOG(ERROR)
            << __func__ << " wrong date format. date [" << date << "]";
        return -1;
    }

    int ret = 0;
    for (int32_t type = 1; type <= 12; ++type) {
        // via data, NOT via hard-code.
        ret = GetHoroscopeAttr(type, horoscope_attr);
        if (ret != 0) continue;
        if (((month == horoscope_attr->start_month())
             && (day >= horoscope_attr->start_day()))
            || ((month == horoscope_attr->end_month())
                && (day <= horoscope_attr->start_day()))) {
            return 0;
        }
    }

    horoscope_attr->Clear();
    return -2;
}

int StorageRedisClient::GetHoroscopeAttr(
    int32_t horoscope_type,
    horoscope::HoroscopeAttr* horoscope_attr)
{
    std::string key;
    MakeHoroscopeAttrKey(horoscope_type, &key);
    return GetPbFromRedis(key, horoscope_attr);
}

int StorageRedisClient::SetHoroscopeAttr(
    int32_t horoscope_type,
    const horoscope::HoroscopeAttr& horoscope_attr)
{
    std::string key;
    MakeHoroscopeAttrKey(horoscope_type, &key);
    return SetPbToRedis(key, horoscope_attr);
}

int StorageRedisClient::GetAllHoroscopeAttr(std::string* content)
{
    int ret = 0;
    horoscope::HoroscopeAttr horoscope_attr;

    content->clear();
    for (int type = 0; type <HoroscopeType_UnknownHoroscope; ++type) {
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
    }

    StringTrim(content);

    return 0;
}

void StorageRedisClient::ResetRedisClient()
{
    if (!m_redis_client) {
        LOG(INFO) << "ip " << FLAGS_storage_redis_ip;
        
        m_redis_client.reset(
            new RedisClient(
                FLAGS_storage_redis_ip,
                FLAGS_storage_redis_port,
                FLAGS_storage_redis_index));
    }
}

void StorageRedisClient::MakeUserAttrKey(
    const std::string& openid,
    std::string* key)
{
    StringConcatTo(key, "user_attr_", openid);
}

void StorageRedisClient::MakeUserMessagesKey(
    const std::string& openid,
    std::string* key)
{
    StringConcatTo(key, "user_messages_", openid);
}

void StorageRedisClient::MakeUserLocationsKey(
    const std::string& openid,
    std::string* key)
{
    StringConcatTo(key, "user_locations_", openid);
}

void StorageRedisClient::MakeLuckyMessageKey(
    const std::string& date,
    int32_t horoscope_type,
    std::string* key)
{
    StringConcatTo(key, "lucky_message_", date, '_', horoscope_type);
}

void StorageRedisClient::MakeHoroscopeAttrKey(
    int32_t horoscope_type, std::string* key)
{
    StringConcatTo(key, "horoscope_attr_", horoscope_type);
}

int StorageRedisClient::GetPbFromRedis(
    const std::string& key,
    google::protobuf::Message* message)
{
    ResetRedisClient();

    std::string value;
    int ret = m_redis_client->Get(key, &value);
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " read from redis fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    std::string error;
    if (!JsonToProtoMessage(value, message, &error, false)) {
        LOG(ERROR)
            << __func__ << " json to pb fail. json_string ["
            << value << "] error [" << error << "]";
        return 1;
    }

    return 0;
}

int StorageRedisClient::SetPbToRedis(
    const std::string& key,
    const google::protobuf::Message& message)
{
    ResetRedisClient();

    std::string value, error;
    if (!ProtoMessageToJson(message, &value, &error, false)) {
        LOG(ERROR)
            << __func__ << " pb to json fail. pb ["
            << message.ShortDebugString() << "] error [" << error << "]";
        return 1;
    }

    int ret = m_redis_client->Set(key, value);
    LOG(INFO)
        << "set '" << key << "' '" << value << "'";
    if (ret != 0) {
        LOG(ERROR)
            << __func__ << " set to redis fail. key [" << key
            << "] ret " << ret;
        return ret;
    }

    if (*value.rbegin() == '\n') value.resize(value.size() - 1);
    LOG(INFO)
        << "set '" << key << "' '" << value << "'";

    return 0;
}

