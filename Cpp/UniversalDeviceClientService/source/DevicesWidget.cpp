#include "DevicesWidget.h"

#include "Defines.h"
#include "Logger.h"
#include "RequestHelper.h"
#include "ExtendedDeviceDescription.h"

DevicesWidget::DevicesWidget(const Settings& settings) :
    _settings(settings)
{
    _mainLayout = setLayout(std::make_unique<Wt::WGridLayout>());
    auto text = _mainLayout->addWidget(std::make_unique<Wt::WText>(), 0, 0);
    text->setText("Список известных устройств:");
    _refreshButton = _mainLayout->addWidget(std::make_unique<Wt::WPushButton>(), 0, 1, Wt::AlignmentFlag::Right);
    _refreshButton->setText("Обновить...");
    _refreshButton->clicked().connect([&](){ this->Refresh(); });

    Refresh();
}

void DevicesWidget::Clear()
{

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
}