#pragma once

#include <cstdint>

#include <Wt/WComboBox.h>

class SecondsComboBox : public Wt::WComboBox {
public:
    SecondsComboBox();

    virtual ~SecondsComboBox() = default;

    std::uint64_t GetSeconds() const;
};
