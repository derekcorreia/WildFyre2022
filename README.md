## Wild Fyre Reignyted

Note: This code has a dependency on Retro Pin Upgrade - it won't build without those files. A version is included in this repository, however, you may consider starting with the most recent library. More information is here:
https://github.com/RetroPinUpgrade


## [Wild Fyre Reignyted Rules](RULES.md)
## [Credits, Licenses, Acknowledgements](CREDITS.md)

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
    
  
Refer to the RPU Wiki for instructions on how to build the hardware.  
