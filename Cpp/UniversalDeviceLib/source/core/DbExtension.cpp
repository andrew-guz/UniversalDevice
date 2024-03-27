#include "DbExtension.h"

#include <algorithm>

 std::string DbExtension::FindValueByName(const std::vector<std::string>& dbStrings, const std::string_view name)
{
    auto iter = std::find(dbStrings.begin(), dbStrings.end(), name);
    if (iter != dbStrings.end())
    {
        auto index = iter - dbStrings.begin();
        if (index % 2 == 0)
            return dbStrings.at(index + 1);
    }
    return {};
}
