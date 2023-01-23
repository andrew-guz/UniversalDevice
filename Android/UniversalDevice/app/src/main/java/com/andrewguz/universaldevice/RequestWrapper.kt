package com.andrewguz.universaldevice

import okhttp3.*

class RequestWrapper {
    var client = OkHttpClient();

    fun GetRequest(url: String, callbackObject : Callback)
    {
        val request = Request.Builder()
            .url(url)
            .build()
        client
            .newCall(request)
            .enqueue(callbackObject)
    }
}