#pragma once

#include <sstream>
#include <string>
#include <tuple>

#include <Wt/WDialog.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WSpinBox.h>
#include <Wt/WString.h>

class WidgetHelper final {
public:
    static void SetUsualButtonSize(Wt::WPushButton* button);

    static std::string TextWithFontSize(const std::string& txt, int size);

    template<typename T>
    static std::string TextWithFontSize(T value, int size) {
        std::stringstream ss;
        ss.precision(1);
        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
            ss << std::fixed;
        ss << "<div style='font-size:" << size << "px'>" << value << "</div>";
        ss.flush();
        return ss.str();
    }

    template<typename T, typename U>
    static std::string TextWithFontSize(T value, U type, int size) {
        std::stringstream ss;
        ss.precision(1);
        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
            ss << std::fixed;
        ss << "<div style='font-size:" << size << "px'>" << value << type << "</div>";
        ss.flush();
        return ss.str();
    }

    static std::tuple<Wt::WDialog*, Wt::WGridLayout*, Wt::WLineEdit*, Wt::WLineEdit*, Wt::WSpinBox*, Wt::WPushButton*>
    CreateBaseSettingsDialog(Wt::WContainerWidget* parent, int height, const Wt::WString& name, const Wt::WString& group, float period,
                             bool useDefaultValidation);

    static void ShowSimpleErrorMessage(Wt::WWidget* parent, const std::string& header, const std::string& message);
};
