package com.example.android.myapplication.Utilities;

/**
 * Created by Admin on 5/27/2017.
 */

public class ParkingDuration {

    private Parking parking;
    private float duration;
    private float distance;

    public ParkingDuration(){

    }

    public ParkingDuration(Parking parking, float duration, float distance) {
        this.parking = parking;
        this.duration = duration;
        this.distance = distance;
    }

    public Parking getParking() {
        return parking;
    }

    public void setParking(Parking parking) {
        this.parking = parking;
    }

    public float getDuration() {
        return duration;
    }

    public void setDuration(float duration) {
        this.duration = duration;
    }

    public float getDistance() {
        return distance;
    }

    public void setDistance(float distance) {
        this.distance = distance;
    }
}
