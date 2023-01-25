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

    static T CreateFromDbStrings(const std::vector<std::string>& dbStrings)
    {
        T t;
        t.FromDbStrings(dbStrings);
        return t;
    }

    static std::vector<T> CreateFromDbStrings(const std::vector<std::vector<std::string>>& dbStringsVector)
    {
        std::vector<T> result;
        for (auto& dbStrings : dbStringsVector)
            result.push_back(CreateFromDbStrings(dbStrings));
        return result;
    }
};

#endif //_I_DB_H_
