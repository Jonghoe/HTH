package com.hth.autoselfdrivingdrone.drone;

public interface IDroneComunicator {
    boolean connectWithDrone();
    void sendControlData();
    boolean isConnected();
}
