 /*
 OLEDuino Demo
 Copyright (C) 2012 Rick Pannen

 The Breakout game is ported from Sebastian Goscik's version for hackvision

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 
 This needs the u8glib from http://code.google.com/p/u8glib
 */

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_PIN_NONE, 5, 6);

const uint8_t title_1[] U8G_PROGMEM = {
0x00,0x07,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xE0,0x00,
0x00,0x08,0x00,0x60,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x03,0xF4,0x10,0x00,
0x00,0x0B,0xFF,0x10,0x00,0x00,0x00,0x20,0x80,0x00,0x00,0x04,0x09,0xD0,0x00,
0x00,0x0B,0x83,0xD0,0x00,0x00,0x00,0x2E,0xB8,0x00,0x00,0x05,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xD0,0x00,0x00,0x00,0x2E,0x44,0x00,0x00,0x05,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xD0,0x00,0x00,0x00,0x2E,0x34,0x00,0x00,0x05,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xCF,0xFF,0xF9,0xFF,0xAE,0x35,0xFF,0x7C,0xF9,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xC0,0x00,0x06,0x00,0x4E,0x66,0x00,0x83,0x01,0xE1,0xD0,0x00,
0x00,0x0B,0xFF,0x13,0xE7,0xF0,0xFF,0x0F,0xE0,0x7E,0x38,0x73,0xF9,0xD0,0x00,
0x00,0x0B,0xFF,0x13,0xE7,0xF0,0xFF,0x0F,0xC0,0x7E,0x38,0x73,0xF9,0xD0,0x00,
0x00,0x0B,0x83,0xDC,0x0E,0x0C,0x01,0xCF,0x80,0xE1,0x38,0x71,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xDC,0xCE,0x0C,0x01,0xCF,0xC0,0xE1,0x38,0x71,0xE1,0xD0,0x00,
0x00,0x0B,0x83,0xDD,0x2F,0xF0,0x7F,0xCE,0xE0,0xE1,0x38,0x71,0xE0,0x10,0x00,
0x00,0x0B,0x83,0xDD,0x2F,0xF0,0xFF,0xCE,0x70,0xE1,0x38,0x71,0xE0,0x10,0x00,
0x00,0x0B,0x83,0xDD,0x2E,0x00,0x81,0xCE,0x38,0xE1,0x38,0x71,0xE1,0xD0,0x00,
0x00,0x0B,0xFF,0x1D,0x27,0xFC,0xFE,0xCE,0x1C,0x7E,0x1F,0xC4,0x79,0xD0,0x00,
0x00,0x0B,0xFF,0x1D,0x17,0xFC,0x7E,0xCE,0x0C,0x7E,0x1F,0xCA,0x79,0xD0,0x00,
0x00,0x08,0x00,0x41,0x10,0x01,0x00,0x00,0xE1,0x00,0xC0,0x11,0x00,0x10,0x00,
0x00,0x07,0xFF,0xBE,0x0F,0xFE,0xFF,0xFF,0x1E,0xFF,0x3F,0xE0,0xFF,0xE0,0x00,
};

int leftButton = 9;
int rightButton = 8;
int enterButton = 7;

const byte width = 128;   //Width of screen
const byte height = 64;   //Hight of screen
int dx = -1;              //Initial movement of ball
int dy = -1;              //Initial movement of ball
int xb;                   //Balls starting possition
int yb;                   //Balls starting possition
boolean released;         //If the ball has been released by the player
byte xPaddle;             //X position of paddle
boolean isHit[5][12];     //Array of if bricks are hit or not
boolean bounced=false;    //Used to fix double bounce glitch
byte lives = 3;           //Amount of lives
byte level = 1;           //Current level
unsigned int score=0;     //Score for the game
unsigned int brickCount;  //Amount of bricks hit
byte pad1,pad2,pad3;       //Button press buffer used to stop pause repeating
byte oldpad1,oldpad2,oldpad3;
char text[16];            //General string buffer
boolean start=false;      //If in menu or in game
boolean gamemode=false;      //If in menu or in game
boolean initialDraw=false;//If the inital draw has happened
char initials[3];         //Initials used in high score
int divisor = 2;
int moveStep = 0;
int stringPos = 0;

