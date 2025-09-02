# JBUS 3.0A!

**The A in 3.0A means that this is for Arduino!**

### So this isnn't confusing at all. I've never used the RS485 branch. I'm creating this new branch, which is based off the work I did on the XT7 rotary table. I re-wrote JBUS so that it can work with the RPI. This version will be a continuation of that. 

## What changed?
The original version of Jbus for the Arduino used to send over raw bytes, and it used byte stuffing to negotiate problems. The newer jbus is an ASCI-only based protocol. You send strings back and forth. Sending numbers is very easy. The entire packet structure is broken down later in this readme. 

  > TODO Add RS485 Support (whatever that means)

  > TODO MERGE RS485 Support Branch with this branch

  > TODO Rename entire branch JBUS_3.1

  > ~~TODO Make a fucking readme~~

  > ~~TODO Merge all info from the .h into the readme~~