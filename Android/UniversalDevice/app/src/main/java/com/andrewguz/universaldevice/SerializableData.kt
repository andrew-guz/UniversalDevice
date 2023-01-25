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
data class ThermometerCurrentValue(
    val value: Float)

@Serializable
data class DeviceDescription(
    val type: String,
    val id: String)

@Serializable
data class MessageHeader(
    val deviceDescription: DeviceDescription,
    var subject: String)

@Serializable
data class Message(
    val header: MessageHeader,
    val data: String)
