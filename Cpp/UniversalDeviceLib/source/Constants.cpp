#include "Constants.h"

//login
const std::string Constants::LoginService = "service";

const std::string Constants::LoginDevice = "device";

// device types
const std::string Constants::DeviceTypeUndefined = "undefined_type";

const std::string Constants::DeviceTypeThermometer = "thermometer";

//subjects - depends on type
const std::string Constants::SubjectUndefined = "undefined_subject";

const std::string Constants::SubjectGetDeviceInformation = "get_device_information";

const std::string Constants::SubjectThermometerCurrentValue = "current_value";
