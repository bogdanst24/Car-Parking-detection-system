package com.example.android.myapplication.Main;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
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

import com.example.android.myapplication.R;
import com.example.android.myapplication.Utilities.AsyncResponse;
import com.example.android.myapplication.Utilities.DownloadTask;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapView;
import com.google.android.gms.maps.MapsInitializer;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import java.util.ArrayList;

import static android.content.Context.LOCATION_SERVICE;


public class Routing extends Fragment implements OnMapReadyCallback,AsyncResponse

    {

        private GoogleMap mGoogleMap;
        private MapView mapView;
        private View mView;
        private LatLng position = new LatLng(46.770647, 23.589646);
        private Marker userMarker = null;
        private Boolean firstTime = true;

        ArrayList<LatLng> markerPoints;

        private LocationManager locationManager;
        private LocationListener locationListener;
        TextView distance, duration;


        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            mView = inflater.inflate(R.layout.fragment_routing, container, false);
            return mView;
        }


        @Override
        public void onCreate(@Nullable Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);



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
                locationManager.requestLocationUpdates(LocationManager.NETWORK_PROVIDER, 5000, 0, locationListener);
            }
        }

        @Override
        public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
            switch (requestCode) {
                case 10:
                    if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                        if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                            return;
                        }
                        locationManager.requestLocationUpdates("gps", 5000, 0, locationListener);

                    }
                    return;
            }
        }

        @Override
        public void onViewCreated(View view, Bundle savedInstanceState) {
            super.onViewCreated(view, savedInstanceState);

            mapView = (MapView) mView.findViewById(R.id.closer_spot_map);
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

            if (ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(getContext(), Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED)
                return;


            mGoogleMap.setMyLocationEnabled(true);

            MarkerOptions marker = new MarkerOptions().position(getPosition());
            marker.icon(BitmapDescriptorFactory.fromResource(R.mipmap.map_icon_car));
            userMarker = mGoogleMap.addMarker(marker);

            mGoogleMap.animateCamera(CameraUpdateFactory.newLatLng(new LatLng(46.770647, 23.589646)));
            mGoogleMap.setMinZoomPreference(14.0f);
            mGoogleMap.setMaxZoomPreference(20.0f);

            // Initializing array List
            markerPoints = new ArrayList<LatLng>();

            mGoogleMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {

                @Override
                public void onMapClick(LatLng point) {

                    if (markerPoints.size() > 1) {
                        markerPoints.clear();
                        refreshWindow();
                    }

                    markerPoints.add(point);

                    MarkerOptions options = new MarkerOptions();

                    options.position(point);

                    if (markerPoints.size() == 1) {
                        options.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_GREEN));
                    } else if (markerPoints.size() == 2) {
                        options.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_RED));
                    }

                    mGoogleMap.addMarker(options);

                    if (markerPoints.size() >= 2) {
                        LatLng origin = markerPoints.get(0);
                        LatLng dest = markerPoints.get(1);

                        String url = getDirectionsUrl(origin, dest);

                        DownloadTask downloadTask = new DownloadTask(mGoogleMap);
                        downloadTask.execute(url,"true","red");
                        downloadTask.delegate = Routing.this;




                    }
                }
            });


        }


    public LatLng getPosition() {
        return position;
    }

    public void setPosition(LatLng position) {
        this.position = position;
    }

    public void updatePosition() {
        userMarker.setPosition(getPosition());
        if (firstTime) {
            mGoogleMap.animateCamera(CameraUpdateFactory.newLatLng(getPosition()));
            firstTime = false;
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
        String url = "https://maps.googleapis.com/maps/api/directions/" + output + "?" + parameters;
        return url;
    }

    @Override
    public void getDistance(String output) {
        LinearLayout ll = (LinearLayout) mView.findViewById(R.id.routing_layout);
        ll.setVisibility(View.VISIBLE);
        distance = (TextView)mView.findViewById(R.id.routing_distance);
        String text = output;
        distance.setText(text);
    }

    @Override
    public void getDuration(String output) {
        LinearLayout ll = (LinearLayout) mView.findViewById(R.id.routing_layout);
        ll.setVisibility(View.VISIBLE);
        duration = (TextView)mView.findViewById(R.id.routing_duration);
        String text = output;
        duration.setText(text);
    }
}
