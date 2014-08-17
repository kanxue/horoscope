#ifndef STORAGE_COMMON_MEMORY_CACHE_H_
#define STORAGE_COMMON_MEMORY_CACHE_H_

#include <map>
#include <string>
#include <vector>

#include "common/base/singleton.h"
#include "common/system/concurrency/mutex.h"
#include "common/system/concurrency/rwlock.h"

namespace google { namespace protobuf {
class Message;
}}

class CommonMemoryCache {
public:
    CommonMemoryCache(const std::string& cache_name = "unnamed");

    virtual ~CommonMemoryCache();

public:
    void Set(
        const std::string& key,
        const google::protobuf::Message& message);

    bool Get(
        const std::string& key,
        google::protobuf::Message* message);

    bool Has(const std::string& key);

    void Del(const std::string& key);

    size_t Size() const;

    const std::string& Name() const;

private:
    typedef std::map<std::string, std::string> CacheMap;

    std::string        m_name;
    mutable RWLock     m_rw_lock;
    CacheMap           m_cache_map;
}; // class CommonMemoryCache

class CommonMemoryCacheGroup {
public:
    CommonMemoryCacheGroup();

    virtual ~CommonMemoryCacheGroup();

public:
    void CreateCache(const std::string& cache_name);

    void ClearCache(const std::string& cache_name);

    void ClearAll();

    void Set(
        const std::string& name,
        const std::string& key,
        const google::protobuf::Message& message);

    bool Get(
        const std::string& name,
        const std::string& key,
        google::protobuf::Message* message);

    bool Has(
        const std::string& name,
        const std::string& key);

    void Del(
        const std::string& name,
        const std::string& key);

private:
    typedef std::pair<std::string, CommonMemoryCache*> CommonMemoryCacheItem;
    typedef std::map<std::string, CommonMemoryCache*> CommonMemoryCacheMap;

    Mutex                  m_mutex;
    CommonMemoryCacheMap   m_cache_map;
}; // class CommonMemoryCacheGroup

typedef Singleton<CommonMemoryCacheGroup> CommonMemoryCacheGroupSingleton;

#endif // STORAGE_COMMON_MEMORY_CACHE_H_

