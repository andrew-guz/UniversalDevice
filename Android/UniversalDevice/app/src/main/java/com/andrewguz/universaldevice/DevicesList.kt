package com.andrewguz.universaldevice

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import java.net.URL
import java.io.IOException
import okhttp3.*

class DevicesList : AppCompatActivity() {
    var requestWrapper = RequestWrapper()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_devices_list)

        requestWrapper.GetRequest("http://188.242.186.162:7315/api/client/list", object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                e.printStackTrace()
            }
            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful)
                        throw IOException("Unexpected code $response")
                    println(response.body!!.string())
                }
            }
        });
    }
}