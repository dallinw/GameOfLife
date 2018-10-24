#include <Adafruit_GFX.h>
#include <TouchScreen.h>
#include <Adafruit_TFTLCD.h>
#include "GOLCell.h"

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

// calibration mins and max for raw data when touching edges of screen
// YOU CAN USE THIS SKETCH TO DETERMINE THE RAW X AND Y OF THE EDGES TO GET YOUR HIGHS AND LOWS FOR X AND Y
#define TS_MINX 210
#define TS_MINY 210
#define TS_MAXX 915
#define TS_MAXY 910

//SPI Communication
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

//Color Definitons
#define BLACK   0x0000
#define MAGENTA 0xB0B0
#define RED     0xD020
#define ORANGE  0xB50F
#define WHITE   0xFFFF

#define RLED 11
#define BLED 10

#define MINPRESSURE 1
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 364);
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//Container variables for touch coordinates
int X, Y, Z;
int width; int height;
bool toggle = 1;
const int NUM_ROWS = 8;
const int NUM_COLS = 9;
int BOX_SIZE = 28;
Cell cells[NUM_ROWS][NUM_COLS];

void setup() {
  Serial.begin(9600);

  tft.reset();
  width = tft.width();
  height = tft.height();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);

  //Background color
  tft.fillScreen(BLACK);
  drawBoard();
  drawButton("START");
  updateBoard();
  digitalWrite(RLED, HIGH);
}
void loop() {
    check_pause();
    //if unpaused, run the game
    if(toggle == 0){
      theGame();
      updateBoard();
    }
    //if paused, look for touch input
    if(toggle == 1){
      check_squares();
    }
}

void check_pause(){
  retrieveTouch();
    if (Z > MINPRESSURE && Z < MAXPRESSURE) {
      //button
      if(Y > 270){
        toggle = !toggle;
        if(toggle == 0){
          digitalWrite(BLED, HIGH);
          digitalWrite(RLED, LOW);
          drawButton("PAUSE");
        }
        else if(toggle == 1){
          digitalWrite(RLED, HIGH);
          digitalWrite(BLED, LOW);
          drawButton("START");
        }
      }
   }
}
void check_squares(){
  retrieveTouch();
    if (Z > MINPRESSURE && Z < MAXPRESSURE) {
      flipSquare(X, Y);
      updateBoard();
    }
}

void drawBoard() {
  int ITER = 29;
  //update board
  for(int i = 0; i < NUM_ROWS; i++){
    for(int j = 0; j < NUM_COLS; j++){
      int xpx = (i*ITER)+5;
      int ypx = (j*ITER)+5;
      cells[i][j].setXmin(xpx);
      cells[i][j].setXmax(xpx+BOX_SIZE);
      cells[i][j].setYmin(ypx);
      cells[i][j].setYmax(ypx+BOX_SIZE);
      tft.drawRect(xpx, ypx, BOX_SIZE, BOX_SIZE, BLACK);
    }
  }
}
void updateBoard(){
  for(int i = 0; i < NUM_ROWS; i++){
    check_pause();
    for(int j = 0; j < NUM_COLS; j++){
      Cell temp = cells[i][j];
      bool color = temp.isAlive();
      //alive
      if(color == 1){
        tft.fillRect(temp.getXmin(), temp.getYmin(), BOX_SIZE, BOX_SIZE, RED);
      }
      if(color == 0){
        tft.fillRect(temp.getXmin(), temp.getYmin(), BOX_SIZE, BOX_SIZE, WHITE);
      }
      //dead
    }
  }
}
void drawButton(String text){
  int BUTTON_H = 230;
  int BUTTON_W = 36;
  tft.fillRect(5, 277, BUTTON_H, BUTTON_W, WHITE);
  tft.setCursor(80,282);
  tft.setTextSize(3);
  if(text == "START"){
    tft.setTextColor(ORANGE);
  }
  if(text == "PAUSE")
  {
    tft.setTextColor(MAGENTA);
  }
  tft.print(text);
}
void flipSquare(int x, int y){
  for(int i = 0; i < NUM_ROWS; i++){
    for(int j = 0; j < NUM_COLS; j++){
      Cell temp = cells[i][j];
      if(temp.getXmin() < x && temp.getXmax() > x &&
         temp.getYmin() < y && temp.getYmax() > y){
         cells[i][j].setAlive(!temp.isAlive());
       }
    }
  }
}

void theGame(){
  for (int i=0; i<NUM_ROWS; i++){
    check_pause();
    for (int j=0; j<NUM_COLS; j++){
      // Check the cell's current state, and count its living neighbors.
      bool living = cells[i][j].isAlive();
      int count = how_many_neighbors(i, j);
      bool result = false;
      // Apply the rules and set the next state.
      if (living && count < 2)
        result = false;
      if (living && (count == 2 || count == 3))
        result = true;
      if (living && count > 3)
        result = false;
      if (!living && count == 3)
        result = true;
 
       cells[i][j].setAlive(result);
    }
  }
}

int how_many_neighbors(int x, int y)
{
  int count = 0;
  //right
  if (x != NUM_ROWS - 1)
    if (cells[x + 1][y].isAlive())
      count++;
 
    //lower right
    if (x != NUM_ROWS - 1 && y != NUM_COLS - 1)
        if (cells[x + 1][y + 1].isAlive())
            count++;
 
    //bottom
    if (y != NUM_COLS - 1)
        if (cells[x][y + 1].isAlive())
            count++;
 
    //lower left
    if (x != 0 && y != NUM_COLS - 1)
        if (cells[x - 1][y + 1].isAlive())
            count++;
 
    //left
    if (x != 0)
        if (cells[x - 1][y].isAlive())
            count++;
 
    //upper left
    if (x != 0 && y != 0)
        if (cells[x - 1][y - 1].isAlive())
            count++;
 
    //top
    if (y != 0)
        if (cells[x][y - 1].isAlive())
            count++;
 
    //upper right
    if (x != NUM_ROWS - 1 && y != 0)
        if (cells[x + 1][y - 1].isAlive())
            count++;
 
  return count;
}
void retrieveTouch()
{
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  //If sharing pins, you'll need to fix the directions of the touchscreen pins
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  //finagling for horizontal orientation again
  X = width - map(p.x, TS_MAXX, TS_MINX, 0, width);
  Y = height - map(p.y, TS_MAXY, TS_MINY, 0, height);
  Z = p.z;

}
