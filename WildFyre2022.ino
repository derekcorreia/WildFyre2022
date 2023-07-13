/**************************************************************************
    WildFyreReignyted is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    See <https://www.gnu.org/licenses/>.
*/

/*
  DCO Todo:

  What Next:
          xxBonusCountdown needs to be implemented/lamps need to be reviewed
          xxEB/Special Handling
          xxDecide what to do with the 4 bank/reset
          xxTop lane skill shots (and switch handling in general)
    Sharpshooter Mode
    Wild Fyre Spinner on 2x 4bank completions
    Bonus collect saucer
    Music/Sound Effect Creation
    Music/Sound Effect Linking
    Attract Mode
    Code Cleanup from prior game
    Copy most of ManageGame from Trident

*/


#include "RPU_Config.h"
#include "RPU.h"
#include "WildFyre2022.h"
#include "SelfTestAndAudit.h"
#include "AudioHandler.h"
#include <EEPROM.h>

#define USE_SCORE_OVERRIDES

#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
//#include "SendOnlyWavTrigger.h"
//wavTrigger wTrig;             // Our WAV Trigger object
AudioHandler Audio;
#endif

#define PINBALL_MACHINE_BASE_MAJOR_VERSION  22023
#define PINBALL_MACHINE_BASE_MINOR_VERSION  713
#define DEBUG_MESSAGES  1



/*********************************************************************

    Game specific code

*********************************************************************/

// MachineState
//  0 - Attract Mode
//  negative - self-test modes
//  positive - game play
char MachineState = 0;
boolean MachineStateChanged = true;
#define MACHINE_STATE_ATTRACT         0
#define MACHINE_STATE_INIT_GAMEPLAY   1
#define MACHINE_STATE_INIT_NEW_BALL   2
#define MACHINE_STATE_NORMAL_GAMEPLAY 4
#define MACHINE_STATE_COUNTDOWN_BONUS 99
#define MACHINE_STATE_BALL_OVER       100
#define MACHINE_STATE_MATCH_MODE      110

#define MACHINE_STATE_ADJUST_FREEPLAY           -17
#define MACHINE_STATE_ADJUST_BALL_SAVE          -18
#define MACHINE_STATE_ADJUST_MUSIC_LEVEL        -19
#define MACHINE_STATE_ADJUST_TOURNAMENT_SCORING -20
#define MACHINE_STATE_ADJUST_TILT_WARNING       -21
#define MACHINE_STATE_ADJUST_AWARD_OVERRIDE     -22
#define MACHINE_STATE_ADJUST_BALLS_OVERRIDE     -23
#define MACHINE_STATE_ADJUST_SCROLLING_SCORES   -24
#define MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD   -25
#define MACHINE_STATE_ADJUST_SPECIAL_AWARD      -26
#define MACHINE_STATE_ADJUST_DIM_LEVEL          -27
#define MACHINE_STATE_ADJUST_DONE               -28

// The lower 4 bits of the Game Mode are modes, the upper 4 are for frenzies
// and other flags that carry through different modes
#define GAME_MODE_SKILL_SHOT                        0
#define GAME_MODE_UNSTRUCTURED_PLAY                 4
#define GAME_MODE_WILDFYRE                          6
#define GAME_MODE_WILDFYRE_END                      7
#define GAME_MODE_SS_READY                          8
#define GAME_MODE_SS                                9
#define GAME_MODE_SS_END                            10
#define GAME_MODE_WIZARD_START                      32
#define GAME_MODE_WIZARD                            33
#define GAME_MODE_WIZARD_FINISHED                   34
#define GAME_BASE_MODE                              0x0F


#define EJECT_1_MASK     0x04
#define EJECT_2_MASK     0x02
#define EJECT_3_MASK     0x01


#define EEPROM_BALL_SAVE_BYTE           100
#define EEPROM_FREE_PLAY_BYTE           101
#define EEPROM_MUSIC_LEVEL_BYTE         102
#define EEPROM_SKILL_SHOT_BYTE          103
#define EEPROM_TILT_WARNING_BYTE        104
#define EEPROM_AWARD_OVERRIDE_BYTE      105
#define EEPROM_BALLS_OVERRIDE_BYTE      106
#define EEPROM_TOURNAMENT_SCORING_BYTE  107
#define EEPROM_SCROLLING_SCORES_BYTE    110
#define EEPROM_DIM_LEVEL_BYTE           113
#define EEPROM_EXTRA_BALL_SCORE_BYTE    140
#define EEPROM_SPECIAL_SCORE_BYTE       144


#define SOUND_EFFECT_NONE               0
#define SOUND_EFFECT_BONUS_COUNT        1
#define SOUND_EFFECT_INLANE_UNLIT       3
#define SOUND_EFFECT_OUTLANE_UNLIT      4
#define SOUND_EFFECT_INLANE_LIT         5
#define SOUND_EFFECT_OUTLANE_LIT        6
#define SOUND_EFFECT_BUMPER_HIT         7
#define SOUND_EFFECT_ADD_CREDIT         10
#define SOUND_EFFECT_BALL_OVER          19
#define SOUND_EFFECT_GAME_OVER          200
#define SOUND_EFFECT_EXTRA_BALL         23
#define SOUND_EFFECT_MACHINE_START      210
#define SOUND_EFFECT_SKILL_SHOT         220
#define SOUND_EFFECT_SKILL_SHOTA1       221
#define SOUND_EFFECT_SKILL_SHOTA2       222
#define SOUND_EFFECT_SKILL_SHOTA3       223
#define SOUND_EFFECT_SKILL_SHOTA4       224
#define SOUND_EFFECT_TILT_WARNING       28
#define SOUND_EFFECT_MATCH_SPIN         29
#define SOUND_EFFECT_BONUSCOUNTDOWN     201
#define SOUND_EFFECT_SPINNER            203
#define SOUND_EFFECT_POP                204
#define SOUND_EFFECT_POP2               205
#define SOUND_EFFECT_POP3               206
#define SOUND_EFFECT_POP4               207
#define SOUND_EFFECT_WILD4_COMPLETE     210
#define SOUND_EFFECT_LANES              30
#define SOUND_EFFECT_LANESA1            31
#define SOUND_EFFECT_LANESA2            32
#define SOUND_EFFECT_4BANK              33
#define SOUND_EFFECT_4BANKA1            34
#define SOUND_EFFECT_4BANKA2            35
#define SOUND_EFFECT_4BANKA3            36
#define SOUND_EFFECT_3BANK              37
#define SOUND_EFFECT_3BANKA1            38
#define SOUND_EFFECT_3BANKA2            39
#define SOUND_EFFECT_BALL_SAVE          40
#define SOUND_EFFECT_FIRE               50

#define SOUND_EFFECT_SLING_SHOT         34
#define SOUND_EFFECT_ROLLOVER           35
#define SOUND_EFFECT_10PT_SWITCH        36
#define SOUND_EFFECT_ADD_PLAYER_1       20
#define SOUND_EFFECT_ADD_PLAYER_2       (SOUND_EFFECT_ADD_PLAYER_1+1)
#define SOUND_EFFECT_ADD_PLAYER_3       (SOUND_EFFECT_ADD_PLAYER_1+2)
#define SOUND_EFFECT_ADD_PLAYER_4       (SOUND_EFFECT_ADD_PLAYER_1+3)
#define SOUND_EFFECT_PLAYER_1_UP        110
#define SOUND_EFFECT_PLAYER_1_UPA1      111
#define SOUND_EFFECT_PLAYER_1_UPA2      112
#define SOUND_EFFECT_PLAYER_1_UPA3      113
#define SOUND_EFFECT_PLAYER_1_UPA4      114
#define SOUND_EFFECT_PLAYER_1_UPA5      115
#define SOUND_EFFECT_PLAYER_2_UP        120
#define SOUND_EFFECT_PLAYER_2_UPA1      121
#define SOUND_EFFECT_PLAYER_2_UPA2      122
#define SOUND_EFFECT_PLAYER_2_UPA3      123
#define SOUND_EFFECT_PLAYER_2_UPA4      124
#define SOUND_EFFECT_PLAYER_2_UPA5      125
#define SOUND_EFFECT_PLAYER_3_UP        130
#define SOUND_EFFECT_PLAYER_3_UPA1      131
#define SOUND_EFFECT_PLAYER_3_UPA2      132
#define SOUND_EFFECT_PLAYER_3_UPA3      133
#define SOUND_EFFECT_PLAYER_3_UPA4      134
#define SOUND_EFFECT_PLAYER_3_UPA5      135
#define SOUND_EFFECT_PLAYER_4_UP        140
#define SOUND_EFFECT_PLAYER_4_UPA1      141
#define SOUND_EFFECT_PLAYER_4_UPA2      142
#define SOUND_EFFECT_PLAYER_4_UPA3      143
#define SOUND_EFFECT_PLAYER_4_UPA4      144
#define SOUND_EFFECT_PLAYER_4_UPA5      145
#define SOUND_EFFECT_SHOOT_AGAIN        60
#define SOUND_EFFECT_TILT               61
#define SOUND_EFFECT_VOICE_EXTRA_BALL             81
#define SOUND_EFFECT_WIZARD_MODE_START            88
#define SOUND_EFFECT_WIZARD_MODE_FINISHED         89
#define SOUND_EFFECT_BACKGROUND_1       90
#define SOUND_EFFECT_BACKGROUND_2       91
#define SOUND_EFFECT_BACKGROUND_3       92
#define SOUND_EFFECT_BACKGROUND_WIZ     93

#define SOUND_EFFECT_STALLBALL_MODE     165
#define SOUND_EFFECT_STALLBALL_STALL    160
#define SOUND_EFFECT_STALLBALL_ELIM     166
#define SOUND_EFFECT_STALLBALL_ELIM_2   167
#define SOUND_EFFECT_STALLBALL_ELIM_3   168

#define SOUND_EFFECT_SHOOTER_GROOVE     101
#define SOUND_EFFECT_BG_SOUND           102

#define MAX_DISPLAY_BONUS     15  // equates to 30k
#define TILT_WARNING_DEBOUNCE_TIME      1000
#define TILT_THROUGH_DEBOUNCE_TIME      8000


/*********************************************************************

    Machine state and options

*********************************************************************/
unsigned long HighScore = 0;
unsigned long AwardScores[3];
byte Credits = 0;
boolean FreePlayMode = false;
byte MusicLevel = 3;
byte BallSaveNumSeconds = 0;
unsigned long ExtraBallValue = 30000;
unsigned long SpecialValue = 30000;
unsigned long CurrentTime = 0;
unsigned long TiltThroughTime = 0;
byte MaximumCredits = 40;
byte BallsPerGame = 3;
byte DimLevel = 2;
byte ScoreAwardReplay = 0;
boolean HighScoreReplay = true;
boolean MatchFeature = true;
boolean TournamentScoring = true;
boolean ScrollingScores = true;
boolean PriorPlayerTilted = false;


/*********************************************************************

    Game State

*********************************************************************/
byte CurrentPlayer = 0;
byte CurrentBallInPlay = 1;
byte CurrentNumPlayers = 0;
byte Bonus = 1;
byte BonusX;
byte BonusAdvanceArrows = 0;
byte GameMode = GAME_MODE_SKILL_SHOT;
byte SpinnerLit = 0;
byte Num3BankCompletions = 0;
byte Num4BankCompletions = 0;
byte MaxTiltWarnings = 2;
byte NumTiltWarnings = 0;
byte WildFyreMultiplier = 1;
byte DropsUntilWildFyre = 2;

boolean SamePlayerShootsAgain = false;
boolean BallSaveUsed = false;
boolean CurrentlyShowingBallSave = false;
boolean ExtraBallCollected = false;
boolean SpecialCollected = false;
boolean ShowingModeStats = false;

