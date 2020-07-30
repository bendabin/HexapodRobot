
/*
 
File Name:-  MainActivity.Java
Author:-     Ben Dabin  
Date:-       29/10/13
Version:-    3.0.0

          Program Description:

  	The following program deals with the GUI interface control and the bluetooth communication 
 
 */
package com.Hexapod_Bluetooth;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Method;
//import java.util.Set;
import java.util.UUID;


import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
//import android.util.<a in_rurl="http://i.tracksrv.com/click?v=Tlo6NDg0MzQ6MTkxOTpsb2c6NDNhZGFhNWQ0ZTI0MThkZDhiNDhhMTcwMjk3YWU4N2Y6ei0xNTQzLTI5MjU3NjplbmdsaXNoLmN4ZW0ubmV0OjkyODgyOmUzZDEwNTgxNmJhNzM0Njg4MWNlMDcxNmZiZTMzMTIy" href="#" style="text-decoration:underline" id="_GPLITA_0" title="Click to Continue > by safesaver">Log</a>;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
//import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Toast;
import android.widget.ToggleButton;

public class MainActivity extends Activity 
{
	
private static final String TAG = "Hexapod_Bluetooth";
	
	Button button1, button2, button3, button4, button5, button6;
	ToggleButton toggleButton1;
	  int count = 0;     
	  private BluetoothAdapter btAdapter = null;
	  private BluetoothSocket btSocket = null;
	  private OutputStream outStream = null;
	  
	  // SPP UUID service
	  private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
	  
	  // MAC-address of module (you must edit this line)
	  private static String address = "00:06:66:4E:06:F6"; 
	     
	  public void onCreate(Bundle savedInstanceState)
	  {
		  super.onCreate(savedInstanceState);
	      setContentView(R.layout.activity_main);

	     button1 = (Button) findViewById(R.id.button1);
	     button2 = (Button) findViewById(R.id.button2);
	     button3 = (Button) findViewById(R.id.button3); 
	     button4 = (Button) findViewById(R.id.button4); 
	     button5 = (Button) findViewById(R.id.button5); 
	     button6 = (Button) findViewById(R.id.button6); 
	     toggleButton1 = (ToggleButton) findViewById(R.id.toggleButton1);
//Display message for button 1 press   
	      
	      button1.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('F');		//Lift up all  
	  	 }
	  	                                  });      
	      
	     button2.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('B');		//Lift up all servos 
	  	 }
	  	                                  });   
	     
	     button3.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('L');		//Lift up all servos 
	  	 }
	  	                                  });      
	     button4.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('R');		//Lift up all servos 
	  	 }
	  	                                  });
	     
	     button5.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('U');		//Lift up all servos 
	  	 }
	  	                                  });  
	     
	     button6.setOnClickListener(new OnClickListener()
	  	 {
	  	  public void onClick(View v)
	  	  {
	  		 	sendData('D');		//Lift up all servos 
	  	 }
	  	                                  }); 
	     
	     toggleButton1.setOnClickListener(new OnClickListener()
	     {
			@Override
			public void onClick(View v) {
			if (toggleButton1.isChecked())
			{
				sendData('O');		//reset hexapod
			}
			else{
				sendData('C');		//stop hexapod
			}
				
			}
	     });
	            
	   
	   
		   //********************turn on bluetooth***************************
			  btAdapter = BluetoothAdapter.getDefaultAdapter();       // get Bluetooth adapter
			  if (btAdapter == null) {
				  Toast msg4 = Toast.makeText(getBaseContext(),
				  "BLUETOOTH Not supported", Toast.LENGTH_LONG);
				   msg4.show();           
				  		  
				}
			  else{
				   if (btAdapter.isEnabled()) {
				        Log.d(TAG, "...Bluetooth ON...");
				      } else {
				        //Prompt user to turn on Bluetooth
				        Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				        startActivityForResult(enableBtIntent, 1);
				      }
				    }
	  }			  

			  private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {
			      if(Build.VERSION.SDK_INT >= 10){
			          try {
			              final Method  m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", new Class[] { UUID.class });
			              return (BluetoothSocket) m.invoke(device, MY_UUID);
			          } catch (Exception e) {
			              Log.e(TAG, "Could not create Insecure RFComm Connection",e);
			          }
			      }
			      return  device.createRfcommSocketToServiceRecord(MY_UUID);
			  }
			    
			  @Override
			  public void onResume() {
			    super.onResume();
			  
			    Log.d(TAG, "...onResume - try connect...");
			    
			    // Set up a pointer to the remote node using it's address.
			    BluetoothDevice device = btAdapter.getRemoteDevice(address);
			    
			    // Two things are needed to make a connection:
			    //   A MAC address, which we got above.
			    //   A Service ID or UUID.  In this case we are using the
			    //     UUID for SPP.
			    
			    try {
			        btSocket = createBluetoothSocket(device);
			    } catch (IOException e1) {
			        errorExit("Fatal Error", "In onResume() and socket create failed: " + e1.getMessage() + ".");
			    }
			        
			    // Discovery is resource intensive.  Make sure it isn't going on
			    // when you attempt to connect and pass your message.
			    btAdapter.cancelDiscovery();
			    
			    // Establish the connection.  This will block until it connects.
			    Log.d(TAG, "...Connecting...");
			    try {
			      btSocket.connect();
			      Log.d(TAG, "...Connection ok...");
			    } catch (IOException e) {
			      try {
			        btSocket.close();
			      } catch (IOException e2) {
			        errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
			      }
			    }
			      
			    // Create a data stream so we can talk to server.
			    Log.d(TAG, "...Create Socket...");
			  
			    try {
			      outStream = btSocket.getOutputStream();
			    } catch (IOException e) {
			      errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
			    }
			  }
			  
			  //@Override
			  public void onPause() {
			    super.onPause();
			  
			    Log.d(TAG, "...In onPause()...");
			  
			    if (outStream != null) {
			      try {
			        outStream.flush();
			      } catch (IOException e) {
			        errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
			      }
			    }
			  
			    try     {
			      btSocket.close();
			    } catch (IOException e2) {
			      errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
			    }
			  }

			  private void errorExit(String title, String message){
			    Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
			    finish();
			  }
			  
			  private void sendData(char message) {
				    //byte[] msgBuffer = message.getBytes();
				  char msgBuffer = message;
				   // Log.d(TAG, message );
				  
				    try {
				      outStream.write(msgBuffer);
				    } catch (IOException e) {
				      String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
				      if (address.equals("00:00:00:00:00:00"))
				        msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 35 in the java code";
				        msg = msg +  ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";
				        
			    }
			  }
			  
			}
