# R4-04-Astromech
A small Astromech droid I decided to make
  
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/standingLookAtMe.jpg" alt="the droid" width=200 height=300>

# IN PROGRESS! CHECK BACK FOR UPDATES!
   --> finishing up the .ino control code, but hardware and ESP32 code is finished!
   
# Description  
 <h2>This is R4-04!</h2> <br>Designed with the <a href="https://starwars.fandom.com/wiki/R4_astromech_droid">R4 Astromech droid</a> in mind, but getting lost along the way resulted in R4-04, the droid for a galaxy not so far away.<br>
 This droid hosts its own WiFi network, and anyone in reach can accesss a web server holding a livestream of the IP camera and controls for the droid.<br>
 The IP of the droid's control server is printed on the bright blue LCD screen.<br>
 The droid is controlled using websocket communication on the ESP32 and relays information from the user to an Arduino Mega  controlling the motors via I2C commands. The Arduino also handles a self balancing routine, adjusting its shoulders with PID so that rougher terrain won't be a problem.<br> 
Along with the motor control, R4-04 also shows information concerning its environment on a second screen. The information is obtained via a BME280 Sensor.          

# Web App
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/R4-04WebApp.png " alt="the webApp" width=225 height=300>
The droid implements a simple web application to control it along with a live video feed.
The feed is implemented using a timed function to poll the server to update it. It is sent in Jpeg format from the ESP32's onboard  OV2640 camera.<br>
The control buttons are implemented using websocket communication.<br>
Each button is given a specific ID command, which is passed through to the ESP32's I2C wires and sent to the arduino.<br>
Upon a new command event on the I2C wires the Arduino will perform the desired motor operations.

--> a CSS Transform on the img and possible pretty-ing up of the UI should be done in V2

# Stack
ESP32 Components:

    ESP-CAM
    ESP-WiFi
    SPIFFS
    freeRTOS
    I2C Driver
    HTTP-Server
 IDF-Version:
 
    ESP-IDF v4.4-dev-744-g1cb31e509

  
Arduino:
  
    Unistep2
    AdaFruit BME280
    GY521
    Wire
    U8G2Lib
  
    
    
    
# Hardware / Materials
    AI-Thinker ESP32-CAM 
    Arduino Pro Mega 2650 
    I2C 1602 LCD (blue) 
    I2C 1.3'' LCD Screen (white) 
    GYU-521 Accelerometer and Gyroscope 
    BME280 
    28byj-48 Stepper motor and 2003 ULN Driver (X3)
    L298N DC Motor Driver 
    DC "TT" Motor and wheel (X2)
    5V 30mA Mini Solar Panel 50x33mm (x2)
    4.8V NiMh 2200 mAh Battery (x2)
    608 Abec 9 Bearings (x8)<br>
    M4 X 30mm Screw and nuts X 6
    M3 x 35mm Screws X 4<br>
    M3 X various smaller sizes X 16
    5/16'' Bolt and nut (2.5+inches) X3 
    Springs that fit over the 5/16'' bolt (these are optional but help)
    Dupont Wires (Mostly Female to Female, but a Few male to Female)
    Various proto boards cut to size
    some passive components listed in the schematic
    Epoxy glue
    Hot glue 
    Solder
    PETG (testing parts took a lot of filament, not sure what the end result requires, also PLA should be fine)<br>
  
  
  # Tools
    3D printer (designed with the ender 3 in mind)
    Soldering Iron
    Screwdriver
    Hot Glue Gun
    Files / post processing tools
    Wire cutter
    A ventilated area
    Socket Wrench / Sockets come in handy 
    Something to spread epoxy with 
    Hacksaw
  
  # Schematic
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/Schematic_R4-04 Astromech_2021-06-02.png" alt="A simple schematic" width=700 height=500>


# 3D Printer Settings

    Printer: Creality Ender 3 pro V2
    Filament: Duramic PETG
    Infill: 20% 
    Support: 2% everywhere
    Temp: 235
    Slicer: Cura
--> Slow on the first layer<br>
--> PLA Should be just fine for the parts as well, i just had PETG because it was cheaper at the time.<br>

