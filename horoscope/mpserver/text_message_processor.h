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
    void ProcessText(mpserver::TextMessage* output_message);

    void ProcessChineseYangYear(
        mpserver::NewsMessage* output_message,
        bool& use_error_message, mpserver::TextMessage* error_message);

    void ProcessYear(
        mpserver::NewsMessage* output_message,
        bool& use_error_message, mpserver::TextMessage* error_message);
    
    void ProcessDailyReport(
        mpserver::NewsMessage* output_message,
        bool& use_error_message,mpserver::TextMessage* error_message);
    
    int UpdateUserInfo(const std::string& open_id);

private:
    mpserver::TextMessage m_input_message;
}; // class TextMessageProcessor

#endif // MPSERVER_TEXT_MESSAGE_PROCESSOR_H_

