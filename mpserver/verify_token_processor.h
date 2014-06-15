#ifndef MPSERVER_VERIFY_TOKEN_PROCESSOR_H_
#define MPSERVER_VERIFY_TOKEN_PROCESSOR_H_

#include <map>
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
    void SetQuerys(const std::map<std::string, std::string>& querys);

    virtual void Run();

private:
    std::map<std::string, std::string> m_querys;
}; // class VerifyTokenProcessor

#endif // MPSERVER_VERIFY_TOKEN_PROCESSOR_H_

