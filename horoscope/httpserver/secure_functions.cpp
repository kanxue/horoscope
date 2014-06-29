#include "horoscope/httpserver/secure_functions.h"

#include "common/crypto/symmetric/aes.h"
#include "common/encoding/base64.h"

bool DefaultSecureFunction::Encode(
    const std::string& input,
    std::string* output)
{
    output->assign(input);
    return true;
}

bool DefaultSecureFunction::Decode(
    const std::string& input,
    std::string* output)
{
    output->assign(input);
    return true;
}

bool AesSecureFunction::Encode(
    const std::string& input,
    std::string* output)
{
    common::Aes256 aes(m_key_data);
    return aes.Encode(input, output);
}

bool AesSecureFunction::Decode(
    const std::string& input,
    std::string* output)
{
    common::Aes256 aes(m_key_data);
    return aes.Decode(input, output);
}

bool Base64SecureFunction::Encode(
    const std::string& input,
    std::string* output)
{
    return Base64::Encode(input, output);
}

bool Base64SecureFunction::Decode(
    const std::string& input,
    std::string* output)
{
    return Base64::Decode(input, output);
}

bool Base64AndAesSecureFunction::Encode(
    const std::string& input,
    std::string* output)
{
    std::string buffer;
    common::Aes256 aes(m_key_data);

    return aes.Encode(input, &buffer)
        && Base64::Encode(buffer, output);
}

bool Base64AndAesSecureFunction::Decode(
    const std::string& input,
    std::string* output)
{
    std::string buffer;
    common::Aes256 aes(m_key_data);

    return Base64::Decode(input, &buffer)
        && aes.Decode(buffer, output);
}

