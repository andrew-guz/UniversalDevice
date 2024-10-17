#pragma once

#include <string>
#include <vector>

#include "Types.hpp"
#include "Uuid.hpp"

enum class StorageCacheProblemType {
    NoProblems,
    Empty,
    NotExists,
    TooMany,
    SQLError,
};

struct StorageCacheProblem {
    StorageCacheProblemType _type;
    std::string _message;
};

struct SelectInput {};

struct SelectOutput {};

struct SelectAllOutput {};

struct InsertOrReplaceInput {};

struct UpdateInput {};

struct DeleteInput {};

struct SimpleTableSelectInput final : public SelectInput {
    Uuid _id;
};

template<typename T>
struct SimpleTableSelectAllOutput final : public SelectAllOutput {
    std::vector<T> _data;
};

template<typename T>
struct SimpleTableSelectOutput final : public SelectOutput {
    T _data;
};

template<typename T>
struct SimpleTableInsertOrReplaceInput final : public InsertOrReplaceInput {
    Uuid _id;
    T _data;
};

template<typename T>
struct SimpleTableUpdateInput final : public UpdateInput {
    Uuid _id;
    T _data;
};

struct SimpleTableDeleteInput final : public DeleteInput {
    Uuid _id;
};

struct EventTableSelectInput final : public SelectInput {
    Uuid _id;
    ActorType _type;
};

struct EventTableSelectOutput final : public SelectOutput {
    std::vector<std::string> _data;
};

struct EventTableSelectAllOutput final : public SelectAllOutput {
    std::vector<std::string> _data;
};

struct EventTableInsertOrReplaceInput final : public InsertOrReplaceInput {
    Uuid _id;
    bool _active;
    Uuid _providerId;
    ActorType _providerType;
    std::string _event;
};

struct EventTableUpdateInput final : public UpdateInput {
    Uuid _id;
    bool _active;
    Uuid _providerId;
    ActorType _providerType;
    std::string _event;
};

struct EventTableDeleteInput final : public DeleteInput {
    Uuid _id;
};
