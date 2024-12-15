#include "FirmwareService.hpp"

#include <filesystem>
#include <fstream>

#include "Defines.hpp"
#include "WebsocketsCache.hpp"

FirmwareService::FirmwareService(IQueryExecutor* queryExecutor, const Settings& settings) :
    BaseService(queryExecutor),
    _settings(settings) {}

void FirmwareService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_FIRMWARE).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &FirmwareService::UploadFirmware));
}

crow::response FirmwareService::UploadFirmware(const crow::request& request, const std::string& idString) const {
    try {
        const std::filesystem::path firmwarePath = _settings._firmwarePath / idString / "firmware.bin";
        if (!std::filesystem::exists(firmwarePath.parent_path()))
            std::filesystem::create_directories(firmwarePath.parent_path());
        { //
            std::ofstream{ firmwarePath, std::ios::binary } << request.body;
        }
        return crow::response{
            crow::NO_CONTENT,
        };
    } catch (...) {
        LOG_ERROR_MSG("Something went wrong in FirmwareService::UploadFirmware");
        return crow::response{
            crow::BAD_REQUEST,
        };
    }
}
