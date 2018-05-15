package com.example.android.myapplication.Login;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.Toast;

import com.example.android.myapplication.R;

public class Register extends AppCompatActivity {
    EditText nameEt, surnameEt, ageEt, usernameEt, passwordEt;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.register);

        nameEt = (EditText) findViewById(R.id.add_name);
        surnameEt = (EditText) findViewById(R.id.add_surname);
        ageEt = (EditText) findViewById(R.id.add_age);
        usernameEt = (EditText) findViewById(R.id.add_username);
        passwordEt = (EditText) findViewById(R.id.add_password);

        showKeyboard();
    }

    public void register_proceed(View view){
        String name = nameEt.getText().toString();
        String surname = surnameEt.getText().toString();
        String age = ageEt.getText().toString();
        String username = usernameEt.getText().toString();
        String password = passwordEt.getText().toString();

        String type = "register";

        boolean inputValidate = checkInputRegister(name,surname,age,username,password);

        if(inputValidate) {
            BackgroundWorker backgroundWorker = new BackgroundWorker(this);
            backgroundWorker.execute(type, name, surname, age, username, password);
        }
    }

    private void showKeyboard(){
        nameEt.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });
        surnameEt.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });
        ageEt.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });
        usernameEt.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });
        passwordEt.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });
    }
    public void hideKeyboard(View view) {
        InputMethodManager inputMethodManager =(InputMethodManager)getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

    private Boolean checkInputRegister(String name, String surname, String age, String username, String password) {
        if(name.contentEquals("")){
            Toast.makeText(this, "Empty name", Toast.LENGTH_SHORT).show();
            return false;
        }
        if(surname.contentEquals("")){
            Toast.makeText(this, "Empty surname", Toast.LENGTH_SHORT).show();
            return false;
        }
        if(age.contentEquals("")){
            Toast.makeText(this, "Empty age", Toast.LENGTH_SHORT).show();
            return false;
        }
        if(username.contentEquals("")){
            Toast.makeText(this, "Empty username", Toast.LENGTH_SHORT).show();
            return false;
        }
        if(password.contentEquals("")){
            Toast.makeText(this, "Empty password", Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

    public void changeToLogin(View view){
        Intent intent = new Intent(this, Login.class);
        this.startActivity(intent);
    }
}
