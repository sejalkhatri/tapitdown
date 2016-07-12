(OPEN FOR DISCUSSION AND CHANGES!!!)
# tapitdown

OBJECTIVE:
- Developing an IOT based tap leakage notification system.

HARDWARE USED:
- Arduino Uno controller
- hmc5883l compass module
- esp8266 wifi module
- YF-S201 water flow sensor
- circuit setup for leakage detection

SOFTWARE REQUIRED:
- Arduino IDE

SETUP:
- Compass module is to be fixated on the faucet.
- the water flow sensor to be  attached to the tap mouth with the circuit setup.
- wifi module connected using uart to arduino uno controller.

WORKING:
- Reading from the compass tells us about the status of the faucet(CLOSED/OPEN).
  The circuit setup detects the water flowing(even a single drop),and we get 
  the reading as high(water present) and low(water not present) .Water flow gives 
  us the amount of water flowing in mililiters/sec.

LOGIC:
Possibility1 : So if the faucet (compass) reading is CLOSED and still our circuit gives 
               us high for a considerable amount of time than there is a leakage.

Possibility2 : If the faucet is OPEN(max), and we get high reading from our circuit BUT 
			   if the water flow sensor reading is V.less ,that means thers a leakage 
               in the supplying pipe(Bingo!)

Possibility3 : So what if the faucet reading is decreasing ,then we know the user is 
				closing the tap, and if he does'nt close it completely then we know 
				the leakage is taking place .

ALERT!!!!!!!!!!
- Alert is sent after 10 min of leakage detection using wifi module to our web Interface (www.tapitdown.tk) and if the repairing is done
  in one day then again one alert is sent with some Quote.
 

