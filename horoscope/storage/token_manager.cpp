
#include "horoscope/storage/token_manager.h"

#include "common/crypto/random/true_random.h"

#include "common/compress/checksum/checksum.h"
#include "common/encoding/base64.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/storage/hesapierrno.h"

TokenManager::TokenManager()
{
    TrueRandom tr;
    m_rsa.InitRandomStruct(tr.NextUInt32());
    int32_t ret = m_rsa.GenerateKeyPair(1023);
    LOG(INFO) << "GenerateKeyPair done. ret " << ret;
}

TokenManager::~TokenManager()
{}

void TokenManager::GenerateToken(
    const uint32_t uid,
    const std::string& ip,
    const uint32_t now,
    std::string* token)
{
    horoscope::TokenAttr attr;
    attr.set_uid(uid);
    attr.set_ip(ip);
    attr.set_created(now);
    generate_nonce(attr.mutable_nonce());
    attr.set_crc(generate_crc(attr));

    std::string buffer;
    attr.SerializeToString(&buffer);

    Generate(buffer, token);
}

int TokenManager::VerifyToken(
    const std::string& token,
    const std::string& ip,
    const uint32_t stamp_lhs,
    const uint32_t stamp_rhs,
    uint32_t& uid)
{
    std::string buffer;
    if (!Verify(token, &buffer)) {
        return API_INVALID_TOKEN;
    }

    horoscope::TokenAttr attr;
    if (!attr.ParseFromString(buffer)) {
        LOG(ERROR) << __func__ << "ParseFromString failed.";
        return API_INVALID_TOKEN;
    }

    uint32_t crc = generate_crc(attr);
    if (attr.crc() != crc) {
        LOG(ERROR)
            << __func__ << " crc NOT match. token.crc " << attr.crc()
            << " verify_crc " << crc;
        return API_INVALID_TOKEN;
    }

    if (attr.ip() != ip) {
        LOG(ERROR)
            << __func__ << " ip NOT match. token.ip '" << attr.ip()
            << "' verify_ip '" << ip << "'";
        return API_TOKEN_DIFF_CLIENT_IP;
    }

    if ((attr.created() < stamp_lhs) || (attr.created() > stamp_rhs)) {
        LOG(ERROR)
            << __func__ << "stamp NOT match. token.stamp " << attr.created()
            << " verify range [" << stamp_lhs << " " << stamp_rhs << "]";
        return API_TOKEN_EXPIRED;
    }

    uid = attr.uid();

    LOG(INFO)
        << __func__ << " success, token '" << token
        << "' attr '" << attr.ShortDebugString() << "'";

    return 0;
}

void TokenManager::Generate(const std::string& buffer, std::string* token)
{
    std::string rsa_buffer;
    rsa_buffer.resize(buffer.size() * 4u + 1024u);
    uint32_t real_token_size = 0;
    int32_t ret = m_rsa.PublicEncrypt(
        (unsigned char*)(buffer.data()), buffer.size(),
        (unsigned char*)(rsa_buffer.data()), &real_token_size);
    rsa_buffer.resize(real_token_size);
    LOG(INFO) << "PublicEncrypt done. ret " << ret;

    // base64
    if (!Base64::RfcWebSafeEncode(rsa_buffer, token)) {
        LOG(ERROR)
            << __func__ << " base64.encode failed.";
        return;
    }
}

bool TokenManager::Verify(const std::string& token, std::string* buffer)
{
    // base64
    std::string rsa_str;
    if (!Base64::RfcWebSafeDecode(token, &rsa_str)) {
        LOG(ERROR)
            << __func__ << "base64.decode failed.";
        return false;
    }

    buffer->resize(token.size() * 4u + 1024u);
    uint32_t real_buffer_size = 0u;
    int32_t ret = m_rsa.PrivateDecrypt(
        (unsigned char*)(rsa_str.data()), rsa_str.size(),
        (unsigned char*)(buffer->data()), &real_buffer_size);
    buffer->resize(real_buffer_size);
    LOG(INFO) << "PrivateDecrypt done. ret " << ret;

    return true;
}

void TokenManager::generate_nonce(std::string* nonce)
{
    nonce->clear();

    TrueRandom tr;
    for (int i = 0; i < 50; ++i) {
        uint32_t idx = tr.NextUInt32(52u);
        if (idx < 26u) {
            nonce->push_back(idx + 'a');
        } else {
            nonce->push_back(idx - 26u + 'A');
        }
    } ///< for
}

uint32_t TokenManager::generate_crc(const horoscope::TokenAttr& attr)
{
    uint32_t tmp = attr.uid();
    uint32_t crc = common::crc32(&tmp, sizeof(tmp));
    crc = common::crc32_update(crc, attr.ip().data(), attr.ip().size());
    tmp = attr.created();
    crc = common::crc32_update(crc, &tmp, sizeof(tmp));
    crc = common::crc32_update(crc, attr.nonce().data(), attr.nonce().size());
    return crc;
}

