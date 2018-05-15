package com.example.android.myapplication.Utilities;

/**
 * Created by Admin on 5/26/2017.
 */

public class Parking {

    private int id;
    private String address;
    private String lat;
    private String lon;
    private String name;
    private int spots_total;
    private int spots_free;

    public Parking(){

    }

    public Parking(int id, String address, String lat, String lon, String name, int spots_total, int spots_free) {
        this.id = id;
        this.address = address;
        this.lat = lat;
        this.lon = lon;
        this.name = name;
        this.spots_total = spots_total;
        this.spots_free = spots_free;
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getLat() {
        return lat;
    }

    public void setLat(String lat) {
        this.lat = lat;
    }

    public String getLon() {
        return lon;
    }

    public void setLon(String lon) {
        this.lon = lon;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getSpots_total() {
        return spots_total;
    }

    public void setSpots_total(int spots_total) {
        this.spots_total = spots_total;
    }

    public int getSpots_free() {
        return spots_free;
    }

    public void setSpots_free(int spots_free) {
        this.spots_free = spots_free;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
        this.address = address;
    }
}
