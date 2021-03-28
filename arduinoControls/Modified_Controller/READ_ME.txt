Changes to the AltSoftSerial.cpp:

Line 95 - 98: Starting to set up a callback function for receiving data from the base.

Line 145 - 148: Initializing the callback function.

Line 157 - 159: More initialization.

Line 582 - 588: Checks for the data indication message. Calls the callback function and should give content
of message in the parameters of the callback function. This may need to be edited as I am not able to test it.
It should get you guys started at least and headed in the right direction. 


Changes to AltSoftSerial.h:

Line 172 - 175: Initialization

Changes to MLinkAndLed.ino:

Line 43 - 46: Initialization of callback function

Line 72 - 73: More initialization.


Changes to MLinkControlTemplate.ino:

Line 43 - 46: Declare the callback function.

Line 72 - 73: Initialization of callback function.

Line 120 - 125: Callback function for the data being received. Please note that this callback is in the 
middle of an interrupt funtion. Code in the receiveMessages callback must be quick else issues may result.
Create a variable to store your data then process the data later in a different funtion.


**IMPORTANT**
I was not able to test these changes, there may need to be edits to the following changes 
for your needs.