unsigned long CurrentScores[4];
unsigned long BallFirstSwitchHitTime = 0;
unsigned long BallTimeInTrough = 0;
unsigned long GameModeStartTime = 0;
unsigned long GameModeEndTime = 0;
unsigned long LastTiltWarningTime = 0;
unsigned long ScoreAdditionAnimation;
unsigned long ScoreAdditionAnimationStartTime;
unsigned long LastRemainingAnimatedScoreShown;
unsigned long ScoreMultiplier;

// byte DropTargetLampArray[] = {DROP_TARGET_1, DROP_TARGET_2, DROP_TARGET_3, DROP_TARGET_4, DROP_TARGET_5};
byte DropTarget3BankSwitchArray[] = {SW_3DROP_1, SW_3DROP_2, SW_3DROP_3};

byte DropTarget4BankSwitchArray[] = {SW_4DROP_1, SW_4DROP_2, SW_4DROP_3, SW_4DROP_4};
byte DropTarget4BankWildLampArray [] = {LAMP_WILD_W, LAMP_WILD_I, LAMP_WILD_L, LAMP_WILD_D};
byte DropTarget4BankFyreLampArray [] = {LAMP_FYRE_F, LAMP_FYRE_Y, LAMP_FYRE_R, LAMP_FYRE_E};
byte EjectSwitchArray[] = {SW_EJECT_1, SW_EJECT_2, SW_EJECT_3};
byte EjectLampArray[] = {LAMP_TOP_EJECT_1, LAMP_TOP_EJECT_2, LAMP_TOP_EJECT_3};

/*********************************************************************

    Game Specific State Variables

*********************************************************************/
byte TotalSpins[4];
byte LanePhase;
byte RolloverPhase;
byte TenPointPhase;
byte LastAwardShotCalloutPlayed;
byte LastWizardTimer;
byte SkillShotEject = 0;
byte CurrentEjectsHit = 0;
byte NumEjectSets = 0;
byte Num3BankTargets = 0;
byte Num4BankTargets = 0;
byte AdvancedViaArrows = 0;
byte inlaneMultiplier = 1;
byte StartButtonHits = 0;

boolean StallBallMode = false;

unsigned long TopEjectHitTime;
unsigned long BonusEjectHitTime;
unsigned long BonusTargetHitTime;
unsigned long RightInlaneLastHitTime = 0;
unsigned long LeftInlaneLastHitTime = 0;


boolean WizardScoring;

unsigned long LastInlaneHitTime;
unsigned long BonusXAnimationStart;
unsigned long LastSpinnerHit;

#define WIZARD_START_DURATION             5000
#define WIZARD_DURATION                   39000
#define WIZARD_DURATION_SECONDS           39
#define WIZARD_FINISHED_DURATION          5000
#define WIZARD_SWITCH_SCORE               5000
#define WIZARD_MODE_REWARD_SCORE          250000
#define WILDFYRE_DOUBLE_TIME              15000
#define WILDFYRE_EXTEND_TIME              20000

#define SPINNER_MAX_GOAL                  100

void ReadStoredParameters() {
  HighScore = RPU_ReadULFromEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, 10000);
  Credits = RPU_ReadByteFromEEProm(RPU_CREDITS_EEPROM_BYTE);
  if (Credits > MaximumCredits) Credits = MaximumCredits;

  ReadSetting(EEPROM_FREE_PLAY_BYTE, 0);
  FreePlayMode = (EEPROM.read(EEPROM_FREE_PLAY_BYTE)) ? true : false;

  BallSaveNumSeconds = ReadSetting(EEPROM_BALL_SAVE_BYTE, 15);
  if (BallSaveNumSeconds > 20) BallSaveNumSeconds = 20;

  MusicLevel = ReadSetting(EEPROM_MUSIC_LEVEL_BYTE, 3);
  if (MusicLevel > 3) MusicLevel = 3;

  TournamentScoring = (ReadSetting(EEPROM_TOURNAMENT_SCORING_BYTE, 0)) ? true : false;

  MaxTiltWarnings = ReadSetting(EEPROM_TILT_WARNING_BYTE, 2);
  if (MaxTiltWarnings > 2) MaxTiltWarnings = 2;

  byte awardOverride = ReadSetting(EEPROM_AWARD_OVERRIDE_BYTE, 99);
  if (awardOverride != 99) {
    ScoreAwardReplay = awardOverride;
  }

  byte ballsOverride = ReadSetting(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  if (ballsOverride == 3 || ballsOverride == 5) {
    BallsPerGame = ballsOverride;
  } else {
    if (ballsOverride != 99) EEPROM.write(EEPROM_BALLS_OVERRIDE_BYTE, 99);
  }

  ScrollingScores = (ReadSetting(EEPROM_SCROLLING_SCORES_BYTE, 1)) ? true : false;

  ExtraBallValue = RPU_ReadULFromEEProm(EEPROM_EXTRA_BALL_SCORE_BYTE);
  if (ExtraBallValue % 1000 || ExtraBallValue > 100000) ExtraBallValue = 20000;

  SpecialValue = RPU_ReadULFromEEProm(EEPROM_SPECIAL_SCORE_BYTE);
  if (SpecialValue % 1000 || SpecialValue > 100000) SpecialValue = 40000;

  DimLevel = ReadSetting(EEPROM_DIM_LEVEL_BYTE, 2);
  if (DimLevel < 2 || DimLevel > 3) DimLevel = 2;
  RPU_SetDimDivisor(1, DimLevel);

  AwardScores[0] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_1_EEPROM_START_BYTE);
  AwardScores[1] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_2_EEPROM_START_BYTE);
  AwardScores[2] = RPU_ReadULFromEEProm(RPU_AWARD_SCORE_3_EEPROM_START_BYTE);

}


void setup() {
  if (DEBUG_MESSAGES) {
    Serial.begin(57600);
  }

  // Tell the OS about game-specific lights and switches
  RPU_SetupGameSwitches(NUM_SWITCHES_WITH_TRIGGERS, NUM_PRIORITY_SWITCHES_WITH_TRIGGERS, SolenoidAssociatedSwitches);

  // Set up the chips and interrupts
  RPU_InitializeMPU();
  // Clear saucers if ball left in there, just in case...
  RPU_PushToSolenoidStack(SOL_EJECT_BONUS, 4, false);
  RPU_PushToSolenoidStack(SOL_EJECT_TOP, 4, false);
  RPU_DisableSolenoidStack();
  RPU_SetDisableFlippers(true);

  // Read parameters from EEProm
  ReadStoredParameters();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);

  CurrentScores[0] = PINBALL_MACHINE_BASE_MAJOR_VERSION;
  CurrentScores[1] = PINBALL_MACHINE_BASE_MINOR_VERSION;
  CurrentScores[2] = RPU_OS_MAJOR_VERSION;
  CurrentScores[3] = RPU_OS_MINOR_VERSION;

#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
  // WAV Trigger startup at 57600
  Audio.InitDevices(AUDIO_PLAY_TYPE_WAV_TRIGGER | AUDIO_PLAY_TYPE_ORIGINAL_SOUNDS);
  delayMicroseconds(10000);
  Audio.StopAllAudio();
#endif

  CurrentTime = millis();
  PlaySoundEffect(SOUND_EFFECT_MACHINE_START);
}

byte ReadSetting(byte setting, byte defaultValue) {
  byte value = EEPROM.read(setting);
  if (value == 0xFF) {
    EEPROM.write(setting, defaultValue);
    return defaultValue;
  }
  return value;
}


// This function is useful for checking the status of drop target switches
byte CheckSequentialSwitches(byte startingSwitch, byte numSwitches) {
  byte returnSwitches = 0;
  for (byte count = 0; count < numSwitches; count++) {
    returnSwitches |= (RPU_ReadSingleSwitchState(startingSwitch + count) << count);
  }
  return returnSwitches;
}


////////////////////////////////////////////////////////////////////////////
//
//  Lamp Management functions
//
////////////////////////////////////////////////////////////////////////////
void SetPlayerLamps(byte numPlayers, byte playerOffset = 0, int flashPeriod = 0) {
  for (int count = 0; count < 4; count++) {
    RPU_SetLampState(LAMP_PLAYER_1 + playerOffset + count, (numPlayers == (count + 1)) ? 1 : 0, 0, flashPeriod);
  }
}

void ShowBonusLamps() {
  // if ((GameMode & GAME_BASE_MODE) == GAME_MODE_SKILL_SHOT) {

  // } else {
    /* DCO this is a mess and crap hack, clean up
      Since lamp value/names are ints, can use ex: LAMP_BONUS_2K+Bonus or something instead of caseing everything
    */
    switch (Bonus){
      case 1: {
        RPU_SetLampState(LAMP_BONUS_2K, 1);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 2: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 1);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 3: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 1);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 4: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 1);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 5: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 6: {
        RPU_SetLampState(LAMP_BONUS_2K, 1);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 7: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 1);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 8: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 1);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 9: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 1);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 0);
        break;
      }
      case 10: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
      case 11: {
        RPU_SetLampState(LAMP_BONUS_2K, 1);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
      case 12: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 1);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
      case 13: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 1);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
      case 14: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 1);
        RPU_SetLampState(LAMP_BONUS_10K, 0);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
      case 15: {
        RPU_SetLampState(LAMP_BONUS_2K, 0);
        RPU_SetLampState(LAMP_BONUS_4K, 0);
        RPU_SetLampState(LAMP_BONUS_6K, 0);
        RPU_SetLampState(LAMP_BONUS_8K, 0);
        RPU_SetLampState(LAMP_BONUS_10K, 1);
        RPU_SetLampState(LAMP_BONUS_20K, 1);
        break;
      }
    // }
  }
}


void ShowBonusXLamps() {
  if ((GameMode & GAME_BASE_MODE) == GAME_MODE_SKILL_SHOT) {
  } else {
    if (BonusX == 2) {RPU_SetLampState(LAMP_2X_BONUS, 1); }
    if (BonusX == 5) {RPU_SetLampState(LAMP_5X_BONUS, 1); }
    if (BonusX == 1){
      RPU_SetLampState(LAMP_2X_BONUS, 0);
      RPU_SetLampState(LAMP_5X_BONUS, 0);
    }
    //    RPU_SetLampState(LAMP_BONUS_2X, BonusX[CurrentPlayer]==2);
    //    RPU_SetLampState(LAMP_BONUS_3X, BonusX[CurrentPlayer]==2);
    //    RPU_SetLampState(LAMP_BONUS_5X, BonusX[CurrentPlayer]==2);
  }
}

void ShowSpinnerLamp(){
  if ((GameMode & GAME_BASE_MODE) == GAME_MODE_SKILL_SHOT) {
  } else {
    if (CurrentTime < RightInlaneLastHitTime + 3000){
      RPU_SetLampState(LAMP_SPINNER, 1, 0, 100);
    } else {
      if (SpinnerLit){
        RPU_SetLampState(LAMP_SPINNER, 1);
      } else {
        RPU_SetLampState(LAMP_SPINNER, 0);
      }
    }
  }
}

void ShowEjectLamps(){
  if (GameMode == GAME_MODE_SKILL_SHOT){
    //RPU_SetLampState(DROP_TARGET_1, RPU_ReadSingleSwitchState(SW_DROP_TARGET_1)?0:1);
    for (byte count=0; count<3; count++) {
      RPU_SetLampState(LAMP_TOP_EJECT_1 + count, (count == SkillShotEject), 0, (count == SkillShotEject)?200:0 );
    }
  } else {
    //RPU_SetLampState(STAND_UP_PURPLE, CurrentStandupsHit&STANDUP_PURPLE_MASK);
    RPU_SetLampState(LAMP_TOP_EJECT_1, CurrentEjectsHit&EJECT_1_MASK);
    RPU_SetLampState(LAMP_TOP_EJECT_2, CurrentEjectsHit&EJECT_2_MASK);
    RPU_SetLampState(LAMP_TOP_EJECT_3, CurrentEjectsHit&EJECT_3_MASK);
  }
  if (NumEjectSets > 0){
    RPU_SetLampState(LAMP_4K_EJECT, 1);
  } else {
    RPU_SetLampState(LAMP_4K_EJECT, 0);
  }
}

