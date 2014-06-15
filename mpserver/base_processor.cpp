#include "horoscope/mpserver/base_processor.h"

#include "common/base/string/algorithm.h"
#include "common/web/url.h"
#include "thirdparty/glog/logging.h"

#include "horoscope/mpserver/verify_token_processor.h"

BaseProcessor* GenerateProcessor(
    const std::string& uri,
    const std::string& input,
    std::string* output)
{
    web::url::Url url_obj(uri.c_str(), uri.size());
    std::map<std::string, std::string> querys;
    url_obj.GetQuerys(querys);

    if ((querys.find("signature") != querys.end())
        && (querys.find("timestamp") != querys.end())
        && (querys.find("nonce") != querys.end())
        && (querys.find("echostr") != querys.end())) {
        VerifyTokenProcessor* real_processor = new VerifyTokenProcessor(
            uri, input, output);
        real_processor->SetQuerys(querys);

        return reinterpret_cast<BaseProcessor*>(real_processor);
    }

    LOG(ERROR)
        << "NOT found processor. uri [" << uri
        << "] input [" << ReplaceAll(input, "\n", "") << "]";

    return NULL;
}

