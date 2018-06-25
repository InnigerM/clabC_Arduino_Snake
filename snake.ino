/******************************************
 * Snake game for Arduino Uno and Adafruit 2,8" touch screen for Arduino
 * 
 * Author:  Inniger Marco, Koller Kevin
 * 
 * Adafruit libraries used:
 *  Adafruit_ILI9341(Screen controller library): https://github.com/adafruit/Adafruit_ILI9341
 *  Adafruit-GFX-Library(Graphics Library): https://github.com/adafruit/Adafruit-GFX-Library
 *  Adafruit_STMPE610(Touchscreen): https://github.com/adafruit/Adafruit_STMPE610
 *  
 ******************************************/
 
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//1=main menu,2=game,3=game over
byte screen;

//Size of the snake (on proper situation it would be 22x23=506 positions of the grid, though it's limited by arduino's memory
#define snakesize 250
//Snake's speed between movement(miliseconds)
#define velocity 250

//Screen sizes and grid
#define maxx 230
#define minx 190
#define maxy 80
#define miny 40
#define gridx 24
#define gridy 29

//Direction assignments and read out value from processing part
char val;
const int UP = 1;
const int LEFT = 2;
const int DOWN = 3;
const int RIGHT = 4;

void setup(void) {
  randomSeed(analogRead(0));
  Serial.begin(115200);         //Start serial
  tft.begin();                //Start touchscreen
  tft.setRotation(2);         //SetRotation to 180°
  printMenu();
}

void printScore(int score){
  tft.fillRect(90,297,90,20,ILI9341_BLACK);
  tft.setCursor(90, 297);
  tft.print(score);
}

void updateSnake(byte newPos[], byte oldPos[]){
  tft.fillRect(oldPos[0]*10,oldPos[1]*10,10,10,ILI9341_BLACK);  //Fill old position of the snake
  tft.fillRect(newPos[0]*10,newPos[1]*10,10,10,ILI9341_GREEN);  //Fill new position of the snake
}


void printGameOver(int score, byte grid[gridy][gridx]){
  screen=3;
  //Set screen to NULL except Score
  for(int y=0;y<gridy;y++){
    for(int x=0;x<gridx;x++){
      grid[y][x]=0;
    }
  }
  //Print game Over screen
  tft.fillRect(40,55,160,200,ILI9341_LIGHTGREY);
  tft.drawRect(41,56,158,198,ILI9341_MAROON);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(4);
  tft.setCursor(55, 60);
  tft.print("Game");
  tft.setCursor(90, 90);
  tft.print("Over");
  tft.setTextColor(ILI9341_NAVY);
  tft.setTextSize(3);
  tft.setCursor(70, 120);
  tft.print("Score");
  tft.drawRect(65,150,110,30,ILI9341_NAVY);
  tft.setTextColor(ILI9341_DARKCYAN);
  tft.setCursor(70, 155);
  tft.print(score);
  tft.fillRect(45,185,151,20,ILI9341_RED);
  tft.fillRect(65, 217, 115, 20, ILI9341_BLUE);
  tft.setCursor(50, 188);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print("Enter / Play");
  tft.setCursor(70, 220);
  tft.print("<- / Menu");
}

void printGameScreen(){
  screen=2;
  tft.fillScreen(ILI9341_BLACK);
  tft.drawLine(0, 290, 240, 290, ILI9341_YELLOW);

  // tft.drawLine(0, 290, 240, 290, ILI9341_YELLOW);
  //Print score overlay
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10 , 297);
  tft.println("Score: ");
  printScore(0);
}

void printMenu(){
  screen=1;
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(5);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(50 , 60);
  tft.println("SNAKE");
  tft.fillRect(0,120,300,70,ILI9341_RED);
  tft.setTextSize(3);
  tft.setCursor(53 , 133);
  tft.println("To Start");
  tft.setCursor(28, 158);
  tft.println("Press Enter");
  tft.setCursor(0 , 310);
  tft.setTextSize(1);
  tft.println("Created by Marco Inniger, Kevin Koller");
}

void loop()
{
  byte snakeBuffer[snakesize][2]={0};
  byte grid[gridy][gridx]={0};
  long addpos;
  long delpos;
  byte arrow, lastarrow;
  int score;
  byte newPos[2];
  byte oldPos[2];
  bool gendot;
  bool incaxys;
  byte increment;
  bool initgame=false;
  
  while(true){
    //This if statement will update the snake position while we are in the game screen
    if (screen==2){
      if((arrow!=0)&&(((lastarrow+2!=arrow)&&(lastarrow-2!=arrow))||(score==0))){
        if((arrow%2)!=0){
          incaxys=true;
        }else{
          incaxys=false;
        }
        if((arrow>1)&&(arrow<4)){
          increment=1;
        }else{
          increment=-1;
        }
        lastarrow=arrow;
      }
      arrow=0;
      newPos[incaxys]+=increment;
      if((newPos[0]<0)|(newPos[0]>23)|(newPos[1]<0)|(newPos[1]>28)){        
        printGameOver(score,grid);
        
      }else{
        snakeBuffer[addpos][0]=newPos[0];
        snakeBuffer[addpos][1]=newPos[1];
        addpos++;
        if(addpos>(snakesize-1)){
          addpos=0;
        }
        grid[newPos[1]][newPos[0]]++;
        grid[oldPos[1]][oldPos[0]]=0;

        //Check various grid events
        switch(grid[newPos[1]][newPos[0]]){
          case 2:
            printGameOver(score, grid);
            break;
          case 4:
            score+=10;
            printScore(score);
            gendot=true;
            if((score/10)<snakesize){
              delpos--;
            }
          default:
            updateSnake(newPos,oldPos);
            delpos++;
            if(delpos>(snakesize-1)){
              delpos=0;
            }
            oldPos[0]=snakeBuffer[delpos][0];
            oldPos[1]=snakeBuffer[delpos][1];
            
            break;
        }
      }
      //We generate a random dot to feed the snake
      while(gendot){
        byte posy=random(gridy-1);
        byte posx=random(gridx-1);
        if(grid[posy][posx]==0){
          tft.fillRect(posx*10,posy*10,10,10,ILI9341_RED);
          grid[posy][posx]=3;
          gendot=false;
        }
      }
    }
    // Button control, only triggered when there's data from the arduino
    val = Serial.read();            
    if(val > 0){
      switch(screen){
        case 1:
          //menu and start game parameters
          if (val == '1'){
            initgame=true;
          }
        break;
        case 2:
        //Check arrows on in game screen
          if (val == '4'){
            arrow=LEFT;
          } else if(val == '3'){
            arrow = DOWN;
          } else if(val == '2'){
            arrow = RIGHT;
          } else if(val == '5'){
            arrow = UP;
          } else if(val == '6'){
            printMenu();
          }
          break;
        case 3:
          if(val == '6'){
            printMenu();
          }else if(val == '1'){
            initgame=true;
          }
          break;
      }
      if (initgame){//Set variables for the game initialization
        initgame=false;
        arrow=2;
        lastarrow=1;
        score=0;
        addpos=1;
        delpos=0;
        incaxys=false;
        increment=1;
        gendot=true;
        newPos[0]=-1;newPos[1]=0;
        printGameScreen();
      }
    }
    delay(velocity);
  }
}

