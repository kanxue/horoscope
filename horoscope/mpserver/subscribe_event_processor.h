#ifndef MPSERVER_SUBSCRIBE_PROCESSOR_H_
#define MPSERVER_SUBSCRIBE_PROCESSOR_H_

#include <string>

#include "horoscope/mpserver/base_processor.h"
#include "horoscope/mpserver/proto/event.pb.h"
#include "horoscope/mpserver/proto/message.pb.h"

class SubscribeEventProcessor : public BaseProcessor {
public:
    SubscribeEventProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output);

    virtual ~SubscribeEventProcessor();

public:
    virtual void Run();

private:
    void Process(mpserver::TextMessage* output_message);
    int UpdateUserInfo(const std::string& open_id);

private:
    mpserver::SubscribeEvent m_input_event;
}; // class SubscribeEventProcessor

#endif // MPSERVER_SUBSCRIBE_PROCESSOR_H_

