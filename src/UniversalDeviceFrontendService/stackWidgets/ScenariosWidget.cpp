#include "ScenariosWidget.hpp"

#include "WidgetHelper.hpp"

ScenariosWidget::ScenariosWidget(IStackHolder* stackHolder, const Settings& settings) :
    BaseStackWidget(stackHolder, settings) {
    _mainLayout = setLayout(std::make_unique<Wt::WGridLayout>());

    auto backButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>("Назад..."), 0, 0, Wt::AlignmentFlag::Left);
    WidgetHelper::SetUsualButtonSize(backButton);
    backButton->clicked().connect([this]() { _stackHolder->SetWidget(StackWidgetType::Devices, {}); });

    auto refreshButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>("Обновить..."), 0, 1, Wt::AlignmentFlag::Right);
    WidgetHelper::SetUsualButtonSize(refreshButton);
    refreshButton->clicked().connect([this]() { Refresh(); });
}

void ScenariosWidget::Initialize(const std::string& data) { Refresh(); }

void ScenariosWidget::Clear() {}

void ScenariosWidget::Refresh() { Clear(); }
