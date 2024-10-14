#pragma once

#include <Wt/WContainerWidget.h>
#include <Wt/WGridLayout.h>

#include "BaseStackWidget.hpp"

class ScenariosWidget final : public Wt::WContainerWidget, public BaseStackWidget {
public:
    ScenariosWidget(IStackHolder* stackHolder, const Settings& settings);

    virtual ~ScenariosWidget() = default;

    virtual void Initialize(const std::string& data) override;

private:
    void Clear();

    void Refresh();

private:
    Wt::WGridLayout* _mainLayout;
};
