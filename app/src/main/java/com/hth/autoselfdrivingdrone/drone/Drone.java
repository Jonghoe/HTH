package com.hth.autoselfdrivingdrone.drone;

public abstract class Drone implements IDroneCamera, IDroneComunicator, IDroneControl{
    protected boolean isConnected;
    protected boolean isLand;
    public boolean getLand(){
        return isLand;
    }
}
