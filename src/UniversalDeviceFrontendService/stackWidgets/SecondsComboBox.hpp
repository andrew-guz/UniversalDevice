#pragma once

#include <Wt/WComboBox.h>

class SecondsComboBox : public Wt::WComboBox {
public:
    SecondsComboBox();

    virtual ~SecondsComboBox() = default;

    uint64_t GetSeconds() const;
};
