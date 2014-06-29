#include "horoscope/mpserver/base_processor.h"

#include "common/base/string/algorithm.h"
#include "common/web/url.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/click_event_processor.h"
#include "horoscope/mpserver/subscribe_event_processor.h"
#include "horoscope/mpserver/text_message_processor.h"
#include "horoscope/mpserver/unsubscribe_event_processor.h"
#include "horoscope/mpserver/verify_token_processor.h"
#include "horoscope/mpserver/view_event_processor.h"

BaseProcessor* GenerateProcessor(
    const std::string& uri,
    const std::map<std::string, std::string>& querys,
    const std::string& input,
    std::string* output)
{
    std::map<std::string, std::string>::const_iterator it
        = querys.find("echostr");
    if (it != querys.end()) {
        VerifyTokenProcessor* real_processor = new VerifyTokenProcessor(uri, input, output);
        real_processor->SetEchoStr(it->second);

        return reinterpret_cast<BaseProcessor*>(real_processor);
    }

    if (input.find("<MsgType><![CDATA[text]]></MsgType>") != std::string::npos) {
        return new TextMessageProcessor(uri, input, output);
    } else if (input.find("<MsgType><![CDATA[event]]></MsgType>") != std::string::npos) {
        if (input.find("<Event><![CDATA[CLICK]]></Event>") != std::string::npos) {
            return new ClickEventProcessor(uri, input, output);
        } else if (input.find("<Event><![CDATA[VIEW]]></Event>") != std::string::npos) {
            return new ViewEventProcessor(uri, input, output);
        } else if (input.find("<Event><![CDATA[subscribe]]></Event>") != std::string::npos) {
            return new SubscribeEventProcessor(uri, input, output);
        } else if (input.find("<Event><![CDATA[unsubscribe]]></Event>") != std::string::npos) {
            return new UnSubscribeEventProcessor(uri, input, output);
        } else {
            LOG(ERROR) << "UNKOWN event type. input [" << input << "]";
        }
    }

    LOG(ERROR)
        << "NOT found processor. uri [" << uri
        << "] input [" << ReplaceAll(input, "\n", "") << "]";

    return NULL;
}

