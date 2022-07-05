/*==========================================
           Snake
        - ZombieNW -

        Made With Devkitpro And GRRLIB
============================================*/
//Libraries
#include <grrlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <wiiuse/wpad.h>
#include <time.h>
#include <asndlib.h>
#include <mp3player.h>

//Code
#include "global.h"
//Fonts
#include "Rubik_ttf.h"
#include "LCDSolid_ttf.h"
//Backgrounds and popups
#include "textbox_png.h"
#include "scorebg_png.h"
#include "mmbg_png.h"
#include "logo_png.h"
#include "logo_intro_png.h"
#include "wii_warning_png.h"
//Hand
#include "pointer_png.h"
//Sounds
#include "eat_mp3.h"
//Music
#include "menu_mp3.h"



//Images
GRRLIB_texImg *GFX_textbox_texture;
GRRLIB_texImg *GFX_scorebg_texture;
GRRLIB_texImg *GFX_mmbg_texture;
GRRLIB_texImg *GFX_logo_texture;
GRRLIB_texImg *GFX_logo_intro_texture;
GRRLIB_texImg *GFX_pointer_texture;
GRRLIB_texImg *GFX_wii_warning_texture;

//Structures
typedef struct{ //Snake Structure
    Vector2 body[732];
    int length;
}t_snake;
typedef struct{ //Apple structure
    Vector2 pos;
}t_fruit;

ir_t P1Mote; //Player 1 wiimote

//Variables
int squareSize = 20; //The size of each mark on the grid
u32 snakecolor = 0x4AB340FF; //Snake color
u32 snakeheadcolor = 0x3B7A22FF; //Snake head color
u32 snakebackcolor = 0x65B340FF; //Snake back color
u32 fruitcolor = 0xB52D2DFF; //Fruit color
int directionX = 1; //X Direction
int directionY = 0; //Y Direction
int updateDelay = 0;//timing
int targetDelay = 5;//timing
int startingSpeed = 5;//timing
int inputdelay = 0;
int mmselect = 1;
int creditmenu = 0;
int titledrop = 200;
int wiiwarningcooldown = 300;

//Importante
//Importante
//Importante
int gamemode = 0;
//Importante
//Importante
//Importante

//Functions
void ExitGame(){ //Clear Ram and Exit Game
    GRRLIB_Exit(); // Be a good boy, clear the memory allocated by GRRLIB
    SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);//Power-Off System
}

//Draw the snake
void DrawSnake(t_snake snake, int gridSize){//Draw the snake to the screen
    for (int i = 0; i < snake.length; i++) { //for each loop
        if(snake.body[i].x){//Checks if the part exists
            GRRLIB_Rectangle(snake.body[i].x * gridSize, snake.body[i].y * gridSize, squareSize, squareSize, snakecolor, true);//Draw Square
        }
    }
    //Head
    GRRLIB_Rectangle(snake.body[0].x * gridSize, snake.body[0].y * gridSize, squareSize, squareSize, snakeheadcolor, true);//Draw Square
    //Back
    GRRLIB_Rectangle(snake.body[snake.length].x * gridSize, snake.body[snake.length].y * gridSize, squareSize, squareSize, snakebackcolor, true);//Draw Square
}
//Draw the fruit
void DrawFruit(t_fruit fruit, int gridSize){//Draw the fruit to the screen
    GRRLIB_Rectangle(fruit.pos.x * gridSize, fruit.pos.y * gridSize, squareSize, squareSize, fruitcolor, true);//Draw Square
}

