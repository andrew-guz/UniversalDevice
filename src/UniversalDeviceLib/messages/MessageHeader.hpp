#pragma once

#include "ComponentDescription.hpp"
#include "Constants.hpp"
#include "Enums.hpp"
#include "Uuid.hpp"

struct MessageHeader final {
    Uuid _id;
    ComponentDescription _description;
    Subject _subject = Subject::Undefined;
};
