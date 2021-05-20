# Software Design Description: Mower

## M1.1 The Mower shall be capable of running autonomously within a confined area. 
#### To be able to let the mower run autonomously within a confined area, the following functions are being used.

### listenForBtCommand() :
This function allows the mower to listen to commands from Bluetooth

### ledRed(), ledGreen(), ledBlue(), ledOff() :
These functions controls the led of the mower. 
Calling one of these functions will set the led to that color or turn it off. These functions are used in the different states of the mower, and it allows the user to know what state the mower is in. 

### updateState() :
Will check the data given from Bluetooth and let the mower be in different states. In this case for autonomous driving and stop state, the stop state is also the idle state. 

### autoRun() :
When this function is being called, the mower will be running autonomously. In this function, the mower will check for the state of linesensorStateGlobal and the ultrasonic sensor(Look at M1.2 for more info about Ultrasonic). 
When sensing a line, the mower will work as following:
1. Call moveStop() to stop the mower from crossing the line 
2. Wait, with the delay() function
3. Call sendPosVectorToPi() to send position of the line to Raspberry Pi
4. Call moveBackwards() to allow the mower more space to rotate. 
5. Call sendPosVectorToPi() again to send the position where the mower will rotate. 
6. The mower will rotate with a random angle and random side, and while rotating, the linesensor will check for line so that the mower will not rotate out of the confined area. 
   6.1 If the linesensor detect a new line, the mower will stop and it will break out of the funtion.
   6.2 Otherwise, the mower will finish the rotation and continue to move forward. 

If the linesensor does not sense a line, the mower will call moveForward().

### updateLinesensorState() :
This function will update the state of the linesensor. 

#
## M1.2: The Mower shall be able to avoid collision objects during automous operation. 

The mower will run autonomously according to the description mentioned above by calling autoRun(). When sensing a collision with less or equal to 5 cm with the UltraSonic sensor, the mower will do the following. 
1. call ledRed() to change the color of the led to red. 
2. call sendPosVectorToPiCollision(), to send the position and the collision to Raspberry Pi. 
3. continue the same process as mentioned above in autoRun description from point 4(call moveBackwards()).

#
## M1.3 The Mower shall be able to accept and execute drive commands given by a remote device. 
To be able to run the mower manually the following functions are being used:

### see: listenForBtCommand() from M1.1 :

### see: ledRed(), ledGreen(), ledBlue(), ledOff() from M1.1 :

### updateState() :
Will check the data given from Bluetooth and let the mower be in different states by changing the state of mowerStateGlobal and in this case for moving forward, backwards, left, right and stop(idle). 

### mowerDriveState() :
Depending on the what the value of mowerStateGlobal is, the mower will be in the state of either moving forwars, backward, goin left or going right. 

#


