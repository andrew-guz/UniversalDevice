package com.andrewguz.universaldevice

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.serialization.decodeFromString
import java.io.IOException
import okhttp3.*
import kotlinx.serialization.*
import kotlinx.serialization.json.*

class DevicesList : AppCompatActivity() {
    var requestWrapper = RequestWrapper()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_devices_list)

        //requestWrapper.GetRequest("http://188.242.186.162:7315/api/client/list", object : Callback{
        requestWrapper.GetRequest("http://192.168.1.187:7315/api/client/list", object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                println("Failed!")
                e.printStackTrace()
            }
            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful)
                        throw IOException("Unexpected code $response")
                    println("!!!")
                    var bodyData = response.body!!.string();
                    println(bodyData);
                    var deviceDescriptions = Json.decodeFromString<List<ExtendedDeviceDescription>>(bodyData);
                    println("!!!")
                    println(deviceDescriptions.size);
                }
            }
        });
    }
}