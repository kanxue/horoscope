
#include <iostream>
#include <string>

#include "horoscope/storage/common_def.h"
#include "horoscope/storage/storage_redis_client.h"
#include "horoscope/storage/storage_mysql_client.h"

struct Item 
{
    int type;
    int start_month;
    int start_day;
    int end_month;
    int end_day;
    std::string en_name;
    std::string zh_cn_name;
    std::string zh_tw_name;
};

Item g_item_list[] = {
    {1, 1, 20, 2, 18, "Aquarius", "Ë®Æ¿×ù", "Ë®Æ¿×ù"},
    {2, 2, 19, 3, 20, "Pisces", "Ë«Óã×ù", "Ë«Óã×ù"},
    {3, 3, 21, 4, 19, "Aries", "°×Ñò×ù", "°×Ñò×ù"},
    {4, 4, 20, 5, 20, "Taurus", "½ðÅ£×ù", "½ðÅ£×ù"},
    {5, 5, 21, 6, 21, "Gemini", "Ë«×Ó×ù", "Ë«×Ó×ù"},
    {6, 6, 22, 7, 22, "Cancer", "¾ÞÐ·×ù", "¾ÞÐ·×ù"},
    {7, 7, 23, 8, 22, "Leo", "Ê¨×Ó×ù", "Ê¨×Ó×ù"},
    {8, 8, 23, 9, 22, "Virgo", "´¦Å®×ù", "´¦Å®×ù"},
    {9, 9, 23, 10, 23, "Libra", "Ìì³Ó×ù", "Ìì³Ó×ù"},
    {10, 10, 24, 11, 22, "Scorpio", "ÌìÐ«×ù", "ÌìÐ«×ù"},
    {11, 11, 23, 12, 21, "Sagittarius", "ÉäÊÖ×ù", "ÉäÊÖ×ù"},
    {12, 12, 22, 1, 19, "Capricorn", "Ä¦ôÉ×ù", "Ä¦ôÉ×ù"}};


int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    StorageRedisClient client;
    StorageMysqlClient mysql_client;
    size_t size = sizeof(g_item_list) / sizeof(g_item_list[0]);
    for (size_t idx = 0; idx < size; ++idx) {
        horoscope::HoroscopeAttr attr;
#define SET_INT_ATTR(item) attr.set_##item(g_item_list[idx].item)
#define SET_STR_ATTR(item) attr.set_##item(GetUtf8String(g_item_list[idx].item))
        SET_INT_ATTR(type);
        SET_INT_ATTR(start_month);
        SET_INT_ATTR(start_day);
        SET_INT_ATTR(end_month);
        SET_INT_ATTR(end_day);
        SET_STR_ATTR(en_name);
        SET_STR_ATTR(zh_cn_name);
        SET_STR_ATTR(zh_tw_name);
#undef SET_STR_ATTR
#undef SET_INT_ATTR

        int ret = client.SetHoroscopeAttr(g_item_list[idx].type, attr);
        std::cout << "set " << idx << " ret " << ret << std::endl;

        int mysql_ret = mysql_client.SetHoroscopeAttr(g_item_list[idx].type, attr);
        std::cout << "set " << idx << " mysql_ret " << mysql_ret << std::endl;
    }

    return 0;
}

