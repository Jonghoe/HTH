package com.hth.autoselfdrivingdrone.drone;

public class ARDrone extends Drone {
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

    }

    @Override
    public boolean isConnected() {
        return isConnected;
    }

    @Override
    public void setLeftControl(float throttle, float yaw) {

    }

    @Override
    public void setRightControl(float pitch, float roll) {

    }
}
