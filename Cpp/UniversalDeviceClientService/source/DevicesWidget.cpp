#include "DevicesWidget.h"

#include "Defines.h"
#include "Logger.h"
#include "RequestHelper.h"
#include "ExtendedDeviceDescription.h"

using namespace Wt;

DevicesWidget::DevicesWidget(const Settings& settings) :
    _settings(settings)
{
    _mainLayout = setLayout(std::make_unique<WGridLayout>());
    auto text = _mainLayout->addWidget(std::make_unique<WText>(), 0, 0, 0, 4, AlignmentFlag::Left);
    text->setText("Список известных устройств:");
    _refreshButton = _mainLayout->addWidget(std::make_unique<WPushButton>(), 0, 4, AlignmentFlag::Right);
    _refreshButton->setText("Обновить...");
    _refreshButton->clicked().connect([&](){ this->Refresh(); });

    Refresh();
}

void DevicesWidget::Clear()
{
    for(auto& button : _deviceButtons)
        _mainLayout->removeWidget(button);
    _deviceButtons.clear();
}

void DevicesWidget::Refresh()
{
    Clear();

    auto result = RequestHelper::DoGetRequest({"127.0.0.1", _settings._servicePort, API_CLIENT_LIST_DEVICES});
    if (!result.size())
    {
        LOG_ERROR << "Empty " << API_CLIENT_LIST_DEVICES << " response." << std::endl;
        return;
    }
    std::vector<ExtendedDeviceDescription> descriptions;
    try
    {
        auto jsonArray = nlohmann::json::parse(result);
        for (auto& json : jsonArray)
        {
            ExtendedDeviceDescription description;
            description.FromJson(json);
            descriptions.push_back(description);
        }
    }
    catch(...)
    {
        LOG_ERROR << "Broken JSON " << result << "." << std::endl;
        return;
    }
    if (descriptions.empty())
        return;
    LOG_INFO << descriptions.size() << " descriptions found." << std::endl;
    int row = 2;
    int column = 0;
    for (auto& description : descriptions)
    {
        auto button = _mainLayout->addWidget(std::make_unique<WPushButton>(), row, column, AlignmentFlag::Center);
        button->setText(description._name.size() ? description._name : description._id.data());
        button->setMinimumSize(200, 150);
        _deviceButtons.push_back(button);
        ++column;
        if (column == 5)
        {
            ++row;
            column = 0;
        }
    }
}
