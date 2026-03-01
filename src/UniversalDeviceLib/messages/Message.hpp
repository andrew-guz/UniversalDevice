#pragma once

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include "MessageHeader.hpp"

struct Message final {
    MessageHeader _header;
    nlohmann::json _data;
};
