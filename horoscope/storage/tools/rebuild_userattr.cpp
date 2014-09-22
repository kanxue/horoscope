
#include <fstream>
#include <iostream>
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

#include "common/base/string/string_number.h"
#include "horoscope/mpserver/proto/message.pb.h"
#include "horoscope/mpserver/proto/xml_to_pb.h"
#include "horoscope/storage/common_def.h"
#include "horoscope/storage/storage_redis_client.h"
#include "horoscope/storage/storage_mysql_client.h"

DEFINE_string(bind_log, "", "");

int Process(const std::string& xml, StorageRedisClient& redis_client, StorageMysqlClient& mysql_client);

int main(int argc, char** argv)
{
    FLAGS_alsologtostderr = true;
    google::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);

    StorageRedisClient redis_client;
    redis_client.ResetRedisClient();
    StorageMysqlClient mysql_client;

    std::ifstream ifs(FLAGS_bind_log.c_str());
    if (ifs.fail()) {
        std::cout << "open bind log failed. <"<< FLAGS_bind_log << ">\n";
        return -1;
    }

    std::string line, content;
    while (ifs >> line) {
        content.append(line);
    }

    size_t pos = 0;
    while (true) {
        pos = content.find("input", pos);
        if (pos == std::string::npos) {
            std::cout << "break1";
            break;
        }
        size_t start_pos = content.find("<xml>", pos);
        if (start_pos == std::string::npos) {
            std::cout << "break2" << std::endl;
            break;
        }
        size_t end_pos = content.find("</xml>", pos);
        if (end_pos == std::string::npos) {
            std::cout << "break3" << std::endl;
            break;
            
        }
        

        std::string xml_content = content.substr(start_pos, (end_pos - start_pos) + 6);
        Process(xml_content, redis_client, mysql_client);

        pos = end_pos;
    }

    return 0;
}


int Process(const std::string& xml_content, StorageRedisClient& redis_client, StorageMysqlClient& mysql_client)
{
    std::cout << xml_content << std::endl;
    
    std::string error;
    mpserver::TextMessage message;
    if (!XmlToProtoMessage(xml_content, &message, &error)) {
        std::cout << "error " << error << std::endl;
        return 0;
    }

    std::string content = GetGbkString(message.content());
    const std::string& user = message.fromusername();

    int horoscope_type = GetHoroscopeTypeByText(content);
    if (horoscope_type == HoroscopeType_UnknownHoroscope) {
        return 0;
    }

    horoscope::HoroscopeAttr horoscope_attr;
    mysql_client.GetHoroscopeAttr(horoscope_type, &horoscope_attr);

    horoscope::UserAttr userattr;
    userattr.set_horoscope_type(horoscope_type);
	
    std::string month = "";
    std::string day = "";
    if(GetMonthDay(content, month, day)){
        if(!month.empty() && !day.empty()) {
            if(month[0] == '0') month = month.substr(1);
            if(day[0] == '0') day = day.substr(1);
            int nMonth = 0;
            int nDay = 0;
            ParseNumber(month.c_str(), &nMonth);
            ParseNumber(day.c_str(), &nDay);
            userattr.set_birth_month(nMonth);
            userattr.set_birth_day(nDay);
        }
    } else {
        userattr.set_birth_month(0);
        userattr.set_birth_day(0);
    }

    {
        int mysql_ret = mysql_client.SetUserAttr(user, userattr);
        LOG(INFO) << "set " << user << " mysql_ret " << mysql_ret;
    }

    //ret = redis_client.MergeUserAttr(user, userattr);

    //if (ret != 0) {
    //    LOG(ERROR) << " set " << user << " failed. ret " << ret;
    //    return 0;
    //}

    //LOG(INFO)
    //    << __func__ << " done. user " << user << " type " << horoscope_type
    //    << " content [" << message.content() << "]";
    return 0;
}
