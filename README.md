# Monkey64
A Nintendo 64 emulator for PSP.

This repository (master branch) has original code from the Monkey64 2.0 release, with the only modifications being in the Makefile to compile under the latest toolchain.

# Credits:
- PSMonkey (http://nemo.dcemu.co.uk/)
- Pochi
- Zilmar & PJ64 Team (special thanks)
- Lac
- Strmnnrmn
- GPF
- $n!pR
- Wally
- Evab3va
- Psyco
- wraggster
- Zeenbor
- Clessy

## Notes
- This emulator is in a primitive state. Commercial games run, but slowly, and glitches are present. It is running with an interpreter, with no dynamic recompiler.
- Before loading a ROM, you must set the microcode used by the game. Selecting the wrong microcode may result in incompatibility.
- ROMs must be located in a folder called "n64" at the root of your memory stick (or flash0 for PSP go).

## Original Info
// Info
Monkey64 is an n64 emulator started from scratch that takes the psp limitations into consideration. The
ultimate goal is solid n64 emulation at 30-60fps on the psp. This will not come instantly but maybe 6 months 
now it will be a reality.

At the current time Monkey64 supports a large array of n64 homebrew / PD roms that draw to the framebuffer & commercial roms.
While not all commercial roms run & the ones that do are a bit glitchy, things are improving on a daily basis.

// Changelog

- 2.0 -
.) Way too much to list, Commercial roms work.
.) Fast 3D/SW 2.0, Fast 3D Ext, F3DEX1.0 & F3DEX2.0 Microcodes supported
.) Many gfx & Hle issues still exist. Missing a surface clipper & n64 combiner support.
.) 4Kb Eeprom supported 
- (if you wish to use your eeproms from pc emu's, rename them the same as the rom and change the extension to .epr)

- 1.0 -
.) First Release, Runs a majority of homebrew roms that render directly to the CFB.

// To Use
1) Create a folder labeled "n64" on the root of your memory stick.
2) Copy your roms into the "n64" folder (roms MUST NOT be zipped).
..) Example "G:\n64\" (replace G: with your psp drive letter)
3) Copy the files from the folder containing your psp version number
..) Example, 1.0 users will go into "1.0 or 2.0" folder and copy the psp directory to the root of the memory stick.
4) Run the software :)

// Controls

// Menu
Up & Down - Change Selection
Left & Right - Change Option
X / Cross - Select

// In Game
Left Trigger & Select - Capture Screenshot (stored in location of eboot.pbp)
Select - Exit Emulation

// N64 Controls
PSP      N64
DPad   - DPad
Analog - Analog
Start  - Start
R Trig - Z Trig

If L Trig Held
Triangle - C Up
Circle   - C Right
Cross    - C Down
Square   - C Left
else
Square   - L Trig
Triangle - R Trig
Cross    - B Button
Circle   - A Button

// Thanks & Greetings
Zilmar & PJ64 Team, Lac, Strmnnrmn, GPF, $n!pR, Wally, Evab3va, Psyco, wraggster, Pochi, Zeenbor, Clessy & all at dcemu!
