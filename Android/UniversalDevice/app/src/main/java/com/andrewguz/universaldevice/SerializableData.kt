package com.andrewguz.universaldevice

import kotlinx.serialization.*

@Serializable
class ThermometerCurrentValue(
    val value: Float = 0.0f)

@Serializable
open class DeviceDescription(
    val type: String = "",
    val id: String = "")

@Serializable
class ExtendedDeviceDescription(
    val type: String = "",
    val id: String = "",
    val name: String = "",
    val timestamp: String = "")

@Serializable
class MessageHeader(
    val deviceDescription: DeviceDescription,
    var subject: String = "")

@Serializable
class Message(
    val header: MessageHeader,
    val data: String = "")