//All this is the fruit detection stuff
struct cfeOne{
    t_snake snake;
    t_fruit fruit;
};
typedef struct cfeOne Struct;
Struct CheckForEating(t_snake snake, t_fruit fruit){
    Struct s;
    if(snake.body[0].x == fruit.pos.x && snake.body[0].y == fruit.pos.y){
        //Add one more to the snake
        snake.body[snake.length + 1].x = snake.body[snake.length].x;
        snake.body[snake.length + 1].y = snake.body[snake.length].y;
        snake.length += 1;

        MP3Player_Stop();
        MP3Player_PlayBuffer(eat_mp3, eat_mp3_size, NULL);

        //Make sure the fruit doesn't spawn on the snake
        bool isPosGood = false;
        while(isPosGood == false){
            fruit.pos.x = rand() % 32;
            fruit.pos.y = rand() % 24;
            isPosGood = true;
            for (int i = 0; i < snake.length; i++) { //for each loop
                if(snake.body[i].x == fruit.pos.x && snake.body[i].y == fruit.pos.y){
                    isPosGood = false;
                }
            }
            if(fruit.pos.x < 7){
                isPosGood = false;
            }
        }
    }
    //Return the snake and fruit
    s.snake = snake;
    s.fruit = fruit;
    return s;
}
//End of fruit detection stuff
int main(int argc, char **argv) {
    srand(time(0));
    GRRLIB_Init();// Initialise the Graphics & Video subsystem
    WPAD_Init();// Initialise the Wiimotes
    ASND_Init();// Initialise the audio subsystem
	MP3Player_Init();// Initialise the audio subsystem
    GRRLIB_ttfFont *RubikFont = GRRLIB_LoadTTF(Rubik_ttf, Rubik_ttf_size);//Font
    GRRLIB_ttfFont *LCDSolidFont = GRRLIB_LoadTTF(LCDSolid_ttf, LCDSolid_ttf_size);//Font
    GFX_textbox_texture = GRRLIB_LoadTexturePNG(textbox_png);
    GFX_scorebg_texture = GRRLIB_LoadTexturePNG(scorebg_png);
    GFX_mmbg_texture = GRRLIB_LoadTexturePNG(mmbg_png);
    GFX_logo_texture = GRRLIB_LoadTexturePNG(logo_png);
    GFX_logo_intro_texture = GRRLIB_LoadTexturePNG(logo_intro_png);
    GFX_pointer_texture = GRRLIB_LoadTexturePNG(pointer_png);
    GFX_wii_warning_texture = GRRLIB_LoadTexturePNG(wii_warning_png);
    GRRLIB_SetMidHandle(GFX_textbox_texture, true);
    GRRLIB_SetMidHandle(GFX_logo_texture, true);
    GRRLIB_SetMidHandle(GFX_logo_intro_texture, true);
    GRRLIB_SetMidHandle(GFX_pointer_texture, true);
    GRRLIB_SetMidHandle(GFX_wii_warning_texture, true);

    WPAD_SetIdleTimeout(90);//Timeout Wiimotes after 90 seconds
    WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);//Something?
    u16 WinW = rmode->fbWidth; //Screen Width
    u16 WinH = rmode->efbHeight; //Screen Height
    int gridWidth = WinW / squareSize; //32
    int gridHeight = WinH / squareSize; //24

    //Snake Init Stuff
    t_snake snake;
    snake.length = 2;
    snake.body[0].x = 16;
    snake.body[0].y = 12;
    snake.body[1].x = 15;
    snake.body[1].y = 12;
    snake.body[2].x = 14;
    snake.body[2].y = 12;
    bool isAlive = true;
    //Fruit init stuff
    t_fruit fruit;
    //Make sure the fruit doesn't spawn on the snake
    bool isPosGood = false;
    while(isPosGood == false){
        fruit.pos.x = rand() % 32;
        fruit.pos.y = rand() % 24;
        isPosGood = true;
        for (int i = 0; i < snake.length; i++) { //for each loop
            if(snake.body[i].x == fruit.pos.x && snake.body[i].y == fruit.pos.y){
                isPosGood = false;
            }
        }
        if(fruit.pos.x < 7){
            isPosGood = false;
        }
    }

    //update the snake's position and move it
    void updateSnakePos(){
        //if snake hits wall
        //this first part is 6 because Im putting the score info there due to a glitch with the snake on the border so it just works out
        if(snake.body[0].x + directionX < 6 || snake.body[0].x + directionX > gridWidth - 1 || snake.body[0].y + directionY < 0 || snake.body[0].y + directionY > gridHeight - 1){
            isAlive = false;
        }
        else{
            //move da snake
            for (int i = snake.length; i > 0; i--) { //for each loop
                if(snake.body[i].x){//Checks if the part exists
                    snake.body[i].y = snake.body[i - 1].y;
                    snake.body[i].x = snake.body[i - 1].x;
                }
            }
            //Move Head
            snake.body[0].x += directionX;
            snake.body[0].y += directionY;
        }
        //if snake hits itself
        for (int i = 2; i < snake.length; i++) { //start at 3 so the first few dont count
            if(snake.body[i].x == snake.body[0].x && snake.body[i].y == snake.body[0].y){
                isAlive = false;
            }
        }
    }

    //
    void ResetSnake(){
        snake.length = 2;
        snake.body[0].x = 16;
        snake.body[0].y = 12;
        snake.body[1].x = 15;
        snake.body[1].y = 12;
        snake.body[2].x = 14;
        snake.body[2].y = 12;
        isAlive = true;
        directionX = 1;
        directionY = 0;
        targetDelay = 5;//timing
        startingSpeed = 5;//timing
        fruit.pos.x = rand() % 32;
        fruit.pos.y = rand() % 24;
        //Make sure the fruit doesn't spawn on the snake
        bool isPosGood = false;
        while(isPosGood == false){
            fruit.pos.x = rand() % 32;
            fruit.pos.y = rand() % 24;
            isPosGood = true;
            for (int i = 0; i < snake.length; i++) { //for each loop
                if(snake.body[i].x == fruit.pos.x && snake.body[i].y == fruit.pos.y){
                    isPosGood = false;
                }
            }
            if(fruit.pos.x < 7){
                isPosGood = false;
            }
        }
    }

    //trigger the snake movement and fruit eating
    void updateEverything(){
        if(isAlive == true){
            //snake movement
            updateSnakePos();
            updateDelay = 0;

            //Fruit eating check
            Struct cfeResult;
            cfeResult = CheckForEating(snake, fruit);
            snake = cfeResult.snake;
            fruit = cfeResult.fruit;
        }
    }

    while(1) {// Loop forever
        WPAD_ScanPads();  // Scan the Wiimotes
        WPAD_SetVRes(WPAD_CHAN_0, WinW, WinH); //Set the virtual resolution to the screen height/width from earlier
        WPAD_IR(WPAD_CHAN_0, &P1Mote); //Init IR Pointer

        int P1MX = P1Mote.sx - 150;// WiiMote IR Viewport Correction
        int P1MY = P1Mote.sy - 150;// WiiMote IR Viewport Correction
        if(inputdelay > 0){
            inputdelay--;
        }

        if(gamemode == 0){//Safety Warning
            if(wiiwarningcooldown > 0){
                GRRLIB_DrawImg(WinW / 2, WinH / 2, GFX_wii_warning_texture, 0, 0.76, 1, 0xFFFFFFFF);
                wiiwarningcooldown--;
            }
            else{
                gamemode = 1;
                wiiwarningcooldown = 300;
            }
        }
        if(gamemode == 1){//Logo
            if(wiiwarningcooldown > 0){
                GRRLIB_DrawImg(WinW / 2, WinH / 2, GFX_logo_intro_texture, 0, 0.76, 1, 0xFFFFFFFF);
                wiiwarningcooldown--;
            }
            else{
                gamemode = 2;
                wiiwarningcooldown = 300;
            }
        }
        if(gamemode == 2){//Main Menu
            if(MP3Player_IsPlaying() == false){
                MP3Player_Stop();
                MP3Player_PlayBuffer(menu_mp3, menu_mp3_size, NULL);
            }
            if(titledrop > 125){
                titledrop--;
            }
            GRRLIB_DrawImg(0, 0, GFX_mmbg_texture, 0, 0.8, 1, 0xFFFFFF50);//Draw BG
            GRRLIB_DrawImg(WinW / 2, WinH / 2 + 55, GFX_textbox_texture, 0, 0.3, 0.3, 0xFFFFFFFF);//Draw Popup
            GRRLIB_DrawImg(WinW / 2, WinH / 2 - titledrop, GFX_logo_texture, 0, 0.4, 0.4, 0xFFFFFFFF);//Draw Logo
            if(GRRLIB_PtInRect(255, WinH / 2 + 10, 120, 25, P1MX, P1MY)){//If button is being hovered
                mmselect = 1;
            }
            if(GRRLIB_PtInRect(255, WinH / 2 + 40, 120, 25, P1MX, P1MY)){//If button is being hovered
                mmselect = 2;
            }
            if(GRRLIB_PtInRect(255, WinH / 2 + 70, 120, 25, P1MX, P1MY)){//If button is being hovered
                mmselect = 3;
            }
            if(creditmenu == 1){
                GRRLIB_DrawImg(WinW / 2, WinH / 2, GFX_textbox_texture, 0, 0.8, 0.8, 0xFFFFFFFF);//Draw Popup
                GRRLIB_PrintfTTF(275, 100, RubikFont, "Credits", 32, 0xFFFFFFFF);
                GRRLIB_PrintfTTF(100, 200, RubikFont, "•ZombieNW - Creator", 24, 0xFFFFFFFF);
                GRRLIB_PrintfTTF(100, 225, RubikFont, "•Pixabay - Assets", 24, 0xFFFFFFFF);
                GRRLIB_PrintfTTF(100, 250, RubikFont, "•michibros/WiiSNEK - Reference", 24, 0xFFFFFFFF);
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                    creditmenu = 0;
                    inputdelay = 10;
                }
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                    creditmenu = 0;
                    inputdelay = 10;
                }
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A && inputdelay == 0){
                    creditmenu = 0;
                    inputdelay = 10;
                }
            }
            else{
                if(mmselect == 1){
                    GRRLIB_PrintfTTF(255, WinH / 2 + 10, LCDSolidFont, ">Play", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 40, LCDSolidFont, " Credits", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 70, LCDSolidFont, " Exit", 24, 0xFFFFFFFF);
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                        gamemode = 3;
                        ResetSnake();
                        inputdelay = 10;
                        MP3Player_Stop();
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                        mmselect = 3;
                        inputdelay = 10;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A && inputdelay == 0){
                        gamemode = 3;
                        ResetSnake();
                        inputdelay = 10;
                        MP3Player_Stop();
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN && inputdelay == 0){
                        mmselect = 2;
                        inputdelay = 10;
                    }
                }
                if(mmselect == 2){
                    GRRLIB_PrintfTTF(255, WinH / 2 + 10, LCDSolidFont, " Play", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 40, LCDSolidFont, ">Credits", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 70, LCDSolidFont, " Exit", 24, 0xFFFFFFFF);
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                        creditmenu = 1;
                        inputdelay = 10;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                        mmselect = 3;
                        inputdelay = 10;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A && inputdelay == 0){
                        creditmenu = 1;
                        inputdelay = 10;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_UP && inputdelay == 0){
                        mmselect = 1;
                        inputdelay = 10;
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN && inputdelay == 0){
                        mmselect = 3;
                        inputdelay = 10;
                    }
                }
                if(mmselect == 3){
                    GRRLIB_PrintfTTF(255, WinH / 2 + 10, LCDSolidFont, " Play", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 40, LCDSolidFont, " Credits", 24, 0xFFFFFFFF);
                    GRRLIB_PrintfTTF(255, WinH / 2 + 70, LCDSolidFont, ">Exit", 24, 0xFFFFFFFF);
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                        ExitGame();
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                        ExitGame();
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_A && inputdelay == 0){
                        ExitGame();
                    }
                    if(WPAD_ButtonsDown(0) & WPAD_BUTTON_UP && inputdelay == 0){
                        mmselect = 2;
                        inputdelay = 10;
                    }
                }
            }
            if (P1Mote.state == 1) {//If wii pointer active, try to keep this the last thing rendered
                GRRLIB_DrawImg( P1MX, P1MY, GFX_pointer_texture, 0, 1, 1, 0xFFFFFFFF);//Draw Wii Pointer
            }
        }
        if(gamemode == 3){//Gameplay
            if(true){ //Handle Input
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_RIGHT && directionX != -1){
                    directionX = 1;
                    directionY = 0;
                    updateEverything();
                }
                else if (WPAD_ButtonsDown(0) & WPAD_BUTTON_LEFT && directionX != 1){
                    directionX = -1;
                    directionY = 0;
                    updateEverything();
                }
                else if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP && directionY != 1){
                    directionX = 0;
                    directionY = -1;//Opposite since wii's direction is weird
                    updateEverything();
                }
                else if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN && directionY != -1){
                    directionX = 0;
                    directionY = 1;//Opposite since wii's direction is weird
                    updateEverything();
                }
            }

            //Lower the timer if the snake is longer
            if (targetDelay > 1){
                targetDelay = startingSpeed - ((snake.length - 2) / 10);
            }
            //Timing with automatic snake movements
            if(updateDelay > targetDelay){
                updateEverything();
            }
            else{
                updateDelay++;
            }
            //Background
            GRRLIB_DrawImg(115, 0, GFX_mmbg_texture, 0, 1, 1, 0xFFFFFF50);//Draw BG
            GRRLIB_DrawImg(0, 0, GFX_scorebg_texture, 0, 0.9, 1, 0xFFFFFFFF);//Draw Score BG
            //Draw Wall
            GRRLIB_Rectangle(115, 0, 5, 640, 0xFFFFFFFF, true);//Draw Square
            //Draw the snake and the fruit
            DrawSnake(snake, squareSize);
            DrawFruit(fruit, squareSize);
            //Score Display
            char p1s [100];
            sprintf(p1s, "%d", snake.length - 2);
            GRRLIB_PrintfTTF(1, 1, LCDSolidFont, "Score:", 32, 0xFFFFFFFF);
            GRRLIB_PrintfTTF(50, 40, LCDSolidFont, p1s, 32, 0xFFFFFFFF);

            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0 && isAlive == true){
                gamemode = 4;
                inputdelay = 10;
                MP3Player_Stop();
            }
            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0 && isAlive == true){
                gamemode = 4;
                inputdelay = 10;
                MP3Player_Stop();
            }

            //Handle Death or pause
            if(isAlive == false){
                GRRLIB_DrawImg(WinW / 2, WinH / 2, GFX_textbox_texture, 0, 0.9, 0.5, 0xFFFFFFFF);//Draw Popup
                GRRLIB_PrintfTTF(200, 200, RubikFont, "Press + To Play Again", 24, 0xFFFFFFFF);
                GRRLIB_PrintfTTF(75, 225, RubikFont, "Press Home To Return To The Main Menu", 24, 0xFFFFFFFF);
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                    gamemode = 2;
                    inputdelay = 10;
                    MP3Player_Stop();
                }
                if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                    inputdelay = 10;
                    ResetSnake();
                    MP3Player_Stop();
                }
            }
        }
        if(gamemode == 4){//Pause
            GRRLIB_DrawImg(0, 0, GFX_mmbg_texture, 0, 0.8, 1, 0xFFFFFF50);//Draw BG
            GRRLIB_DrawImg(WinW / 2, WinH / 2, GFX_textbox_texture, 0, 0.9, 0.5, 0xFFFFFFFF);//Draw Popup
            GRRLIB_PrintfTTF(240, 200, RubikFont, "Game Paused", 24, 0xFFFFFFFF);
            GRRLIB_PrintfTTF(220, 225, RubikFont, "Press + Resume", 24, 0xFFFFFFFF);
            GRRLIB_PrintfTTF(80, 250, RubikFont, "Press Home To Return To The Main Menu", 24, 0xFFFFFFFF);
            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_PLUS && inputdelay == 0){
                gamemode = 3;
                inputdelay = 10;
                MP3Player_Stop();
            }
            if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME && inputdelay == 0){
                gamemode = 2;
                inputdelay = 10;
                MP3Player_Stop();
            }
        }
        GRRLIB_Render();  // Render the frame buffer to the TV
    }
    ExitGame();
}