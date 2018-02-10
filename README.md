# Android-based-Mecanum-Wheel-Robot-Car

The detail of this project can be found at my [personal blog](http://www.aaronguan.com/robot-car.html) 

This Mecanum Wheel Robot car is based on an Android application that enables user to control its speed and direction. The PID control program can make sure the car always maintain the required speed on any floor regardless of its friction. As one of the field buses, CAN bus is used for main controller to communicate with 2 motor controllers. Bluetooth HC-05 is used for communication between the phone and main controller.

## System Structure

The system would be divided into 3 parts: Mobile Application, Main Control Unit (MCU), and Motor Control Section.

![image](http://www.aaronguan.com/images/robotcar/structure.png)

### [Mobile Application](https://github.com/aaronzguan/Android-based-Mecanum-Wheel-Robot-Car/tree/master/Android_App)

We used Cordova as development platform and we have used CSS3, HTML5 and Javascript to develop this web-based android application. The layout of our mobile android app is shown as below:

![image](http://www.aaronguan.com/images/robotcar/mobileapp.png)

There are totally four buttons on the Android app. The left turn button and the right turn button on the left part are used to turn around the car, but the user needs to constantly press it. The 8 directional movement button is on the right part of the android app, each direction has 45° difference.

The Bluetooth button is on the top right corner of the app, which is a switch of the Bluetooth.

### [Main Control Unit](https://github.com/aaronzguan/Android-based-Mecanum-Wheel-Robot-Car/blob/master/MainController.c)

Main Control Unit (MCU) is responsible for establishing the communication link between the mobile application and the Motor Controllers (MC). 

![image](http://www.aaronguan.com/images/robotcar/communication.png)

### [Motor Control](https://github.com/aaronzguan/Android-based-Mecanum-Wheel-Robot-Car/blob/master/MotorController.c)

![image](http://www.aaronguan.com/images/robotcar/motor.png)


