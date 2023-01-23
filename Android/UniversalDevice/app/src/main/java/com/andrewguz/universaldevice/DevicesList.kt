package com.andrewguz.universaldevice

import android.os.Bundle
import android.os.StrictMode
import android.os.StrictMode.ThreadPolicy
import androidx.appcompat.app.AppCompatActivity
import java.net.URL
import java.io.IOException
import okhttp3.*

class DevicesList : AppCompatActivity() {
    var client: OkHttpClient = OkHttpClient();

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_devices_list)

        //val policy = ThreadPolicy.Builder().permitAll().build()
        //StrictMode.setThreadPolicy(policy)

        //println("!!!")
        //var result = getRequest("http://192.168.1.187:8080/api/client/list");
        //println("!!! - {$result}")

        //println("1")
        //run()
        //println("2")

        run2(object : Callback{
            override fun onFailure(call: Call, e: IOException) {
                println("!!! - Failure")
            }
            override fun onResponse(call: Call, response: Response) {
                println("!!! - ${response.body!!.string()}")
            }
        });

    }

    private fun getRequest(sUrl: String): String? {
        var result: String? = null
        try {
            // Create URL
            val url = URL(sUrl)
            // Build request
            val request = Request.Builder().url(url).build()
            // Execute request
            val response = client.newCall(request).execute()
            result = response.body?.string()
        }
        catch(err:Error) {
            print("Error when executing get request: "+err.localizedMessage)
        }
        return result
    }

    fun run() {
        println("3")
        val request = Request.Builder()
            .url("http://192.168.1.187:8080/api/client/list")
            .build()
        println("4")
        client.newCall(request).enqueue(object : Callback {
            override fun onFailure(call: Call, e: IOException) {
                println("5")
                e.printStackTrace()
            }
            override fun onResponse(call: Call, response: Response) {
                response.use {
                    println("6")
                    if (!response.isSuccessful)
                        throw IOException("Unexpected code $response")
                    for ((name, value) in response.headers) {
                        println("$name: $value")
                    }
                    println("7")
                    println(response.body!!.string())
                }
            }
        })
    }

    fun run2(myCallback : Callback) {
        println("3")
        val request = Request.Builder()
            .url("http://192.168.1.187:8080/api/client/list")
            .build()
        println("4")
        client.newCall(request).enqueue(myCallback)
    }
}