void ShowDropTargetLamps(){
  if (Num3BankCompletions == 1){
    RPU_SetLampState(LAMP_2X_BONUS_3BANK, 1);
  } else {
    RPU_SetLampState(LAMP_2X_BONUS_3BANK, 0);
  }
  if (Num3BankCompletions > 1){
    RPU_SetLampState(LAMP_4K_3BANK, 1);
  } else {
    RPU_SetLampState(LAMP_2X_BONUS_3BANK, 0);
  }
}

void ShowBonusXArrowLamps(){
  for (byte count=0; count<4; count++) {
    if (CurrentTime < LeftInlaneLastHitTime + 3000){
      RPU_SetLampState(LAMP_SAUCER_ARROW_1 + count, 1, 0, 100);
    } else {
      RPU_SetLampState(LAMP_SAUCER_ARROW_1 + count, (BonusAdvanceArrows == count)?1:0);
    }
  }
  RPU_SetLampState(LAMP_10K_ROLLOVER, (BonusAdvanceArrows >= 3)?1:0);
}


void ShowLaneAndRolloverLamps() {
  if ((GameMode & GAME_BASE_MODE) == GAME_MODE_SKILL_SHOT) {
  } else {
    //    RPU_SetLampState(LAMP_LEFT_INLANE, LanePhase&0x01);
    //    RPU_SetLampState(LAMP_LEFT_OUTLANE, LanePhase&0x02);
    //    RPU_SetLampState(LAMP_RIGHT_INLANE, LanePhase&0x01);
    //    RPU_SetLampState(LAMP_RIGHT_OUTLANE, LanePhase&0x02);
  }
}


void ShowShootAgainLamps() {

  if (!BallSaveUsed && !StallBallMode && BallSaveNumSeconds > 0 && (CurrentTime - BallFirstSwitchHitTime) < ((unsigned long)(BallSaveNumSeconds - 1) * 1000)) {
    unsigned long msRemaining = ((unsigned long)(BallSaveNumSeconds - 1) * 1000) - (CurrentTime - BallFirstSwitchHitTime);
    RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, (msRemaining < 1000) ? 100 : 500);
  } else {
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
  }
}




////////////////////////////////////////////////////////////////////////////
//
//  Display Management functions
//
////////////////////////////////////////////////////////////////////////////
unsigned long LastTimeScoreChanged = 0;
unsigned long LastTimeOverrideAnimated = 0;
unsigned long LastFlashOrDash = 0;
#ifdef USE_SCORE_OVERRIDES
unsigned long ScoreOverrideValue[4] = {0, 0, 0, 0};
byte ScoreOverrideStatus = 0;
#define DISPLAY_OVERRIDE_BLANK_SCORE 0xFFFFFFFF
#endif
byte LastScrollPhase = 0;

byte MagnitudeOfScore(unsigned long score) {
  if (score == 0) return 0;

  byte retval = 0;
  while (score > 0) {
    score = score / 10;
    retval += 1;
  }
  return retval;
}

#ifdef USE_SCORE_OVERRIDES
void OverrideScoreDisplay(byte displayNum, unsigned long value, boolean animate) {
  if (displayNum > 3) return;
  ScoreOverrideStatus |= (0x10 << displayNum);
  if (animate) ScoreOverrideStatus |= (0x01 << displayNum);
  else ScoreOverrideStatus &= ~(0x01 << displayNum);
  ScoreOverrideValue[displayNum] = value;
}
#endif

byte GetDisplayMask(byte numDigits) {
  byte displayMask = 0;
  for (byte digitCount = 0; digitCount < numDigits; digitCount++) {
    displayMask |= (0x20 >> digitCount);
  }
  return displayMask;
}


void ShowPlayerScores(byte displayToUpdate, boolean flashCurrent, boolean dashCurrent, unsigned long allScoresShowValue = 0) {

#ifdef USE_SCORE_OVERRIDES
  if (displayToUpdate == 0xFF) ScoreOverrideStatus = 0;
#endif

  byte displayMask = 0x3F;
  unsigned long displayScore = 0;
  unsigned long overrideAnimationSeed = CurrentTime / 250;
  byte scrollPhaseChanged = false;

  byte scrollPhase = ((CurrentTime - LastTimeScoreChanged) / 250) % 16;
  if (scrollPhase != LastScrollPhase) {
    LastScrollPhase = scrollPhase;
    scrollPhaseChanged = true;
  }

  boolean updateLastTimeAnimated = false;

  for (byte scoreCount = 0; scoreCount < 4; scoreCount++) {

#ifdef USE_SCORE_OVERRIDES
    // If this display is currently being overriden, then we should update it
    if (allScoresShowValue == 0 && (ScoreOverrideStatus & (0x10 << scoreCount))) {
      displayScore = ScoreOverrideValue[scoreCount];
      if (displayScore != DISPLAY_OVERRIDE_BLANK_SCORE) {
        byte numDigits = MagnitudeOfScore(displayScore);
        if (numDigits == 0) numDigits = 1;
        if (numDigits < (RPU_OS_NUM_DIGITS - 1) && (ScoreOverrideStatus & (0x01 << scoreCount))) {
          // This score is going to be animated (back and forth)
          if (overrideAnimationSeed != LastTimeOverrideAnimated) {
            updateLastTimeAnimated = true;
            byte shiftDigits = (overrideAnimationSeed) % (((RPU_OS_NUM_DIGITS + 1) - numDigits) + ((RPU_OS_NUM_DIGITS - 1) - numDigits));
            if (shiftDigits >= ((RPU_OS_NUM_DIGITS + 1) - numDigits)) shiftDigits = (RPU_OS_NUM_DIGITS - numDigits) * 2 - shiftDigits;
            byte digitCount;
            displayMask = GetDisplayMask(numDigits);
            for (digitCount = 0; digitCount < shiftDigits; digitCount++) {
              displayScore *= 10;
              displayMask = displayMask >> 1;
            }
            RPU_SetDisplayBlank(scoreCount, 0x00);
            RPU_SetDisplay(scoreCount, displayScore, false);
            RPU_SetDisplayBlank(scoreCount, displayMask);
          }
        } else {
          RPU_SetDisplay(scoreCount, displayScore, true, 1);
        }
      } else {
        RPU_SetDisplayBlank(scoreCount, 0);
      }

    } else {
#endif
      // No override, update scores designated by displayToUpdate
      //CurrentScores[CurrentPlayer] = CurrentScoreOfCurrentPlayer;
      if (allScoresShowValue == 0) displayScore = CurrentScores[scoreCount];
      else displayScore = allScoresShowValue;

      // If we're updating all displays, or the one currently matching the loop, or if we have to scroll
      if (displayToUpdate == 0xFF || displayToUpdate == scoreCount || displayScore > RPU_OS_MAX_DISPLAY_SCORE) {

        // Don't show this score if it's not a current player score (even if it's scrollable)
        if (displayToUpdate == 0xFF && (scoreCount >= CurrentNumPlayers && CurrentNumPlayers != 0) && allScoresShowValue == 0) {
          RPU_SetDisplayBlank(scoreCount, 0x00);
          continue;
        }

        if (displayScore > RPU_OS_MAX_DISPLAY_SCORE) {
          // Score needs to be scrolled
          if ((CurrentTime - LastTimeScoreChanged) < 4000) {
            RPU_SetDisplay(scoreCount, displayScore % (RPU_OS_MAX_DISPLAY_SCORE + 1), false);
            RPU_SetDisplayBlank(scoreCount, RPU_OS_ALL_DIGITS_MASK);
          } else {

            // Scores are scrolled 10 digits and then we wait for 6
            if (scrollPhase < 11 && scrollPhaseChanged) {
              byte numDigits = MagnitudeOfScore(displayScore);

              // Figure out top part of score
              unsigned long tempScore = displayScore;
              if (scrollPhase < RPU_OS_NUM_DIGITS) {
                displayMask = RPU_OS_ALL_DIGITS_MASK;
                for (byte scrollCount = 0; scrollCount < scrollPhase; scrollCount++) {
                  displayScore = (displayScore % (RPU_OS_MAX_DISPLAY_SCORE + 1)) * 10;
                  displayMask = displayMask >> 1;
                }
              } else {
                displayScore = 0;
                displayMask = 0x00;
              }

              // Add in lower part of score
              if ((numDigits + scrollPhase) > 10) {
                byte numDigitsNeeded = (numDigits + scrollPhase) - 10;
                for (byte scrollCount = 0; scrollCount < (numDigits - numDigitsNeeded); scrollCount++) {
                  tempScore /= 10;
                }
                displayMask |= GetDisplayMask(MagnitudeOfScore(tempScore));
                displayScore += tempScore;
              }
              RPU_SetDisplayBlank(scoreCount, displayMask);
              RPU_SetDisplay(scoreCount, displayScore);
            }
          }
        } else {
          if (flashCurrent) {
            unsigned long flashSeed = CurrentTime / 250;
            if (flashSeed != LastFlashOrDash) {
              LastFlashOrDash = flashSeed;
              if (((CurrentTime / 250) % 2) == 0) RPU_SetDisplayBlank(scoreCount, 0x00);
              else RPU_SetDisplay(scoreCount, displayScore, true, 2);
            }
          } else if (dashCurrent) {
            unsigned long dashSeed = CurrentTime / 50;
            if (dashSeed != LastFlashOrDash) {
              LastFlashOrDash = dashSeed;
              byte dashPhase = (CurrentTime / 60) % 36;
              byte numDigits = MagnitudeOfScore(displayScore);
              if (dashPhase < 12) {
                displayMask = GetDisplayMask((numDigits == 0) ? 2 : numDigits);
                if (dashPhase < 7) {
                  for (byte maskCount = 0; maskCount < dashPhase; maskCount++) {
                    displayMask &= ~(0x01 << maskCount);
                  }
                } else {
                  for (byte maskCount = 12; maskCount > dashPhase; maskCount--) {
                    displayMask &= ~(0x20 >> (maskCount - dashPhase - 1));
                  }
                }
                RPU_SetDisplay(scoreCount, displayScore);
                RPU_SetDisplayBlank(scoreCount, displayMask);
              } else {
                RPU_SetDisplay(scoreCount, displayScore, true, 2);
              }
            }
          } else {
            RPU_SetDisplay(scoreCount, displayScore, true, 2);
          }
        }
      } // End if this display should be updated
#ifdef USE_SCORE_OVERRIDES
    } // End on non-overridden
#endif
  } // End loop on scores

  if (updateLastTimeAnimated) {
    LastTimeOverrideAnimated = overrideAnimationSeed;
  }

}

void ShowFlybyValue(byte numToShow, unsigned long timeBase) {
  byte shiftDigits = (CurrentTime - timeBase) / 120;
  byte rightSideBlank = 0;

  unsigned long bigVersionOfNum = (unsigned long)numToShow;
  for (byte count = 0; count < shiftDigits; count++) {
    bigVersionOfNum *= 10;
    rightSideBlank /= 2;
    if (count > 2) rightSideBlank |= 0x20;
  }
  bigVersionOfNum /= 1000;

  byte curMask = RPU_SetDisplay(CurrentPlayer, bigVersionOfNum, false, 0);
  if (bigVersionOfNum == 0) curMask = 0;
  RPU_SetDisplayBlank(CurrentPlayer, ~(~curMask | rightSideBlank));
}

