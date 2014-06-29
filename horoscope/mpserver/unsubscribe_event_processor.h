#ifndef MPSERVER_UNSUBSCRIBE_PROCESSOR_H_
#define MPSERVER_UNSUBSCRIBE_PROCESSOR_H_

#include <string>

#include "horoscope/mpserver/base_processor.h"
#include "horoscope/mpserver/proto/event.pb.h"
#include "horoscope/mpserver/proto/message.pb.h"

class UnSubscribeEventProcessor : public BaseProcessor {
public:
    UnSubscribeEventProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output);

    virtual ~UnSubscribeEventProcessor();

public:
    virtual void Run();

private:
    void Process(mpserver::TextMessage* output_message);

private:
    mpserver::UnSubscribeEvent m_input_event;
}; // class UnSubscribeEventProcessor

#endif // MPSERVER_UNSUBSCRIBE_PROCESSOR_H_

