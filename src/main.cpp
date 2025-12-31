#include "Arduino.h"

// Pins for the first shift register
const int LATCH_PIN = 8;
const int CLOCK_PIN = 12;
const int DATA_PIN = 11;

// Pins for the joystick
const int X_PIN = A0;
const int Y_PIN = A1;
const int BUTTON_PIN = 7;

struct GameState
{
  // Tic tac toe board representation
  // 0 = empty, 1 = X, 2 = O
  uint8_t ticTacToe[9];

  // Position on the board (0..8) that is currently active
  uint8_t cursorPosition;

  // Blink state to indicate cursor position
  bool blinkIsOn;
  unsigned long lastBlinkMs = 0;

  bool isXTurn;

  // 0 = none, 1 = X, 2 = O, 3 = stalemate
  uint8_t winner;
};

static GameState gameState;

void initializeGameState(GameState &gameState)
{
  for (int i = 0; i < 9; i++)
  {
    gameState.ticTacToe[i] = 0;
  }
  gameState.isXTurn = true;
  gameState.cursorPosition = 0;
  gameState.blinkIsOn = false;
  gameState.lastBlinkMs = 0;
  gameState.winner = 0;
}

void updateCursorPosition(GameState &gameState)
{
  // Joystick movement classification
  static const int LEFT_THRESHOLD = 400;
  static const int RIGHT_THRESHOLD = 800;
  static const int UP_THRESHOLD = 400;
  static const int DOWN_THRESHOLD = 800;

  // todo add a deadzone around center so that writes aren't sent when joystick is centered

  // Prevent the joystick from moving too fast
  static const unsigned long JOYSTICK_LIMITER_MS = 300;
  static unsigned long lastMoveMs = 0;

  int xValue = analogRead(X_PIN);
  int yValue = analogRead(Y_PIN);

  unsigned long now = millis();

  if (now - lastMoveMs >= JOYSTICK_LIMITER_MS)
  {
    const int oldColumn = gameState.cursorPosition % 3;
    const int oldRow = gameState.cursorPosition / 3; // C++ automatically rounds down for integer division, so no need for something like Math.floor()

    int newColumn = oldColumn;
    int newRow = oldRow;

    if (xValue < LEFT_THRESHOLD)
    {
      // moving left
      newColumn = max(0, oldColumn - 1);
    }
    else if (xValue > RIGHT_THRESHOLD)
    {
      // moving right
      newColumn = min(2, oldColumn + 1);
    }

    if (yValue < UP_THRESHOLD)
    {
      // moving up
      newRow = max(0, oldRow - 1);
    }
    else if (yValue > DOWN_THRESHOLD)
    {
      // moving down
      newRow = min(2, oldRow + 1);
    }

    gameState.cursorPosition = (newRow * 3) + newColumn;

    lastMoveMs = now;
  }
}

void ticTacToeToBytes(const uint8_t ticTacToe[9], uint8_t cursorPosition, bool blinkIsOn, uint8_t outBytes[3])
{
  outBytes[0] = outBytes[1] = outBytes[2] = 0;

  for (uint8_t i = 0; i < 9; i++)
  {
    // In tic tac toe array, 1 = X = red, 2 = O = green, 0 = neither
    bool redOn = (ticTacToe[i] == 1);
    bool greenOn = (ticTacToe[i] == 2);

    // If blinking, both red and green are on regardless
    if (i == cursorPosition && blinkIsOn)
    {
      redOn = true;
      greenOn = true;
    }

    // On the board, the bicolor LEDs are wired to the shift registers in alternating color order
    // (LED 1 red pin, LED 1 green pin, LED 2 red pin, ...)
    // so red pins are even and green pins are odd
    uint8_t redBitIndex = i * 2;       // 0, 2, ...16
    uint8_t greenBitIndex = i * 2 + 1; // 1, 3, ...17

    // Use |= instead of = to write just that bit in the byte
    if (redOn)
    {
      outBytes[redBitIndex / 8] |= (1u << (redBitIndex % 8));
    }
    if (greenOn)
    {
      outBytes[greenBitIndex / 8] |= (1u << (greenBitIndex % 8));
    }
  }
}

void uniformColorToBytes(uint8_t color, uint8_t outBytes[3])
{
  static uint8_t board[9];

  for (int i = 0; i < 9; i++)
  {
    board[i] = color;
  }

  ticTacToeToBytes(board, 0, false, outBytes);
}

