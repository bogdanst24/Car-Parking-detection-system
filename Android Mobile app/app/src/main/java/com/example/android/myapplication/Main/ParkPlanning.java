package com.example.android.myapplication.Main;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.support.v4.app.ActivityCompat;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.example.android.myapplication.R;
import com.example.android.myapplication.Utilities.AsyncParkings;
import com.example.android.myapplication.Utilities.AsyncResponse;
import com.example.android.myapplication.Utilities.DownloadTask;
import com.example.android.myapplication.Utilities.Parking;
import com.example.android.myapplication.Utilities.ParkingDuration;
import com.example.android.myapplication.Utilities.ParkingsWorker;
import com.google.android.gms.common.api.Status;
import com.google.android.gms.location.places.Place;
import com.google.android.gms.location.places.ui.PlaceAutocompleteFragment;
import com.google.android.gms.location.places.ui.PlaceSelectionListener;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapView;
import com.google.android.gms.maps.MapsInitializer;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.concurrent.ExecutionException;

import static android.content.Context.LOCATION_SERVICE;


public class ParkPlanning extends Fragment implements OnMapReadyCallback, AsyncResponse, AsyncParkings {

    private GoogleMap mGoogleMap;
    private MapView mapView;
    private View mView;
    private LatLng position = new LatLng(46.770647, 23.589646);
    private Marker userMarker = null;
    private Boolean firstTime = true;
    private LatLng searchPos = null;
    private Marker parkingMarker = null;
    private MarkerOptions options = null;

    private ArrayList<LatLng> markerPoints;

    private LocationManager locationManager;
    private LocationListener locationListener;

    private ArrayList<ParkingDuration> allDurations;

    private TextView upText, downText;