/*

  XXdddddd---
           10
          100
         1000
        10000
       10x000
      10xx000
     10xxx000
    10xxxx000
   10xxxxx000
  10xxxxxx000
*/

////////////////////////////////////////////////////////////////////////////
//
//  Machine State Helper functions
//
////////////////////////////////////////////////////////////////////////////
boolean AddPlayer(boolean resetNumPlayers = false) {

  if (Credits < 1 && !FreePlayMode) return false;
  if (resetNumPlayers) CurrentNumPlayers = 0;
  if (CurrentNumPlayers >= 4) return false;

  CurrentNumPlayers += 1;
  RPU_SetDisplay(CurrentNumPlayers - 1, 0);
  RPU_SetDisplayBlank(CurrentNumPlayers - 1, 0x30);

  if (!FreePlayMode) {
    Credits -= 1;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(false);
  }
  PlaySoundEffect(SOUND_EFFECT_ADD_PLAYER_1 + (CurrentNumPlayers - 1));
  SetPlayerLamps(CurrentNumPlayers);

  RPU_WriteULToEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_PLAYS_EEPROM_START_BYTE) + 1);

  return true;
}

void AddCoinToAudit(byte switchHit) {

  unsigned short coinAuditStartByte = 0;

  switch (switchHit) {
    case SW_COIN_3: coinAuditStartByte = RPU_CHUTE_3_COINS_START_BYTE; break;
    case SW_COIN_2: coinAuditStartByte = RPU_CHUTE_2_COINS_START_BYTE; break;
    case SW_COIN_1: coinAuditStartByte = RPU_CHUTE_1_COINS_START_BYTE; break;
  }

  if (coinAuditStartByte) {
    RPU_WriteULToEEProm(coinAuditStartByte, RPU_ReadULFromEEProm(coinAuditStartByte) + 1);
  }

}


void AddCredit(boolean playSound = false, byte numToAdd = 1) {
  if (Credits < MaximumCredits) {
    Credits += numToAdd;
    if (Credits > MaximumCredits) Credits = MaximumCredits;
    RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
    if (playSound) PlaySoundEffect(SOUND_EFFECT_ADD_CREDIT);
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(false);
  } else {
    RPU_SetDisplayCredits(Credits);
    RPU_SetCoinLockout(true);
  }

}

void AddSpecialCredit() {
  AddCredit(false, 1);
  RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime, true);
  RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 1);
}

void AwardSpecial() {
  if (SpecialCollected) return;
  SpecialCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += SpecialValue;
  } else {
    AddSpecialCredit();
  }
}

void AwardExtraBall() {
  if (ExtraBallCollected) return;
  ExtraBallCollected = true;
  if (TournamentScoring) {
    CurrentScores[CurrentPlayer] += ExtraBallValue;
  } else {
    SamePlayerShootsAgain = true;
    RPU_SetLampState(LAMP_SHOOT_AGAIN, SamePlayerShootsAgain);
    StopAudio();
    PlaySoundEffect(SOUND_EFFECT_EXTRA_BALL);
    //    PlayBackgroundSongBasedOnLevel(StarLevel[CurrentPlayer]);
    //ResumeBackgroundSong();
  }
}

#define ADJ_TYPE_LIST                 1
#define ADJ_TYPE_MIN_MAX              2
#define ADJ_TYPE_MIN_MAX_DEFAULT      3
#define ADJ_TYPE_SCORE                4
#define ADJ_TYPE_SCORE_WITH_DEFAULT   5
#define ADJ_TYPE_SCORE_NO_DEFAULT     6
byte AdjustmentType = 0;
byte NumAdjustmentValues = 0;
byte AdjustmentValues[8];
unsigned long AdjustmentScore;
byte *CurrentAdjustmentByte = NULL;
unsigned long *CurrentAdjustmentUL = NULL;
byte CurrentAdjustmentStorageByte = 0;
byte TempValue = 0;


int RunSelfTest(int curState, boolean curStateChanged) {
  int returnState = curState;
  CurrentNumPlayers = 0;

  if (curStateChanged) {
    // Send a stop-all command and reset the sample-rate offset, in case we have
    //  reset while the WAV Trigger was already playing.
    StopAudio();
  }

  // Any state that's greater than CHUTE_3 is handled by the Base Self-test code
  // Any that's less, is machine specific, so we handle it here.
  if (curState >= MACHINE_STATE_TEST_CHUTE_3_COINS) {
    returnState = RunBaseSelfTest(returnState, curStateChanged, CurrentTime, SW_CREDIT_RESET, SW_SLAM);
  } else {
    byte curSwitch = RPU_PullFirstFromSwitchStack();

    if (curSwitch == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
      SetLastSelfTestChangedTime(CurrentTime);
      returnState -= 1;
    }

    if (curSwitch == SW_SLAM) {
      returnState = MACHINE_STATE_ATTRACT;
    }

    if (curStateChanged) {
      for (int count = 0; count < 4; count++) {
        RPU_SetDisplay(count, 0);
        RPU_SetDisplayBlank(count, 0x00);
      }
      RPU_SetDisplayCredits(MACHINE_STATE_TEST_SOUNDS - curState);
      RPU_SetDisplayBallInPlay(0, false);
      CurrentAdjustmentByte = NULL;
      CurrentAdjustmentUL = NULL;
      CurrentAdjustmentStorageByte = 0;

      AdjustmentType = ADJ_TYPE_MIN_MAX;
      AdjustmentValues[0] = 0;
      AdjustmentValues[1] = 1;
      TempValue = 0;

      switch (curState) {
        case MACHINE_STATE_ADJUST_FREEPLAY:
          CurrentAdjustmentByte = (byte *)&FreePlayMode;
          CurrentAdjustmentStorageByte = EEPROM_FREE_PLAY_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALL_SAVE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 5;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 8;
          AdjustmentValues[3] = 10;
          AdjustmentValues[4] = 15;
          CurrentAdjustmentByte = &BallSaveNumSeconds;
          CurrentAdjustmentStorageByte = EEPROM_BALL_SAVE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_MUSIC_LEVEL:
          AdjustmentType = ADJ_TYPE_MIN_MAX_DEFAULT;
          AdjustmentValues[1] = 3;
          CurrentAdjustmentByte = &MusicLevel;
          CurrentAdjustmentStorageByte = EEPROM_MUSIC_LEVEL_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TOURNAMENT_SCORING:
          CurrentAdjustmentByte = (byte *)&TournamentScoring;
          CurrentAdjustmentStorageByte = EEPROM_TOURNAMENT_SCORING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_TILT_WARNING:
          NumAdjustmentValues = 2;
          AdjustmentValues[1] = 1;
          AdjustmentValues[2] = 2;
          CurrentAdjustmentByte = &MaxTiltWarnings;
          CurrentAdjustmentStorageByte = EEPROM_TILT_WARNING_BYTE;
          break;
        case MACHINE_STATE_ADJUST_AWARD_OVERRIDE:
          AdjustmentType = ADJ_TYPE_MIN_MAX_DEFAULT;
          AdjustmentValues[1] = 7;
          CurrentAdjustmentByte = &ScoreAwardReplay;
          CurrentAdjustmentStorageByte = EEPROM_AWARD_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_BALLS_OVERRIDE:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 3;
          AdjustmentValues[0] = 3;
          AdjustmentValues[1] = 5;
          AdjustmentValues[2] = 99;
          CurrentAdjustmentByte = &BallsPerGame;
          CurrentAdjustmentStorageByte = EEPROM_BALLS_OVERRIDE_BYTE;
          break;
        case MACHINE_STATE_ADJUST_SCROLLING_SCORES:
          CurrentAdjustmentByte = (byte *)&ScrollingScores;
          CurrentAdjustmentStorageByte = EEPROM_SCROLLING_SCORES_BYTE;
          break;

        case MACHINE_STATE_ADJUST_EXTRA_BALL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &ExtraBallValue;
          CurrentAdjustmentStorageByte = EEPROM_EXTRA_BALL_SCORE_BYTE;
          break;

        case MACHINE_STATE_ADJUST_SPECIAL_AWARD:
          AdjustmentType = ADJ_TYPE_SCORE_WITH_DEFAULT;
          CurrentAdjustmentUL = &SpecialValue;
          CurrentAdjustmentStorageByte = EEPROM_SPECIAL_SCORE_BYTE;
          break;

        case MACHINE_STATE_ADJUST_DIM_LEVEL:
          AdjustmentType = ADJ_TYPE_LIST;
          NumAdjustmentValues = 2;
          AdjustmentValues[0] = 2;
          AdjustmentValues[1] = 3;
          CurrentAdjustmentByte = &DimLevel;
          CurrentAdjustmentStorageByte = EEPROM_DIM_LEVEL_BYTE;
          //          for (int count = 0; count < 7; count++) RPU_SetLampState(MIDDLE_ROCKET_7K + count, 1, 1);
          break;

        case MACHINE_STATE_ADJUST_DONE:
          returnState = MACHINE_STATE_ATTRACT;
          break;
      }

    }

    // Change value, if the switch is hit
    if (curSwitch == SW_CREDIT_RESET) {

      if (CurrentAdjustmentByte && (AdjustmentType == ADJ_TYPE_MIN_MAX || AdjustmentType == ADJ_TYPE_MIN_MAX_DEFAULT)) {
        byte curVal = *CurrentAdjustmentByte;
        curVal += 1;
        if (curVal > AdjustmentValues[1]) {
          if (AdjustmentType == ADJ_TYPE_MIN_MAX) curVal = AdjustmentValues[0];
          else {
            if (curVal > 99) curVal = AdjustmentValues[0];
            else curVal = 99;
          }
        }
        *CurrentAdjustmentByte = curVal;
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, curVal);
      } else if (CurrentAdjustmentByte && AdjustmentType == ADJ_TYPE_LIST) {
        byte valCount = 0;
        byte curVal = *CurrentAdjustmentByte;
        byte newIndex = 0;
        for (valCount = 0; valCount < (NumAdjustmentValues - 1); valCount++) {
          if (curVal == AdjustmentValues[valCount]) newIndex = valCount + 1;
        }
        *CurrentAdjustmentByte = AdjustmentValues[newIndex];
        if (CurrentAdjustmentStorageByte) EEPROM.write(CurrentAdjustmentStorageByte, AdjustmentValues[newIndex]);
      } else if (CurrentAdjustmentUL && (AdjustmentType == ADJ_TYPE_SCORE_WITH_DEFAULT || AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT)) {
        unsigned long curVal = *CurrentAdjustmentUL;
        curVal += 5000;
        if (curVal > 100000) curVal = 0;
        if (AdjustmentType == ADJ_TYPE_SCORE_NO_DEFAULT && curVal == 0) curVal = 5000;
        *CurrentAdjustmentUL = curVal;
        if (CurrentAdjustmentStorageByte) RPU_WriteULToEEProm(CurrentAdjustmentStorageByte, curVal);
      }

      if (curState == MACHINE_STATE_ADJUST_DIM_LEVEL) {
        RPU_SetDimDivisor(1, DimLevel);
      }
    }

    // Show current value
    if (CurrentAdjustmentByte != NULL) {
      RPU_SetDisplay(0, (unsigned long)(*CurrentAdjustmentByte), true);
    } else if (CurrentAdjustmentUL != NULL) {
      RPU_SetDisplay(0, (*CurrentAdjustmentUL), true);
    }

  }

  if (curState == MACHINE_STATE_ADJUST_DIM_LEVEL) {
    //    for (int count = 0; count < 7; count++) RPU_SetLampState(MIDDLE_ROCKET_7K + count, 1, (CurrentTime / 1000) % 2);
  }

  if (returnState == MACHINE_STATE_ATTRACT) {
    // If any variables have been set to non-override (99), return
    // them to dip switch settings
    // Balls Per Game, Player Loses On Ties, Novelty Scoring, Award Score
    //    DecodeDIPSwitchParameters();
    RPU_SetDisplayCredits(Credits, true);
    ReadStoredParameters();
  }

  return returnState;
}




