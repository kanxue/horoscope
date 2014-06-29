#include "horoscope/mpserver/verify_token_processor.h"

#include "common/base/string/algorithm.h"
#include "common/crypto/hash/sha1.h"

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(verify_token, "MY_VERIFY_TOKEN", "verify token");

void VerifyTokenProcessor::SetQuerys(
    const std::map<std::string, std::string>& querys)
{
    m_querys = querys;
}

void VerifyTokenProcessor::Run()
{
    std::string signature = m_querys["signature"];
    std::string echostr = m_querys["echostr"];
    std::vector<std::string> segments(3);
    segments[0] = FLAGS_verify_token;
    segments[1] = m_querys["timestamp"];
    segments[2] = m_querys["nonce"];
    std::sort(segments.begin(), segments.end());
    std::string join = JoinStrings(segments, "");
    std::string result = SHA1::HexDigest(join);

    if (result != signature) {
        LOG(ERROR)
            << "verify failed. join [" << join
            << "] result [" << result
            << "] signature [" << signature << "]";
        m_output->assign("verify failed.");
    } else {
        LOG(INFO)
            << "verify success. join [" << join
            << "] result [" << result
            << "] signature [" << signature << "]";
        m_output->assign("verify success.");
    }
}

