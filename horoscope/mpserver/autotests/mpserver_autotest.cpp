
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

// ��ע����
// Ԥ�ڣ����Ͱ���ʾ��
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
    std::string some_expect("���Ѿ��ɹ��������Ů�׵�Ϊ������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// ����������
// Ԥ�ڣ��ɹ�
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
        "<Content><![CDATA[����Ы]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы���󶨳ɹ���");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �쳣������
// Ԥ�ڣ�ʧ��
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
        "<Content><![CDATA[����]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("����������ָ��");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�1.1 ��������
// Ԥ�ڣ����ؽ�������
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
        std::string some_expect("2015�������");
        EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
    }
    {
        std::string some_expect("<MsgType><![CDATA[news]]></MsgType>");
        EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
    }
}

// �˵�1.2 ��������
// Ԥ�ڣ����ؽ�������
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
    std::string some_expect("��Ы���������ƣ�");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�1.3 ��������
// Ԥ�ڣ�������������
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
    std::string some_expect("��Ы���������ƣ�");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�1.4 ��������
// Ԥ�ڣ����ر�������
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
    std::string some_expect("��Ы���������ƣ�");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�1.5 ��������
// Ԥ�ڣ�������ʾ��
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
    std::string some_expect("��������Ҫ��ѯ���������߶�Ӧ������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�2.1 ��������ɫ
// Ԥ�ڣ�
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
    std::string some_expect("ɫ");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�2.2 ���տ�����Ʒ
// Ԥ�ڣ�
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
    std::string some_expect("ʯ");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�2.3 ������������
// Ԥ�ڣ�
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
    std::string some_expect("��Ы��������:");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�2.4 ������������
// Ԥ�ڣ�
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
    std::string some_expect("��Ы���������֣�");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�3.1 �ҵ�����
// Ԥ�ڣ�
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
    std::string some_expect("��Ы��\n\n�����޸������������ظ���ָ�");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// �˵�3.2 �ҵĿ���
// Ԥ�ڣ�
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

// �˵�3.3 ������ѯ
// Ԥ�ڣ�
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
    std::string some_expect("��Ы�� : 10��24�� - 11��22��");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text ����
// Ԥ�ڣ�
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
        "<Content><![CDATA[��Ы]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы����������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text ��������
// Ԥ�ڣ�
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
        "<Content><![CDATA[10��24��]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы����������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text ����+����
// Ԥ�ڣ�
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
        "<Content><![CDATA[���� ��Ы]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы����������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

// Text ����+����
// Ԥ�ڣ�
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
        "<Content><![CDATA[��Ы����]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы����������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}


// Text ����+����
// Ԥ�ڣ�
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
        "<Content><![CDATA[��Ы����]]></Content>"
        "<MsgId>6076042443754928685</MsgId> </xml>");
    std::string some_expect("��Ы����������");
    std::string response;
    EXPECT_TRUE(Post(request, &response)) << GetLastErrorMsg();
    EXPECT_TRUE(response.find(some_expect) != std::string::npos) << response;
}

