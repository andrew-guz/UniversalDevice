#pragma once

#include <chrono>
#include <cstddef>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

template<typename KeyType, typename ObjectType, typename Container = std::unordered_map<KeyType, ObjectType>>
class Cache {
public:
    Cache() = default;

    virtual ~Cache() = default;

    std::optional<ObjectType> Get(const KeyType& key) const {
        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            return std::nullopt;

        return iter->second;
    }

    bool Add(const KeyType& key, const ObjectType& object) { return _cache.try_emplace(key, object).second; }

    bool Update(const KeyType& key, const ObjectType& object) {
        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            return false;

        iter->second = object;

        return true;
    }

    void AddOrUpdate(const KeyType& key, const ObjectType& object) {
        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            _cache.try_emplace(key, object);
        else
            iter->second = object;
    }

    const Container& Get() const { return _cache; }

    std::vector<ObjectType> List() const {
        std::vector<ObjectType> result;
        result.reserve(_cache.size());
        for (const auto& [_, value] : _cache) {
            result.emplace_back(value);
        }

        return result;
    }

    bool Remove(const KeyType& key) {
        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            return false;

        _cache.erase(iter);

        return true;
    }

    std::size_t Size() const { return _cache.size(); }

protected:
    Container _cache;
};

template<typename ObjectType>
class ChronoCache : public Cache<std::chrono::system_clock::time_point, ObjectType, std::map<std::chrono::system_clock::time_point, ObjectType>> {
public:
    ChronoCache() = default;

    virtual ~ChronoCache() = default;

    void Cleanup(const std::chrono::system_clock::time_point& timestamp) {
        this->_cache.erase(this->_cache.begin(), this->_cache.lower_bound(timestamp));
    }
};
