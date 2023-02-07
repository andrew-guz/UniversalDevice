#ifndef _WIDGET_HELPER_H_
#define _WIDGET_HELPER_H_

#include <string>
#include <sstream>

#include <Wt/WPushButton.h>

class WidgetHelper final
{
public:
    static void SetUsualButtonSize(Wt::WPushButton* button);

    template<typename T>
    static std::string TextWithFontSize(T value, int size)
    {
        std::stringstream ss;
        ss.precision(1);
        ss << "<p style='font-size:" << size << "px'>" << std::fixed << value << "</p>";
        ss.flush();
        return ss.str();
    }

    template<typename T, typename U>
    static std::string TextWithFontSize(T value, U type, int size)
    {
        std::stringstream ss;
        ss.precision(1);
        ss << "<p style='font-size:" << size << "px'>" << std::fixed << value << type <<"</p>";
        ss.flush();
        return ss.str();
    }
};

#endif //WIDGET_HELPER_H_
