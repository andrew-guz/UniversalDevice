#pragma once

#include "ComponentDescription.hpp"

struct MessageHeader final {
    ComponentDescription _description;
    std::string _subject = std::string{ Constants::SubjectUndefined };
};
