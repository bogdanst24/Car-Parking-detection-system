package com.example.android.myapplication.Utilities;

import android.graphics.Color;
import android.os.AsyncTask;

import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.PolylineOptions;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Created by Admin on 5/21/2017.
 */

public class ParserTask extends AsyncTask<String, Integer, List<List<HashMap<String, String>>>> {

    PolylineOptions lineOptions = null;
    public AsyncDownload delegate = null;

    public String getDurationETA(String... jsonData)  {
        JSONObject jObject;
        String duration="error";


        try {
            jObject = new JSONObject(jsonData[0]);
            JSONArray jRoutes = jObject.getJSONArray("routes");
            JSONArray JLegs = ( (JSONObject)jRoutes.get(0)).getJSONArray("legs");
            Object JDuration = ( (JSONObject)JLegs.get(0)).getJSONObject("duration").get("text");
            duration = JDuration.toString();
        } catch (JSONException e) {
            e.printStackTrace();
        }


        return duration;
    }

    public String getDistanceETA(String... jsonData)  {
        JSONObject jObject;
        String distance="error";

        try {
            jObject = new JSONObject(jsonData[0]);
            JSONArray jRoutes = jObject.getJSONArray("routes");
            JSONArray JLegs = ( (JSONObject)jRoutes.get(0)).getJSONArray("legs");
            Object JDistance = ( (JSONObject)JLegs.get(0)).getJSONObject("distance").get("text");
            distance = JDistance.toString();
        } catch (JSONException e) {
            e.printStackTrace();
        }


        return distance;
    }

    @Override
    protected List<List<HashMap<String, String>>> doInBackground(String... jsonData) {

        JSONObject jObject;


        List<List<HashMap<String, String>>> routes = null;

        try {
            jObject = new JSONObject(jsonData[0]);

            DirectionsJSONParser parser = new DirectionsJSONParser();

            // Starts parsing data
            routes = parser.parse(jObject);



        } catch (Exception e) {
            e.printStackTrace();
        }
        return routes;
    }

    @Override
    protected void onPostExecute(List<List<HashMap<String, String>>> result) {



        ArrayList<LatLng> points = null;

        MarkerOptions markerOptions = new MarkerOptions();

        // Traversing through all the routes
        for (int i = 0; i < result.size(); i++) {
            points = new ArrayList<LatLng>();
            lineOptions = new PolylineOptions();

            // Fetching i-th route
            List<HashMap<String, String>> path = result.get(i);

            // Fetching all the points in i-th route
            for (int j = 0; j < path.size(); j++) {
                HashMap<String, String> point = path.get(j);

                double lat = Double.parseDouble(point.get("lat"));
                double lng = Double.parseDouble(point.get("lng"));
                LatLng position = new LatLng(lat, lng);

                points.add(position);
            }

            // Adding all the points in the route to LineOptions
            lineOptions.addAll(points);
            lineOptions.width(5);
            lineOptions.color(Color.RED);
        }

        // Drawing polyline in the Google Map for the i-th route
        delegate.processFinish(lineOptions);
        this.setLineOptions(lineOptions);

    }

    public PolylineOptions getLineOptions() {
        return lineOptions;
    }

    public void setLineOptions(PolylineOptions lineOptions) {
        this.lineOptions = lineOptions;
    }
}
