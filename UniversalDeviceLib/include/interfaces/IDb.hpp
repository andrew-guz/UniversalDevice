#pragma once

#include <string>
#include <vector>

template<typename T>
class IDb {
public:
    IDb() = default;

    virtual ~IDb() = default;

    virtual std::vector<std::string> ToDbStrings() const = 0;

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) = 0;
};
