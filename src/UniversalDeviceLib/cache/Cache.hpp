#pragma once

#include <optional>
#include <unordered_map>

template<typename KeyType, typename ObjectType>
class Cache final {
public:
    Cache() = default;

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

    bool AddOrUpdate(const KeyType& key, const ObjectType& object) {

        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            _cache.try_emplace(key, object);
        else
            iter->second = object;

        return true;
    }

    bool Remove(const KeyType& key) {

        const auto iter = _cache.find(key);
        if (iter == _cache.end())
            return false;

        _cache.erase(iter);

        return true;
    }

private:
    std::unordered_map<KeyType, ObjectType> _cache;
};