//Ball Bounds used in collision detection
byte leftBall;
byte rightBall;
byte topBall;
byte bottomBall;

//Brick Bounds used in collision detection
byte leftBrick;
byte rightBrick;
byte topBrick;
byte bottomBrick;

long lastKeypress;

int lastButtonState;    // the debounce time;

void setup()
{ 
  pinMode(leftButton, INPUT);
  digitalWrite(leftButton, HIGH);
  pinMode(rightButton, INPUT);
  digitalWrite(rightButton, HIGH);
  pinMode(enterButton, INPUT);
  digitalWrite(enterButton, HIGH);
  
}

void movePaddle()
{
  //Move right
  if(xPaddle < width-20)
  {
    if(!digitalRead(rightButton))
    {  
      xPaddle++;
      lastKeypress = millis();
    }
  }

  //Move left
  if(xPaddle > 0)
  {
    if(!digitalRead(leftButton))
    {  
      xPaddle--;
      lastKeypress = millis();
    }
  } 
}

void moveBall()
{
  if(released)
  {
    //Move ball
    xb=xb + dx;
    yb=yb + dy;

    //Set bounds
    leftBall = xb;
    rightBall = xb+2;
    topBall = yb;
    bottomBall = yb+2;

    //Bounce off top edge
    if (yb<=0)
    {
      yb=2;
      dy=-dy;
    }

    //Lose a life if bottom edge hit
    if (yb>=70)
    {
      xPaddle = 54;
      yb=60; 
      released = false;
      lives--;
      drawLives();
      if (random(0, 2) == 0) {
        dx = 1;
      } 
      else {
        dx = -1;
      }
    }

    //Bounce off left side
    if (xb<=0)
    {
      xb=2;
      dx=-dx;
    }

    //Bounce off right side
    if (xb>=width-2)
    {
      xb=width-4;
      dx=-dx;
    }
    //Bounce off paddle
    if (xb+1>=xPaddle && xb<=xPaddle+20 && yb+2==63){
      dy=-dy;
      dx=((xb-(xPaddle+10))/2); //Applies spin on the ball
    }

    //Bounce off Bricks
    for (byte row = 0; row < 3; row++) {
      for (byte column = 0; column < 12; column++) {
        if (!isHit[row][column]){

          //Sets Brick bounds
          leftBrick = 10*column;
          rightBrick = 10*column+10;
          topBrick = 6*row+6;
          bottomBrick = 6*row+13;

          //If A collison has occured
          if (topBall <= bottomBrick && bottomBall>=topBrick && leftBall<=rightBrick && rightBall >=leftBrick){
            brickCount++;
            score += (level*10);
            isHit[row][column] = true;
            u8g.setColorIndex(1);
            u8g.drawBox(10*column ,2+6*row,8,4);

            //Vertical collision
            if (bottomBall>bottomBrick || topBall<topBrick){
              //Only bounce once each ball move
              if(!bounced){
                dy=-dy;
                yb+=dy;
                bounced=true;
              } 
            }

            //Hoizontal collision
            if (leftBall<leftBrick || rightBall>rightBrick){

              //Only bounce once brick each ball move
              if(!bounced){
                dx=-dx;
                xb+=dx;
                bounced=true;
              } 
            }
          }
        }
      }
    }
    //Reset Bounce
    bounced=false;
  }
  else{
    //Ball follows paddle
    xb=xPaddle+9;

    //Release ball if FIRE pressed
    pad3 = !digitalRead(enterButton);
    if(pad3 == 1 && oldpad3==0){  
      released=true;
      lastKeypress = millis();
      
      //Apply random direction to ball on release
      if (random(0, 2) == 0) {
        dx = 1;
      } 
      else {
        dx = -1;
      }
      //Makes sure the ball heads upwards 
      dy = -1;
    }
    oldpad3=pad3; 
  }
}

