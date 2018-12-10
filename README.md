Micrographer
by Andrew Lynn and Alex Matus

PARTS:
  STM32F0x1 (microcontroller),
  CFAL1602 (lcd display),
  3x4 Matrix Keypad (x2),
  Adafruit 32x32 RGB LED Matrix.
  
Methodology and pinouts can be found inside main.c.

Function:
  The user is prompted with a Mode Select - 2D or 3D.
  After the user selects the mode, 'y=' or 'z=' is displayed to begin the function input.
  The user then types in a valid function (no current error handling) before pressing enter.
  The graph is displayed linearly (2D) or by a "heat map" (3D).