////////////////////////////////////////////////////////////////////////////
//
//  Audio Output functions
//
////////////////////////////////////////////////////////////////////////////

#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
byte CurrentBackgroundSong = SOUND_EFFECT_NONE;
#endif

void StopAudio() {
#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
  Audio.StopAllAudio();
  CurrentBackgroundSong = SOUND_EFFECT_NONE;
#endif
}

void ResumeBackgroundSong() {
#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
  byte curSong = CurrentBackgroundSong;
  CurrentBackgroundSong = SOUND_EFFECT_NONE;
  PlayBackgroundSong(curSong);
#endif
}

void PlayBackgroundSong(byte songNum) {

#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
  if (MusicLevel > 1) {
    if (CurrentBackgroundSong != songNum) {
      if (CurrentBackgroundSong != SOUND_EFFECT_NONE) Audio.StopSound(CurrentBackgroundSong);
      if (songNum != SOUND_EFFECT_NONE) {
        Audio.SetMusicDuckingGain(8);
        Audio.PlayBackgroundSong(songNum, true);
      }
      CurrentBackgroundSong = songNum;
    }
  }
#else
  byte test = songNum;
  songNum = test;
#endif

}



unsigned long NextSoundEffectTime = 0;

void PlaySoundEffect(byte soundEffectNum) {

  if (MusicLevel == 0) return;

#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)

  Audio.PlaySound(soundEffectNum, AUDIO_PLAY_TYPE_WAV_TRIGGER);
  //  char buf[128];
  //  sprintf(buf, "s=%d\n", soundEffectNum);
  //  Serial.write(buf);
#endif

  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }

}

inline void StopSoundEffect(byte soundEffectNum) {
#if defined(RPU_OS_USE_WAV_TRIGGER) || defined(RPU_OS_USE_WAV_TRIGGER_1p3)
  Audio.StopSound(soundEffectNum);
#else
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Sound # %d\n", soundEffectNum);
    Serial.write(buf);
  }
#endif
}

////////////////////////////////////////////////////////////////////////////
//
//  Attract Mode
//
////////////////////////////////////////////////////////////////////////////

unsigned long AttractLastLadderTime = 0;
byte AttractLastLadderBonus = 0;
unsigned long AttractDisplayRampStart = 0;
byte AttractLastHeadMode = 255;
byte AttractLastPlayfieldMode = 255;
byte InAttractMode = false;


int RunAttractMode(int curState, boolean curStateChanged) {

  int returnState = curState;

  if (curStateChanged) {
    RPU_DisableSolenoidStack();
    RPU_TurnOffAllLamps();
    RPU_SetDisableFlippers(true);
    if (DEBUG_MESSAGES) {
      Serial.write("Entering Attract Mode\n\r");
    }
    AttractLastHeadMode = 0;
    AttractLastPlayfieldMode = 0;
    RPU_SetDisplayCredits(Credits, true);
  }

  // Alternate displays between high score and blank
  if (CurrentTime < 16000) {
    if (AttractLastHeadMode != 1) {
      ShowPlayerScores(0xFF, false, false);
      RPU_SetDisplayCredits(Credits, true);
      RPU_SetDisplayBallInPlay(0, true);
    }
  } else if ((CurrentTime / 8000) % 2 == 0) {

    if (AttractLastHeadMode != 2) {
      RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 1, 0, 250);
      RPU_SetLampState(LAMP_HEAD_GAME_OVER, 0);
      LastTimeScoreChanged = CurrentTime;
    }
    AttractLastHeadMode = 2;
    ShowPlayerScores(0xFF, false, false, HighScore);
  } else {
    if (AttractLastHeadMode != 3) {
      if (CurrentTime < 32000) {
        for (int count = 0; count < 4; count++) {
          CurrentScores[count] = 0;
        }
        CurrentNumPlayers = 0;
      }
      RPU_SetLampState(LAMP_HEAD_HIGH_SCORE, 0);
      RPU_SetLampState(LAMP_HEAD_GAME_OVER, 1);
      LastTimeScoreChanged = CurrentTime;
    }
    ShowPlayerScores(0xFF, false, false);

    AttractLastHeadMode = 3;
  }

  byte attractPlayfieldPhase = ((CurrentTime / 5000) % 5);

  if (attractPlayfieldPhase != AttractLastPlayfieldMode) {
    RPU_TurnOffAllLamps();
    AttractLastPlayfieldMode = attractPlayfieldPhase;
    if (attractPlayfieldPhase == 2) GameMode = GAME_MODE_SKILL_SHOT;
    else GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
    AttractLastLadderBonus = 1;
    AttractLastLadderTime = CurrentTime;
  }

  if (attractPlayfieldPhase < 2) {
    //ShowLampAnimation(1, 40, CurrentTime, 14, false, false);
  } else if (attractPlayfieldPhase == 3) {
    //ShowLampAnimation(0, 40, CurrentTime, 11, false, false);
  } else if (attractPlayfieldPhase == 2) {
    if ((CurrentTime - AttractLastLadderTime) > 200) {
      Bonus = AttractLastLadderBonus;
      ShowBonusLamps();
      AttractLastLadderBonus += 1;
      if (AttractLastLadderBonus > 20) AttractLastLadderBonus = 0;
      AttractLastLadderTime = CurrentTime;
    }
#if not defined (BALLY_STERN_OS_SOFTWARE_DISPLAY_INTERRUPT)
    RPU_DataRead(0);
#endif
  } else {
    //ShowLampAnimation(2, 40, CurrentTime, 14, false, false);
  }

  byte switchHit;
  while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
    if (switchHit == SW_CREDIT_RESET) {
      if (AddPlayer(true)) returnState = MACHINE_STATE_INIT_GAMEPLAY;
    }
    if (switchHit == SW_COIN_1 || switchHit == SW_COIN_2 || switchHit == SW_COIN_3) {
      AddCoinToAudit(switchHit);
      AddCredit(true, 1);
    }
    if (switchHit == SW_SELF_TEST_SWITCH && (CurrentTime - GetLastSelfTestChangedTime()) > 250) {
      returnState = MACHINE_STATE_TEST_LIGHTS;
      SetLastSelfTestChangedTime(CurrentTime);
    }
  }

  return returnState;
}





////////////////////////////////////////////////////////////////////////////
//
//  Game Play functions
//
////////////////////////////////////////////////////////////////////////////
byte CountBits(byte byteToBeCounted) {
  byte numBits = 0;

  for (byte count = 0; count < 8; count++) {
    numBits += (byteToBeCounted & 0x01);
    byteToBeCounted = byteToBeCounted >> 1;
  }

  return numBits;
}


void AddToBonus(byte amountToAdd = 1) {
  Bonus += amountToAdd;
  if (Bonus >= MAX_DISPLAY_BONUS) {
    Bonus = MAX_DISPLAY_BONUS;
  }
}

void AddToBonusArrows(byte amountToAdd = 1) {
  BonusAdvanceArrows += amountToAdd;
  // we want 2x to come from either dropping the 3 bank or advancing arrows, and 5x to come from doing both in a ball
  if (BonusAdvanceArrows >= 3 && AdvancedViaArrows == 0){
    AdvancedViaArrows = 1;
    if (BonusX == 2) {BonusX = 5;}
    if (BonusX == 1) {BonusX = 2;}
  }
}

void Handle4BankDropSwitch (byte switchHit){
  CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
  PlaySoundEffect(SOUND_EFFECT_4BANK + Num4BankTargets%4);
  Num4BankTargets++;
  // Lights for drops? Gotta check that
  // we want to light the spinner if the 4 bank has been completed
  if (  RPU_ReadSingleSwitchState(SW_4DROP_1) &&
        RPU_ReadSingleSwitchState(SW_4DROP_2) &&
        RPU_ReadSingleSwitchState(SW_4DROP_3) &&
        RPU_ReadSingleSwitchState(SW_4DROP_4))
        {
          if (Num4BankCompletions < 1) {
              PlaySoundEffect(SOUND_EFFECT_WILD4_COMPLETE);
              SpinnerLit = 1;
              Reset4Bank();
            };
          if (Num4BankCompletions == 3) {RPU_SetLampState(LAMP_EXTRA_BALL, 1);}
          if (Num4BankCompletions == 4) {RPU_SetLampState(LAMP_LEFT_RETURN_SPECIAL, 1);}
            Num4BankCompletions++;

          if (Num4BankCompletions == DropsUntilWildFyre) {GameMode = GAME_MODE_WILDFYRE;}
        }
}

void HandleTopEjectHit (byte switchHit){
  if (GameMode == GAME_MODE_SKILL_SHOT){
    if (switchHit == EjectSwitchArray[SkillShotEject] && StallBallMode == false){
      PlaySoundEffect(SOUND_EFFECT_SKILL_SHOT + CurrentTime%5);
      CurrentScores[CurrentPlayer] += 10000;
    } else {
      PlaySoundEffect(SOUND_EFFECT_FIRE);
      CurrentScores[CurrentPlayer] += (3000 * WildFyreMultiplier);
    }
  } else {
    PlaySoundEffect(SOUND_EFFECT_FIRE);
    CurrentScores[CurrentPlayer] += (3000 * WildFyreMultiplier);
  }

  if (switchHit == SW_EJECT_1 && NumEjectSets > 0 ){
    CurrentScores[CurrentPlayer] += (4000 * WildFyreMultiplier);
  }  

  byte switchMask = (1<<(switchHit-21));
  CurrentEjectsHit |= switchMask;

  if (StallBallMode){
    PlaySoundEffect(SOUND_EFFECT_STALLBALL_STALL);
  }

  if (CurrentEjectsHit==0x07){
    NumEjectSets++;
    CurrentEjectsHit = 0;
  }
  EjectTopSaucers();
}

void Show4BankLamps(){
  if (Num4BankCompletions < 1){
    for (byte count=0; count<4; count++) {
      RPU_SetLampState(DropTarget4BankWildLampArray[count], RPU_ReadSingleSwitchState(DropTarget4BankSwitchArray[count])?0:1);
    }
  }
  if (Num4BankCompletions >= 1){
    for (byte count=0; count<4; count++) {
      RPU_SetLampState(DropTarget4BankWildLampArray[count], 1);
      RPU_SetLampState(DropTarget4BankFyreLampArray[count], RPU_ReadSingleSwitchState(DropTarget4BankSwitchArray[count])?0:1);
    }
  }
}

void Handle3BankDropSwitch (byte switchHit){
  CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
  PlaySoundEffect(SOUND_EFFECT_3BANK + Num3BankTargets%3);
  Num3BankTargets++;
  //start with 2x lit to collect
  //move to 4k points if not
  if (  RPU_ReadSingleSwitchState(SW_3DROP_1) &&
        RPU_ReadSingleSwitchState(SW_3DROP_2) &&
        RPU_ReadSingleSwitchState(SW_3DROP_3))
        {
          if (Num3BankCompletions == 1){
            if (BonusX == 2) {BonusX = 5;}
            if (BonusX == 1) {BonusX = 2;}
            CurrentScores[CurrentPlayer] += (3000 * WildFyreMultiplier);
          } 
          if (Num3BankCompletions >=2){
            CurrentScores[CurrentPlayer] += (7000 * WildFyreMultiplier);
          }
          Num3BankCompletions++;
          Reset3Bank();
        }
}

