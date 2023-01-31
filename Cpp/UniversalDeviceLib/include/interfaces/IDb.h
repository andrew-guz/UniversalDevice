#ifndef _I_DB_H_
#define _I_DB_H_

#include <string>
#include <vector>

template<typename T>
class IDb
{
public:
    IDb() = default;
    
    virtual ~IDb() = default;

    virtual std::vector<std::string> ToDbStrings() const = 0;

    virtual void FromDbStrings(const std::vector<std::string>& dbStrings) = 0;
};

class DbExtension final
{
public:
    DbExtension() = delete;
    
    ~DbExtension() = default;

    template<typename T>
    static T CreateFromDbStrings(const std::vector<std::string>& dbStrings)
    {
        T t;
        t.FromDbStrings(dbStrings);
        return t;
    }

    template<typename T>
    static std::vector<T> CreateVectorFromDbStrings(const std::vector<std::vector<std::string>>& dbStringsVector)
    {
        std::vector<T> result;
        for (auto& dbStrings : dbStringsVector)
            result.push_back(CreateFromDbStrings<T>(dbStrings));
        return result;
    }

    static std::string FindValueByName(const std::vector<std::string>& dbStrings, const std::string& name)
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
};

#endif //_I_DB_H_
