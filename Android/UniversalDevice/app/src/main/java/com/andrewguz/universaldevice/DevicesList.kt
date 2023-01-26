package com.andrewguz.universaldevice

import android.graphics.*
import android.os.Bundle
import android.widget.*
import androidx.appcompat.app.AppCompatActivity
import kotlinx.serialization.decodeFromString
import java.io.IOException
import okhttp3.*
import kotlinx.serialization.json.*

class DevicesList : AppCompatActivity() {
    var requestWrapper = RequestWrapper()
    var buttonsList = mutableListOf<Button>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_devices_list)

        listDevices();
    }

    private fun clearButtons()
    {
        var buttonsLayout = findViewById<LinearLayout>(R.id.buttons_layout);
        buttonsLayout.removeAllViews();
        buttonsList.clear();
    }

    private fun listDevices()
    {
        clearButtons()

        requestWrapper.GetRequest("http://188.242.186.162:7315/api/client/list", object : Callback{
        //requestWrapper.GetRequest("http://192.168.1.187:7315/api/client/list", object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                println("[ERROR] List  devices failed!")
                e.printStackTrace()
            }
            override fun onResponse(call: Call, response: Response) {
                response.use {
                    if (!response.isSuccessful)
                        throw IOException("Unexpected code $response")
                    var bodyData = response.body!!.string();
                    try {
                        var deviceDescriptions = Json.decodeFromString<List<ExtendedDeviceDescription>>(bodyData);
                        println("!!!")
                        println(deviceDescriptions.size)
                        addDeviceButtons(deviceDescriptions);
                    }
                    catch (e: Exception)
                    {
                        println("List  devices failed!")
                    }
                }
            }
        });
    }

    private fun addDeviceButtons(deviceDescriptions : List<ExtendedDeviceDescription>) {
        var buttonsLayout = findViewById<LinearLayout>(R.id.buttons_layout);
        for (deviceDescription: ExtendedDeviceDescription in deviceDescriptions)
        {
            var button = Button(this)
            button.layoutParams = LinearLayout.LayoutParams(
                300,
                200,
            )
            button.text = if (deviceDescription.name.isEmpty()) deviceDescription.id else deviceDescription.name;
            button.setBackgroundColor(Color.GREEN)
            buttonsLayout.addView(button)
            buttonsList.add(button);
        }
    }
}