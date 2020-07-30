/*******************************************************************************
* File: MainActivity.java
* By  : Ben Dabin
* Date: 10 August 2013
*
* Description :
*     The following program allows allows the user to send different strings
* 	  for the hexapod robot to recieve when the user presses buttons on the 
*     Android App. The bluetooh configured. 
*
*******************************************************************************/

package com.Hexapod_Bluetooth;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.UUID;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Toast;
import android.widget.Adapter;
import android.widget.BaseAdapter;

public class MainActivity extends Activity 
{
	
	private static final String TAG = "Hexapod_Bluetooth";

	Button button1, button2, button3, button4, button5, button6;

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
		button3= (Button) findViewById(R.id.button3);
		button4= (Button) findViewById(R.id.button4);
		button5= (Button) findViewById(R.id.button5);
		button6= (Button) findViewById(R.id.button6);

		button1.setVisibility(View.INVISIBLE);
		button2.setVisibility(View.INVISIBLE);
		button3.setVisibility(View.INVISIBLE);
		button4.setVisibility(View.INVISIBLE);
		button5.setVisibility(View.VISIBLE);
		button6.setVisibility(View.VISIBLE);

		//**************Display individual messages to the GUI for every button pressed*******************************

		//Display message for button 1 press
		button1.setOnClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				sendData("0");		//send character '0' 
				Toast msg = Toast.makeText(getBaseContext(),
				  "Moving Forward", Toast.LENGTH_LONG);
				msg.show();
			}
		});

		//Display message for button 2 press
		button2.setOnClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				sendData("R");		//send character 'R' 
				Toast msg = Toast.makeText(getBaseContext(),
					"Moving to Right", Toast.LENGTH_LONG);
				msg.show();
			}
		});

		//Display message for button 3 press (Robot will move Left)
		button3.setOnClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				sendData("L");		//send character 'L' 
				Toast msg = Toast.makeText(getBaseContext(),
					"Moving left", Toast.LENGTH_LONG);
				msg.show();
			}
		}); 

		//Display message for button 4 press (Robot will move Left)
		button4.setOnClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				sendData("B");		//send character 'B' 
				Toast msg = Toast.makeText(getBaseContext(),
					"Backwards", Toast.LENGTH_LONG);
				msg.show();
			}
		});

		//************************DISABLE BLUETOOTH MODULE HERE*******************************************
		button5.setOnClickListener(new OnClickListener()
		{				  
			public void onClick(View v)				
			{
				//Change visibility of smartphone buttons 
				button1.setVisibility(View.INVISIBLE);
				button2.setVisibility(View.INVISIBLE);
				button3.setVisibility(View.INVISIBLE);
				button4.setVisibility(View.INVISIBLE);
				button5.setVisibility(View.VISIBLE);
				button6.setVisibility(View.VISIBLE);

				Toast msg = Toast.makeText(getBaseContext(),
					"COMMUNICATION ABORTED!!!!", Toast.LENGTH_LONG);	
				msg.show();
			}
		}); 	  

		//****************ENABLE BLUETOOTH MODULE HERE***************************************************	  
		button6.setOnClickListener(new OnClickListener()
		{
			public void onClick(View v)
			{
				button1.setVisibility(View.VISIBLE);
				button2.setVisibility(View.VISIBLE);
				button3.setVisibility(View.VISIBLE);
				button4.setVisibility(View.VISIBLE);
				button6.setVisibility(View.INVISIBLE);
				Toast msg = Toast.makeText(getBaseContext(),
				"BLUETOOTH ENABLED!", Toast.LENGTH_LONG);
				msg.show();	   
			}
		});

		//********************turn on bluetooth***************************
		btAdapter = BluetoothAdapter.getDefaultAdapter();       // get Bluetooth adapter
		if (btAdapter == null) 
		{
			Toast msg4 = Toast.makeText(getBaseContext(),
			"BLUETOOTH Not supported", Toast.LENGTH_LONG);
			msg4.show();           
		  
		}
		else
		{
			if (btAdapter.isEnabled()) 
			{
				Log.d(TAG, "...Bluetooth ON...");
			} 
			else 
			{
				//Prompt user to turn on Bluetooth
				Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
				startActivityForResult(enableBtIntent, 1);
			}
		}
	}
	
	//************************Create Bluetooth socket***************************************************************************
	private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException 
	{
		if(Build.VERSION.SDK_INT >= 10)
		{
		  try 
		  {
			final Method  m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", new Class[] { UUID.class });
			return (BluetoothSocket) m.invoke(device, MY_UUID);
		  } 
		  catch (Exception e) 
		  {
			Log.e(TAG, "Could not create Insecure RFComm Connection",e);
		  }
		}
		return  device.createRfcommSocketToServiceRecord(MY_UUID);
	}

	@Override
	public void onResume() 
	{
		super.onResume();
		Log.d(TAG, "...onResume - try connect...");

		// Set up a pointer to the remote node using it's address.
		BluetoothDevice device = btAdapter.getRemoteDevice(address);

		// Two things are needed to make a connection:
		//   A MAC address, which we got above.
		//   A Service ID or UUID.  In this case we are using the
		//     UUID for SPP.
		try 
		{
			btSocket = createBluetoothSocket(device);
		} 
		catch (IOException e1) 
		{
			errorExit("Fatal Error", "In onResume() and socket create failed: " + e1.getMessage() + ".");
		}

		// Discovery is resource intensive.  Make sure it isn't going on
		// when you attempt to connect and pass your message.
		btAdapter.cancelDiscovery();

		// Establish the connection.  This will block until it connects.
		Log.d(TAG, "...Connecting...");
		try 
		{
			btSocket.connect();
			Log.d(TAG, "...Connection ok...");
		} 
		catch (IOException e) 
		{
			try 
			{
				btSocket.close();
			} 
			catch (IOException e2) 
			{
				errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
			}
		}

		// Create a data stream so we can talk to server.
		Log.d(TAG, "...Create Socket...");

		try 
		{
			outStream = btSocket.getOutputStream();
		} 
		catch (IOException e) 
		{
			errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
		}
	}
	
	@Override
	public void onPause() 
	{
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

	private void errorExit(String title, String message)
	{		
		Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
		finish();
	}

	//***********************Send character data to the robot function***********************************************************
	private void sendData(String message) 
	{
		byte[] msgBuffer = message.getBytes();

		Log.d(TAG, "...Send data: " + message + "...");

		try 
		{		
		  outStream.write(msgBuffer);				//send string to robot
		} 
		catch (IOException e) 
		{
		  String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
		  if (address.equals("00:00:00:00:00:00"))
			msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 35 in the java code";
			msg = msg +  ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";			
			errorExit("Fatal Error", msg);      
		}
	}
}


	  

	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  
	  

	  


	

        



    
    