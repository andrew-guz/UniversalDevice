package com.example.agzviewstest

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.Button
import android.widget.CheckedTextView
import android.widget.EditText

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        findViewById<Button>(R.id.calculate_btn).setOnClickListener {
            var number1_txt:EditText = findViewById<EditText>(R.id.number1);
            var number2_txt:EditText = findViewById<EditText>(R.id.number2);
            var sum_txt = findViewById<CheckedTextView>(R.id.textResult);
            if (number1_txt.text.any() &&
                number2_txt.text.any())
            {
                var num1 = number1_txt.text.toString().toInt();
                var num2 = number2_txt.text.toString().toInt();
                var sum = num1 + num2;
                sum_txt.text = sum.toString();
            }
            else {
                sum_txt.text = "";
                val intent = Intent(this@MainActivity, AdditionalActivity::class.java);
                startActivity(intent);
            }
        }
    }
}
