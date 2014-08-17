
#include "common_memory_cache.h"

#include "common/base/stl_container_deleter.h"

#include "thirdparty/glog/logging.h"
#include "thirdparty/protobuf/message.h"

CommonMemoryCache::CommonMemoryCache(const std::string& cache_name)
    : m_name(cache_name)
{}

CommonMemoryCache::~CommonMemoryCache()
{}

void CommonMemoryCache::Set(
    const std::string& key,
    const google::protobuf::Message& message)
{
    std::string buffer;
    if (!message.SerializeToString(&buffer)) {
        LOG(ERROR)
            << "serialize to string failed. message ["
            << message.ShortDebugString() << "]";
        return;
    }

    RWLock::WriterLocker locker(&m_rw_lock);
    m_cache_map[key] = buffer;
}

bool CommonMemoryCache::Get(
    const std::string& key,
    google::protobuf::Message* message)
{
    std::string buffer;
    {
        RWLock::ReaderLocker locker(&m_rw_lock);
        CacheMap::const_iterator it = m_cache_map.find(key);
        if (it != m_cache_map.end()) {
            buffer = it->second;
        }
    }

    if (buffer.empty()) {
        LOG(ERROR)
            << "miss cache. name [" << m_name
            << "] key [" << key << "]";

        return false;
    }

    if (!message->ParseFromString(buffer)) {
        LOG(ERROR)
            << "bad record in cache. buffer_size " << buffer.size();

        RWLock::WriterLocker locker(&m_rw_lock);
        m_cache_map.erase(key);

        return false;
    }

    LOG(INFO)
        << "hit cache. name [" << m_name
        << "] key [" << key << "] value_size " << buffer.size();

    return true;
}

bool CommonMemoryCache::Has(const std::string& key)
{
    RWLock::WriterLocker locker(&m_rw_lock);
    CacheMap::const_iterator it = m_cache_map.find(key);
    return (it != m_cache_map.end());
}

void CommonMemoryCache::Del(const std::string& key)
{
    RWLock::WriterLocker locker(&m_rw_lock);
    m_cache_map.erase(key);
}

size_t CommonMemoryCache::Size() const
{
    RWLock::ReaderLocker locker(&m_rw_lock);
    return m_cache_map.size();
}

const std::string& CommonMemoryCache::Name() const
{
    RWLock::ReaderLocker locker(&m_rw_lock);
    return m_name;
}

CommonMemoryCacheGroup::CommonMemoryCacheGroup()
{}

CommonMemoryCacheGroup::~CommonMemoryCacheGroup()
{}

void CommonMemoryCacheGroup::CreateCache(const std::string& cache_name)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(cache_name);
    if (it != m_cache_map.end()) {
        delete it->second;
        m_cache_map.erase(it);
    }
    m_cache_map.insert(
        CommonMemoryCacheItem(
            cache_name, new CommonMemoryCache(cache_name)));
}

void CommonMemoryCacheGroup::ClearCache(const std::string& cache_name)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(cache_name);
    if (it != m_cache_map.end()) {
        delete it->second;
        m_cache_map.erase(it);
    }
}

void CommonMemoryCacheGroup::ClearAll()
{
    Mutex::Locker locker(&m_mutex);
    STLDeleteValues(&m_cache_map);
}

void CommonMemoryCacheGroup::Set(
    const std::string& name,
    const std::string& key,
    const google::protobuf::Message& message)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(name);
    if ((it != m_cache_map.end()) && it->second) {
        it->second->Set(key, message);
    }
}

bool CommonMemoryCacheGroup::Get(
    const std::string& name,
    const std::string& key,
    google::protobuf::Message* message)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(name);
    return (it != m_cache_map.end()) && it->second->Get(key, message);
}

bool CommonMemoryCacheGroup::Has(
    const std::string& name,
    const std::string& key)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(name);
    return (it != m_cache_map.end()) && it->second->Has(key);
}

void CommonMemoryCacheGroup::Del(
    const std::string& name,
    const std::string& key)
{
    Mutex::Locker locker(&m_mutex);
    CommonMemoryCacheMap::iterator it = m_cache_map.find(name);
    if ((it != m_cache_map.end()) && it->second) {
        it->second->Del(key);
    }
}

