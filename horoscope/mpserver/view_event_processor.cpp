#include "view_event_processor.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/proto/xml_to_pb.h"

ViewEventProcessor::ViewEventProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
    : BaseProcessor(uri, input, output)
{}

ViewEventProcessor::~ViewEventProcessor()
{}

void ViewEventProcessor::Run()
{
    // parse inout.
    std::string error;
    if (!XmlToProtoMessage(m_input, &m_input_event, &error)) {
        LOG(ERROR)
            << " xml to view event failed. input [" << m_input
            << "] error [" << error << "]";
        return;
    }

    // TODO(boxerzhang) record view action.

    // just log it.
    LOG(INFO) << "[VIEW_EVENT] " << m_input_event.ShortDebugString();

    // serialize output.
    m_output->clear();
}

