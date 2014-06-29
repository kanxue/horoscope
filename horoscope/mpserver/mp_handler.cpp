#include "horoscope/mpserver/mp_handler.h"

#include <algorithm>
#include "common/base/scoped_ptr.h"
#include "common/base/string/algorithm.h"
#include "common/crypto/hash/sha1.h"
#include "common/web/url.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/tinyxml/tinyxml.h"

#include "horoscope/mpserver/base_processor.h"

DEFINE_string(verify_token, "MY_VERIFY_TOKEN", "verify token");

MpHandler::MpHandler(HttpServer* server)
    : BaseHandler("/mp_handler", server)
{}

MpHandler::~MpHandler()
{}

void MpHandler::Process(
    const std::string& uri,
    const std::string& input,
    std::string* output)
{
    // check signature firstly
    // make sure this request came from wx-server.
    web::url::Url url_obj(uri.c_str(), uri.size());
    std::map<std::string, std::string> querys;
    url_obj.GetQuerys(querys);
    std::vector<std::string> segments(3);
    segments[0] = FLAGS_verify_token;
    segments[1] = querys["timestamp"];
    segments[2] = querys["nonce"];
    std::sort(segments.begin(), segments.end());
    std::string join = JoinStrings(segments, "");
    std::string result = SHA1::HexDigest(join);
    std::string signature = querys["signature"];
    if (result != signature) {
        LOG(ERROR)
            << "verify failed. join [" << join
            << "] result [" << result
            << "] signature [" << signature << "]";
        output->assign("verify failed.");
        return;
    }

    // generate processor by request type.
    // processor will be deleted by descontruct.
    scoped_ptr<BaseProcessor> auto_delete_processor(
        GenerateProcessor(uri, querys, input, output));

    if (auto_delete_processor.get()) {
        auto_delete_processor->Run();
    }
}
