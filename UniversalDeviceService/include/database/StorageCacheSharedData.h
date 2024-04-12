#ifndef _STORAGE_CACHE_SHARED_DATA_H_
#define _STORAGE_CACHE_SHARED_DATA_H_

#include <string>
#include <vector>

enum StorageCacheProblemType { NoProblems, Empty, NotExists, TooMany, SQLError };

struct StorageCacheProblem {
    StorageCacheProblemType _type;
    std::string _message;
};

struct SelectInput {
    virtual ~SelectInput() = default;
};

struct SelectOutput {
    virtual ~SelectOutput() = default;
};

struct SelectAllOutput {
    virtual ~SelectAllOutput() = default;
};

struct InsertOrReplaceInput {
    virtual ~InsertOrReplaceInput() = default;
};

struct UpdateInput {
    virtual ~UpdateInput() = default;
};

struct DeleteInput {
    virtual ~DeleteInput() = default;
};

struct SimpleTableSelectInput final : public SelectInput {
    virtual ~SimpleTableSelectInput() = default;

    std::string _id;
};

struct SimpleTableSelectOutput final : public SelectOutput {
    virtual ~SimpleTableSelectOutput() = default;

    std::string _data;
};

struct SimpleTableInsertOrReplaceInput final : public InsertOrReplaceInput {
    virtual ~SimpleTableInsertOrReplaceInput() = default;

    std::string _id;
    std::string _data;
};

struct SimpleTableDeleteInput final : public DeleteInput {
    virtual ~SimpleTableDeleteInput() = default;

    std::string _id;
};

struct EventTableSelectInput final : public SelectInput {
    virtual ~EventTableSelectInput() = default;

    std::string _id;
    std::string _type;
};

struct EventTableSelectOutput final : public SelectOutput {
    virtual ~EventTableSelectOutput() = default;

    std::vector<std::string> _data;
};

struct EventTableSelectAllOutput final : public SelectAllOutput {
    virtual ~EventTableSelectAllOutput() = default;

    std::vector<std::string> _data;
};

struct EventTableInsertOrReplaceInput final : public InsertOrReplaceInput {
    virtual ~EventTableInsertOrReplaceInput() = default;

    std::string _id;
    bool _active;
    std::string _providerId;
    std::string _providerType;
    std::string _event;
};

struct EventTableUpdateInput final : public UpdateInput {
    virtual ~EventTableUpdateInput() = default;

    std::string _id;
    bool _active;
    std::string _providerId;
    std::string _providerType;
    std::string _event;
};

struct EventTableDeleteInput final : public DeleteInput {
    virtual ~EventTableDeleteInput() = default;

    std::string _id;
};

#endif //_STORAGE_CACHE_SHARED_DATA_H_
