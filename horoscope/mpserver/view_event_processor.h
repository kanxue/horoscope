#ifndef MPSERVER_VIEW_PROCESSOR_H_
#define MPSERVER_VIEW_PROCESSOR_H_

#include <string>

#include "horoscope/mpserver/base_processor.h"
#include "horoscope/mpserver/proto/event.pb.h"

class ViewEventProcessor : public BaseProcessor {
public:
    ViewEventProcessor(
        const std::string& uri,
        const std::string& input,
        std::string* output);

    virtual ~ViewEventProcessor();

public:
    virtual void Run();

private:
    mpserver::ViewEvent m_input_event;
}; // class ViewEventProcessor

#endif // MPSERVER_VIEW_PROCESSOR_H_