void SetGameMode(byte newGameMode) {
  GameMode = newGameMode | (GameMode & ~GAME_BASE_MODE);
  GameModeStartTime = 0;
  GameModeEndTime = 0;
  if (DEBUG_MESSAGES) {
    char buf[129];
    sprintf(buf, "Game mode set to %d\n", newGameMode);
    Serial.write(buf);
  }
}

void StartScoreAnimation(unsigned long scoreToAnimate) {
  if (ScoreAdditionAnimation != 0) {
    CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
  }
  ScoreAdditionAnimation = scoreToAnimate;
  ScoreAdditionAnimationStartTime = CurrentTime;
  LastRemainingAnimatedScoreShown = 0;
}

void Reset3Bank() {
  RPU_PushToTimedSolenoidStack(SOL_3BANK_RESET, 6, CurrentTime + 400);
}

void Reset4Bank() {
  RPU_PushToTimedSolenoidStack(SOL_4BANK_RESET, 6, CurrentTime + 400);
}

void EjectTopSaucers() {
  RPU_PushToTimedSolenoidStack(SOL_EJECT_TOP, 4, CurrentTime + 1300);
}




void IncreaseBonusX() {
  boolean soundPlayed = false;
  if (BonusX < 5) {
    BonusX += 1;
    BonusXAnimationStart = CurrentTime;

    if (BonusX == 4) {
      BonusX += 1;
    }
  }

  if (!soundPlayed) {
    //    PlaySoundEffect(SOUND_EFFECT_BONUS_X_INCREASED);
  }

}


int InitGamePlay() {

  if (DEBUG_MESSAGES) {
    Serial.write("Starting game\n\r");
  }

  StallBallMode = false;
  StartButtonHits = 0;
  // The start button has been hit only once to get
  // us into this mode, so we assume a 1-player game
  // at the moment
  RPU_EnableSolenoidStack();
  RPU_SetCoinLockout((Credits >= MaximumCredits) ? true : false);
  RPU_TurnOffAllLamps();
  StopAudio();

  // Reset displays & game state variables
  for (int count = 0; count < 4; count++) {
    // Initialize game-specific variables
    BonusX = 1;

    TotalSpins[count] = 0;
    Bonus = 0;
  }
  memset(CurrentScores, 0, 4 * sizeof(unsigned long));

  SamePlayerShootsAgain = false;
  CurrentBallInPlay = 1;
  CurrentNumPlayers = 1;
  CurrentPlayer = 0;
  ShowPlayerScores(0xFF, false, false);

  return MACHINE_STATE_INIT_NEW_BALL;
}


int InitNewBall(bool curStateChanged, byte playerNum, int ballNum) {

  // If we're coming into this mode for the first time
  // then we have to do everything to set up the new ball
  if (curStateChanged) {
    RPU_TurnOffAllLamps();
    BallFirstSwitchHitTime = 0;
    byte PlayerNumMask (playerNum * 10);
    byte RandoSound = ((SOUND_EFFECT_PLAYER_1_UP) + (PlayerNumMask) + (CurrentTime%6));
    if (DEBUG_MESSAGES) {
        char buf[129];
        sprintf(buf, "CurTime int %d\nPlayer pn mask int %d\nPlayer sound int %d\n", CurrentTime%6, PlayerNumMask, RandoSound);
        // sprintf(buf, "Player pn mask int %d\n", PlayerNumMask);
        // sprintf(buf, "Player sound int %d\n", RandoSound);
        Serial.write(buf);
    }

    RPU_SetDisableFlippers(false);
    RPU_EnableSolenoidStack();
    RPU_SetDisplayCredits(Credits, true);
    SetPlayerLamps(playerNum + 1, 4);
    if (CurrentNumPlayers > 1 && (ballNum != 1 || playerNum != 0) && !SamePlayerShootsAgain) PlaySoundEffect(RandoSound);
    SamePlayerShootsAgain = false;
    StartButtonHits = 0;

    PlayBackgroundSong(SOUND_EFFECT_SHOOTER_GROOVE);

    RPU_SetDisplayBallInPlay(ballNum);
    RPU_SetLampState(LAMP_HEAD_TILT, 0);

    if (BallSaveNumSeconds > 0) {
      RPU_SetLampState(LAMP_SHOOT_AGAIN, 1, 0, 500);
    }

    BallSaveUsed = false;
    BallTimeInTrough = 0;
    NumTiltWarnings = 0;
    LastTiltWarningTime = 0;
    SkillShotEject = (0 + CurrentTime%3);

    TopEjectHitTime = 0;
    BonusEjectHitTime = 0;
    BonusTargetHitTime = 0;

    // Initialize game-specific start-of-ball lights & variables
    GameModeStartTime = 0;
    GameModeEndTime = 0;
    WildFyreMultiplier = 1;
    DropsUntilWildFyre = 2;
    GameMode = GAME_MODE_SKILL_SHOT;

    // Check prior player tilt, set tilt thru time
    TiltThroughTime = CurrentTime;
    if (PriorPlayerTilted){
      PriorPlayerTilted = false;
      TiltThroughTime = CurrentTime + TILT_THROUGH_DEBOUNCE_TIME;
    }

    ExtraBallCollected = false;
    SpecialCollected = false;

    // Start appropriate mode music
    if (RPU_ReadSingleSwitchState(SW_OUTHOLE)) {
      RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime + 600);
    }

    // Reset progress unless holdover awards
    Bonus = 1;
    BonusX = 1;
    
    // WF specific
    BonusAdvanceArrows = 0;
    SpinnerLit = 0;
    Num4BankCompletions = 0;
    Num3BankCompletions = 0;
    Num3BankTargets = 0;
    Num4BankTargets = 0;
    NumEjectSets = 0;
    CurrentEjectsHit = 0;
    AdvancedViaArrows = 0;


    ScoreMultiplier = 1;
    LanePhase = 0;
    LastInlaneHitTime = 0;
    ScoreAdditionAnimation = 0;
    ScoreAdditionAnimationStartTime = 0;
    BonusXAnimationStart = 0;
    LastSpinnerHit = 0;
    //    PlayBackgroundSongBasedOnLevel(StarLevel[CurrentPlayer]);
    TenPointPhase = 0;
    WizardScoring = false;
  }

  Reset3Bank();
  Reset4Bank();

  // We should only consider the ball initialized when
  // the ball is no longer triggering the SW_OUTHOLE
  if (RPU_ReadSingleSwitchState(SW_OUTHOLE)) {
    return MACHINE_STATE_INIT_NEW_BALL;
  } else {
    return MACHINE_STATE_NORMAL_GAMEPLAY;
  }

}


void PlayBackgroundSongBasedOnBall(byte ballNum) {
  if (ballNum == 1) {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_1);
  } else if (ballNum == BallsPerGame) {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_3);
  } else {
    PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_2);
  }
}

void PlayBackgroundSongBasedOnLevel(byte level) {
  if (level > 2) level = 2;
  PlayBackgroundSong(SOUND_EFFECT_BACKGROUND_1 + level);
}


// This function manages all timers, flags, and lights
int ManageGameMode() {
  int returnState = MACHINE_STATE_NORMAL_GAMEPLAY;

  boolean specialAnimationRunning = false;

  byte currentWizardTimer;

  switch (GameMode) {
    case GAME_MODE_SKILL_SHOT:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = 0;
      }

      if (BallFirstSwitchHitTime != 0) {
        GameModeStartTime = 0;
        SetGameMode(GAME_MODE_UNSTRUCTURED_PLAY);
      }

      if (GameModeEndTime != 0 && CurrentTime > GameModeEndTime) {
        ShowPlayerScores(0xFF, false, false);
      }
      break;


    case GAME_MODE_UNSTRUCTURED_PLAY:
      // If this is the first time in this mode
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
      }
      break;
    
    case GAME_MODE_WILDFYRE:
      if (GameModeStartTime == 0) {
        GameModeStartTime = CurrentTime;
        GameModeEndTime = CurrentTime + WILDFYRE_DOUBLE_TIME;
        WildFyreMultiplier = 2;
        //todo sound effect to start wildfyre scoring
      }

      if ((CurrentTime-GameModeStartTime)>MODE_START_DISPLAY_DURATION) {
        for (byte count=0; count<4; count++) {
          if (count!=CurrentPlayer) OverrideScoreDisplay(count, (GameModeEndTime-CurrentTime)/1000, true);
        }
      }

      if (CurrentTime>GameModeEndTime) {
        GameModeEndTime = 0;
        GameModeStartTime = 0;
        WildFyreMultiplier = 1;
        ShowPlayerScores(0xFF, false, false);
        PlayBackgroundSong(SOUND_EFFECT_NONE);
        GameMode = GAME_MODE_WILDFYRE_END;
      }
      break;
    
    case GAME_MODE_WILDFYRE_END:
    //todo light show and sound effect to end
      GameMode = GAME_MODE_UNSTRUCTURED_PLAY;
      DropsUntilWildFyre = (Num4BankCompletions + 3);
      PlayBackgroundSong(SOUND_EFFECT_BG_SOUND);
      break;
  }

  if ( !specialAnimationRunning && NumTiltWarnings <= MaxTiltWarnings ) {
    ShowBonusLamps();
    ShowBonusXLamps();
    Show4BankLamps();
    ShowSpinnerLamp();
    ShowEjectLamps();
    ShowDropTargetLamps();
    ShowBonusXArrowLamps();
#if not defined (BALLY_STERN_OS_SOFTWARE_DISPLAY_INTERRUPT)
    RPU_DataRead(0);
#endif
    ShowShootAgainLamps();
    ShowLaneAndRolloverLamps();
  }


  // Three types of display modes are shown here:
  // 1) score animation
  // 2) fly-bys
  // 3) normal scores
  if (ScoreAdditionAnimationStartTime != 0) {
    // Score animation
    if ((CurrentTime - ScoreAdditionAnimationStartTime) < 2000) {
      byte displayPhase = (CurrentTime - ScoreAdditionAnimationStartTime) / 60;
      byte digitsToShow = 1 + displayPhase / 6;
      if (digitsToShow > 6) digitsToShow = 6;
      unsigned long scoreToShow = ScoreAdditionAnimation;
      for (byte count = 0; count < (6 - digitsToShow); count++) {
        scoreToShow = scoreToShow / 10;
      }
      if (scoreToShow == 0 || displayPhase % 2) scoreToShow = DISPLAY_OVERRIDE_BLANK_SCORE;
      byte countdownDisplay = (1 + CurrentPlayer) % 4;

      for (byte count = 0; count < 4; count++) {
        if (count == countdownDisplay) OverrideScoreDisplay(count, scoreToShow, false);
        else if (count != CurrentPlayer) OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, false);
      }
    } else {
      byte countdownDisplay = (1 + CurrentPlayer) % 4;
      unsigned long remainingScore = 0;
      if ( (CurrentTime - ScoreAdditionAnimationStartTime) < 5000 ) {
        remainingScore = (((CurrentTime - ScoreAdditionAnimationStartTime) - 2000) * ScoreAdditionAnimation) / 3000;
        if ((remainingScore / 1000) != (LastRemainingAnimatedScoreShown / 1000)) {
          LastRemainingAnimatedScoreShown = remainingScore;
          PlaySoundEffect(SOUND_EFFECT_SLING_SHOT);
        }
      } else {
        CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
        remainingScore = 0;
        ScoreAdditionAnimationStartTime = 0;
        ScoreAdditionAnimation = 0;
      }

      for (byte count = 0; count < 4; count++) {
        if (count == countdownDisplay) OverrideScoreDisplay(count, ScoreAdditionAnimation - remainingScore, false);
        else if (count != CurrentPlayer) OverrideScoreDisplay(count, DISPLAY_OVERRIDE_BLANK_SCORE, false);
        else OverrideScoreDisplay(count, CurrentScores[CurrentPlayer] + remainingScore, false);
      }
    }
    if (ScoreAdditionAnimationStartTime) ShowPlayerScores(CurrentPlayer, false, false);
    else ShowPlayerScores(0xFF, false, false);
  } else if (LastSpinnerHit != 0) {
    byte numberToShow = TotalSpins[CurrentPlayer];

    if (numberToShow < 100) {
      ShowFlybyValue(100 - numberToShow, LastSpinnerHit);
    } else {
      ShowPlayerScores(CurrentPlayer, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - LastTimeScoreChanged) > 2000)) ? true : false);
    }

    if ((CurrentTime - LastSpinnerHit) > 1000) LastSpinnerHit = 0;
  } else {
    ShowPlayerScores(CurrentPlayer, (BallFirstSwitchHitTime == 0) ? true : false, (BallFirstSwitchHitTime > 0 && ((CurrentTime - LastTimeScoreChanged) > 2000)) ? true : false);
  }

  // Check to see if ball is in the outhole
  if (RPU_ReadSingleSwitchState(SW_OUTHOLE)) {
    if (BallTimeInTrough == 0) {
      BallTimeInTrough = CurrentTime;
    } else {
      // Make sure the ball stays on the sensor for at least
      // 0.5 seconds to be sure that it's not bouncing
      if ((CurrentTime - BallTimeInTrough) > 500) {

        if (BallFirstSwitchHitTime == 0 && NumTiltWarnings <= MaxTiltWarnings) {
          // Nothing hit yet, so return the ball to the player
          RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime);
          BallTimeInTrough = 0;
          returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
        } else {
          CurrentScores[CurrentPlayer] += ScoreAdditionAnimation;
          ScoreAdditionAnimationStartTime = 0;
          ScoreAdditionAnimation = 0;
          ShowPlayerScores(0xFF, false, false);
          // if we haven't used the ball save, and we're under the time limit, then save the ball
          if (!BallSaveUsed && !StallBallMode && ((CurrentTime - BallFirstSwitchHitTime)) < ((unsigned long)BallSaveNumSeconds * 1000)) {
            RPU_PushToTimedSolenoidStack(SOL_OUTHOLE, 4, CurrentTime + 100);
            BallSaveUsed = true;
            PlaySoundEffect(SOUND_EFFECT_BALL_SAVE);
            RPU_SetLampState(LAMP_SHOOT_AGAIN, 0);
            BallTimeInTrough = CurrentTime;
            returnState = MACHINE_STATE_NORMAL_GAMEPLAY;
          } else {
            ShowPlayerScores(0xFF, false, false);
            PlayBackgroundSong(SOUND_EFFECT_NONE);
            StopAudio();
            if (StallBallMode) PlaySoundEffect(SOUND_EFFECT_STALLBALL_ELIM + CurrentTime%3);
            if (CurrentBallInPlay < BallsPerGame) PlaySoundEffect(SOUND_EFFECT_BALL_OVER);
            returnState = MACHINE_STATE_COUNTDOWN_BONUS;
          }
        }
      }
    }
  } else {
    BallTimeInTrough = 0;
  }

  return returnState;
}



