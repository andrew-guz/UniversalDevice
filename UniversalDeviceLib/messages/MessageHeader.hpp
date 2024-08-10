#pragma once

#include "ComponentDescription.hpp"
#include "Uuid.hpp"

struct MessageHeader final {
    Uuid _id;
    ComponentDescription _description;
    std::string _subject = std::string{ Constants::SubjectUndefined };
};
