# Arduino Tic Tac Toe

## Components

- 9 bicolor red/blue common cathode LEDs
- 18 470 ohm or similar resistors
- 3 shift registers similar or equivalent to 74HC595
- 1 joystick with X, Y, and switch outputs similar or equivalent to KY-023
- 1 Arduino uno

## Wiring

- Joystick pin VRx to Arduino pin A0
- Joystick pin VRy to Arduino pin A1
- Joystick pin SW to Arduino pin D7
- Joystick pin 5V to 5V
- Joystick pin ground to ground

- Shift register #1 pin 1 (Q1) to 470 ohm resistor to LED #1 blue pin
- Shift register #1 pin 2 (Q2) to 470 ohm resistor to LED #2 red pin
- Shift register #1 pin 3 (Q3) to 470 ohm resistor to LED #2 blue pin
- Shift register #1 pin 4 (Q4) to 470 ohm resistor to LED #3 red pin
- Shift register #1 pin 5 (Q5) to 470 ohm resistor to LED #3 blue pin
- Shift register #1 pin 6 (Q6) to 470 ohm resistor to LED #4 red pin
- Shift register #1 pin 7 (Q7) to 470 ohm resistor to LED #4 blue pin
- Shift register #1 pin 8 (ground) to ground
- Shift register #1 pin 9 (Serial out) to Shift register #2 pin 14
- Shift register #1 pin 10 (MR) to 5V
- Shift register #1 pin 11 (SH_CP, clock pin) to Arduino pin D12 + Shift register #2 pin 11
- Shift register #1 pin 12 (ST_CP, latch pin) to Arduino pin D8 + Shift register #2 pin 12
- Shift register #1 pin 13 (OE) to ground
- Shift register #1 pin 14 (DS, serial in) to Arduino pin D11
- Shift register #1 pin 15 (Q0) to 470 ohm resistor to LED #1 red pin
- Shift register #1 pin 16 (Vcc) to 5V

- Shift register #2 pin 1 (Q1) to 470 ohm resistor to LED #5 blue pin
- Shift register #2 pin 2 (Q2) to 470 ohm resistor to LED #6 red pin
- Shift register #2 pin 3 (Q3) to 470 ohm resistor to LED #6 blue pin
- Shift register #2 pin 4 (Q4) to 470 ohm resistor to LED #7 red pin
- Shift register #2 pin 5 (Q5) to 470 ohm resistor to LED #7 blue pin
- Shift register #2 pin 6 (Q6) to 470 ohm resistor to LED #8 red pin
- Shift register #2 pin 7 (Q7) to 470 ohm resistor to LED #8 blue pin
- Shift register #2 pin 8 (ground) to ground
- Shift register #2 pin 9 (Serial out) to Shift register #3 pin 14
- Shift register #2 pin 10 (MR) to 5V
- Shift register #2 pin 11 (SH_CP, clock pin) to Shift register #1 pin 11 + Shift register #3 pin 11
- Shift register #2 pin 12 (ST_CP, latch pin) to Shift register #1 pin 12 + Shift register #3 pin 12
- Shift register #2 pin 13 (OE) to ground
- Shift register #2 pin 14 (DS, serial in) to Shift register #1 pin 9
- Shift register #2 pin 15 (Q0) to 470 ohm resistor to LED #5 red pin
- Shift register #2 pin 16 (Vcc) to 5V

- Shift register #3 pin 1 (Q1) to 470 ohm resistor to LED #9 blue pin
- Shift register #3 pin 8 (ground) to ground
- Shift register #3 pin 10 (MR) to 5V
- Shift register #3 pin 11 (SH_CP, clock pin) to Shift register #2 pin 11
- Shift register #3 pin 12 (ST_CP, latch pin) to Shift register #2 pin 12
- Shift register #3 pin 13 (OE) to ground
- Shift register #3 pin 14 (DS, serial in) to Shift register #2 pin 9
- Shift register #3 pin 15 (Q0) to 470 ohm resistor to LED #9 red pin
- Shift register #3 pin 16 (Vcc) to 5V

- All LED cathodes to ground
- LEDs should be in a 9x9 grid. The code assumes this layout:

  Column 1 | Column 2 | Column 3
  --- | --- | ---
  LED #1 | LED #2 | LED #3
  LED #4 | LED #5 | LED #6
  LED #7 | LED #8 | LED #9

## Output

- All LEDs are off initially. LED #1 flashes between off and red+blue.
- Moving the joystick changes which LED is flashing to indicate your current position.
- Clicking the joystick places the current player's color at that position.
- If 3 in a row of a color is achieved, the whole board flashes that color several times and then the game resets.
- If all 9 LEDs are lit without a 3 in a row of any color, the whole board flashes alternating red/blue and then the game resets.

Demo:

https://github.com/user-attachments/assets/7d1e7ce6-662b-48b1-90e0-7256749e72a0

## Potential future additions

- Add a debounce for the joystick button
- Add a dead zone around the center of the joystick so that writes aren't sent when joystick is centered
- Make the code color agnostic, and just refer to X/O instead. (Right now the code talks about red/green, even though any bicolor combo is supported and the README mentions red/blue)
- Make winner an enum (and possibly board values too)
- Add a check for whether no win is possible (all possible three in a rows contain both X and O), and indicate a stalemate at that point
- Look into using direct port writes instead of shiftOut
