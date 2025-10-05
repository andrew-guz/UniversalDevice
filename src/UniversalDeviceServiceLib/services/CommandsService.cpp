#include "CommandsService.hpp"

#include <optional>

#include <crow.h>
#include <nlohmann/json.hpp>

#include "BaseService.hpp"
#include "Command.hpp"
#include "Marshaling.hpp"
#include "Middleware.hpp"
#include "WebsocketsCache.hpp"

CommandsService::CommandsService(CrowApp& app, CommandsController& controller) :
    _controller(controller) //
{
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::GET)(ServiceExtension::bind(this, &CommandsService::GetCommands));
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::POST)(ServiceExtension::bind(this, &CommandsService::SetCommands));
}

crow::response CommandsService::GetCommands(const std::string& idString) const {
    try {
        const Uuid id = Uuid{ idString };
        const std::optional<Command> command = _controller.Get(id);
        if (command.has_value())
            return crow::response(crow::OK, static_cast<nlohmann::json>(command.value()).dump());
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::GetCommands");
        return crow::response(crow::BAD_REQUEST);
    }

    return crow::response(crow::OK, std::string());
}

crow::response CommandsService::SetCommands(const crow::request& request, const std::string& idString) {
    try {
        const Uuid id = Uuid{ idString };
        const Command command = nlohmann::json::parse(request.body).get<Command>();
        if (_controller.AddOrUpdate(id, command)) {
            auto connection = WebsocketsCache::Instance()->GetWebSocketConnection(Uuid(idString));
            if (connection)
                connection->send_text(static_cast<nlohmann::json>(command).dump());
            return crow::response(crow::OK);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::SetCommands");
    }

    return crow::response(crow::BAD_REQUEST);
}
