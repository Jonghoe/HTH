package com.hth.autoselfdrivingdrone.drone;

import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

import com.hth.autoselfdrivingdrone.MainActivity;

public class DroneControl {
    Drone drone;
    WifiManager wifiManager;
    boolean droneConnected = false;
    public DroneControl(WifiManager manager){
        wifiManager = manager;
    }
    public boolean connectDrone(){
        WifiInfo info = wifiManager.getConnectionInfo();
        drone = droneFactoryMethod(info.getSSID());
        if(drone == null)
            return false;
        droneConnected = drone.connectWithDrone();
        MainActivity.instance.showToast(info.getSSID()+"is connected");

        return true;
    }
    private Drone droneFactoryMethod(String ssid){
        if(ssid.contains("MAVIC"))
            return new MavicDrone();
        else if(ssid.contains("ARDrone"))
            return new ARDrone();
        else return null;
    }
}