bool isBoardFull(const uint8_t ticTacToe[9])
{
  for (byte i = 0; i < 9; i++)
  {
    if (ticTacToe[i] == 0)
      return false;
  }
  return true;
}

int checkForGameOver(const uint8_t ticTacToe[9])
{
  static const byte THREE_IN_A_ROWS[8][3] = {
      {0, 1, 2}, // rows
      {3, 4, 5},
      {6, 7, 8},
      {0, 3, 6}, // columns
      {1, 4, 7},
      {2, 5, 8},
      {0, 4, 8}, // diagonals
      {2, 4, 6}};

  for (byte i = 0; i < 8; i++)
  {
    byte firstPosition = THREE_IN_A_ROWS[i][0];
    byte secondPosition = THREE_IN_A_ROWS[i][1];
    byte thirdPosition = THREE_IN_A_ROWS[i][2];

    if (ticTacToe[firstPosition] != 0 &&
        ticTacToe[firstPosition] == ticTacToe[secondPosition] &&
        ticTacToe[firstPosition] == ticTacToe[thirdPosition])
    {
      return ticTacToe[firstPosition]; // 1 (X) or 2 (O) wins
    }
  }

  // todo could later add a check for whether no win is possible (all possible three in a rows contain both X and O)

  if (isBoardFull(ticTacToe))
  {
    return 3; // stalemate
  }

  return 0; // no winner
}

void setup()
{
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(X_PIN, INPUT);
  pinMode(Y_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  initializeGameState(gameState);
}

void updateBoard(GameState &gameState)
{
  static int lastButtonValue = HIGH;

  int buttonValue = digitalRead(BUTTON_PIN);

  if (buttonValue == LOW && lastButtonValue == HIGH && gameState.ticTacToe[gameState.cursorPosition] == 0)
  {
    // todo may want to add button debounce later

    // Update tic tac toe board
    gameState.ticTacToe[gameState.cursorPosition] = gameState.isXTurn ? 1 : 2;

    // Check for game over (win or stalemate)
    gameState.winner = checkForGameOver(gameState.ticTacToe);

    // Switch players
    gameState.isXTurn = !gameState.isXTurn;
  }

  lastButtonValue = buttonValue;
}

void renderLEDs(uint8_t bytesToRender[3])
{
  digitalWrite(LATCH_PIN, LOW);

  // todo look into using direct port writes instead of shiftOut
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, bytesToRender[2]); // shift register 3
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, bytesToRender[1]); // shift register 2
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, bytesToRender[0]); // shift register 1

  digitalWrite(LATCH_PIN, HIGH);
}

void renderGameOver(GameState &gameState)
{
  // Stalemate (winner = 3): flash the whole board 4x alternating red/green
  // Winner (winner = 1 or 2): flash the whole board 4x in the winner color
  uint8_t winBytesState1[3];
  uint8_t winBytesState2[3];

  uniformColorToBytes(gameState.winner == 3 ? 1 : 0, winBytesState1);
  uniformColorToBytes(gameState.winner == 3 ? 2 : gameState.winner, winBytesState2);

  // Want to block here, so use delay
  for (size_t i = 0; i < 8; i++)
  {
    renderLEDs(winBytesState1);
    delay(200);
    renderLEDs(winBytesState2);
    delay(200);
  }
}

void renderGame(GameState &gameState)
{
  static const unsigned long BLINK_PERIOD_MS = 200;

  unsigned long now = millis();

  if (now - gameState.lastBlinkMs >= BLINK_PERIOD_MS)
  {
    gameState.blinkIsOn = !gameState.blinkIsOn;
    gameState.lastBlinkMs = now;
  }

  uint8_t boardBytes[3];

  ticTacToeToBytes(gameState.ticTacToe, gameState.cursorPosition, gameState.blinkIsOn, boardBytes);

  renderLEDs(boardBytes);
}

void loop()
{

  updateCursorPosition(gameState);

  updateBoard(gameState);

  if (gameState.winner == 3 || gameState.winner == 1 || gameState.winner == 2)
  {

    // Give a visual indication that the game is over
    renderGameOver(gameState);

    // Reinitialize the variables for a new game
    initializeGameState(gameState);
  }
  else
  {
    // no winner yet; render the board
    renderGame(gameState);
  }
}