void drawBall()
{
  if (moveStep++ > divisor) {
    moveBall();
    moveStep = 0;
  }

  u8g.setColorIndex(1);
  u8g.drawPixel(xb,yb);
  u8g.drawPixel(xb+1,yb);
  u8g.drawPixel(xb,yb+1);
  u8g.drawPixel(xb+1,yb+1);
}

void drawPaddle()
{
  movePaddle();
  u8g.setColorIndex(1);
  u8g.drawBox(xPaddle,63,19,1);
}

void drawLives()
{
  sprintf(text, "LIVES:%u", lives);
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_04b_24);
  u8g.drawStr(0, 5, text);
}

void drawGameOver()
{
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_04b_24);
  u8g.drawStr(52, 54, "Game Over");
}

void drawScore()
{
  sprintf(text, "SCORE:%u", score);
  u8g.setColorIndex(1);
  u8g.setFont(u8g_font_04b_24);
  u8g.drawStr(80, 5, text);
}

void newLevel(){
  //Alter various variables to reset the game
  xPaddle = 50;
  yb=60;
  brickCount=0;
  released=false;
  
  //resets their values
  u8g.setColorIndex(1);
  for (byte row = 0; row < 3; row++) {
    for (byte column = 0; column < 12; column++) {
      isHit[row][column] = false;
    }
  }
}

void drawBricks() {
  u8g.setColorIndex(1);
  for (byte row = 0; row < 3; row++) {
    for (byte column = 0; column < 12; column++) {
      if (!isHit[row][column]) {
        u8g.drawBox(10*column ,6+6*row,8,4);
      }
    }
  }
}

boolean titleScreen() {
  
  u8g.setColorIndex(1);
  //Draws the main title
  u8g.drawBitmapP(4, 0, 15, 19, title_1);

  u8g.setColorIndex(1);
  if (score > 0) {
    u8g.setFont(u8g_font_tpssb);
    u8g.drawStr(30, 35, F("GAME OVER!"));
    sprintf(text, "SCORE:%u", score);
    u8g.drawStr(30, 48, text);
  } else {
    u8g.setFont(u8g_font_6x12);
    u8g.drawStr(10, 35, F("Left two buttons:"));
    u8g.drawStr(57, 45, F("Move paddle"));
  }
  
  u8g.setFont(u8g_font_6x12);
  u8g.drawStr(10, 60, F("Right button: Start"));

  pad3 = !digitalRead(enterButton);
  if(pad3 == 1 && oldpad3==0) {
    newLevel();
    score = 0;
    lives = 3;
    lastKeypress = millis();
    return true;
  }
  oldpad3 = pad3;
  
  return false;
}

void game()
{  
  if (lives>0){
    
    drawLives();
    drawScore();
    drawBricks();
    drawPaddle();
    drawBall();

    if(brickCount==36){
      level++;
      newLevel(); 
    }
  }
  else{
    delay(1000);
    start=false;
    // gamemode=false;
  }
}

void displayName() {
  
  u8g.setFont(u8g_font_fub20);
  u8g.drawStr(0, 30, "Rick P.");
  u8g.drawLine(0, 32, 128, 32);
  u8g.setFont(u8g_font_unifont);
  u8g.drawStr( 20, 44, "attraktor e.v.");
  pad1 = !digitalRead(leftButton);
  if(pad1 == 1 && oldpad1==0) {
    gamemode = true;
    lastKeypress = millis();
  }
  oldpad1 = pad1;
  
  pad2 = !digitalRead(rightButton);
  if(pad2 == 1 && oldpad2==0) {
    gamemode = true;
    lastKeypress = millis();
  }
  oldpad1 = pad1;
  
  pad3 = !digitalRead(enterButton);
  if(pad3 == 1 && oldpad3==0) {
    gamemode = true;
    lastKeypress = millis();
  }
  oldpad3 = pad3;
}

void loop() 
{
  u8g.firstPage();
  do {
    if (gamemode) {
      if (start) {
        game();
      } else {
        start = titleScreen();
      }
      if (millis() - lastKeypress > 15000) {
        gamemode = false;
      }
    } else {
      displayName();
    }
  }
  while(u8g.nextPage());
}
