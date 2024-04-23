#minimum version
cmake_minimum_required(VERSION 3.18)

#name of a project
project("UniversalDevice")

#allow unit tests
enable_testing()

#subprojects
add_subdirectory(vendor)
add_subdirectory(UniversalDeviceLib)
add_subdirectory(UniversalDeviceService)
#add_subdirectory(UniversalDeviceFrontendService)
#add_subdirectory(UniversalDeviceSimulators/SimulatorLib)
#add_subdirectory(UniversalDeviceSimulators/ThermometerSimulator)
#add_subdirectory(UniversalDeviceSimulators/RelaySimulator)
add_subdirectory(UniversalDeviceTests)
