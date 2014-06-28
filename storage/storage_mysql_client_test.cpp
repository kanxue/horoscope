#include "thirdparty/gtest/gtest.h"

#include "storage_mysql_client.h"

TEST(StorageMysqlClientTest, NormalTest)
{
    StorageMysqlClientOptions options(
        "db_name", "127.0.0.1", 3306, "root", "pwd");
    StorageMysqlClient& client = StorageMysqlClientSingleton::Instance();
    client.Connect(options);
    std::string content;
    client.GetTodayForture(0, &content);
    client.GetTomorrowForture(1, &content);
    client.GetTswkForture(2, &content);
}
