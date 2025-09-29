#include "SettingsService.hpp"

#include <optional>
#include <variant>

#include <boost/hof.hpp>
#include <nlohmann/json.hpp>

#include "Marshaling.hpp"
#include "Settings.hpp"
#include "Uuid.hpp"
#include "WebsocketsCache.hpp"

SettingsService::SettingsService(IQueryExecutor* queryExecutor, SettingsController& controller) :
    BaseService(queryExecutor),
    _controller(controller) {}

void SettingsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &SettingsService::GetSettings));
    CROW_ROUTE(app, API_DEVICE_SETTINGS).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &SettingsService::SetSettings));
}

crow::response SettingsService::GetSettings(const std::string& idString) const {
    try {
        const Uuid id = Uuid{ idString };
        const std::optional<Settings> settings = _controller.Get(id);
        if (settings.has_value())
            return crow::response(crow::OK, static_cast<nlohmann::json>(settings.value()).dump());
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::GetSettings");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK, std::string());
}

crow::response SettingsService::SetSettings(const crow::request& request, const std::string& idString) {
    try {
        const Uuid id = Uuid{ idString };
        const Settings settings = nlohmann::json::parse(request.body).get<Settings>();
        if (_controller.AddOrUpdate(id, settings)) {
            auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
            if (connection)
                connection->send_text(std::visit(
                    boost::hof::match([](const PeriodSettings& value) -> std::string { return static_cast<nlohmann::json>(value).dump(); },
                                      [](const MotionRelaySettings& value) -> std::string { return static_cast<nlohmann::json>(value).dump(); }),
                    settings));
            return crow::response(crow::OK);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::SetSettings");
    }

    return crow::response(crow::BAD_REQUEST);
}
