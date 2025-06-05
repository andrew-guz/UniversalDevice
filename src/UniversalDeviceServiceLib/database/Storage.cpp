#include "Storage.hpp"

namespace {

    static std::map<std::string, std::string> Tables = {
        { "Devices",
          "CREATE TABLE IF NOT EXISTS Devices (id TEXT UNIQUE, type TEXT, name TEXT, grp TEXT, timestamp INTEGER, PRIMARY KEY(id, type))" },
        { "Settings", "CREATE TABLE IF NOT EXISTS Settings (id TEXT, settings TEXT, PRIMARY KEY(id))" },
        { "Commands", "CREATE TABLE IF NOT EXISTS Commands (id TEXT, commands TEXT, PRIMARY KEY(id))" },
        { "Events",
          "CREATE TABLE IF NOT EXISTS Events (id TEXT UNIQUE, active INTEGER, providerId TEXT, providerType TEXT, event TEXT, PRIMARY KEY(id))" },
        { "Thermometers",
          "CREATE TABLE IF NOT EXISTS Thermometers (idx INTEGER, id TEXT, timestamp INTEGER, value REAL, PRIMARY KEY(idx AUTOINCREMENT))" },
        { "Relays", "CREATE TABLE IF NOT EXISTS Relays (idx INTEGER, id TEXT, timestamp INTEGER, state INTEGER, PRIMARY KEY(idx AUTOINCREMENT))" },
        { "MotionRelays",
          "CREATE TABLE IF NOT EXISTS MotionRelays (idx INTEGER, id TEXT, timestamp INTEGER, motion INTEGER, state INTEGER, PRIMARY "
          "KEY(idx AUTOINCREMENT))" },
        { "Scenarios", "CREATE TABLE IF NOT EXISTS Scenarios (id TEXT, scenarios TEXT, PRIMARY KEY(id))" },
        { "UniversalDevices",
          "CREATE TABLE IF NOT EXISTS UniversalDevices (idx INTEGER, id TEXT, timestamp INTEGER, 'values' TEXT, PRIMARY KEY(idx AUTOINCREMENT))" },
    };

}

std::vector<std::string> Storage::GetAllTables() const {
    return {
        "Devices", "Settings", "Commands", "Events", "Thermometers", "Relays", "MotionRelays", "Scenarios", "UniversalDevices",
    };
}

std::vector<std::string> Storage::GetDeviceRelatedTables() const {
    return {
        "Devices", "Thermometers", "Relays", "MotionRelays", "UniversalDevices",
    };
}

std::vector<std::string> Storage::GetDataTables() const {
    return {
        "Thermometers",
        "Relays",
        "MotionRelays",
        "UniversalDevices",
    };
}

void Storage::InitializeDb() {
    for (const auto& pair : Tables)
        Execute(pair.second);
}
