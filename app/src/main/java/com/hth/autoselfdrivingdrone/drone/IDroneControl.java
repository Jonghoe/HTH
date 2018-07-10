package com.hth.autoselfdrivingdrone.drone;

public interface IDroneControl {
    void setLeftControl(float throttle, float yaw);
    void setRightControl(float pitch, float roll);
}
