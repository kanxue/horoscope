#ifndef MPSERVER_VERIFY_TOKEN_PROCESSOR_H_
#define MPSERVER_VERIFY_TOKEN_PROCESSOR_H_

#include <string>
#include "horoscope/mpserver/base_processor.h"

class VerifyTokenProcessor : public BaseProcessor {
public:
    VerifyTokenProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output)
        : BaseProcessor(uri, input, output)
    {}

    virtual ~VerifyTokenProcessor() {}

public:
    void SetEchoStr(const std::string& echostr) {
        m_echostr = echostr;
    }

    virtual void Run() {
        m_output->assign(m_echostr);
    }

private:
    std::string m_echostr;
}; // class VerifyTokenProcessor

#endif // MPSERVER_VERIFY_TOKEN_PROCESSOR_H_

