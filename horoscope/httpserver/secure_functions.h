#ifndef HTTPSERVER_SECURE_FUNCTION_H_
#define HTTPSERVER_SECURE_FUNCTION_H_

#include <string>

class BasicSecureFunction {
public:
    explicit BasicSecureFunction(const std::string& key_data = "")
        : m_key_data(key_data)
    {}

    virtual ~BasicSecureFunction() {}

public:
    virtual bool Encode(
        const std::string& input,
        std::string* output) = 0;

    virtual bool Decode(
        const std::string& input,
        std::string* output) = 0;
}; // class BasicSecureFunction

class DefaultSecureFunction : pubic BasicSecureFunction {
public:
    explicit DefaultSecureFunction(const std::string& key_data = "")
        : BasicSecureFunction(key_data)
    {}

    virtual ~DefaultSecureFunction() {}

public:
    virtual bool Encode(
        const std::string& input,
        std::string* output);

    virtual bool Decode(
        const std::string& input,
        std::string* output);
}; // class DefaultSecureFunction

class AesSecureFunction : public BasicSecureFunction {
public:
    explicit AesSecureFunction(const std::string& key_data = "")
        : BasicSecureFunction(key_data)
    {}

    virtual ~AesSecureFunction() {}

public:
    virtual bool Encode(
        const std::string& input,
        std::string* output);

    virtual bool Decode(
        const std::string& input,
        std::string* output);
}; // class AesSecureFunction

class Base64SecureFunction : public BasicSecureFunction {
public:
    explicit Base64SecureFunction(const std::string& key_data = "")
        : BasicSecureFunction(key_data)
    {}

    virtual ~Base64SecureFunction() {}

public:
    virtual bool Encode(
        const std::string& input,
        std::string* output);

    virtual bool Decode(
        const std::string& input,
        std::string* output);
}; // class Base64SecureFunction

class Base64AndAesSecureFunction : public BasicSecureFunction {
public:
    explicit Base64AndAesSecureFunction(const std::string& key_data = "")
        : BasicSecureFunction(key_data)
    {}

    virtual ~Base64AndAesSecureFunction() {}

public:
    virtual bool Encode(
        const std::string& input,
        std::string* output);

    virtual bool Decode(
        const std::string& input,
        std::string* output);
}; // class Base64AndAesSecureFunction

#endif // HTTPSERVER_SECURE_FUNCTION_H_

