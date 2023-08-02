## Wild Fyre Reignyted

Wild Fyre Reignyted is a revised ruleset for the 1978 Stern Electronics pinball game "Wild Fyre". The code runs on an Ardunio board attached to the MPU utilizing RetroPinUpgrade to control the solenoids, lamps, and scoring. Optionally (but highly recommended), you can use a WAVTrigger board in concert with the Arduino for enhanced callouts, sound effects, and music.

The code provided is clean re-implementation of rules that tries to add some modern sensibilities, improve the game for tournament/competitive play, add some fun new modes of play, and provide an idealized "Early Solid State" game "remastered".

Note: This code has a dependency on Retro Pin Upgrade - it won't build without those files. A version is included in this repository, however, you may consider starting with the most recent library. More information is here:
https://github.com/RetroPinUpgrade

In order to use this on a pinball machine, a RPU-compatible hardware board is needed. It needs to be revision 3 capable or higher. More information on building or buying a board can be found at the RPU wiki: pinballindex.com


## [Wild Fyre Reignyted Rules](RULES.md)
## [Credits, Licenses, Acknowledgements](CREDITS.md)
## [Adjustments Guide](ADJUSTMENTS.md)

### To use this code
* Download the zip file (Code > Download ZIP) or clone the repository to your hard drive.  
* If upgrading RPU, get the RetroPinUpgrade files ( RPU.* and SelfTestAndAudit.* ) from the repository here:  
 * https://github.com/BallySternOS/BallySternOS/tree/master
 * (Code > Download ZIP)
* Unzip the Wild Fyre Reignyted repository
* Copy RPU.* and SelfTestAndAudit.* into the Wild Fyre Reignyted folder
* Open the RPU_Config in your IDE
* At the top of RPU_config.h, you'll see a couple of parameters in #define statements
  * Review the commented areas and uncomment the lines applicable to your build, and change parameters as directed.
  * if you want to use your machine's sound card, make sure this line is uncommented:  
   * #define RPU_OS_USE_SB100
  * if you have a Wav Trigger installed, uncomment this line 
    * #define RPU_OS_USE_WAV_TRIGGER_1p3 
  * The WAV files for the Wav Trigger board can be found here:  
    * Currently unavailable, this will be available at full release  
* Review the Adjustments guide and set relevant settings, including free play if desired.
    
  
Refer to the RPU Wiki for instructions on how to build the hardware.  