# STL / FREECAD FILES
I designed the droid using FreeCAD, which is free and works on all major OS platforms.<br>
You are free to edit / change all designs, but please give a reference to the original!<br>
Printing all parts goes like this:<br>

    2 droidSolarPanelHolder1.stl, 1 of all others in the UPPER folder (head / neck)
    2 legs, 2 Shoulders, 2 FourthLeg_body.stl (Stepper holders), and 1 of each on the rest in the MIDDLE folder. (main body)
    2 droidFoot1.stl, 1 of each 3rd foot component in the LOWER folder. (lower assembly)

# Wiring / Testing

<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/standing1.jpg" alt="the droid" width="200" height="300"> 

   <img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/processingBoards.jpg" alt="droid brain and heart"  width="200" height="200">
   
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/screensExample.jpg" alt="Screen Test"  width="200" height="200">

   <img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/powerDistribution.jpg" alt="messy wires" width="200" height="200">
  

# Mechanical Notes
The TT motor and wheel attatch to the foot using  M3 X 35MM screw and nuts. The leg is attached using 2 bearings and a 5/16'' bolt and nut.
There is a hole for the wires in the middle of the foot, as well as a space for them to fit on the side of the leg.<br>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/footDetail.jpg" alt="messy wires" width=200 height=200>

<br>
The backside view of the droid.
There is a door that utilizes an M3 X 20 screw to hold both pieces in place.
Springs are attached from the stepper motor's M4 screw to the Foot's 5/16'' bolt. These are optional but help the droid remain standing under its own weight. The M4 screws I used hindered the neck from reaching the gear, this will be fixed in V2.
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/backview.jpg" alt="messy wires" width=200 height=200>

<br>
There is a stepper motor with the smaller neck control gear attached to the top of the body using M4 screw and nuts. <br>
The neck assembly is glued into place on the head and is not glued to the body. SDA,SCL, 5V, and GND are run through the neck to reach the ESP32-CAM.<br>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/neckGears.jpg" alt="messy wires" width=200 height=200>
<br>
The Front foot uses 4 bearings and an M3 x 8mm nut and screw that fits in the middle. Along with 2 5/16'' bolts to hold the wheel and leg in place. The springs are attached on the outside of each bolt which help restrict the freedom of movement.
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/frontFootCloseup.jpg" alt="messy wires" width=200 height=200>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/frontFootSide.jpg" alt="messy wires"width=200 height=200>

# V2 Notes for the future
  As with everything, theres always something to upgrade or clean up.<br>
  Here's a list of things I've thought of to work on if a second version is going to be built. 



 Hardware upgrades:
  
    Voice Recognition Module (elecHouse)
    Buzzer
    Stepper motor for 3rd Leg
    Distance Sensors
    More Environmental Sensors
    **POSSIBLY** Replace ESP32 with orange/Raspberry Pi
    Arc Generator (would also require Linear Actuator)
    Change DuPont wires to something that connects better (MOLEX/JST/something)
    ***Fix up power components** (5V is being fed through the Arduino's Regulator, causing current bottleneck)
    
Software Upgrades:

    Web app CSS needs to transform the Image vertically and make things prettier
    Send Environmental Data to web UI
    logging??
    Give additional hardware options on web UI
    train elecHouse voice module to recognize commands
    
    
 Mechanical Upgrades:
 
    Combine 3/4ths of the body into 1 .stl
    Body can be taller
    create spaces in body for hardware components (slots, screw holes, etc.)
    Figure out a 3Rd leg drive mechanism (leg also needs to fit in the body)
    change direct stepper connection to shoulders into something better (timing belts??)
    

# Disclaimer:
 This R4 Unit is a personal voyage in learning freeCAD and helped me form a new curriculum for a Robotics Class.<br>Also, I wanted another ESP32-CAM project, they're too much fun!<br>
 It is not in any way meant to represent work of Disney, Star Wars, or any legal stuff. I don't know how to put this correctly, but its not a 501st worthy model. But still fun to build!


