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
        ss << "<p style='font-size:" << size << "px'>" << value << "°C</p>";
        ss.flush();
        return ss.str();
    }
};

#endif //WIDGET_HELPER_H_
