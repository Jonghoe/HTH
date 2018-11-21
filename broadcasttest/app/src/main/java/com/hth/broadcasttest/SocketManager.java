package com.hth.broadcasttest;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class SocketManager {
    private static Socket client;
    private static Socket socket1;
    private static Socket socket2;
    private static BufferedReader networkReader1;
    private static BufferedWriter networkWriter1;
    private static BufferedReader networkReader2;
    private static BufferedWriter networkWriter2;
    private static boolean send;
    private static boolean send2;
    static int comp=0;
    private String ip = "192.168.1.3"; // IP
    private static ServerSocket server;
    private int port1 = 9999; // PORT번호 video
    private int port2 = 8888; // PORT번호 command

    public void setSocket() throws IOException {
        new Thread() {
            @Override
            public void run() {
                try {
//                    socket1 = new Socket(ip, port1);
//                    networkWriter1 = new BufferedWriter(new OutputStreamWriter(socket1.getOutputStream()));
//                    networkReader1 = new BufferedReader(new InputStreamReader(socket1.getInputStream()));
                    socket2 = new Socket(ip, port2);
                    networkWriter2 = new BufferedWriter(new OutputStreamWriter(socket2.getOutputStream()));
                    networkReader2 = new BufferedReader(new InputStreamReader(socket2.getInputStream()));

                } catch (Exception e) {
                    System.out.println(e);
                    e.printStackTrace();
                }
            }
        }.start();

    }

    public void setSocket1() throws IOException {
        try {
            socket1 = new Socket(ip, port1);
            networkWriter1 = new BufferedWriter(new OutputStreamWriter(socket1.getOutputStream()));
            networkReader1 = new BufferedReader(new InputStreamReader(socket1.getInputStream()));
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }

    public void setSocket2() throws IOException {
        try {
            socket2 = new Socket(ip, port2);
            networkWriter2 = new BufferedWriter(new OutputStreamWriter(socket2.getOutputStream()));
            networkReader2 = new BufferedReader(new InputStreamReader(socket2.getInputStream()));
        } catch (Exception e) {
            System.out.println(e);
            e.printStackTrace();
        }
    }

    public void closeSocket() throws IOException {
        try {
            socket1.close();
            socket2.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void sendVideoClient(byte[] videoBuffer) {
        OutputStream out = null;
        comp++;
        try {
            Log.d("buffer_len",videoBuffer.length+" ");
            out = client.getOutputStream();
            out.write(videoBuffer);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void sendVideo(byte[] videoBuffer) {
        OutputStream out = null;
        comp++;
        try {
            Log.d("buffer_len",videoBuffer.length+" ");
            out = socket1.getOutputStream();
            out.write(videoBuffer);

        } catch (IOException e) {
            e.printStackTrace();
        }

        send = false;
    }
    public byte[] readCommand2() throws IOException {
        try {
            InputStream receiver = socket1.getInputStream();
            byte[] data=new byte[5];
            receiver.read(data,0,4);

            int len = 0;
            for(int i=0;i<4;i++)
            {
                len |= ((data[i]& 0xff)<< (8*i));
            }
            Log.d("buffer_len",len+" ");
            byte[] recvBuffer = new byte[len];
            receiver.read(recvBuffer,0,len);
            return recvBuffer;

        } catch (Exception e) {
            return null;
        }

    }
//    public byte[] readCommand() throws IOException {
//        try {
//            InputStream receiver = socket2.getInputStream();
//            byte[] data=new byte[5];
//            receiver.read(data,0,4);
//
//            int len = 0;
//            for(int i=0;i<4;i++)
//            {
//                len |= ((data[i]& 0xff)<< (8*i));
//            }
//            len+=1;
//            byte[] recvBuffer = new byte[len];
//            receiver.read(recvBuffer,0,len);
//            recvBuffer[len-1]='\0';
//            return recvBuffer;
//
//        } catch (Exception e) {
//            return null;
//        }
//
//    }

    public byte[] readCommand() throws IOException {
        try {
            InputStream receiver = socket2.getInputStream();
            int len = 16;
            byte[] recvBuffer = new byte[len];

            receiver.read(recvBuffer,0,len);
            //recvBuffer[16]='\0';

            return recvBuffer;

        } catch (Exception e) {
            return null;
        }

    }

}

