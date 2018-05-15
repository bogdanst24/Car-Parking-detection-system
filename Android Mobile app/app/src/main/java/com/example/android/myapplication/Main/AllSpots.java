package com.example.android.myapplication.Main;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.ListFragment;
import android.view.View;
import android.widget.ListView;
import android.widget.SimpleAdapter;

import com.example.android.myapplication.R;
import com.example.android.myapplication.Utilities.AsyncParkings;
import com.example.android.myapplication.Utilities.Parking;
import com.example.android.myapplication.Utilities.ParkingsWorker;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.ExecutionException;


public class AllSpots extends ListFragment implements AsyncParkings {

    private ArrayList<Parking> allParkings;

    int[] flags = new int[]{
            R.mipmap.park_icon
    };

    List<HashMap<String,String>> aList = null;

    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        allParkings = new ArrayList<>();
        ParkingsWorker parkingsWorker = new ParkingsWorker();
        JSONArray jObject;
        try {
            String parkings = parkingsWorker.execute().get();

            jObject = new JSONArray(parkings);
            for(int index = 0;index < jObject.length(); index++) {
                JSONObject json = jObject.getJSONObject(index);
                Parking parking = new Parking();
                parking.setId(Integer.parseInt(json.get("id").toString()));
                parking.setLat(json.get("lat").toString());
                parking.setLon(json.get("lon").toString());
                parking.setName(json.get("name").toString());
                parking.setAddress(json.get("address").toString());
                parking.setSpots_total(Integer.parseInt(json.get("spots_total").toString()));
                parking.setSpots_free(Integer.parseInt(json.get("spots_free").toString()));
                allParkings.add(parking);
             }

        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (ExecutionException e) {
            e.printStackTrace();
        }catch (Exception e) {
        e.printStackTrace();
        }
        parkingsWorker.delegate = AllSpots.this;




        aList = new ArrayList<HashMap<String,String>>();
        for(Parking p : allParkings){
            HashMap<String, String> hm = new HashMap<String,String>();
            hm.put("parking_name", p.getName());
            hm.put("parking_address","Address : " + p.getAddress());
            hm.put("parking_coordinates","Coordinates : " + p.getLat() + ", "+p.getLon());
            hm.put("parking_total","# Total lots : " + p.getSpots_total());
            hm.put("parking_free","# Free lots : " + p.getSpots_free());
            hm.put("flag", Integer.toString(flags[0]) );
            hm.put("lat",p.getLat());
            hm.put("lon",p.getLon());
            aList.add(hm);
        }


        String[] from = { "flag","parking_name","parking_address", "parking_coordinates",
                "parking_total","parking_free"};
        int[] to = { R.id.flag,R.id.parking_name,R.id.parking_address,R.id.parking_coordinates
                ,R.id.parking_total,R.id.parking_free};




        SimpleAdapter adapter = new SimpleAdapter(
                getActivity(), aList, R.layout.fragment_all_spots, from, to);

        setListAdapter(adapter);

    }

    @Override
    public void onListItemClick(ListView l, View v, int position, long id) {
        HashMap<String, String> hm = aList.get(position);
        Double lat = Double.parseDouble(hm.get("lat"));
        Double lon = Double.parseDouble(hm.get("lon"));
        String parkName = hm.get("name");
        String uri = String.format(Locale.ENGLISH, "http://maps.google.com/maps?daddr=%f,%f (%s)", lon, lat, parkName);
        Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(uri));
        intent.setPackage("com.google.android.apps.maps");
        startActivity(intent);
    }


    @Override
    public void getParkings(ArrayList<Parking> allParkings) {
        allParkings = new ArrayList<>();
        for(Parking p : allParkings){
            allParkings.add(p);
        }
    }
}
