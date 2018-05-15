package com.example.android.myapplication.Login;

/**
 * Add your package below. Package name can be found in the project's AndroidManifest.xml file.
 * This is the package name our example uses:
 *
 * package com.example.android.justjava;
 */



import android.app.Activity;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.Toast;

import com.example.android.myapplication.R;


/**
 * This app displays an order form to order coffee.
 */
public class Login extends AppCompatActivity {

    EditText usernameEt,passwordEt;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);

        usernameEt = (EditText) findViewById(R.id.username_edit);
        passwordEt = (EditText) findViewById(R.id.password_edit);

        showKeyboard();
    }


    public void login_proceed(View view){
        String username = usernameEt.getText().toString();
        String password = passwordEt.getText().toString();

        String type = "login";

        Boolean inputValidation = checkInputLogin(username,password);

        if(inputValidation) {
            BackgroundWorker backgroundWorker = new BackgroundWorker(this);
            backgroundWorker.execute(type, username, password);
        }

    }

    public void open_register(View view){
        startActivity(new Intent(this, Register.class));
    }

    private void showKeyboard(){
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



    protected boolean checkInputLogin(String user_name, String password) {
        if(user_name.equals("")){
            Toast.makeText(this, "Empty username", Toast.LENGTH_SHORT).show();
            return false;
        }
        if(password.equals("")){
            Toast.makeText(this, "Empty password", Toast.LENGTH_SHORT).show();
            return false;
        }
        return true;
    }

}