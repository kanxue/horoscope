
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
    std::string                          m_test_openid;
    std::string                          m_url;
    common::HttpDownloader::ErrorType    m_last_error;
}; // class MpServerAutoTest

void MpServerAutoTest::SetUp()
{
    m_test_openid.assign("TEST_OPENID_FOR_AUTOTEST");
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

// 关注请求
// 预期：发送绑定提示语
TEST_F(MpServerAutoTest, SubscribeTest)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName><CreateTime>1414826147</CreateTime>"
        "<MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[subscribe]]></Event>"
        "<EventKey><![CDATA[]]></EventKey> </xml>");
    std::string some_expect("你已经成功添加闹闹女巫店为好友啦");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 正常绑定请求
// 预期：成功
TEST_F(MpServerAutoTest, TextMessage_BindTest1)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[绑定天蝎]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座绑定成功啦");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 异常绑定请求
// 预期：失败
TEST_F(MpServerAutoTest, TextMessage_BindTest2)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[绑定天]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("不理解输入的指令");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单1.1 今年运势
// 预期：返回今年运势
TEST_F(MpServerAutoTest, ClickMessage_YearFortune)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[FORTUNE_HOROSCOPE_YEAR]]></EventKey></xml>");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    {
        std::string some_expect("2015年度运势");
        EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
    }
    {
        std::string some_expect("<MsgType><![CDATA[news]]></MsgType>");
        EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
    }
}

// 菜单1.2 今日运势
// 预期：返回今日运势
TEST_F(MpServerAutoTest, ClickMessage_TodayFortune)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[FORTUNE_HOROSCOPE_TODAY]]></EventKey></xml>");
    std::string some_expect("天蝎座今日运势：");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单1.3 明日运势
// 预期：返回明日运势
TEST_F(MpServerAutoTest, ClickMessage_TomorrowFortune)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[FORTUNE_HOROSCOPE_TOMORROW]]></EventKey></xml>");
    std::string some_expect("天蝎座明日运势：");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单1.4 本周运势
// 预期：返回本周运势
TEST_F(MpServerAutoTest, ClickMessage_WeekFortune)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[FORTUNE_HOROSCOPE_WEEK]]></EventKey></xml>");
    std::string some_expect("天蝎座本周运势：");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单1.5 其他运势
// 预期：返回提示语
TEST_F(MpServerAutoTest, ClickMessage_OtherFortune)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[FORTUNE_HOROSCOPE_OTHERS]]></EventKey></xml>");
    std::string some_expect("请输入你要查询的星座或者对应的日期");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单2.1 今日幸运色
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_TodayLuckyColor)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[TODAY_LUCKY_COLOR]]></EventKey></xml>");
    std::string some_expect("色");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单2.2 今日开运物品
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_TodayLuckyGoods)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[TODAY_LUCKY_GOODS]]></EventKey></xml>");
    std::string some_expect("石");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单2.3 今日契合星座
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_TodayLuckyAstro)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[TODAY_LUCKY_ASTRO]]></EventKey></xml>");
    std::string some_expect("天蝎座契合星:");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单2.4 今日幸运数字
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_TodayLuckyNumber)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[TODAY_LUCKY_NUMBER]]></EventKey></xml>");
    std::string some_expect("天蝎座幸运数字：");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单3.1 我的星座
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_UserDetail)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[ME_HOROSCOPE_DETAIL]]></EventKey></xml>");
    std::string some_expect("天蝎座\n\n若想修改你的星座，请回复绑定指令。");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// 菜单3.2 我的开运
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_ShopView)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[VIEW]]></Event>"
        "<EventKey><![CDATA[http://wd.koudai.com/vshop/1/H5/index.html?userid=80852]]></EventKey></xml>");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.empty()) << response;
}

// 菜单3.3 星座查询
// 预期：
TEST_F(MpServerAutoTest, ClickMessage_AllAstroDetail)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[event]]></MsgType>"
        "<Event><![CDATA[CLICK]]></Event>"
        "<EventKey><![CDATA[PLUGIN_HOROSCOPE_DETAIL]]></EventKey></xml>");
    std::string some_expect("天蝎座 : 10月24日 - 11月22日");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text 星座
// 预期：
TEST_F(MpServerAutoTest, TextMessage_Astro)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[天蝎]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座今日运势");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text 星座日期
// 预期：
TEST_F(MpServerAutoTest, TextMessage_Astro_Date)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[10月24日]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座今日运势");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text 今日+星座
// 预期：
TEST_F(MpServerAutoTest, TextMessage_Astro_Today)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[今日 天蝎]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座今日运势");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text 星座+明天
// 预期：
TEST_F(MpServerAutoTest, TextMessage_Astro_Tomorrow)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[天蝎明天]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座明日运势");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}


// Text 星座+本周
// 预期：
TEST_F(MpServerAutoTest, TextMessage_Astro_Week)
{
    std::string request;
    StringConcatTo(
        &request,
        "<xml><ToUserName><![CDATA[gh_67e2f63ff038]]></ToUserName>"
        "<FromUserName><![CDATA[",
        m_test_openid,
        "]]></FromUserName>"
        "<CreateTime>1414688873</CreateTime> <MsgType><![CDATA[text]]></MsgType>"
        "<Content><![CDATA[天蝎本周]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("天蝎座本周运势");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

