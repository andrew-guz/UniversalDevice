package com.andrewguz.universaldevice

import kotlinx.serialization.*
import kotlinx.serialization.descriptors.*
import kotlinx.serialization.internal.*
import kotlinx.serialization.builtins.*
import kotlinx.serialization.Serializable
import kotlinx.serialization.json.Json
import kotlinx.serialization.encodeToString
import java.util.UUID

@Serializable
class ThermometerCurrentValue(
    val value: Float = 0.0f)

@Serializable
class DeviceDescription(
    val type: String = "",
    val id: String = "")

@Serializable
class MessageHeader(
    val deviceDescription: DeviceDescription,
    var subject: String = "")

@Serializable
class Message(
    val header: MessageHeader,
    val data: String = "")
