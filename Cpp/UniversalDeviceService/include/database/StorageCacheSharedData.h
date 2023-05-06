#ifndef _STORAGE_CACHE_SHARED_DATA_H_
#define _STORAGE_CACHE_SHARED_DATA_H_

#include <string>
#include <vector>
#include "NightModeDevice.h"

enum StorageCacheProblemType
{
    NoProblems,
    Empty,
    NotExists,
    TooMany,
    SQLError
};

struct StorageCacheProblem
{
    StorageCacheProblemType _type;
    std::string             _message;
};

struct SelectInput
{
    virtual ~SelectInput() = default;
};

struct SelectOutput
{
    virtual ~SelectOutput() = default;
};

struct SelectAllOutput
{
    virtual ~SelectAllOutput() = default;
};

struct AddInput
{
    virtual ~AddInput() = default;
};

struct InsertOrReplaceInput
{
    virtual ~InsertOrReplaceInput() = default;
};

struct UpdateInput
{
    virtual ~UpdateInput() = default;
};

struct DeleteInput
{
    virtual ~DeleteInput() = default;
};

struct SimpleTableSelectInput final : public SelectInput
{
    virtual ~SimpleTableSelectInput() = default;

    std::string _id;
};

struct SimpleTableSelectOutput final : public SelectOutput
{
    virtual ~SimpleTableSelectOutput() = default;

    std::string _data;
};

struct SimpleTableAddInput final : public AddInput
{
    virtual ~SimpleTableAddInput() = default;

    std::string _id;
    std::string _data;
};

struct SimpleTableInsertOrReplaceInput final : public InsertOrReplaceInput
{
    virtual ~SimpleTableInsertOrReplaceInput() = default;

    std::string _id;
    std::string _data;
};

struct EventTableSelectInput final : public SelectInput
{
    virtual ~EventTableSelectInput() = default;

    ComponentDescription _description;
};

struct EventTableSelectOutput final : public SelectOutput
{
    virtual ~EventTableSelectOutput() = default;

    std::vector<std::string> _data;
};

struct EventTableSelectAllOutput final : public SelectAllOutput
{
    virtual ~EventTableSelectAllOutput() = default;

    std::vector<std::string> _data;
};

struct EventTableAddInput final : public AddInput
{
    virtual ~EventTableAddInput() = default;

    std::string             _id;
    bool                    _active;
    ComponentDescription    _providerDescription;
    std::string             _event;
};

struct EventTableUpdateInput final : public UpdateInput
{
    virtual ~EventTableUpdateInput() = default;

    std::string             _id;
    bool                    _active;
    ComponentDescription    _providerDescription;
    std::string             _event;
};

struct EventTableDeleteInput final : public DeleteInput
{
    virtual ~EventTableDeleteInput() = default;

    std::string _id;
};

struct NightModeTableSelectInput final : public SelectInput
{
    virtual ~NightModeTableSelectInput() = default;

    ComponentDescription _description;
};

struct NightModeTableSelectOuput final : public SelectOutput
{
    virtual ~NightModeTableSelectOuput() = default;

    NightModeTiming _timing;
};

struct NightModeTableSelectAllOuput final : public SelectAllOutput
{
    virtual ~NightModeTableSelectAllOuput() = default;

    std::vector<NightModeDevice> _devices;
};

#endif //_STORAGE_CACHE_SHARED_DATA_H_
