#include "unsubscribe_event_processor.h"

#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/proto/pb_to_xml.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/storage/storage_mysql_client.h"
#include "horoscope/storage/storage_redis_client.h"

UnSubscribeEventProcessor::UnSubscribeEventProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
    : BaseProcessor(uri, input, output)
{}

UnSubscribeEventProcessor::~UnSubscribeEventProcessor()
{}

void UnSubscribeEventProcessor::Run()
{
    // parse inout.
    std::string error;
    if (!XmlToProtoMessage(m_input, &m_input_event, &error)) {
        LOG(ERROR)
            << " xml to view event failed. input [" << m_input
            << "] error [" << error << "]";
        return;
    }

    // TODO(kanxue) record unsubscribe action.

    // just log it.
    LOG(INFO) << "[UNSUBSCRIBE_EVENT] " << m_input_event.ShortDebugString();

    // delete user attr.
    //StorageRedisClient redis_client;
    //StorageMysqlClient& mysql_client = StorageMysqlClientSingleton::Instance();
    StorageMysqlClient mysql_client;
    int ret = mysql_client.DelUserAttr(m_input_event.fromusername());
    LOG(INFO)
        << "delete user attr done. user " << m_input_event.fromusername()
        << " ret " << ret;

    // i'm too sad to output anything.
    m_output->clear();
}

