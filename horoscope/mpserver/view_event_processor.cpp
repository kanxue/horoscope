#include "view_event_processor.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/storage_mysql_client.h"

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

    const std::string& openid = m_input_event.fromusername();
    std::string event_key = m_input_event.eventkey();
    if (event_key.empty()) {
        event_key.assign("VIEW_EVENT");
    }
    StorageMysqlClient mysql_client;
    mysql_client.AddUserClickAction(openid, event_key);

    // just log it.
    LOG(INFO) << "[VIEW_EVENT] " << m_input_event.ShortDebugString();

    // serialize output.
    m_output->clear();
}

