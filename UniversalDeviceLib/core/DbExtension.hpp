#pragma once

#include <string>
#include <string_view>
#include <vector>

class DbExtension final {
public:
    DbExtension() = delete;

    ~DbExtension() = default;

    template<typename T>
    static T CreateFromDbStrings(const std::vector<std::string>& dbStrings) {
        T t;
        t.FromDbStrings(dbStrings);
        return t;
    }

    template<typename T>
    static std::vector<T> CreateVectorFromDbStrings(const std::vector<std::vector<std::string>>& dbStringsVector) {
        std::vector<T> result;
        for (auto& dbStrings : dbStringsVector)
            result.push_back(CreateFromDbStrings<T>(dbStrings));
        return result;
    }

    static std::string FindValueByName(const std::vector<std::string>& dbStrings, std::string_view name);
};
