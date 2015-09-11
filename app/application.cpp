#include <stdlib.h>
#include <stdio.h>
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/LiquidCrystal/LiquidCrystal_I2C.h>
#include "NtpClientDelegateDemo.h"

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "see_dum" // Put you SSID and Password here
	#define WIFI_PWD "0863219053"
#endif

#define I2C_LCD_ADDR 0x27
LiquidCrystal_I2C lcd(I2C_LCD_ADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void onNtpReceive(NtpClient& client, time_t timestamp);

Timer printTimer;

// Option 1
// Use this option if you want to have full control of NtpTime client
// Default : no automatic NTP requests sent, no automatic update of SystemTime
// Default : NTP_DEFAULT_SERVER and NTP_DEFAULT_AUTO_UPDATE_INTERVAL from ntpclient.h

// NtpClient ntpClient(onNtpReceive);

// Option 2
// Use this option if you want control but other server/timeout options
// Default : use server as defined in call, no automatic update of SystemTime
// Default : automatic NTP request at myRequestInterval seconds
// Default : if myRequestInterval == 0 -> no automatic NTP request

// NtpClient ntpClient ("my_ntp_server", myRequestInterval, onNtpReceive);

// Option 3
// Use this option if you want to start wit autorefresh and autosystemupdate
// No further callback from ntpClient
// NtpClient ntpClient("pool.ntp.org", 30);

// Option 4
// only create pointer and initialize on ConnectOK
// NtpClient *ntpClient;


// Callback example using defined class ntpClientDemo
ntpClientDemo *demo;

// CallBack example 1 
// ntpClientDemo dm1 = ntpClientDemo();
// or use 
// ntpClientDemo dm1;

void onPrintSystemTime() {
	//Serial.print("Local Time    : ");
	//Serial.println(SystemClock.getSystemTimeString());
	//Serial.print("UTC Time: ");
	//Serial.println(SystemClock.getSystemTimeString(eTZ_UTC));

	DateTime _dateTime = SystemClock.now();
	Serial.print("DATE: ");
	Serial.println(_dateTime.toShortDateString());
	Serial.print("TIME: ");
	Serial.println(_dateTime.toShortTimeString(true));
	Serial.println("******************************");

	char buf[64];

	lcd.setCursor(6, 0);
	sprintf(buf, "%02d/%02d/%d", _dateTime.Day, _dateTime.Month + 1, _dateTime.Year + 543);
	lcd.print(String(buf));

	lcd.setCursor(6, 1);
	memset(buf, 0, 64);
	sprintf(buf, "%02d:%02d:%02d", _dateTime.Hour, _dateTime.Minute, _dateTime.Second);
	lcd.print(String(buf));
}


// Called when time has been received by NtpClient (option 1 or 2)
// Either after manual requestTime() or when
// and automatic request has been made.
void onNtpReceive(NtpClient& client, time_t timestamp) {
	SystemClock.setTime(timestamp);

	Serial.print("Time synchronized: ");
	Serial.println(SystemClock.getSystemTimeString());
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
// Set specific parameters if started by option 1 or 2
	// Set client to do automatic time requests every 60 seconds.
	// NOTE: you should have longer interval in a real world application
	// no need for query for time every 60 sec, should be at least 10 minutes or so.
//	ntpClient.setAutoQueryInterval(60);
//	ntpClient.setAutoQuery(true);
//  ntpClient.setAutoUpdateSystemClock(true);
	// Request to update time now. 
	// Otherwise the set interval will pass before time
	// is updated.
//	ntpClient.requestTime();

//  When using option 4 -> create client after connect OK
//  ntpClient = new NtpClient("my_ntp_server", myrefreshinterval);

//	When using Delegate Callback Option 2
	demo = new ntpClientDemo();

	printTimer.initializeMs(1000, onPrintSystemTime).start();

}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 10, connectFail); // Repeat and check again
}


// Will be called when WiFi hardware and software initialization was finished
// And system initialization was completed

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true); // Allow debug print to serial
	Serial.println("Sming. Let's do smart things!");
	Wire.pins(2, 0);
	lcd.begin(16, 2);
	lcd.backlight();

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("DATE: 00/00/0000");
	lcd.setCursor(0, 1);
	lcd.print("TIME: 00:00");

	// Station - WiFi client
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD); // Put you SSID and Password here

	// set timezone hourly difference to UTC
	SystemClock.setTimeZone(7);

	

	// Run our method when station was connected to AP (or not connected)
	WifiStation.waitConnection(connectOk, 30, connectFail); // We recommend 20+ seconds at start
}
