import processing.serial.*;
import java.util.*;

Serial port;
float angle = 0;
String data;
String[] dataArray = new String[] {};
float[] currentAngle = new float[3200];
float[] idealAngle = new float[3200];
float error;
int index = 0;


void setup () {
  translate(600, 400);
  strokeWeight(2);
  size(1600, 800);
  background(255, 255, 255);
  frameRate(60);
  
  //for (int i = 0; i < 3200; i++) {
  //  if (i > 1) idealAngle[i] = idealAngle[i-1] + 0.1125;
  //  else idealAngle[i] = 0 + 0.1125;
  //}
  
  // Wait until the COM port is available
  boolean portOpened = false;
  while (!portOpened) {
    try {
      port = new Serial(this, "COM9", 115200);
      portOpened = true;
      println("Serial port opened successfully!");
    } 
    catch (Exception e) {
      println("Waiting for COM8 to be available...");
    }
  }
}


void serialEvent(Serial port) {
  String data = port.readStringUntil('\n');
  if (data != null) {
    data = trim(data);
    dataArray = split(data, '/');
    if (dataArray.length == 4) {
      index = int(dataArray[3]);
      currentAngle[index] = float(dataArray[0]);
      idealAngle[index] = float(dataArray[1]);
      error = float(dataArray[2]);
    }
  }
}

void draw() {
  table();
}

void table () {
  background(255);
  stroke(0);
  strokeWeight(2);
  line(0, 40, 1600, 40);
  for (float i = 0; i < 1600; i+=0.5) {
    int pos = int(i * 2);
    if (pos >= 3200) break;
    stroke(0, 255, 0);
    rect(i, 400, 0.5, -currentAngle[pos]);
    stroke(255, 0, 0);
    rect(i, 400, 0.5, -idealAngle[pos]);
  }
  textSize(16);
  fill(0, 0, 0);
  text("Current Angle: green", 10, 420);
  text("Ideal Angle: red", 10, 450);
  text("Global Error: ", 10, 480);
  text(error, 100, 480);
}
