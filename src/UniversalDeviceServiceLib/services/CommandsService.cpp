#include "CommandsService.hpp"

#include <optional>
#include <variant>

#include <boost/hof.hpp>
#include <nlohmann/json.hpp>

#include "Command.hpp"
#include "Marshaling.hpp"
#include "WebsocketsCache.hpp"

CommandsService::CommandsService(IQueryExecutor* queryExecutor, CommandsController& controller) :
    BaseService(queryExecutor),
    _controller(controller) {}

void CommandsService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::GET)(BaseService::bind(this, &CommandsService::GetCommands));
    CROW_ROUTE(app, API_DEVICE_COMMANDS).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &CommandsService::SetCommands));
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
                connection->send_text(std::visit(
                    boost::hof::match([](const RelayState& value) -> std::string { return static_cast<nlohmann::json>(value).dump(); },
                                      [](const ThermometerLedBrightness& value) -> std::string { return static_cast<nlohmann::json>(value).dump(); }),
                    command));
            return crow::response(crow::OK);
        }
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in DeviceService::SetCommands");
    }

    return crow::response(crow::BAD_REQUEST);
}
