#pragma once

#include <vector>

#include <nlohmann/json.hpp>

#include "Logger.hpp"

class JsonExtension final {
public:
    JsonExtension() = delete;

    ~JsonExtension() = default;

    template<typename T>
    static T CreateFromJson(const nlohmann::json& json) {
        T t;
        try {
            t.FromJson(json);
        } catch (...) {
            LOG_ERROR << "Failed to create from JSON " << json.dump() << "." << std::endl;
        }
        return t;
    }

    template<typename T>
    static std::vector<T> CreateVectorFromJson(const nlohmann::json& json) {
        std::vector<T> result;
        if (json.is_array()) {
            try {
                for (auto& j : json)
                    result.push_back(CreateFromJson<T>(j));
            } catch (...) {
                LOG_ERROR << "Failed to create from JSON " << json.dump() << "." << std::endl;
            }
        }
        return result;
    }
};
