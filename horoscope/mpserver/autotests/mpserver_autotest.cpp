
#include "common/base/string/concat.h"
#include "common/encoding/charset_converter.h"
#include "common/net/http/http_downloader.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/gtest/gtest.h"

DEFINE_string(mp_address, "10.164.10.226:23456/mp_handler", "");
DEFINE_string(mp_args, "?signature=6df3ca0022bfe41cd2ca525a6249c1e108e411d6&timestamp=1414812935&nonce=866658890", "");

class MpServerAutoTest : public testing::Test {
protected:
    virtual void SetUp();

    bool Post(const std::string& request, std::string* response);

    const char* GetLastErrorMsg();

protected:
    std::string                          m_url;
    common::HttpDownloader::ErrorType    m_last_error;
}; // class MpServerAutoTest

void MpServerAutoTest::SetUp()
{
    StringConcatTo(&m_url, "http://", FLAGS_mp_address, FLAGS_mp_args);
    m_last_error = common::HttpDownloader::SUCCESS;
}

bool MpServerAutoTest::Post(
    const std::string& request,
    std::string* response)
{
    std::string utf8_request;
    if (!ConvertGbkToUtf8(request, &utf8_request)) {
        utf8_request = request;
    }
    common::HttpDownloader loader;
    m_last_error = common::HttpDownloader::SUCCESS;
    HttpResponse http_response;
    bool ret = loader.Post(m_url, utf8_request, &http_response, &m_last_error);
    if (ret) {
        if (!ConvertUtf8ToGbk(http_response.http_body(), response)) {
            response->assign(http_response.http_body());
        }
    }
    return ret;
}

const char* MpServerAutoTest::GetLastErrorMsg()
{
    return common::HttpDownloader::GetErrorMessage(m_last_error);
}

TEST_F(MpServerAutoTest, TextMessage_BindTest) 
{
    std::string request("<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName> <FromUserName><![CDATA[TESTOPENID_TextMessage_BindTest]]></FromUserName> <CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType> <Content><![CDATA[绑定天蝎]]></Content> <MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座绑定成功啦");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos);
}

