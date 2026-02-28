#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define BIRD_WIDTH        8
#define BIRD_HEIGHT       10

#define PIPE_WIDTH     6
#define PIPE_OFF      30

void DrawPipe(uint8_t);

#define BTN   2
#define LED   3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void gameover(void);

bool IsJump=0;
volatile bool IsGameStart = 0;
int8_t PipePos = 0;
int8_t PipeHeight = 0;
int8_t I=127;
int8_t BirdX = 20, BirdY=0, UpBirdX, UpBirdY;

uint8_t Score=0;

static const unsigned char PROGMEM bird_bmp[] =
{ 
  0x3C, 0x72, 0xF7, 0xF3, 0xFF, 0xFF, 0x7E, 0x3C, 0x28, 0x48
};

#define BACKGROUND_HEIGHT 64
#define BACKGROUND_WIDTH 128

void setup() {  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  while(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  }
  display.clearDisplay();       //clear display before game start;
  pinMode(BTN , INPUT_PULLUP);  //Init button state
  pinMode(LED, OUTPUT);         //Test led
  PipePos = 127;                 //start pipe drawing in middle of screen 
  PipeHeight = 10;
  BirdY = 32;
  attachInterrupt(digitalPinToInterrupt(BTN), Blink, FALLING);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(32, 10);
  display.println("Flappy Bird");
  display.display();
}

void loop() {
  while(!IsGameStart){}

  display.clearDisplay();

  if(BirdY + BIRD_HEIGHT >= SCREEN_HEIGHT){
    gameover();
  }
  else if(((BirdX + BIRD_WIDTH) >= PipePos) && (BirdY < PipeHeight - 1)){                             //Upper height is sub by 1 to get proper collision
    gameover();
  }
  else if((BirdX + BIRD_WIDTH >= PipePos) && ((BirdY + BIRD_HEIGHT) > (PipeHeight + PIPE_OFF+1))){    //Lower height is Add by 1 to get proper collision
    gameover();
  }
  PipePos = PipePos - 3;
  if(PipePos <= -6) {
    Score ++;               //After successfully chacking about collision update the sore
    PipePos=127;
    PipeHeight = random(5, 40);
  }
  if(UpBirdY){
    UpBirdY --;
    BirdY --;
    if(BirdY<=0){
      BirdY = 0;
      UpBirdY = 0; 
    }
  }else{
    BirdY ++;
  }

  DrawPipe(PipePos);
  display.drawBitmap(BirdX, BirdY, bird_bmp, BIRD_WIDTH, BIRD_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(10);
}

void Blink()
{
  if(!IsGameStart){
    IsGameStart = 1;
  }else{
    UpBirdY = 7;      //when button pressed bird have to jump by 7
  }
}

void DrawPipe(int8_t Pos)
{
  display.fillRect (Pos, 0, PIPE_WIDTH, PipeHeight, SSD1306_WHITE);         //up
  display.fillRect (Pos, (PipeHeight + PIPE_OFF), PIPE_WIDTH, (SCREEN_HEIGHT - (PipeHeight + PIPE_OFF)), SSD1306_WHITE);         //bottom
}

void gameover()
{
  delay(900);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(3, 10);
  display.println("Game over");
  display.setCursor(50, 40);
  display.print(Score);
  display.display();
  PipePos = 127;
  PipeHeight = 10;
  IsGameStart = false;
  BirdY=32;
  Score = 0;
  while(!IsGameStart){};
}