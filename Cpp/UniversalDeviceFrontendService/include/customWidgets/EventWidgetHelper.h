#ifndef _EVENT_WIDGET_HELPER_H_
#define _EVENT_WIDGET_HELPER_H_

class EventWidgetHelper final
{
public:
    template<typename T>
    static void Hide() { }

    template<typename T>
    static void Hide(T t) { t->hide(); }

    template<typename T, typename... Ts>
    static void Hide(T t, Ts... args) { Hide(t); Hide(args...); }

    template<typename T>
    static void Show() { }

    template<typename T>
    static void Show(T t) {t->show(); }

    template<typename T, typename... Ts>
    static void Show(T t, Ts... args) { Show(t); Show(args...); }
};

#endif //_EVENT_WIDGET_HELPER_H_
