#ifndef MPSERVER_BASE_PROCESSOR_H_
#define MPSERVER_BASE_PROCESSOR_H_

#include <map>
#include <string>

class BaseProcessor {
public:
    BaseProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output)
        : m_uri(uri),
        m_input(input),
        m_output(output)
        {}

    virtual ~BaseProcessor() {}

public:
    virtual void Run() = 0;

protected:
    const std::string&  m_uri;
    const std::string&  m_input;
    std::string*        m_output;
}; // class BaseProcessor

// Factory function.
BaseProcessor* GenerateProcessor(
    const std::string& uri,
    const std::map<std::string, std::string>& querys,
    const std::string& input,
    std::string* output);

#endif // MPSERVER_BASE_PROCESSOR_H_

