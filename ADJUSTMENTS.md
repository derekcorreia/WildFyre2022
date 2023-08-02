# Adjustments and Tests
These are accessed by pressing the service button in the coin door. Pressing the service button will move on to the next test/adjustment. At any time, actuating a slam switch will restart and exit operator mode, as will going through all audits and adjusts. 

Note that many items do NOT have default overrides, so it's *highly advised* that you make a pass through the adjustments once to get everything set as you like.


# Tests

## Lamps
**Credit display: 00, Ball in Play: 01**

Upon entering, all lamps will flash. Pressing the start button will cycle through the individual lamps and change the player 1 score to the lamp flashing.

## Displays
**Credit display: variable, Ball in Play: variable**

Upon entering, all displays will cycle through 0-9 repeatedly. Pressing start will cycle the displays one digit at a time.

## Solenoids
**Credit display: 00, Ball in Play: 03**

Upon entering, machine will cycle through all solenoids in order displayed on the Player 1 score display. Pressing start will pause on the currently selected solenoid for repeated testing.

## Switches
**Credit display: 00, Ball in Play: 04**

Upon entering will show any closed switches in the player score boxes 1-4, in the order they are closed. If no switches are closed, the score displays will be blank.

# Adjustments

## Award Score Levels 
**Credit display: 01 through 03**

The game keeps three Award Score Levels. They are set using the Self Test switch (modes 1, 2, and 3). If an Award Score is set to zero, there is no award given. Otherwise, the award is controlled by the DIP switches (Switch 6) or the Award Score Override (Self Test mode 17). The current Award Score (0, 1, or 2) is shown in the Player 1 score box.

During game play, when an Award Score Level is crossed, the player receives the award (extra ball or credit). For tournament scoring mode (when available), the Award Score Levels will be ignored.

In these three setting modes (1, 2, and 3), the scores are changed with the Credit/ Reset button. Pressing the button once increases the score by 1,000 points. Pressing and holding the Start button will gradually increase the score. Double-clicking the Start button will return the given score to zero.

The Award Score Override (Self Test mode 17) can be set to 0-7, or 99. A value of 0-7 overrides the setting of the DIP switches to award either an extra ball or a credit for the given score. A value of 99 turns off the override so the DIP switch (Switch 6) setting will be used.

## High Score to Date
**Credit display: 04**

When a player’s score exceeds the High Score to Date, the new High Score is recorded and shown during the Attract Mode. Depending on the DIP switch setting (Switch 15), the game will award 3 Credits when a high score is reached.

In this mode, the score can be changed with the Start button. Pressing the button once increases the score by 1,000 points. Pressing and holding the Start button will gradually increase the score. 

Double-clicking the Start button will return the score to zero.

## Credits
**Credit display: 05**

Displays the number of current credits in the game. Pressing Start button will cycle number of credits from 0-20, then back to 0. 

## Audits
**Credit display: 06 through 11**

The audit features allow the operator to inspect/reset usage values of the machine.

Double-clicking the Start button will reset any of the audit values back to zero.

**06 - Total plays** - total number of games that have been played since this value was reset.

**07 - Total replays** - total number of credits awarded since this value was reset. 

**08 - Total times high score was beaten** - total number of times the high
score has been beaten since this value was reset.

**09 - Chute #2 coins**

**10 - Chute #1 coins** 

**11 - Chute #3 coins**

## Free Play
**Credit display: 12**

Player 1 score display will show a “0” meaning Free Play is OFF, or a “1” meaning Free Play is ON. The 0 and 1 are toggled by pressing the Start button. As soon as the value is toggled, it is written to memory and the machine can be reset into Free Play mode.

If off, the machine will require a positive credit count in order to start a game. Credits can be added through the coin chutes or through setting mode 5.

If Free Play is on, hitting the Start button will start a game (if in Attract mode), add a player (if ball 1 is in play), or restart the game if ball 2 or more is in play.

## Ball Save
**Credit display: 13**

Player 1 score display will show the current number of seconds of ball save given to a player after validating the playfield. 

