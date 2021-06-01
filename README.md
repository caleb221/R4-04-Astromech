# R4-04-Astromech
A small Astromech droid I decided to make
  
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/standingLookAtMe.jpg" alt="the droid" width=200 height=300>

# IN PROGRESS! CHECK BACK FOR UPDATES!
   
# Hardware / Materials
  AI-Thinker ESP32-CAM <br>
  Arduino Pro Mega 2650 <br>
  I2C 1602 LCD (blue) <br>
  I2C 1.3'' LCD Screen (white) <br>
  GYU-521 Accelerometer and Gyroscope <br>
  BME280 <br>
  28byj-48 Stepper motor and 2003 ULN Driver (X3)<br>
  L298N DC Motor Driver <br>
  DC "TT" Motor and wheel (X2)<br>
  5V 30mA Mini Solar Panel 50x33mm (x2)<br>
  4.8V NiMh 2200 mAh Battery (x2)<br>
  608 Abec 9 Bearings (x8)<br>
  M4 X 30mm Screw and nuts X 6<br>
  M3 x 35mm Screws X 4<br>
  M3 X various smaller sizes X 16<br>
  5/16'' Bolt and nut (2.5+inches) X3 <br>
  Springs that fit over the 5/16'' bolt (these are optional but help)<br>
  Dupont Wires <br>
  Various proto boards cut to size<br>
  some passive components listed in the schematic<br>
  Epoxy glue<br>
  Hot glue <br>
  Solder<br>
  PETG (testing parts took a lot of filament, not sure what the end result requires, also PLA should be fine)<br>
  <br>
  
  # Tools
  3D printer (designed with the ender 3 in mind)<br>
  Soldering Iron<br>
  Screwdriver<br>
  Hot Glue Gun<br>
  Files / post processing tools<br>
  Wire cutter<br>
  A ventilated area<br>
  Socket Wrench / Sockets come in handy <br>
  Something to spread epoxy with <br>
  
  
  # Schematic
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/Schematic_R4-04 Astromech_2021-06-02.png" alt="A simple schematic" width=700 height=500>


# 3D Printer Settings
Printer: Creality Ender 3 pro V2<br>
Filament: Duramic PETG<br> 
Infill: 20% <br>
Support: 2% everywhere<br>
Temp: 235<br>
Slicer: Cura
--> Slow on the first layer<br>
--> PLA Should be just fine for the parts as well, i just had PETG because it was cheaper at the time.<br>

# STL / FREECAD FILES
I designed the droid using FreeCAD, which is free and works on all major OS platforms.<br>
You are free to edit / change all designs, but please give a reference to the original!<br>
Printing all parts goes like this:<br>
1 of each in the UPPER folder (upper head / neck)<br>
2 legs, 2 Shoulders, 2 droidFourthBody sections, and 1 of each on the rest in the MIDDLE folder. (middle body)<br>
2 legs, 2 feet, 1 of each 3rd foot component in the LOWER folder. (lower body)<br>

# Wiring / Testing
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/standing1.jpg" alt="the droid" width=200 height=300>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/processingBoards.jpg" alt="droid brain and heart" width=200 height=200>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/screensExample.jpg" alt="Screen Test" width=200 height=200>
<img src="https://github.com/caleb221/R4-04-Astromech/blob/main/droidImg/powerDistribution.jpg" alt="messy wires" width=200 height=200>





# Mechanical Notes
The TT motor and wheel attatch to the foot using  M3 X 35MM screw and nuts. The leg is attached using 2 bearings and a 5/16'' bolt and nut.
There is a hole for the wires in the middle of the foot, as well as a space for them to fit on the side of the leg.
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





