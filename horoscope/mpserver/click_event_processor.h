#ifndef MPSERVER_CLICK_PROCESSOR_H_
#define MPSERVER_CLICK_PROCESSOR_H_

#include <string>

#include "horoscope/mpserver/base_processor.h"
#include "horoscope/mpserver/proto/event.pb.h"
#include "horoscope/mpserver/proto/message.pb.h"

class ClickEventProcessor : public BaseProcessor {
public:
    ClickEventProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output);

    virtual ~ClickEventProcessor();

public:
    virtual void Run();

private:
    void Process(
        mpserver::TextMessage* text_message,
        bool& use_news_message,
        mpserver::NewsMessage* news_message);

    void Process(mpserver::NewsMessage* output_message);

private:
    mpserver::ClickEvent m_input_event;
}; // class ClickEventProcessor

#endif // MPSERVER_CLICK_PROCESSOR_H_

