#pragma once

#include "Enums.hpp"
#include "Provider.hpp"
#include "Uuid.hpp"

struct MessageHeader final {
    Uuid _id;
    Provider _description;
    Subject _subject = Subject::Undefined;
};