unsigned long CountdownStartTime = 0;
unsigned long LastCountdownReportTime = 0;
unsigned long BonusCountDownEndTime = 0;

int CountdownBonus(boolean curStateChanged) {

  // If this is the first time through the countdown loop
  if (curStateChanged) {
    if (!StallBallMode) { PlayBackgroundSong(SOUND_EFFECT_BONUSCOUNTDOWN); }
    CountdownStartTime = CurrentTime;
    ShowBonusLamps();

    LastCountdownReportTime = CountdownStartTime;
    BonusCountDownEndTime = 0xFFFFFFFF;
  }

  //we want more slow jamz
  //unsigned long countdownDelayTime = 250 - (Bonus * 3);
  unsigned long countdownDelayTime = 350;

  if ((CurrentTime - LastCountdownReportTime) > countdownDelayTime) {

    if (Bonus > 0) {

      // Only give sound & score if this isn't a tilt
      if (NumTiltWarnings <= MaxTiltWarnings) {
        //PlaySoundEffect(SOUND_EFFECT_BONUS_COUNT);
        CurrentScores[CurrentPlayer] += 2000 * ((unsigned long)BonusX);
      }

      Bonus -= 1;

      ShowBonusLamps();
    } else if (BonusCountDownEndTime == 0xFFFFFFFF) {
      BonusCountDownEndTime = CurrentTime + 1000;
    }
    LastCountdownReportTime = CurrentTime;
  }

  if (CurrentTime > BonusCountDownEndTime) {
    BonusCountDownEndTime = 0xFFFFFFFF;
    if (RPU_ReadSingleSwitchState(SW_EJECT_BONUS)){
      StopAudio();
      PlayBackgroundSong(SOUND_EFFECT_BG_SOUND);
      return MACHINE_STATE_NORMAL_GAMEPLAY;
    } else {
      // Reset any lights & variables of goals that weren't completed
      PlayBackgroundSong(SOUND_EFFECT_NONE);
      StopAudio();
      return MACHINE_STATE_BALL_OVER;
    }
  }

  return MACHINE_STATE_COUNTDOWN_BONUS;
}



void CheckHighScores() {
  unsigned long highestScore = 0;
  int highScorePlayerNum = 0;
  for (int count = 0; count < CurrentNumPlayers; count++) {
    if (CurrentScores[count] > highestScore) highestScore = CurrentScores[count];
    highScorePlayerNum = count;
  }

  if (highestScore > HighScore) {
    HighScore = highestScore;
    if (HighScoreReplay) {
      AddCredit(false, 3);
      RPU_WriteULToEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_REPLAYS_EEPROM_START_BYTE) + 3);
    }
    RPU_WriteULToEEProm(RPU_HIGHSCORE_EEPROM_START_BYTE, highestScore);
    RPU_WriteULToEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE, RPU_ReadULFromEEProm(RPU_TOTAL_HISCORE_BEATEN_START_BYTE) + 1);

    for (int count = 0; count < 4; count++) {
      if (count == highScorePlayerNum) {
        RPU_SetDisplay(count, CurrentScores[count], true, 2);
      } else {
        RPU_SetDisplayBlank(count, 0x00);
      }
    }

    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime + 300, true);
    RPU_PushToTimedSolenoidStack(SOL_KNOCKER, 3, CurrentTime + 600, true);
  }
}


unsigned long MatchSequenceStartTime = 0;
unsigned long MatchDelay = 150;
byte MatchDigit = 0;
byte NumMatchSpins = 0;
byte ScoreMatches = 0;

int ShowMatchSequence(boolean curStateChanged) {
  if (!MatchFeature) return MACHINE_STATE_ATTRACT;

  if (curStateChanged) {
    MatchSequenceStartTime = CurrentTime;
    MatchDelay = 1500;
    MatchDigit = CurrentTime % 10;
    NumMatchSpins = 0;
    RPU_SetLampState(LAMP_HEAD_MATCH, 1, 0);
    RPU_SetDisableFlippers();
    ScoreMatches = 0;
  }

  if (NumMatchSpins < 40) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      MatchDigit += 1;
      if (MatchDigit > 9) MatchDigit = 0;
      //PlaySoundEffect(10+(MatchDigit%2));
      PlaySoundEffect(SOUND_EFFECT_MATCH_SPIN);
      RPU_SetDisplayBallInPlay((int)MatchDigit * 10);
      MatchDelay += 50 + 4 * NumMatchSpins;
      NumMatchSpins += 1;
      RPU_SetLampState(LAMP_HEAD_MATCH, NumMatchSpins % 2, 0);

      if (NumMatchSpins == 40) {
        RPU_SetLampState(LAMP_HEAD_MATCH, 0);
        MatchDelay = CurrentTime - MatchSequenceStartTime;
      }
    }
  }

  if (NumMatchSpins >= 40 && NumMatchSpins <= 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      if ( (CurrentNumPlayers > (NumMatchSpins - 40)) && ((CurrentScores[NumMatchSpins - 40] / 10) % 10) == MatchDigit) {
        ScoreMatches |= (1 << (NumMatchSpins - 40));
        AddSpecialCredit();
        MatchDelay += 1000;
        NumMatchSpins += 1;
        RPU_SetLampState(LAMP_HEAD_MATCH, 1);
      } else {
        NumMatchSpins += 1;
      }
      if (NumMatchSpins == 44) {
        MatchDelay += 5000;
      }
    }
  }

  if (NumMatchSpins > 43) {
    if (CurrentTime > (MatchSequenceStartTime + MatchDelay)) {
      return MACHINE_STATE_ATTRACT;
    }
  }

  for (int count = 0; count < 4; count++) {
    if ((ScoreMatches >> count) & 0x01) {
      // If this score matches, we're going to flash the last two digits
      if ( (CurrentTime / 200) % 2 ) {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) & 0x0F);
      } else {
        RPU_SetDisplayBlank(count, RPU_GetDisplayBlank(count) | 0x30);
      }
    }
  }

  return MACHINE_STATE_MATCH_MODE;
}

void ValidatePlayfield (){
  if (BallFirstSwitchHitTime == 0) BallFirstSwitchHitTime = CurrentTime;
  TiltThroughTime = CurrentTime;
  PlayBackgroundSong(SOUND_EFFECT_BG_SOUND);
}

