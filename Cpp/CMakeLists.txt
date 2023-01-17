#minimum version
cmake_minimum_required(VERSION 3.15)

#name of a project
project("UniversalDevice")

#subprojects
add_subdirectory(./UniversalDeviceLib)
add_subdirectory(./UniversalDeviceService)
add_subdirectory(./UniversalDeviceSimulators/SimulatorLib)
add_subdirectory(./UniversalDeviceSimulators/ThermometerSimulator)
