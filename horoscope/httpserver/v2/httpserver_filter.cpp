
#include "httpserver_filter.h"

#include "common/base/string/algorithm.h"

#include "common/base/scoped_ptr.h"
#include "common/crypto/hash/crc.h"
//#include "common/file/file.h"
#include "common/system/io/file.h"
#include "common/system/io/file_utility.h"
#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"

DEFINE_string(httpserver_filter_config, "/home/public/httpserver/etc/filter.conf", "");

HttpServerFilter::HttpServerFilter()
    : m_last_load_time(0)
{}

HttpServerFilter::~HttpServerFilter()
{}

HttpServerFilter* HttpServerFilter::GetDefault()
{
    static HttpServerFilter filter;
    return &filter;
}

bool HttpServerFilter::EasyCheckPass(const std::string& ip)
{
    HttpServerFilter* filter = HttpServerFilter::GetDefault();
    filter->LoadIfModify(FLAGS_httpserver_filter_config);
    return filter->CheckPass(ip);
}

void HttpServerFilter::LoadIfModify(const std::string& config_file)
{
    // uint32_t digest = 0, error_code = 0;
    // if (!common::File::GetDigest(config_file, &digest, &error_code)) {
    //     LOG(ERROR)
    //         << "call GetDigest failed. file [" << config_file
    //         << "] error_code " << error_code;
    //     return;
    // }

    uint32_t modify_time = io::file::GetLastModifyTime(config_file);
    bool need_reload = false;
    {
        RWLock::ReaderLocker locker(&m_rw_lock);
        need_reload = (m_last_load_time < modify_time);
    }

    if (need_reload) {
        Load(config_file, modify_time);
    }
}

bool HttpServerFilter::CheckPass(const std::string& ip)
{
    //暂时关闭ip过滤
    return true;

	std::string ip_seg;
	GetIpSegement(ip, ip_seg);
	
    RWLock::ReaderLocker locker(&m_rw_lock);
    return (m_white_ip_set.find(ip) != m_white_ip_set.end() || m_white_ip_set.find(ip_seg) != m_white_ip_set.end());
}

void HttpServerFilter::GetIpSegement(const std::string& ip, std::string& ip_seg)
{
	std::vector<std::string> str_vector;
    const char* delim = ".";
    SplitStringByAnyOf(ip, delim, &str_vector);
	
	if(str_vector.size() != 4)
		return;
		
	for(int i = 0; i < 3; i++)
    {
		ip_seg.append(str_vector[i]);
		ip_seg.append(".");
	}
	ip_seg.append("*");
	LOG(INFO)<<"ip_seg: " << ip_seg;
}

void HttpServerFilter::Load(const std::string& config_file, uint32_t modify_time)
{
    RWLock::WriterLocker locker(&m_rw_lock);
    // scoped_ptr<common::File> file(
    //     common::File::Open(config_file, common::File::kOpenModeRead));
    // if (!file) {
    //     LOG(ERROR)
    //         << "open config file failed. file [" << config_file << "]";
    //     return;
    // }

    // m_white_ip_set.clear();

    // uint32_t lines_num = 0;
    // std::string line;
    // while (file->ReadLine(&line) > 0) {
    //     if (!line.empty()) {
    //         m_white_ip_set.insert(line);
    //         ++lines_num;
    //     }
    // } ///< while

    m_white_ip_set.clear();
    if (!FileUtility::Load2Set(m_white_ip_set, config_file.c_str())) {
        LOG(ERROR)
            << "call Load2Set failed. config_file [" << config_file
            << "]";
        return;
    }

    LOG(INFO)
        << "load [" << config_file
        << "] done. set_size " << m_white_ip_set.size()
        << " modify_time " << modify_time;
    m_last_load_time = modify_time;
}

