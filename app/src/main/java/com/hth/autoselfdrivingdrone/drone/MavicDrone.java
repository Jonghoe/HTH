package com.hth.autoselfdrivingdrone.drone;

import com.hth.autoselfdrivingdrone.MainActivity;

import dji.common.error.DJIError;
import dji.common.flightcontroller.virtualstick.FlightControlData;
import dji.common.util.CommonCallbacks;
import dji.sdk.base.BaseProduct;
import dji.sdk.flightcontroller.FlightController;
import dji.sdk.products.Aircraft;
import dji.sdk.sdkmanager.DJISDKManager;


public class MavicDrone extends Drone {
    private Aircraft aircraft;
    private FlightController controller;
    private FlightControlData controlData;
    public MavicDrone(){
        super();
        setAircraft();
        controller = aircraft.getFlightController();
        controlData = new FlightControlData(0,0,0,0);
    }
    private synchronized void setAircraft(){
        aircraft = (Aircraft)DJISDKManager.getInstance().getProduct();
    }
    private synchronized BaseProduct getProduct(){
        return DJISDKManager.getInstance().getProduct();
    }
    private synchronized Aircraft getAircraft(){
        return aircraft;
    }
    @Override
    public int getCamera() {
        return 0;
    }

    @Override
    public boolean connectWithDrone() {
        return false;
    }

    @Override
    public void sendControlData() {
        controller.sendVirtualStickFlightControlData(controlData, new CommonCallbacks.CompletionCallback() {
            @Override
            public void onResult(DJIError djiError) {
                MainActivity.instance.showToast(djiError.getDescription());
            }
        });
    }

    @Override
    public boolean isConnected() {
        return getAircraft() != null && getProduct() instanceof Aircraft;
    }

    @Override
    public void setLeftControl(float throttle, float yaw) {
        controlData.setYaw(yaw);
        controlData.setVerticalThrottle(throttle);
    }

    @Override
    public void setRightControl(float pitch, float roll) {
        controlData.setPitch(pitch);
        controlData.setRoll(roll);
    }
}
