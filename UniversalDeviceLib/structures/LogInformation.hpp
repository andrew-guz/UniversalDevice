#pragma once

#include <string>

#include "Base64Helper.hpp"
#include "IJson.hpp"

struct LogInformation final : IJson<LogInformation> {
    std::string _fileName;
    std::string _fileContent;

    virtual nlohmann::json ToJson() const override { return { { "fileName", _fileName }, { "fileContent", Base64Helper::ToBase64(_fileContent) } }; }

    virtual void FromJson(const nlohmann::json& json) override {
        _fileName = json.value("fileName", "");
        _fileContent = Base64Helper::FromBase64(json.value("fileContent", ""));
    }
};
