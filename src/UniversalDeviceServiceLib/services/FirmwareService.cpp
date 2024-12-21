#include "FirmwareService.hpp"

#include <filesystem>
#include <fstream>

#include "Defines.hpp"
#include "Logger.hpp"
#include "Settings.hpp"

FirmwareService::FirmwareService(IQueryExecutor* queryExecutor) :
    BaseService(queryExecutor) {}

void FirmwareService::Initialize(CrowApp& app) {
    CROW_ROUTE(app, API_CLIENT_FIRMWARE).methods(crow::HTTPMethod::POST)(BaseService::bind(this, &FirmwareService::UploadFirmware));
}

crow::response FirmwareService::UploadFirmware(const crow::request& request, const std::string& idString) const {
    try {
        const Settings settings = Settings::ReadSettings();

        const std::filesystem::path firmwarePath = settings._firmwarePath / idString / "firmware.bin";
        if (settings._firmwarePath.empty()) {
            LOG_ERROR_MSG("No firmware path provided");
            return crow::response{
                crow::BAD_REQUEST,
            };
        }

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