Options: 0, 5, 8, 10, 15. Recommended setting: 8

## Music Level
**Credit display: 14**

Not currently implemented.

## Tournament Scoring Mode
**Credit display: 15**

Controls if extra balls and specials are awarded, or are instead awarded as points. 0 in the player 1 display indicates that Tournament Scoring is off, and the game will award Extra Balls or Credits for special. A 1 will award points instead.

## Tilt Warnings
**Credit display: 16**

Sets the number of times the tilt bob can touch the tilt ring and award a danger before tilting. Player 1 display shows the current value, hitting Start will cycle through the values. 

Options: 0, 1, 2. Recommended setting: 1

## Award Score Override
**Credit display: 17**

Normally, DIP switch 6 controls what the player gets for achieving an Award Score. DIP switch 6 set to “0” awards an extra ball, and “1” awards a credit. This override lets the operator set extra ball or credit for each of the 3 award scores (set in modes 1, 2, and 3) individually.

The Start button cycles through the values 0-7 and then jumps to 99. If set to 99, the DIP switch value is used. Otherwise, the award is based on the table below.

TO BE DOCUMENTED

## Balls Per Game
**Credit display: 18**

Normally, the balls per game (3 or 5) is controlled by DIP switch 7. This setting allows that to be overridden so the operator doesn’t have to go inside the head of the machine to change the value.

Pressing the Start button cycles through 3, 5, and 99. A setting of 99 means that the balls per game will be set by DIP switch 7. Otherwise, the balls per game comes from this setting.

## Scrolling Scores
**Credit display: 19**

When the player’s score exceeds 999,999, the scores will either wrap around to 0
or they will begin to scroll. A setting of “0” in this parameter will make the scores wrap to 0. A setting of “1” will allow the scores to scroll through the 6-digit displays.

Pressing the Start button will toggle this value.

## Extra Ball Award Value
**Credit display: 20**

If tournament scoring is enabled, this will control the scoring value given to a player who collects an extra ball from the left inlane when lit.

In this mode, the score is changed with the Start button. Pressing the button once increases the score by 1,000 points. Pressing and holding the Credit/ Reset button will gradually increase the score. Double-clicking the Start button will return the given score to zero.

Recommended Value: 25000

## Special Award Value
**Credit display: 21**

If tournament scoring is enabled, this will control the scoring value given to a player who collects a lit Special award.

In this mode, the score is changed with the Start button. Pressing the button once increases the score by 1,000 points. Pressing and holding the Credit/ Reset button will gradually increase the score. Double-clicking the Start button will return the given score to zero.

Recommended Value: 35000

## Dim Level
**Credit display: 22**

The Dim Level adjustment allows the operator to choose the duty cycle of “dim” lights. At a level of 2, a dim bulb will be lit 50% of the time. At a level of 3, a dim bulb will be lit 33% of the time. With some bulbs, a noticeable flash will be present at a duty cycle of 33%. With other bulbs, a 50% duty cycle will appear nearly the same as a fully-lit lamp. This adjustment should be re-checked on new machines or if the bulbs are changed.

When in this mode, the bonus lamps will toggle between the dim value and the full-lit value (every second). This allows the operator to see the contrast between the two.

Pressing the Start button will toggle the dim level between 2 & 3.

## Wild Fyre Time Award
**Credit display: 23**

This setting controls the amount of Wild Fyre double scoring time that the player is awarded for completing the 4-bank drop targets and achieving Wild Fyre Scoring. Use the start button to toggle through the values on the Player 1 display.

Options: 0, 15, 30, 45, 60
Recommended Value: 30

## Sharpshooter Award Value
**Credit display: 24**

Controls the value awarded to a player who plunges a lit Eject during the Sharpshooter mode.

In this mode, the score is changed with the Start button. Pressing the button once increases the score by 1,000 points. Pressing and holding the Credit/ Reset button will gradually increase the score. Double-clicking the Start button will return the given score to zero.

Recommended Value: 25000
