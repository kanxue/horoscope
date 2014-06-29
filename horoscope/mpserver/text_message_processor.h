#ifndef MPSERVER_TEXT_MESSAGE_PROCESSOR_H_
#define MPSERVER_TEXT_MESSAGE_PROCESSOR_H_

#include <string>

#include "horoscope/mpserver/base_processor.h"
#include "horoscope/mpserver/proto/message.pb.h"

class TextMessageProcessor : public BaseProcessor {
public:
    TextMessageProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output);

    virtual ~TextMessageProcessor();

public:
    virtual void Run();

private:
    void Process(mpserver::TextMessage* output_message);

private:
    mpserver::TextMessage m_input_message;
}; // class TextMessageProcessor

#endif // MPSERVER_TEXT_MESSAGE_PROCESSOR_H_

