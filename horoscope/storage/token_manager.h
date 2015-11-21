
#ifndef STORAGE_TOKEN_MANAGER_H_
#define STORAGE_TOKEN_MANAGER_H_

#include <string>

#include "common/base/inttypes.h"
#include "common/base/singleton.h"
#include "common/crypto/rsa2/rsa2.h"
#include "horoscope/storage/storage.pb.h"

class TokenManager {
public:
    TokenManager();
    ~TokenManager();

public:
    void GenerateToken(
        const uint32_t uid,
        const std::string& ip,
        const uint32_t now,
        std::string* token);

    int VerifyToken(
        const std::string& token,
        const std::string& ip,
        const uint32_t stamp_lhs,
        const uint32_t stamp_rhs,
        uint32_t& uid);

private:
    void Generate(const std::string& buffer, std::string* token);

    bool Verify(const std::string& token, std::string* buffer);

private:
    void generate_nonce(std::string* nonce);

    uint32_t generate_crc(const horoscope::TokenAttr& attr);

private:
    CRSACrypt m_rsa;
}; // class TokenManager

typedef Singleton<TokenManager> TokenManagerSingleton;

#endif // STORAGE_TOKEN_MANAGER_H_

