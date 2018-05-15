package com.example.android.myapplication.Utilities;

import android.graphics.Color;
import android.os.AsyncTask;
import android.util.Log;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.PolylineOptions;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

/**
 * Created by Admin on 5/22/2017.
 */

// Fetches data from url passed
public class DownloadTask extends AsyncTask<String, Void, String> implements AsyncDownload {

    public AsyncResponse delegate = null;


    private String routeDistance;
    private String routeDuration;
    private PolylineOptions lineOptions = null;
    private GoogleMap mGoogleMap;
    private String drawable;
    private String typec;

    public DownloadTask(GoogleMap gm){
        this.mGoogleMap = gm;
    }

    // Downloading data in non-ui thread
    @Override
    protected String doInBackground(String... url) {

        // For storing data from web service
        String data = "";
        drawable = url[1];
        typec = url[2];
        try {
            // Fetching the data from web service
            data = downloadUrl(url[0]);

        } catch (Exception e) {
            Log.d("Background Task", e.toString());
        }
        return data;
    }

    // Executes in UI thread, after the execution of
    // doInBackground()
    @Override
    protected void onPostExecute(String result) {
        super.onPostExecute(result);
        ParserTask parserTask = new ParserTask();

        // Invokes the thread for parsing the JSON data
        parserTask.execute(result);
        parserTask.delegate = DownloadTask.this;

        if(delegate!=null) {
            delegate.getDistance(parserTask.getDistanceETA(result));
            delegate.getDuration(parserTask.getDurationETA(result));
        }
    }


    private String downloadUrl(String strUrl) throws IOException {
        String data = "";
        InputStream iStream = null;
        HttpURLConnection urlConnection = null;
        try {
            URL url = new URL(strUrl);

            urlConnection = (HttpURLConnection) url.openConnection();

            // Connecting to url
            urlConnection.connect();

            // Reading data from url
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
           // Toast.makeText(, "ERROR IN ROUTING", Toast.LENGTH_LONG).show();
        } finally {
            iStream.close();
            urlConnection.disconnect();
        }
        return data;
    }

    @Override
    public void processFinish(PolylineOptions output) {
        if(drawable.contentEquals("true")){
            if(typec.contentEquals("red")){
                output.color(Color.RED);
            }else if ( typec.contentEquals("green")){
                output.color(Color.GREEN);
            }

            mGoogleMap.addPolyline(output);
        }
    }
}
