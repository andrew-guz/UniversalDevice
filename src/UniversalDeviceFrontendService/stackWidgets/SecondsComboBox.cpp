#include "SecondsComboBox.hpp"

#include <cstdint>

SecondsComboBox::SecondsComboBox() {
    setMinimumSize(200, 50);
    setMaximumSize(200, 50);
    addItem("1 час");
    addItem("2 часа");
    addItem("12 часов");
    addItem("1 день");
    addItem("1 неделя");
    setCurrentIndex(1);
}

std::uint64_t SecondsComboBox::GetSeconds() const {
    switch (currentIndex()) {
        case 0:
            return 3600;
        case 1:
            return 7200;
        case 2:
            return 43200;
        case 3:
            return 86400;
        case 4:
            return 604800;
        default:
            return 7200;
    }
}
