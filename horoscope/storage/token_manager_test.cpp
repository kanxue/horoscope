
#include <iostream>
#include "horoscope/storage/token_manager.h"
#include "thirdparty/gtest/gtest.h"

TEST(TokenManagerTest, BasicTest)
{
    TokenManager& tm = TokenManagerSingleton::Instance();

    for (int i = 0; i < 3; ++i) {
        uint32_t uid = 16u;
        std::string ip = "127.0.0.1";
        uint32_t now = 1447594893u;
        std::string token;
        tm.GenerateToken(uid, ip, now, &token);
        std::cout << "new_token '" << token << "'\n";

        uint32_t stamp_lhs = now, stamp_rhs = now + 3600u;
        uint32_t verify_uid = 0;
        EXPECT_EQ(
            0, tm.VerifyToken(token, ip, stamp_lhs, stamp_rhs, verify_uid));
        EXPECT_EQ(uid, verify_uid);
    }
}

