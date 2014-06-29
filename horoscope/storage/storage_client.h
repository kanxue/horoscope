#ifndef STORAGE_STORAGE_CLIENT_H_
#define STORAGE_STORAGE_CLIENT_H_

#include <string>

#include "common/base/inttypes.h"

#include "horoscope/storage/storage.pb.h"

class StorageClient {
public:
    StorageClient() {}

    virtual ~StorageClient() {}

public:
    // user_attr function cluster.
    virtual int GetUserAttr(
        const std::string& openid,
        horoscope::UserAttr* userattr) = 0;

    virtual int SetUserAttr(
        const std::string& openid,
        const horoscope::UserAttr& user_attr) = 0;

    virtual int MergeUserAttr(
        const std::string& openid,
        const horoscope::UserAttr& user_attr) = 0;

    virtual int DelUserAttr(const std::string& openid) = 0;

public:
    // user messages function cluster.
    virtual int GetUserMessages(
        const std::string& openid,
        horoscope::UserMessages* user_messags) = 0;

    virtual int AddUserMessages(
        const std::string& openid,
        const horoscope::UserMessages::Item& item) = 0;

public:
    // user locations function cluster.
    virtual int GetUserLocations(
        const std::string& openid,
        horoscope::UserLocations* user_locations) = 0;

    virtual int AddUserLocation(
        const std::string& openid,
        const horoscope::UserLocations::Item& item) = 0;

public:
    // lucky message function cluster.
    virtual int GetLuckyMessage(
        const std::string& date,
        int32_t horoscope_type,
        horoscope::LuckyMessage* lucky_message) = 0;

    virtual int SetLuckyMessage(
        const std::string& date,
        int32_t horoscope_type,
        const horoscope::LuckyMessage& lucky_message) = 0;

public:
    virtual int GetHoroscopeAttr(
        const std::string& date,
        horoscope::HoroscopeAttr* horoscope_attr) = 0;

    virtual int GetHoroscopeAttr(
        int32_t horoscope_type,
        horoscope::HoroscopeAttr* horoscope_attr) = 0;

    virtual int SetHoroscopeAttr(
        int32_t horoscope_type,
        const horoscope::HoroscopeAttr& horoscope_attr) = 0;
}; // class StorageClient

#endif // STORAGE_STORAGE_CLIENT_H_

