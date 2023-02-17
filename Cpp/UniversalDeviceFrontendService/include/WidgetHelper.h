#ifndef _WIDGET_HELPER_H_
#define _WIDGET_HELPER_H_

#include <string>
#include <sstream>
#include <tuple>

#include <Wt/WPushButton.h>
#include <Wt/WDialog.h>
#include <Wt/WString.h>
#include <Wt/WGridLayout.h>
#include <Wt/WLineEdit.h>
#include <Wt/WSpinBox.h>

class WidgetHelper final
{
public:
    static void SetUsualButtonSize(Wt::WPushButton* button);

    template<typename T>
    static std::string TextWithFontSize(T value, int size)
    {
        std::stringstream ss;
        ss.precision(1);
        if constexpr (std::is_same<T, float>::value ||
            std::is_same<T, double>::value)
            ss << std::fixed;
        ss << "<p style='font-size:" << size << "px'>" << value << "</p>";
        ss.flush();
        return ss.str();
    }

    template<typename T, typename U>
    static std::string TextWithFontSize(T value, U type, int size)
    {
        std::stringstream ss;
        ss.precision(1);
        if constexpr (std::is_same<T, float>::value ||
            std::is_same<T, double>::value)
            ss << std::fixed;
        ss << "<p style='font-size:" << size << "px'>" << value << type <<"</p>";
        ss.flush();
        return ss.str();
    }

    static std::tuple<Wt::WDialog*, Wt::WGridLayout*, Wt::WLineEdit*, Wt::WSpinBox*, Wt::WPushButton*> CreateNamePeriodSettingsDialog(Wt::WContainerWidget* parent, int height, const Wt::WString& name, float period, bool useDefaultValidation);
};

#endif //WIDGET_HELPER_H_
