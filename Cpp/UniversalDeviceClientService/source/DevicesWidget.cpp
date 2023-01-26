#include "DevicesWidget.h"

DevicesWidget::DevicesWidget()
{
    _mainLayout = setLayout(std::make_unique<Wt::WGridLayout>());
    auto text = _mainLayout->addWidget(std::make_unique<Wt::WText>(), 0, 0);
    text->setText("Список известных устройств:");
    _refreshButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>(), 0, 1, Wt::AlignmentFlag::Right);
    _refreshButton->setText("Обновить...");
}

void DevicesWidget::Refresh()
{

}
