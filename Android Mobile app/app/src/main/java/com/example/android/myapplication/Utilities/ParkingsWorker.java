package com.example.android.myapplication.Utilities;

import android.content.Context;
import android.os.AsyncTask;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.ArrayList;

/**
 * Created by Admin on 5/22/2017.
 */

public class ParkingsWorker extends AsyncTask<String, Void, String>{

    Context context;
    String url ="http://dezbateri.ro/android/parkings.php";
    ArrayList<Parking> allParkings = new ArrayList<Parking>();
    public AsyncParkings delegate = null;

    public ArrayList<Parking> getAllParkings() {
        return allParkings;
    }


    @Override
    protected String doInBackground(String... params) {
        String data = "";
        try {
            data = downloadUrl(url);
        } catch (Exception e) {
            Log.d("Background Task", e.toString());
        }
        return data;
    }


    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        parseResult(result);


    }

    private void parseResult(String... result) {

        JSONArray jObject;

        try {
            jObject = new JSONArray(result[0]);

            for(int index = 0;index < jObject.length(); index++) {
                JSONObject json = jObject.getJSONObject(index);
                Parking parking = new Parking();
                parking.setId(Integer.parseInt(json.get("id").toString()));
                parking.setLat(json.get("lat").toString());
                parking.setLon(json.get("lon").toString());
                parking.setName(json.get("name").toString());
                parking.setSpots_total(Integer.parseInt(json.get("spots_total").toString()));
                parking.setSpots_free(Integer.parseInt(json.get("spots_free").toString()));
                allParkings.add(parking);
            }
            delegate.getParkings(allParkings);

        }catch (Exception e) {
            e.printStackTrace();
        }
    }

    private String downloadUrl(String strUrl) throws IOException {
        String data = "";
        InputStream iStream = null;
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(strUrl);
            urlConnection = (HttpURLConnection) url.openConnection();
            urlConnection.connect();
            iStream = urlConnection.getInputStream();
            BufferedReader br = new BufferedReader(new InputStreamReader(iStream));
            StringBuffer sb = new StringBuffer();
            String line = "";
            while ((line = br.readLine()) != null) {
                sb.append(line);
            }
            data = sb.toString();
            br.close();
        } catch (Exception e) {
            //Toast.makeText(, "ERROR IN ROUTING", Toast.LENGTH_LONG).show();
        } finally {
            iStream.close();
            urlConnection.disconnect();
        }
        return data;
    }

}