int RunGamePlayMode(int curState, boolean curStateChanged) {
  int returnState = curState;
  unsigned long scoreAtTop = CurrentScores[CurrentPlayer];

  // Very first time into gameplay loop
  if (curState == MACHINE_STATE_INIT_GAMEPLAY) {
    returnState = InitGamePlay();
  } else if (curState == MACHINE_STATE_INIT_NEW_BALL) {
    returnState = InitNewBall(curStateChanged, CurrentPlayer, CurrentBallInPlay);
  } else if (curState == MACHINE_STATE_NORMAL_GAMEPLAY) {
    returnState = ManageGameMode();
  } else if (curState == MACHINE_STATE_COUNTDOWN_BONUS) {
    if (PriorPlayerTilted) {
      returnState = MACHINE_STATE_BALL_OVER;
    } else {
    returnState = CountdownBonus(curStateChanged);
    ShowPlayerScores(0xFF, false, false);
    }
  } else if (curState == MACHINE_STATE_BALL_OVER) {
    RPU_SetDisplayCredits(Credits);

    if (SamePlayerShootsAgain) {
      PlaySoundEffect(SOUND_EFFECT_SHOOT_AGAIN);
      returnState = MACHINE_STATE_INIT_NEW_BALL;
    } else {

      CurrentPlayer += 1;
      if (CurrentPlayer >= CurrentNumPlayers) {
        CurrentPlayer = 0;
        CurrentBallInPlay += 1;
      }

      scoreAtTop = CurrentScores[CurrentPlayer];

      if (CurrentBallInPlay > BallsPerGame) {
        CheckHighScores();
        PlaySoundEffect(SOUND_EFFECT_GAME_OVER);
        for (int count = 0; count < CurrentNumPlayers; count++) {
          RPU_SetDisplay(count, CurrentScores[count], true, 2);
        }

        returnState = MACHINE_STATE_MATCH_MODE;
      }
      else returnState = MACHINE_STATE_INIT_NEW_BALL;
    }
  } else if (curState == MACHINE_STATE_MATCH_MODE) {
    returnState = ShowMatchSequence(curStateChanged);
    StallBallMode = false;
  }

  byte switchHit;
  //  ScoreMultiplier = 1 + CountBits(GameMode & 0xF0);

  if (NumTiltWarnings <= MaxTiltWarnings) {
    while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {

      if (DEBUG_MESSAGES) {
        char buf[128];
        sprintf(buf, "Switch Hit = %d\n", switchHit);
        Serial.write(buf);
      }

      if (WizardScoring) {
        if (switchHit != SW_SLAM && switchHit != SW_TILT) {
          //          PlaySoundEffect(SOUND_EFFECT_WIZARD_SCORE);
          CurrentScores[CurrentPlayer] += WIZARD_SWITCH_SCORE;
        }
      }

      switch (switchHit) {
        case SW_SLAM:
          //          RPU_DisableSolenoidStack();
          //          RPU_SetDisableFlippers(true);
          //          RPU_TurnOffAllLamps();
          //          RPU_SetLampState(GAME_OVER, 1);
          //          delay(1000);
          //          return MACHINE_STATE_ATTRACT;
          break;
        case SW_TILT:
          // This should be debounced
          if ((CurrentTime - LastTiltWarningTime) > TILT_WARNING_DEBOUNCE_TIME && CurrentTime > TiltThroughTime) {
            LastTiltWarningTime = CurrentTime;
            NumTiltWarnings += 1;
            if (NumTiltWarnings > MaxTiltWarnings) {
              RPU_DisableSolenoidStack();
              RPU_SetDisableFlippers(true);
              RPU_TurnOffAllLamps();
              StopAudio();
              PlaySoundEffect(SOUND_EFFECT_TILT);
              RPU_SetLampState(LAMP_HEAD_TILT, 1);
              PriorPlayerTilted = true;
            }
            PlaySoundEffect(SOUND_EFFECT_TILT_WARNING);
          }
          break;
        case SW_SELF_TEST_SWITCH:
          returnState = MACHINE_STATE_TEST_LIGHTS;
          SetLastSelfTestChangedTime(CurrentTime);
          break;
        //        case SW_10_PTS:

        //DCO CODE INLANES 
        case SW_LEFT_OUTLANE:
        case SW_RIGHT_OUTLANE:
          CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
          AddToBonus(2);
          PlaySoundEffect(SOUND_EFFECT_LANES + CurrentTime%3);
          ValidatePlayfield();
          break;
        case SW_LEFT_RETURN:
          CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
          LeftInlaneLastHitTime = CurrentTime;
          PlaySoundEffect(SOUND_EFFECT_LANES + CurrentTime%3);
          // TKTK: logic for special being lit needs to be added
          if (RPU_ReadLampState(LAMP_LEFT_RETURN_SPECIAL)) {
            AwardSpecial();
            RPU_SetLampState(LAMP_LEFT_RETURN_SPECIAL, 0);
          }
          ValidatePlayfield();
          break;
        case SW_LEFT_MID_LANE:
          // INFO: Playfield reads "Reset Bottom Targets"
          PlaySoundEffect(SOUND_EFFECT_LANES + CurrentTime%3);
          CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
          if (Num4BankCompletions > 1) {
            Reset4Bank();
          }
          ValidatePlayfield();
          break;
        case SW_RIGHT_MID_LANE:
          CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
          PlaySoundEffect(SOUND_EFFECT_LANES + CurrentTime%3);
          AddToBonus(1);
          ValidatePlayfield();
          break;
        case SW_RIGHT_RETURN:
          // INFO: Playfield reads "5k and EB when lit"
          CurrentScores[CurrentPlayer] += (500 * WildFyreMultiplier);
          RightInlaneLastHitTime = CurrentTime;
          PlaySoundEffect(SOUND_EFFECT_LANES + CurrentTime%3);
          ValidatePlayfield();
          if (RPU_ReadLampState(LAMP_EXTRA_BALL) == 1){
            AwardExtraBall();
            RPU_SetLampState(LAMP_EXTRA_BALL, 0);
          }
          break;

        case SW_EJECT_1:
        case SW_EJECT_2:
        case SW_EJECT_3:
          //if (SaucerHitTime==0 || (CurrentTime-SaucerHitTime)>500) {
          if (TopEjectHitTime==0 || (CurrentTime-TopEjectHitTime)>500){
            TopEjectHitTime = CurrentTime;
            HandleTopEjectHit(switchHit);
          }
          ValidatePlayfield();
          break;

        case SW_AB_LEFT:
        case SW_AB_TOP:
          AddToBonus(1);
          CurrentScores[CurrentPlayer] += (10 * WildFyreMultiplier);
          ValidatePlayfield();
          break;

        case SW_ADVANCE_TARGET:
          // INFO: Playfield reads "1000 and advance arrow"
          if (BonusTargetHitTime == 0 || (CurrentTime-BonusTargetHitTime)>500) {
            if (CurrentTime < LeftInlaneLastHitTime + 3000) {AddToBonusArrows(1);}
            BonusTargetHitTime = CurrentTime;
            AddToBonusArrows(1);
            CurrentScores[CurrentPlayer] += (1000 * WildFyreMultiplier);
          }
          ValidatePlayfield();
          break;

        case SW_ADVANCE_ARROW:
          // INFO: Playfield reads "1000 and advance arrow"
          AddToBonusArrows(1);
          if (CurrentTime < LeftInlaneLastHitTime + 3000) {
            AddToBonusArrows(1);
            CurrentScores[CurrentPlayer] += (Bonus * 2000 * WildFyreMultiplier);
            // todo: bonus collect whoop below
            PlaySoundEffect(SOUND_EFFECT_FIRE);
          } else {
            CurrentScores[CurrentPlayer] += (1000 * WildFyreMultiplier);
            if (BonusAdvanceArrows >=4 ) {CurrentScores[CurrentPlayer] += (4000 * WildFyreMultiplier);}
          }
          ValidatePlayfield();
          break;

        case SW_SPINNER:
          // todo spinner lit specific sound
          //byte inlaneMultiplier = 1;
          if (CurrentTime < RightInlaneLastHitTime + 3000) {inlaneMultiplier = 2;} else {inlaneMultiplier = 1;}
          PlaySoundEffect(SOUND_EFFECT_SPINNER);
          if (SpinnerLit == 1){
            CurrentScores[CurrentPlayer] += ( 500 * inlaneMultiplier * WildFyreMultiplier);  
          } else {
            CurrentScores[CurrentPlayer] += ( 100 * inlaneMultiplier * WildFyreMultiplier);
          }
          ValidatePlayfield();
          break;

        case SW_EJECT_BONUS:
          //For now, let's add bonus and eject until we can get some thought around rules
          //CountdownBonus(false);
          if (BonusEjectHitTime==0 || (CurrentTime-BonusEjectHitTime)>500){
            BonusEjectHitTime = CurrentTime;
            //CurrentScores[CurrentPlayer] += (Bonus * 2000);
            CountdownBonus(false);
            if (StallBallMode) {
              PlaySoundEffect(SOUND_EFFECT_STALLBALL_STALL);
            } else {
              PlaySoundEffect(SOUND_EFFECT_FIRE);
            }
          }
          ValidatePlayfield();
          RPU_PushToTimedSolenoidStack(SOL_EJECT_BONUS, 4, CurrentTime + 1500);
          break;

        case SW_3DROP_1:
        case SW_3DROP_2:
        case SW_3DROP_3:
          Handle3BankDropSwitch(switchHit);
          ValidatePlayfield();
          break;

        case SW_4DROP_1:
        case SW_4DROP_2:
        case SW_4DROP_3:
        case SW_4DROP_4:
          Handle4BankDropSwitch(switchHit);
          ValidatePlayfield();
          break;

        case SW_LEFT_BUMPER:
        case SW_CENTER_BUMPER:
        case SW_RIGHT_BUMPER:
          PlaySoundEffect(SOUND_EFFECT_POP + CurrentTime%4);
          //PlaySoundEffect(SOUND_EFFECT_POP3);
          CurrentScores[CurrentPlayer] += (100 * WildFyreMultiplier);
          ValidatePlayfield();
          break;

        case SW_LEFT_SLINGSHOT:
        case SW_RIGHT_SLINGSHOT:
          CurrentScores[CurrentPlayer] += (10 * WildFyreMultiplier);
          PlaySoundEffect(SOUND_EFFECT_SLING_SHOT);
          ValidatePlayfield();
          break;
        case SW_COIN_1:
        case SW_COIN_2:
        case SW_COIN_3:
          AddCoinToAudit(switchHit);
          AddCredit(true, 1);
          break;
        case SW_CREDIT_RESET:
          if (CurrentBallInPlay < 2) {
            // If we haven't finished the first ball, we can add players
            AddPlayer();
            StartButtonHits++;
            if (StartButtonHits == 10){
              if (DEBUG_MESSAGES) {
                Serial.write("Stallballmode\n\r");
              }
              StallBallMode = true;
              PlaySoundEffect(SOUND_EFFECT_STALLBALL_MODE);
            }
          } else {
            // If the first ball is over, pressing start again resets the game
            if (Credits >= 1 || FreePlayMode) {
              if (!FreePlayMode) {
                Credits -= 1;
                RPU_WriteByteToEEProm(RPU_CREDITS_EEPROM_BYTE, Credits);
                RPU_SetDisplayCredits(Credits);
              }
              returnState = MACHINE_STATE_INIT_GAMEPLAY;
            }
          }
          if (DEBUG_MESSAGES) {
            Serial.write("Start game button pressed\n\r");
          }
          break;
      }
    }
  } else {
    // We're tilted, so just wait for outhole
    while ( (switchHit = RPU_PullFirstFromSwitchStack()) != SWITCH_STACK_EMPTY ) {
      switch (switchHit) {
        case SW_SELF_TEST_SWITCH:
          returnState = MACHINE_STATE_TEST_LIGHTS;
          SetLastSelfTestChangedTime(CurrentTime);
          break;
        case SW_EJECT_BONUS:
          RPU_PushToTimedSolenoidStack(SOL_EJECT_BONUS, 4, CurrentTime + 200);
          break;
        case SW_EJECT_1:
        case SW_EJECT_2:
        case SW_EJECT_3:
          EjectTopSaucers();
        case SW_COIN_1:
        case SW_COIN_2:
        case SW_COIN_3:
          AddCoinToAudit(switchHit);
          AddCredit(true, 1);
          break;
      }
    }
  }

  if (!ScrollingScores && CurrentScores[CurrentPlayer] > RPU_OS_MAX_DISPLAY_SCORE) {
    CurrentScores[CurrentPlayer] -= RPU_OS_MAX_DISPLAY_SCORE;
  }

  if (scoreAtTop != CurrentScores[CurrentPlayer]) {
    LastTimeScoreChanged = CurrentTime;
    if (!TournamentScoring) {
      for (int awardCount = 0; awardCount < 3; awardCount++) {
        if (AwardScores[awardCount] != 0 && scoreAtTop < AwardScores[awardCount] && CurrentScores[CurrentPlayer] >= AwardScores[awardCount]) {
          // Player has just passed an award score, so we need to award it
          if (((ScoreAwardReplay >> awardCount) & 0x01)) {
            AddSpecialCredit();
          } else if (!ExtraBallCollected) {
            AwardExtraBall();
          }
        }
      }
    }

  }

  return returnState;
}


void loop() {

  RPU_DataRead(0);

  CurrentTime = millis();
  int newMachineState = MachineState;

  if (MachineState < 0) {
    newMachineState = RunSelfTest(MachineState, MachineStateChanged);
  } else if (MachineState == MACHINE_STATE_ATTRACT) {
    newMachineState = RunAttractMode(MachineState, MachineStateChanged);
  } else {
    newMachineState = RunGamePlayMode(MachineState, MachineStateChanged);
  }

  if (newMachineState != MachineState) {
    MachineState = newMachineState;
    MachineStateChanged = true;
  } else {
    MachineStateChanged = false;
  }

  RPU_ApplyFlashToLamps(CurrentTime);
  RPU_UpdateTimedSolenoidStack(CurrentTime);

}