    private Criteria criteria;
    private String provider;


    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{
                        Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.ACCESS_COARSE_LOCATION,
                        Manifest.permission.INTERNET
                }, 10);
                return;
            }

            locationManager = (LocationManager) getActivity().getSystemService(LOCATION_SERVICE);
            locationListener = new LocationListener() {

                @Override
                public void onLocationChanged(Location location) {
                    setPosition(new LatLng(location.getLatitude(), location.getLongitude()));
                    updatePosition();
                }

                @Override
                public void onStatusChanged(String provider, int status, Bundle extras) {
                }

                @Override
                public void onProviderEnabled(String provider) {

                }

                @Override
                public void onProviderDisabled(String arg0) {
                    Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                    startActivity(intent);
                }
            };
            locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, locationListener);
            locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 1000, 0, locationListener);

        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        if (mView == null) {
            mView = inflater.inflate(R.layout.fragment_park_planning, container, false);
        }
        return mView;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        switch (requestCode) {
            case 10:
                if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                        return;
                    }
                    locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 1000, 0, locationListener);
                    locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 0, locationListener);

                }
                return;
        }
    }


    @Override
    public void onViewCreated(View view, Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        mapView = (MapView) mView.findViewById(R.id.park_planning_map);
        if (mapView != null) {
            mapView.onCreate(null);
            mapView.onResume();
            mapView.getMapAsync(this);
        }

    }


    @Override
    public void onMapReady(GoogleMap googleMap) {
        MapsInitializer.initialize(getContext());

        mGoogleMap = googleMap;
        googleMap.setMapType(GoogleMap.MAP_TYPE_NORMAL);

        // Initializing array List
        markerPoints = new ArrayList<LatLng>();

        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {

            return;
        }
        mGoogleMap.setMyLocationEnabled(true);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{
                        Manifest.permission.ACCESS_FINE_LOCATION,
                        Manifest.permission.ACCESS_COARSE_LOCATION,
                        Manifest.permission.INTERNET
                }, 10);
                return;
            }

        } else {

            locationManager = (LocationManager) getActivity().getSystemService(LOCATION_SERVICE);

            criteria = new Criteria();

            provider = locationManager.getBestProvider(criteria, true);


            locationListener = new LocationListener() {

                @Override
                public void onLocationChanged(Location location) {
                    setPosition(new LatLng(location.getLatitude(), location.getLongitude()));
                    updatePosition();
                }

                @Override
                public void onStatusChanged(String provider, int status, Bundle extras) {
                }

                @Override
                public void onProviderEnabled(String provider) {

                }

                @Override
                public void onProviderDisabled(String arg0) {
                    Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                    startActivity(intent);
                }
            };
            // locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, locationListener);
            locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER,1000, 0, locationListener);

        }

        MarkerOptions marker = new MarkerOptions().position(getPosition());
        marker.icon(BitmapDescriptorFactory.fromResource(R.mipmap.map_icon_car));
        userMarker = mGoogleMap.addMarker(marker);
        markerPoints.add(marker.getPosition());

        mGoogleMap.animateCamera(CameraUpdateFactory.newLatLng(new LatLng(46.770647, 23.589646)));
        mGoogleMap.setMinZoomPreference(14.0f);
        mGoogleMap.setMaxZoomPreference(20.0f);

        PlaceAutocompleteFragment autocompleteFragment = (PlaceAutocompleteFragment)
                getActivity().getFragmentManager().findFragmentById(R.id.place_autocomplete_fragment);

        autocompleteFragment.setBoundsBias(new LatLngBounds(
                new LatLng(46.678343, 23.466695),
                new LatLng(46.835180, 23.700271)));


        autocompleteFragment.setOnPlaceSelectedListener(new PlaceSelectionListener() {
            @Override
            public void onPlaceSelected(Place place) {
                // TODO: Get info about the selected place.
                LatLng fromadd = place.getLatLng();
                mGoogleMap.animateCamera(CameraUpdateFactory.newLatLng(fromadd));
                if (markerPoints.size() > 1) {
                    markerPoints.remove(1);
                    refreshWindow();
                }

                createRoute(place.getLatLng());
            }

            @Override
            public void onError(Status status) {
                // TODO: Handle the error.
                Toast.makeText(getContext(), "ERROR", Toast.LENGTH_LONG).show();
            }
        });


        mGoogleMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {

            @Override
            public void onMapClick(LatLng point) {
                if (markerPoints.size() > 1) {
                    markerPoints.remove(1);
                    refreshWindow();
                }

                createRoute(point);

            }
        });

    }

    private void createRoute(LatLng point) {

        markerPoints.add(point);

        options = new MarkerOptions();
        options.position(point);

        if (markerPoints.size() == 1) {
            options.icon(BitmapDescriptorFactory.fromResource(R.mipmap.map_icon_car));
        } else if (markerPoints.size() == 2) {
            options.icon(BitmapDescriptorFactory.fromResource(R.mipmap.icon_finish));
            options.title("Destination");
        }

        mGoogleMap.addMarker(options);

        if (markerPoints.size() >= 2) {


            ParkingsWorker parkingsWorker = new ParkingsWorker();
            parkingsWorker.execute();
            parkingsWorker.delegate = ParkPlanning.this;
        }
    }

    public LatLng getPosition() {
        return position;
    }

    public void setPosition(LatLng position) {
        this.position = position;
    }

    public void updatePosition() {
        if(getPosition() != null && userMarker!=null) {
            userMarker.setPosition(getPosition());
            if (firstTime) {
                mGoogleMap.animateCamera(CameraUpdateFactory.newLatLng(getPosition()));
                firstTime = false;
            }
            markerPoints.set(0, getPosition());

        }
    }

    private void refreshWindow() {
        mGoogleMap.clear();
        MarkerOptions marker = new MarkerOptions().position(getPosition());
        marker.icon(BitmapDescriptorFactory.fromResource(R.mipmap.map_icon_car));
        userMarker = mGoogleMap.addMarker(marker);
    }

    private String getDirectionsUrl(LatLng origin, LatLng dest) {
        String str_origin = "origin=" + origin.latitude + "," + origin.longitude;
        String str_dest = "destination=" + dest.latitude + "," + dest.longitude;
        String sensor = "sensor=false";
        String parameters = str_origin + "&" + str_dest + "&" + sensor;
        String output = "json";
      // String key = "key=" + "AIzaSyC4CoMA0Eg5_FWDcI89cQAGouYZSDb3o5I" + "&";
        String url = "https://maps.googleapis.com/maps/api/directions/" + output + "?"  + parameters;
        return url;
    }

    @Override
    public void getDistance(String output) {

    }

    @Override
    public void getDuration(String output) {

        //allDurations.add(duration);

    }

    @Override
    public void getParkings(ArrayList<Parking> allParkings) {
        allDurations = new ArrayList<>();
        for(Parking p : allParkings){
            LatLng parking = new LatLng(Double.parseDouble(p.getLon()),Double.parseDouble(p.getLat()));
            String url = getDirectionsUrl(parking,markerPoints.get(1));

            DownloadTask downloadTask = new DownloadTask(mGoogleMap);
            downloadTask.delegate = ParkPlanning.this;
            try {
                String std_result = downloadTask.execute(url, "false", "red").get();
                JSONObject jObject;
                String duration="error";
                String distance="error";
                try {
                    jObject = new JSONObject(std_result);
                    JSONArray jRoutes = jObject.getJSONArray("routes");
                    JSONArray JLegs = ( (JSONObject)jRoutes.get(0)).getJSONArray("legs");
                    Object JDuration = ( (JSONObject)JLegs.get(0)).getJSONObject("duration").get("text");
                    duration = JDuration.toString();
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                try {
                    jObject = new JSONObject(std_result);
                    JSONArray jRoutes = jObject.getJSONArray("routes");
                    JSONArray JLegs = ( (JSONObject)jRoutes.get(0)).getJSONArray("legs");
                    Object JDistance = ( (JSONObject)JLegs.get(0)).getJSONObject("distance").get("text");
                    distance = JDistance.toString();
                } catch (JSONException e) {
                    e.printStackTrace();
                }
                String[] split = duration.split("\\s+");
                Float dur = Float.parseFloat(split[0]);
                String[] split2 = distance.split("\\s+");
                Float dis = Float.parseFloat(split2[0]);
                ParkingDuration pd = new ParkingDuration();
                pd.setParking(p);
                pd.setDuration(dur);
                pd.setDistance(dis);
                allDurations.add(pd);

            } catch (InterruptedException e) {
                e.printStackTrace();
            } catch (ExecutionException e) {
                e.printStackTrace();
            }
        }
        getClosestParking();

    }

    private void getClosestParking() {
        float minim = 99999;
        LatLng origin;
        LatLng dest;
        String url;
        DownloadTask downloadTask;
        Boolean single = true;

        Float destDuration = 0.f;
        int parkID = 0;

        for (ParkingDuration pd : allDurations) {
            destDuration = getDurationToDestination(pd.getParking());
            if ((pd.getDuration()) < minim) {
                if (pd.getParking().getSpots_free() > 0) {
                    minim = pd.getDuration();
                    parkID = pd.getParking().getId();
                }
            }
        }

        for (ParkingDuration pd : allDurations) {
            if (pd.getParking().getId() == parkID && single == true) {
                LatLng parkingPos = new LatLng(Double.parseDouble(pd.getParking().getLon()), Double.parseDouble(pd.getParking().getLat()));
                MarkerOptions marker = new MarkerOptions().position(parkingPos);
                marker.icon(BitmapDescriptorFactory.fromResource(R.mipmap.park_icon));
                marker.title(pd.getParking().getName());
                marker.snippet("Empty parking lots: " + pd.getParking().getSpots_free());
                parkingMarker = mGoogleMap.addMarker(marker);
                parkingMarker.showInfoWindow();

                origin = markerPoints.get(0);
                dest = parkingPos;
                url = getDirectionsUrl(origin, dest);
                downloadTask = new DownloadTask(mGoogleMap);
                downloadTask.execute(url, "true", "red");
                downloadTask.delegate = ParkPlanning.this;

                origin = parkingPos;
                dest = markerPoints.get(1);
                url = getDirectionsUrl(origin, dest);
                downloadTask = new DownloadTask(mGoogleMap);
                downloadTask.execute(url, "true", "green");
                downloadTask.delegate = ParkPlanning.this;


                LinearLayout ll = (LinearLayout) mView.findViewById(R.id.eta_planning);
                ll.setVisibility(View.VISIBLE);
                upText = (TextView) mView.findViewById(R.id.eta_planning_down);
                String text = "ETA Parking : " + destDuration + " MIN";
                upText.setText(text);

                downText = (TextView) mView.findViewById(R.id.eta_planning_up);
                String text2 = "ETA Dest. : " + (pd.getDuration() + destDuration) + " MIN";
                downText.setText(text2);

                single = false;


            }
        }
    }


    private Float getDurationToDestination(Parking p) {
        Float dur = 0.f;
        LatLng parking = new LatLng(Double.parseDouble(p.getLon()),Double.parseDouble(p.getLat()));
        String url = getDirectionsUrl(markerPoints.get(0),parking);
        DownloadTask downloadTask = new DownloadTask(mGoogleMap);
        try {
            String std_result = downloadTask.execute(url, "false","green").get();
            JSONObject jObject;
            String duration="error";
            String distance="error";
            try {
                jObject = new JSONObject(std_result);
                JSONArray jRoutes = jObject.getJSONArray("routes");
                JSONArray JLegs = ( (JSONObject)jRoutes.get(0)).getJSONArray("legs");
                Object JDuration = ( (JSONObject)JLegs.get(0)).getJSONObject("duration").get("text");
                duration = JDuration.toString();
            } catch (JSONException e) {
                e.printStackTrace();
            }

            String[] split = duration.split("\\s+");
            dur = Float.parseFloat(split[0]);
            downloadTask.delegate = ParkPlanning.this;
        } catch (InterruptedException e) {
            e.printStackTrace();
        } catch (ExecutionException e) {
            e.printStackTrace();
        }
        return dur;
    }
}
