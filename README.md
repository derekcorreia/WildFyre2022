## Wild Fyre Reignyted

Note: This code has a dependency on Retro Pin Upgrade - it won't build without those files. More information is here:
https://github.com/RetroPinUpgrade

The base library is separated from this implementation so that it can be used by multiple projects without needing to be updated multiple times. For best results, always get all files (both the base library and the Wild Fyre Reignyted files) each time you build. Read on for basic instructions on how to build this code.

## [Wild Fyre Reignyted Rules](RULES.md)
## [Credits, Licenses, Acknowledgements](CREDITS.md)

### To use this code
* Download the zip file (Code > Download ZIP) or clone the repository to your hard drive.  
* Get the RetroPinUpgrade files ( RPU.* and SelfTestAndAudit.* ) from the repository here:  
 * https://github.com/BallySternOS/BallySternOS/tree/master
 * (Code > Download ZIP)
* Unzip the Wild Fyre Reignyted repository and name the folder that it's in as:
  * Wild Fyre Reignyted  
* Copy RPU.* and SelfTestAndAudit.* into the Wild Fyre Reignyted folder
* Open the WildFyre2022.ino in Arduino's IDE
* At the top of WildFyre2022.ino and you'll see a couple of parameters in #define statements
  * if you want to use your machine's sound card, make sure this line is uncommented:  
   * #define USE_SB100  
  * if you have a Wav Trigger installed, uncomment this line 
    * #define USE_WAV_TRIGGER
    * or 
    * #define USE_WAV_TRIGGER_1p3  
  * The WAV files for the Wav Trigger board can be found here:  
    * https://drive.google.com/file/d/1kFE97fg0SsAZb8K-F-PURkWhEmwNQXq8/view?usp=sharing  
    
  
Refer to the Wiki for instructions on how to build the hardware.  
