#pragma once

#include <functional>
#include <string>
#include <tuple>

#include <Wt/WDialog.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WPushButton.h>
#include <Wt/WSpinBox.h>
#include <Wt/WString.h>
#include <fmt/format.h>

class WidgetHelper final {
public:
    static void SetUsualButtonSize(Wt::WPushButton* button);

    static std::string TextWithFontSize(const std::string& txt, int size);

    template<typename T>
    static std::string TextWithFontSize(T value, int size) {
        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
            return fmt::format("<div style='font-size:{}px'>{:.1f}</div>", size, value);
        return fmt::format("<div style='font-size:{}px'>{}</div>", size, value);
    }

    template<typename T, typename U>
    static std::string TextWithFontSize(T value, U type, int size) {
        if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value)
            return fmt::format("<div style='font-size:{}px'>{:.1f}{}</div>", size, value, type);
        return fmt::format("<div style='font-size:{}px'>{}</div>", size, value, type);
    }

    static std::tuple<Wt::WDialog*, Wt::WGridLayout*, Wt::WLineEdit*, Wt::WLineEdit*, Wt::WSpinBox*, Wt::WPushButton*>
    CreateBaseSettingsDialog(Wt::WContainerWidget* parent,
                             int height,
                             const Wt::WString& name,
                             const Wt::WString& group,
                             float period,
                             bool useDefaultValidation,
                             std::function<void(void)> restartFunction);

    static void ShowSimpleMessage(Wt::WWidget* parent, const std::string& header, const std::string& message, int timeout = 0);
};
