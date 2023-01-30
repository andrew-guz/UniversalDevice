#include "Constants.h"

//service types
const std::string Constants::DeviceServiceType = "device_service";

const std::string Constants::ClientServiceType = "client_service";

const std::string Constants::FrontendServiceType = "frontend_service";

// device types
const std::string Constants::DeviceTypeUndefined = "undefined_type";

const std::string Constants::DeviceTypeThermometer = "thermometer";

//subjects - depends on type
const std::string Constants::SubjectUndefined = "undefined_subject";

const std::string Constants::SubjectThermometerCurrentValue = "current_value";

const std::string Constants::SubjectGetDevicesList = "get_devices_list";

const std::string Constants::SubjectGetDeviceInformation = "get_device_information";
