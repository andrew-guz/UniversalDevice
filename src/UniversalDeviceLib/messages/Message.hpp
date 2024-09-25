#pragma once

#include <nlohmann/json.hpp>

#include "MessageHeader.hpp"

struct Message final {
    MessageHeader _header;
    nlohmann::json _data;
};
