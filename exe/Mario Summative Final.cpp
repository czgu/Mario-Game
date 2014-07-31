/*
Charlie Gu and Wilson Lin ICS3U Summative
2d Mario Game, featuring:
Advanced Mario Sprites
Collision detection leads to game world interaction
Game Tiles allowing quick leveling up
Animations Enhencing the Game play
Real world physics such as gravity and inertia
2011 June 14

Change Log:
Oct 6: Crouch
Oct 9: Advance graphics/New running animation
*/


#include <alfont.h>
#include <allegro.h> 
#include <stdio.h>   
#include <time.h>   
#include <string.h>
#include <cmath>
//The movement function of the game objects, mario, and his fire ball
struct movement
{
    int xPos; //x position
    int yPos; //y position
    int life; //life 0 means death with life comes different appearances
    int move; //its x speed
    int height; //the height of the object, used in collision
    int width; // same as ^, in charges of the width
    int down; // the y speed
    int death; //this triggers the counter below
    int death_counter; //the object's counter, can be used in many different scenarios
};

struct scoredisplay
{
    int xPos;
    int yPos;
    int time_counter;
    int score;
    int life;
};

//the specific information of each individual level
struct level_design
{
    //the names are fairly simple to understand
    int flag_position;
    int castle_position;
    int ladder_position;
    int background_type;
    int heightspawn;
    int numberspawn;
    int numbergoldspawn;
};
level_design level[10];
struct movement Mario; //Controlable character maroi
struct movement object[10][100]; //Game objects, such as mushroom and goomba
//object 1 = Broswer 0
//object 2 = Goomba 1
//object 3 = turtle 2
//object 4 = Meteroid 3
//object 5 = Flower 4
//object 6 = Gold coin 5
//object 7 = cannon 6
//object 8 = switch 7
//object 9 = mushroom 8
//object 10 = pole 9
struct movement bullet[100]; //the fireball mario shoots


const int scrx = 640;
const int scry = 480;

//prototyping------------------------------------------
bool collide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2); 
int Play_Music(int song);
int Sound_Effect(int type);
void Load_Map(int Level);
void Menu_Screen();
void Paused_Screen(int *mode,BITMAP *buf);
void Display_Info(int life, int coin, int score, int level);
void Display_GamerOver(int coin, int score, int level);
int Scoredisplayer(struct scoredisplay *Gamescore,int *i, int x, int y, int score);
//Speed Counter
volatile long speed_counter = 0;                                          
void increment_speed_counter() // A function to increment the speed counter
{
  speed_counter++; // This will just increment the speed counter by one. 
}
END_OF_FUNCTION(increment_speed_counter);



//the map functions, 500 columns with 15 rows
int map[15][500];
/*
Game Tiles
0=Sky
1=Ground 1
2=Ground 2
3=Brick
4=? Box
5=Tube
6=Tube
7=Tube
8=Tube
9=Block
10=Empty ? Box
11=Pole
12=Pole head
13=Shroom ? Box
14=Up brick
*/

                                                                 
int main(int argc, char *argv[]) 
{ 
    allegro_init();        
    alfont_init();
    install_keyboard(); 
   	install_sound (MIDI_AUTODETECT, MIDI_AUTODETECT , 0); //Needed to play Music
    install_timer(); 
    LOCK_VARIABLE(speed_counter); 
    LOCK_FUNCTION(increment_speed_counter);//speed.
    install_int_ex(increment_speed_counter, BPS_TO_TIMER(60));//Set our BPS
    set_color_depth(16); 
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, scrx, scry,0,0);
    srand(time(0));   //Radomize    
    //SETTING FONT
    ALFONT_FONT *arial = alfont_load_font("C:/Windows/Fonts/arial.ttf"); 
    ALFONT_FONT *arial2 = alfont_load_font("C:/Windows/Fonts/arial.ttf");
    set_window_title("Super Mario World");
    alfont_set_font_size(arial, 30);
    alfont_set_font_size(arial2, 22);
    BITMAP *buffer = create_bitmap(640,480); 
    if(buffer == NULL)
    {
          set_gfx_mode(GFX_TEXT,0,0,0,0);
          allegro_message("Could not create buffer!");
          exit(EXIT_FAILURE);
    }        
    clear_bitmap(screen);
    //Animations====================================================================
    //unlike the structs on the top, they have no collision with the environment
    struct movement animation[12][100]; //0=coin //1=brick
    //0=Coin
    //1=Brick
    //2=Squash Goomba
    //3=Knocked Out Goomba
    //4=Dead Turtle
    //5=Explosion
    //6=1UP
    //7=Princess
    //8=Words
    //9=Flag
    //10=BIG explosion
    //11=Dead Cannon
    struct scoredisplay Gamescore[1000];
    //BITMAPS ======================================================================
    BITMAP *MarioImage = load_bitmap("img/Mario.bmp", NULL);//Mario
    //Mario----------------------------------------------- 
    //Fire------------------------
    BITMAP *Fire_Stand_R = load_bitmap("img/FIRE mario R 1.bmp", NULL);
    BITMAP *Fire_Walk_R_1 = load_bitmap("img/FIRE mario R 2.bmp", NULL);
    BITMAP *Fire_Walk_R_2 = load_bitmap("img/FIRE mario R 3.bmp", NULL);
    BITMAP *Fire_Jump_R = load_bitmap("img/FIRE mario R 4.bmp", NULL);
    BITMAP *Fire_Turn_R = load_bitmap("img/FIRE mario L 5.bmp", NULL);
    BITMAP *Fire_Stand_L = load_bitmap("img/FIRE mario L 1.bmp", NULL);
    BITMAP *Fire_Walk_L_1 = load_bitmap("img/FIRE mario L 2.bmp", NULL);
    BITMAP *Fire_Walk_L_2 = load_bitmap("img/FIRE mario L 3.bmp", NULL);
    BITMAP *Fire_Jump_L = load_bitmap("img/FIRE mario L 4.bmp", NULL);
    BITMAP *Fire_Turn_L = load_bitmap("img/FIRE mario R 5.bmp", NULL);
    BITMAP *Fire_Duck_L = load_bitmap("img/FIRE mario L 6.bmp", NULL);
    BITMAP *Fire_Duck_R = load_bitmap("img/FIRE mario R 6.bmp", NULL);
    //Large ----------------------
    BITMAP *Large_Stand_R = load_bitmap("img/L Mario R 1.bmp", NULL);
    BITMAP *Large_Walk_R_1 = load_bitmap("img/L Mario R 2.bmp", NULL);
    BITMAP *Large_Walk_R_2 = load_bitmap("img/L Mario R 4.bmp", NULL);
    BITMAP *Large_Jump_R = load_bitmap("img/L Mario R 3.bmp", NULL);
    BITMAP *Large_Turn_R = load_bitmap("img/L Mario L 5.bmp", NULL);
    BITMAP *Large_Stand_L = load_bitmap("img/L Mario L 1.bmp", NULL);
    BITMAP *Large_Walk_L_1 = load_bitmap("img/L Mario L 2.bmp", NULL);
    BITMAP *Large_Walk_L_2 = load_bitmap("img/L Mario L 4.bmp", NULL);
    BITMAP *Large_Jump_L = load_bitmap("img/L Mario L 3.bmp", NULL);
    BITMAP *Large_Turn_L = load_bitmap("img/L Mario R 5.bmp", NULL);
    BITMAP *Large_Duck_L = load_bitmap("img/L Mario L 6.bmp", NULL);
    BITMAP *Large_Duck_R = load_bitmap("img/L Mario R 6.bmp", NULL);
   //Small-----------------------
    BITMAP *Small_Stand_R = load_bitmap("img/SMALL Mario R 1.bmp", NULL);
    BITMAP *Small_Walk_R_1 = load_bitmap("img/SMALL Mario R 2.bmp", NULL);
    BITMAP *Small_Walk_R_2 = load_bitmap("img/SMALL Mario R 4.bmp", NULL);
    BITMAP *Small_Jump_R = load_bitmap("img/SMALL Mario R 3.bmp", NULL);
    BITMAP *Small_Turn_R = load_bitmap("img/SMALL Mario L 5.bmp", NULL);
    BITMAP *Small_Stand_L = load_bitmap("img/SMALL Mario L 1.bmp", NULL);
    BITMAP *Small_Walk_L_1 = load_bitmap("img/SMALL Mario L 2.bmp", NULL);
    BITMAP *Small_Walk_L_2 = load_bitmap("img/SMALL Mario L 4.bmp", NULL);
    BITMAP *Small_Jump_L = load_bitmap("img/SMALL Mario L 3.bmp", NULL);
    BITMAP *Small_Turn_L = load_bitmap("img/SMALL Mario R 5.bmp", NULL);
    //Dead-------------------------
    BITMAP *Dead_Small_Mario = load_bitmap("img/Dead Mario.bmp", NULL);
    BITMAP *Flash_Mario = load_bitmap("img/Mario_Flash.bmp", NULL);
    //Goomba----------------------------------------------
    BITMAP *GoombaImage = load_bitmap("img/Goomba.bmp", NULL);
    BITMAP *Goomba_Right = load_bitmap("img/Goomba R.bmp", NULL);
    BITMAP *Goomba_Left = load_bitmap("img/Goomba L.bmp", NULL);
    //Turtle----------------------------------------------
    BITMAP *TurtleImage = load_bitmap("img/Turtle R 1.bmp", NULL);
    BITMAP *Turtle_Left1 = load_bitmap("img/Turtle L 1.bmp", NULL);
    BITMAP *Turtle_Left2 = load_bitmap("img/Turtle L 2.bmp", NULL);
    BITMAP *Turtle_Right1 = load_bitmap("img/Turtle R 1.bmp", NULL);
    BITMAP *Turtle_Right2 = load_bitmap("img/Turtle R 2.bmp", NULL);
    BITMAP *Turtle_Shell = load_bitmap("img/Dead turtle.bmp", NULL);
    //Button---------------------------------------------
    BITMAP *Button = load_bitmap("img/Button.bmp", NULL);
    //Cannon---------------------------------------------
    BITMAP *Cannon = load_bitmap("img/Bullet.bmp", NULL);
    //Broswer--------------------------------------------
    BITMAP *Broswer_Fall = load_bitmap("img/Bowers 4.bmp", NULL);
    BITMAP *Broswer_Walk_2 = load_bitmap("img/Bowers 3.bmp", NULL);
    BITMAP *Broswer_Walk_1 = load_bitmap("img/Bowers 1.bmp", NULL);
    BITMAP *Broswer_Jump = load_bitmap("img/Bowers 2.bmp", NULL);
    //Princess--------------------------------------------
    BITMAP *Princess = load_bitmap("img/Princess.bmp", NULL);
    //Coin Multiplier------------------------------------
    BITMAP *Coin_Multi_L = load_bitmap("img/Multiplier L.bmp", NULL);
    BITMAP *Coin_Multi_D = load_bitmap("img/Multiplier D.bmp", NULL);
    //Coin Image-----------------------------------------    
    BITMAP *CoinImage = load_bitmap("img/Coin L.bmp", NULL);
    BITMAP *Coin_D = load_bitmap("img/Coin D.bmp", NULL);
    BITMAP *Coin_L = load_bitmap("img/Coin L.bmp", NULL);
    BITMAP *Coin_M = load_bitmap("img/Coin M.bmp", NULL);
    //Power Ups-------------------------------------------
    BITMAP *ShroomImage = load_bitmap("img/Shroom.bmp", NULL);
    BITMAP *FlowerImage_1 = load_bitmap("img/Flower 1.bmp", NULL);
    BITMAP *FlowerImage_2 = load_bitmap("img/Flower 2.bmp", NULL);
    //Flag Pole----------------------------------------------------   
    BITMAP *Flag_Pole = load_bitmap("img/Whole flag pole.bmp", NULL);
    BITMAP *Flag = load_bitmap("img/Flag.bmp", NULL);
    //Animations------------------------------------------
    BITMAP *Box_Coin = load_bitmap("img/Brick_Coin.bmp", NULL);
    BITMAP *Broken_Brick =  load_bitmap("img/Broken Brick.bmp", NULL);
    BITMAP *Goomba_Squash = load_bitmap("img/Squash.bmp", NULL);
    BITMAP *Goomba_Dead = load_bitmap("img/Dead_Goomba.bmp", NULL);
    BITMAP *Turtle_Dead = load_bitmap("img/Dead_Turtle.bmp", NULL);
    BITMAP *Explosion = load_bitmap("img/Explosion.bmp", NULL);
    BITMAP *UPONE = load_bitmap("img/1UP.bmp", NULL);
    BITMAP *Messege = load_bitmap("img/End Message.bmp",NULL);
    //Fire Ball-----------------------------------------------------
    BITMAP *Fire_ball1 = load_bitmap("img/Fireball 1.bmp", NULL);
    BITMAP *Fire_ball2 = load_bitmap("img/Fireball 2.bmp", NULL);
    BITMAP *Fire_ball3 = load_bitmap("img/Fireball 3.bmp", NULL);
    BITMAP *Fire_ball4 = load_bitmap("img/Fireball 4.bmp", NULL);
    //Falling Fire Ball-----------------------------------------------
    BITMAP *Sky_Fire = load_bitmap("img/Sky Ball.bmp", NULL);
    BITMAP *Boom = load_bitmap("img/Big explosion.bmp", NULL);
    //Game Tiles------------------------------------------
    BITMAP *Sky = load_bitmap("img/Sky.bmp", NULL);//Useless Image, may consider to remove
    BITMAP *Ground1 = load_bitmap("img/Ground1.bmp", NULL);
    BITMAP *Ground2 = load_bitmap("img/Ground2.bmp", NULL);
    BITMAP *Out_Ground[7][3];
    BITMAP *In_Ground[7][5];
    BITMAP *Small_Castle = load_bitmap("img/Small_Castle.bmp", NULL);
    BITMAP *Brick = load_bitmap("img/Brick new.bmp", NULL);
    BITMAP *Night_Brick = load_bitmap("img/N Brick.bmp", NULL);
    BITMAP *Brick_Up = load_bitmap("img/Up Brick.bmp", NULL);
    BITMAP *Tube_Ext_Left = load_bitmap("img/Tube_Ext_Left.bmp", NULL);
    BITMAP *Tube_Ext_Right = load_bitmap("img/Tube_Ext_Right.bmp", NULL);
    BITMAP *Tube_Top_Left = load_bitmap("img/Tube_Top_Left.bmp", NULL);
    BITMAP *Tube_Top_Right = load_bitmap("img/Tube_Top_Right.bmp", NULL);
    BITMAP *Box = load_bitmap("img/Box.bmp", NULL);
    BITMAP *Box_D = load_bitmap("img/Unknown Box D.bmp", NULL);
    BITMAP *Box_M = load_bitmap("img/Unknown Box M.bmp", NULL);
    BITMAP *Box_L = load_bitmap("img/Unknown Box L.bmp", NULL);
    BITMAP *Box_Hit = load_bitmap("img/Box_Hit.bmp", NULL);
    BITMAP *Block = load_bitmap("img/Block new.bmp", NULL);
    BITMAP *Night_Block = load_bitmap("img/N Block.bmp", NULL);
    BITMAP *Pole = load_bitmap("img/Pole.bmp", NULL);
    BITMAP *Pole_Head = load_bitmap("img/Pole Head.bmp", NULL);
    BITMAP *Panaroma[6];
    BITMAP *Panaroma_Background[6];
    BITMAP* testificate = create_bitmap(640,480);
    //Game Tiles-------------------------------------------------------
    //Level 1
    Out_Ground[1][0]=load_bitmap("img/Grass 1.bmp", NULL);
    Out_Ground[1][1]=load_bitmap("img/Grass 2.bmp", NULL);
    Out_Ground[1][2]=load_bitmap("img/Grass 3.bmp", NULL);
    In_Ground[1][0]=load_bitmap("img/Soil 1.bmp", NULL);
    In_Ground[1][1]=load_bitmap("img/Soil 2.bmp", NULL);
    In_Ground[1][2]=load_bitmap("img/Soil 3.bmp", NULL);
    In_Ground[1][3]=load_bitmap("img/Soil 4.bmp", NULL);
    In_Ground[1][4]=load_bitmap("img/Soil 5.bmp", NULL);
    //Level 2
    Out_Ground[2][0]=load_bitmap("img/Ice top 2.bmp", NULL);
    Out_Ground[2][1]=load_bitmap("img/Ice top 1.bmp", NULL);
    Out_Ground[2][2]=load_bitmap("img/Ice top 2.bmp", NULL);
    In_Ground[2][0]=load_bitmap("img/Ice bottom 2.bmp", NULL);
    In_Ground[2][1]=load_bitmap("img/Ice bottom 1.bmp", NULL);
    In_Ground[2][2]=load_bitmap("img/Ice bottom 2.bmp", NULL);
    In_Ground[2][3]=load_bitmap("img/Ice bottom 1.bmp", NULL);
    In_Ground[2][4]=load_bitmap("img/Ice bottom 2.bmp", NULL);
    //Level 3      
    Out_Ground[3][0]=load_bitmap("img/Underground top 1.bmp", NULL);
    Out_Ground[3][1]=load_bitmap("img/Underground top 2.bmp", NULL);
    Out_Ground[3][2]=load_bitmap("img/Underground top 3.bmp", NULL);
    In_Ground[3][0]=load_bitmap("img/Underground bottom 1.bmp", NULL);
    In_Ground[3][1]=load_bitmap("img/Underground bottom 2.bmp", NULL);
    In_Ground[3][2]=load_bitmap("img/Underground bottom 3.bmp", NULL);
    In_Ground[3][3]=load_bitmap("img/Underground bottom 4.bmp", NULL);
    In_Ground[3][4]=load_bitmap("img/Underground bottom 1.bmp", NULL);
    //Level 4                
    Out_Ground[4][0]=load_bitmap("img/Dessert top1.bmp", NULL);
    Out_Ground[4][1]=load_bitmap("img/Dessert top2.bmp", NULL);
    Out_Ground[4][2]=load_bitmap("img/Dessert top1.bmp", NULL);
    In_Ground[4][0]=load_bitmap("img/Dessert sand1.bmp", NULL);
    In_Ground[4][1]=load_bitmap("img/Dessert sand2.bmp", NULL);
    In_Ground[4][2]=load_bitmap("img/Dessert sand1.bmp", NULL);
    In_Ground[4][3]=load_bitmap("img/Dessert sand2.bmp", NULL);
    In_Ground[4][4]=load_bitmap("img/Dessert sand1.bmp", NULL);
    //Level 5       -----------------------------------------
    Out_Ground[5][0]=load_bitmap("img/Mountain top 1.bmp", NULL);
    Out_Ground[5][1]=load_bitmap("img/Mountain top 2.bmp", NULL);
    Out_Ground[5][2]=load_bitmap("img/Mountain top 1.bmp", NULL);
    In_Ground[5][0]=load_bitmap("img/Mountain bottom 1.bmp", NULL);
    In_Ground[5][1]=load_bitmap("img/Mountain bottom 2.bmp", NULL);
    In_Ground[5][2]=load_bitmap("img/Mountain bottom 3.bmp", NULL);
    In_Ground[5][3]=load_bitmap("img/Mountain bottom 4.bmp", NULL);
    In_Ground[5][4]=load_bitmap("img/Mountain bottom 2.bmp", NULL);
    //Level 6       -----------------------------------------
    Out_Ground[6][0]=load_bitmap("img/Castle Top 1.bmp", NULL);
    Out_Ground[6][1]=load_bitmap("img/Castle Top 2.bmp", NULL);
    Out_Ground[6][2]=load_bitmap("img/Castle Top 3.bmp", NULL);
    In_Ground[6][0]=load_bitmap("img/Castle edge.bmp", NULL);
    In_Ground[6][1]=load_bitmap("img/Castle edge.bmp", NULL);
    In_Ground[6][2]=load_bitmap("img/Castle edge.bmp", NULL);
    In_Ground[6][3]=load_bitmap("img/Castle edge.bmp", NULL);
    In_Ground[6][4]=load_bitmap("img/Castle edge.bmp", NULL);
    //Backgrounds----------------------------------------------------
    //Level 1
    Panaroma[1] = load_bitmap("img/Clouds.bmp", NULL);
    Panaroma_Background[1] = load_bitmap("img/Background.bmp", NULL);
    //Level 2
    Panaroma[2] = load_bitmap("img/Ice foreground.bmp", NULL);
    Panaroma_Background[2] = load_bitmap("img/Ice Background.bmp", NULL);
    //Level 3      
    Panaroma[3] = load_bitmap("img/Underground ForeGround.bmp", NULL);
    Panaroma_Background[3] = load_bitmap("img/N background.bmp", NULL);
    //Level 4                
    Panaroma[4] = load_bitmap("img/Stars.bmp", NULL);
    Panaroma_Background[4] = load_bitmap("img/Sand Background.bmp", NULL);
    //Level 5       
    Panaroma[5] = load_bitmap("img/Dense Clouds.bmp", NULL);
    Panaroma_Background[5] = load_bitmap("img/Mountains.bmp", NULL);
    //Level 6
    Panaroma[6] = load_bitmap("img/Castle Foreground.bmp", NULL);
    Panaroma_Background[6] = load_bitmap("img/Star Background.bmp", NULL);


    //Integers=======================================================================
    //Game scrollers--------------------------------------------------------
    int mario_scroll_position = 0;
    int x_scroll = 0;
    int background_scroll[10];    

    int inertia_amount=6;
    //Array variables-------------------------------------------------------------
    int a = 0;
    int b = 0;
    int c = 0;
    int c1 = 0;
    int c2 = 0;
    int d = 0;
    int d1 = 0;
    int d2 = 0;    
    int e = 0;
    int f = 0;
    int i = 0;
    int i1 = 0;
    int i2 = 0;    
    int j = 0;
    int j1 = 0;
    int j2 = 0;
    //Counters---------------------------------------------------------------
    int inertia_counter = 0;
    //Mario Movement---------------------------------------------------------
    int KeyNotJump =FALSE;  
    int speed = 3;
    int mario_facing;
    int On_Hit= TRUE;
    //Collisions-------------------------------------------------------------
    int Up_Collision = 0;
    int Right_Collision = 0;
    int Left_Collision = 0;
    int Down_Collision = 0;
    int Right_Collision_2 = 0;
    int Left_Collision_2 = 0;
    int Down_Collision_2 = 0;    
    //Object Counters------------------------------------------------------
    int mushroom_counter = 0;
    int flower_counter = 0;
    int sky_fire_counter = 0;
    int sky_fire_ball_counter = 0;
    int brick_counter = 0;
    int global_spawn = 640;
    int coin_spawn = 640;
    int goomba_spawn = 0;
    int turtle_spawn = 0;
    int grass_counter;
    int soil_counter;    
    int cannon_counter = 0;
    int cannon_spawn_counter = 0;
    //Sprites---------------------------------------------------------------
    int goomba_sprite_counter = 0;
    int mario_sprite_counter = 0;
    int mario_sprite_change = FALSE;
    int mario_transform = FALSE;
    int mario_transform_counter = 0;
    //Level design-----------------------------------------------------------
    int Game_level=1;
    int level_change = TRUE;
    //Game Display----------------------------------------------------------    
    int score=0;
    int time=0;
    int Gold_Coin=0;   
    int Life = 3;
    int turtle_turning = 0;     
    //Level Complete--------------------------------------------------------
    int level_change_animation = FALSE;
    int complete_song = FALSE;
    int flag_touch = FALSE;
    int song_counter = 0;
    int flag_down = FALSE;
    int Death_Song;
    //Animation Counter-------------------------------------------------------   
    int animation_coin_counter = 0;
    int animation_brick_counter = 0;
    int animation_ball_counter = 0;
    //Menu Controls------------------------------------------
    int Menu_Loop = 0;
    int Paused = 0;
    //Bullets--------------------------------------------------
    int keynotshoot = FALSE;
    int bullet_counter = 0;
    int bullet_cd_counter = 0;;
    int bullet_cd=FALSE;
    int Up_Collision_3 = 0;
    int Right_Collision_3 = 0;
    int Left_Collision_3 = 0;
    int Down_Collision_3 = 0;
    //Game_Complete---------------------------------------------------
    int Spawn_Browser = FALSE;
    int Game_Complete = FALSE;
    int Browser_Jump = 0;
    int Browser_Turtle = 0;
    //Duck===========================================================
    int DuckJump = FALSE;
    int DuckMode = FALSE;
    //New Counter-----------------------------------------------------
    int Walking_counter = 0;
    //Score display------------------------------------------------
    int global_score_counter  = 0;
//=================================================================
//---------------------Level Design(ie Hard coding)
//level 1
    level[1].flag_position=5895;
    level[1].castle_position=6080;
    level[1].ladder_position=5400;
    level[1].background_type=0;
    level[1].heightspawn = 0;
    level[1].numberspawn = 15;
    level[1].numbergoldspawn = 35;
//level 2
    level[2].flag_position=6885;
    level[2].castle_position=7050;
    level[2].ladder_position=6400;
    level[2].background_type=0; 
    level[2].heightspawn = 0;
    level[2].numberspawn = 20;
    level[2].numbergoldspawn = 35;
//level 3
    level[3].flag_position=5635;
    level[3].castle_position=5815;
    level[3].ladder_position=4960;
    level[3].background_type=1; 
    level[3].heightspawn = 100;
    level[3].numberspawn = 25;
    level[3].numbergoldspawn = 90;
//level 4
    level[4].flag_position=7428;
    level[4].castle_position=7616;
    level[4].ladder_position=6800;
    level[4].background_type=0; 
    level[4].heightspawn = 0;
    level[4].numberspawn = 25;
    level[4].numbergoldspawn = 75;
//level 5
    level[5].flag_position=6885;
    level[5].castle_position=7050;
    level[5].ladder_position=6400;
    level[5].background_type=0;
    level[5].heightspawn = 0;
    level[5].numberspawn = 20;
    level[5].numbergoldspawn = 35;
//level 6
    level[6].flag_position=20000; //flag and castle are not needed because it is the last level
    level[6].castle_position=20000;
    level[6].ladder_position=4512;
    level[6].background_type=1;
    level[6].heightspawn = 130;
    level[6].numberspawn = 20;
    level[6].numbergoldspawn = 100;

//=================================================================
//Initializing the objects
//Define all the size for them
    for(i=0;i<10;i++){
        for(j=0;j<100;j++){
            object[i][j].xPos=0;
            object[i][j].yPos=0;
            bullet[j].yPos=0;
            bullet[j].xPos=0;
            animation[i][j].life=0;
            bullet[j].life=0;
            object[i][j].life=0;
            object[i][j].death=FALSE;
            if(i==8){
                object[i][j].height=32;
                object[i][j].width=32;
            }
            if(i==1){
                object[i][j].height=32;
                object[i][j].width=31;
            }
            if(i==2){
                object[i][j].death_counter=0;
                object[i][j].height=32;
                object[i][j].width=31;
            }
            if(i==3){
                object[i][j].height=32;
                object[i][j].width=32;                  
            }            
            if(i==4){
                object[i][j].height=32;
                object[i][j].width=26;                  
            }
            if(i==5){
                object[i][j].height=CoinImage->h;
                object[i][j].width=CoinImage->w;     
            }         
            if(i==7){
                object[i][j].height=30;
                object[i][j].width=30;                  
            }
            if(i==0){
                object[i][j].height=96;
                object[i][j].width=96;                  
            }   
            if(i==6){
                object[i][j].height=28;
                object[i][j].width=32;                  
            }                    
            if(i==9){
                object[i][j].height=304;
                object[i][j].width=18;                  
            }
            bullet[j].height=16;
            bullet[j].width=16;
        }        
    } 
    
    
    //Main Game Function--------------------------------------------------------------------------------
    while(!key[KEY_ESC]){    
        //These are game menus
        if(Paused == 2){
            //When Game is over, this is shown
            Display_GamerOver(Gold_Coin,score,Game_level);
            Paused =0;
        }                     
        if(Paused == 1){
            //Pauses the game
            blit(screen,testificate,0,0,0,0,640,480);
            Paused_Screen(&Paused,testificate);
            Menu_Loop=0;
        }  
        if(Paused == 0){
            //Main Meanu of the Game
            //Also initialize some of the variables that are
            //used throughout the game
            Menu_Screen();
            Mario.life = 1;
            Life=3;
            Game_level=1;
            level_change=TRUE;    
            score=0;
            Gold_Coin=0;
            Menu_Loop=0;
            Spawn_Browser = FALSE;
        }
        //Game Play Loop
        while(Menu_Loop == 0)    
        {
            while(speed_counter > 0)
            {                  
                
                if(key[KEY_P]){
                    //The P key pauses the game by ending the while loop
                    Menu_Loop=1;
                    Paused = 1;
                }
                if(key[KEY_T]){
                    Life=99;
                    if(Mario.life == 1)
                        Mario.yPos-=32;
                    Mario.life=3;
                }
                //when mario is dead, the game will check and reload the game
                if(Mario.life==0  && Life>1 && Mario.death==FALSE && Mario.yPos>480 && Death_Song==FALSE){
                    Life--;
                    Mario.life=1;
                    level_change=TRUE;                     
                }
                else if(Mario.life==0  && Life<=1 && Mario.death==FALSE && Mario.yPos>480){
                     //or in this case, game is over
                    Menu_Loop=1;
                    Paused = 2;
                }
                //Initializes the Variables in each level
                if(level_change==TRUE){    
                    //Level Reset!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!             
                    if(level[Game_level].background_type==0){ //2 different colours are used for same bricks
                        Broken_Brick =  load_bitmap("img/Broken Brick.bmp", NULL);
                        Brick = load_bitmap("img/Brick new.bmp", NULL);
                    }
                    else if (level[Game_level].background_type==1){     
                        Broken_Brick =  load_bitmap("img/Blue Broken Brick.bmp", NULL);
                        Brick = load_bitmap("img/Blue_Brick.bmp", NULL);
                    }      
                    //animations and object reset--------------------------------------------------------
                    for(i=0;i<12;i++){
                        for(j=0;j<100;j++){
                            animation[i][j].life=0;
                            animation[i][j].xPos=0;
                            animation[i][j].yPos=0;
                            animation[i][j].move=0;
                            animation[i][j].down=0;
                            if(i<10){
                                object[i][j].life=0;
                                object[i][j].xPos=0;
                                object[i][j].yPos=0;
                                object[i][j].move=0;
                                object[i][j].down=0;
                            }
                        }
                    }
                    //Counter initialization
                    //Monster Counters------------------------------------------------------
                    mushroom_counter = 0;
                    flower_counter = 0;
                    sky_fire_counter = 0;
                    sky_fire_ball_counter=0;
                    animation_ball_counter = 0;
                    goomba_spawn = 0;
                    turtle_spawn = 0;
                    grass_counter=0;
                    soil_counter=0;
                    cannon_counter = 0;
                    cannon_spawn_counter = 0;
                    animation_brick_counter=0;
                    animation_coin_counter = 0;
                    //Sprites---------------------------------------------------------------
                    goomba_sprite_counter = 0;
                    mario_sprite_counter = 0;
                    mario_sprite_change = FALSE;
                    mario_transform = FALSE;
                    mario_transform_counter = 0;
                    cannon_spawn_counter=0;
                    //level change reset------------------------------------------------------
                    Spawn_Browser=FALSE;     
                    level_change_animation=FALSE;     
                    Game_Complete = FALSE;
                    flag_down=FALSE;   
                    //Final level has princess at the end of the level
                    if(Game_level==6){
                        object[7][0].life=1;                        
                        object[7][0].yPos=200;                        
                        object[7][0].xPos=5506;                        
                        object[7][0].move=0;     
                        animation[7][0].life=1;
                        animation[7][0].yPos=376;  //438
                        animation[7][0].xPos=5929;                                           
                    }
                    //Mario is given a time of 300 seconds
                    time=300;
                    //Mario's starting poisition
                    Mario.yPos=160;
                    Mario.down=0;
                    Mario.xPos=16;
                    mario_facing=1;
                    //Spawning settings
                    global_spawn=640;   
                    coin_spawn=400;  
                    //Spawn a flag----------------------------------------------------------------
                    object[9][0].life=1;                
                    object[9][0].yPos=33;
                    object[9][0].xPos=level[Game_level].flag_position;                 
                    animation[9][0].life=1;                                        
                    animation[9][0].xPos=object[9][0].xPos-25;                        
                    animation[9][0].move=0; 
                    //Monster and Coin and Mushroom and Flower Spawns-------------------------------------------------- 
                    for(a=0;a<100;a++){                        
                        object[8][a].life=0;
                        object[4][a].life=0;   
                        goomba_spawn = rand() % 320;  
                        turtle_spawn = rand() % 320; 
                        if(global_spawn<level[Game_level].ladder_position){
                            //Goomba
                            object[1][a].life=1;
                            object[1][a].yPos=level[Game_level].heightspawn+rand() % 150;
                            object[1][a].xPos=global_spawn-goomba_spawn;
                            object[1][a].move=2; 
                            //Turtle
                            object[2][a].life=2;
                            object[2][a].yPos=level[Game_level].heightspawn +rand() % 150;
                            object[2][a].xPos=global_spawn-turtle_spawn;
                            object[2][a].move=1;
                            //the numbers of goomba and turtles spawned in the map is determined by the numberspawn number
                        }              
                        global_spawn+=level[Game_level].ladder_position/level[Game_level].numberspawn;             
                        if((coin_spawn)<level[Game_level].ladder_position){
                            //Coin                                      
                            object[5][a].life=1;
                            object[5][a].yPos=rand() % 300+40;
                            object[5][a].xPos=coin_spawn-rand() % 80;  
                            //same method but there should be more gold coin
                        }     
                        coin_spawn+=level[Game_level].ladder_position/level[Game_level].numbergoldspawn;    
                    }  
                    //------------------------------------------------------------------------
                    Display_Info(Life,Gold_Coin,score,Game_level); //classic mario information display
                    Load_Map(Game_level); //loads the map
                    Play_Music(Game_level); //Plays the music for each level
                    level_change=FALSE; //This if statement shall not be triggered until next death
                    Death_Song=TRUE; //Music
                    complete_song=TRUE;//Music
                    song_counter=0;//Music
                }
                //Time Out!!! Mario is dead-----------------------------------------------------
                if(time==0 && Mario.life!=0 && (level_change_animation==FALSE && Game_Complete==FALSE)){
                    Mario.life=0;  
                    Mario.down=15;
                }    
                //Turtle Size changes when he is stepped on-------------------------------------------------------------
                for(j=0;j<100;j++){
                    if(object[2][j].life==1){
                        object[2][j].height=32;
                        object[2][j].width=32; 
                    }
                    else if(object[2][j].life==2){
                        object[2][j].height=45;
                        object[2][j].width=32;                          
                    }
                }    
                //Mario Size Changes----------------------------------------------------------------
                if(Mario.life==1){
                    Mario.height=32;
                    Mario.width=23; 
                }
                else if(Mario.life==2 || Mario.life==3){
                    Mario.height=64;
                    Mario.width=30;
                }
                if(key[KEY_DOWN] && Mario.life>1 && (level_change_animation==FALSE && Game_Complete==FALSE) && DuckJump == TRUE){
                    Mario.height=37;
                    Mario.width=30;
                    DuckMode = TRUE;
                }
                else
                {
                    DuckMode = FALSE;
                }
                //Mario Movement Function---------------------------------------------------------------------------------------------------------------
                if(key[KEY_RIGHT] && !key[KEY_DOWN] && Right_Collision ==0 && Mario.life>0 && (level_change_animation==FALSE && Game_Complete==FALSE)){
                    Mario.move+=1;        
                    mario_facing=1;  
                } 
                if(key[KEY_LEFT] && !key[KEY_DOWN] && Left_Collision ==0 && Mario.life>0 && (level_change_animation==FALSE && Game_Complete==FALSE)){
                   Mario.move-=1;   
                   mario_facing=0;       
                }  
                //RUN!------------------------------------------------------------------
                if(key[KEY_Z]){  
                    speed=5;
                    inertia_amount=10;
                    //increases the speed but with greater inertia
                }
                //Fire ball shooting function
                if(key[KEY_Z] && Mario.life==3 && keynotshoot==TRUE && level_change_animation==FALSE && Game_Complete==FALSE  && bullet_cd==FALSE){  
                    //This creates a bullet
                    bullet[bullet_counter].life=1;
                    bullet[bullet_counter].yPos=Mario.yPos+30;
                    
                    //The poistion of the fireball varies where mario is facing
                    if(mario_facing == 0){
                        bullet[bullet_counter].xPos=Mario.xPos-bullet[bullet_counter].width;
                        bullet[bullet_counter].move=-8;
                    }
                    else if(mario_facing == 1){
                        bullet[bullet_counter].xPos=Mario.xPos+Mario.width;
                        bullet[bullet_counter].move=8;
                    }
                    bullet_counter++;
                    Sound_Effect(10);
                    keynotshoot=FALSE;
                    bullet_cd=TRUE;
                    
                }
                //If key Z is not pressed, these things will happen
                if(!key[KEY_Z]){
                    speed=3;
                    inertia_amount=6;
                    keynotshoot=TRUE;
                }
                //Because Jump is related to bottom collision
                //It is after the collision detection
                //Collision Detections---------------------------------------
                Right_Collision=0;
                Up_Collision=0;
                Left_Collision=0;
                Down_Collision=0;
                //TOP===================================================
                if(Mario.down>0){
                    i=(Mario.yPos)/32; //detects which row of brick will be tested
                    j1=Mario.xPos/32; //detects the left column of mario's standing poistion
                    j2=(Mario.xPos+Mario.width)/32; //detects the rught column of mario's standing poistion
                    if((Mario.xPos+Mario.width)%32==0) //if j2 is divisable by 32, it should not be detection that column
                        j2--;      
                    if(Mario.yPos%32==0)//delays the collision until it reaches the the next row
                        i--;
                    for(j=j1;j<=j2;j++){     
                        //If collide, the collision will be true
                        if(collide(j*32, i*32,32,32,Mario.xPos,Mario.yPos,Mario.width,Mario.height) && map[i][j]!=0 && Mario.yPos>0  && Mario.life!=0){
                           Up_Collision++;
                           Sound_Effect(0);
                           //Monsters who stands on these bricks will be killed ---------------------------------------------------------
                           if(map[i][j]==13 || map[i][j]==4 || map[i][j]==3){    
                                 for(a=1;a<10;a++){
                                     for(b=0;b<100;b++){ 
                                         if((a==1 || a==2 || a==5 || a==8) && object[a][b].life>0){
                                            if(collide(j*32, i*32,32,32,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && map[i][j]!=0 && Up_Collision==1){
                                                if(((object[a][b].xPos+object[a][b].width/2)>j*32 && (object[a][b].xPos+object[a][b].width/2)<(j+1)*32) || a==8){
                                                    if(a!=8 && a!=5){ //neutral monster will not be slained
                                                        object[a][b].life=0;                                                
                                                    }                                                         
                                                    if(a==8 && map[i][j]!=13){ //Mushroom will go backwards
                                                            object[a][b].move=-object[a][b].move;
                                                    }      
                                                    else if(a==1){ //Goomba  get killed
                                                        score+=100;
                                                        animation[3][b].life=1;
                                                        animation[3][b].xPos=object[a][b].xPos;
                                                        animation[3][b].yPos=object[a][b].yPos;
                                                        animation[3][b].move=object[a][b].move;
                                                        animation[3][b].down=5; 
                                                        Sound_Effect(2);
                                                        //score display
                                                        Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,100);
                                                    }
                                                    else if(a==2){//Turtle  get killed
                                                        score+=100;
                                                        animation[4][b].life=1;
                                                        animation[4][b].xPos=object[a][b].xPos;
                                                        animation[4][b].yPos=object[a][b].yPos;
                                                        animation[4][b].move=object[a][b].move;   
                                                        animation[4][b].down=5; 
                                                        Sound_Effect(2);
                                                        Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,100);
                                                    }
                                                    else if(a==5 && map[i][j]!=4 && map[i][j]!=13 && (object[a][b].yPos<i*32 && object[a][b].xPos>j*32 && object[a][b].xPos<(j+1)*32) || ((object[a][b].xPos+object[a][b].width)>j*32 && (object[a][b].xPos+object[a][b].width)<(j+1)*32)){
                                                       //Coin will be eaten
                                                       object[a][b].life=0;
                                                       Gold_Coin++;
                                                       score+=100;
                                                       animation[0][animation_coin_counter].life=1;
                                                       animation[0][animation_coin_counter].yPos=(i-1)*32;
                                                       animation[0][animation_coin_counter].xPos=(j)*32+7;
                                                       animation[0][animation_coin_counter].down=15;
                                                       Sound_Effect(1);
                                                    }
                                                }
                                            }
                                         }
                                     }
                                 }
                           }                       
                           //--------------------------------------------------------------------------------------------------------------
                           if(map[i][j]==3 && Up_Collision==1 && Mario.life>1){
                               map[i][j]=0; //Mario destroys the brick
                               //Brick Animation---------------------------------------------------------
                               e=0;
                               for(a=animation_brick_counter;a<(animation_brick_counter+4);a++){
                                   e++;
                                   animation[1][a].life=1;
                                   animation[1][a].yPos=(i-1)*32;
                                   animation[1][a].xPos=(j)*32;
                                   if(e%2==0)
                                       animation[1][a].down=5;
                                   else
                                       animation[1][a].down=2; 
                                   if(e<3)
                                       animation[1][a].move=2;
                                  else
                                       animation[1][a].move=-2;  
                               }  
                               animation_brick_counter+=4;
                               Sound_Effect(9);    
                               //--------------------------------------------------------------------  
                           }
                           if(map[i][j]==3 && Up_Collision==1 && Mario.life==1){
                           //Small Mario can only push the brick but not destorying it
                               map[i][j]=14;
                               brick_counter=0;
                           }
                           if(map[i][j]==4 && Up_Collision==1){
                               //Mario gets a gold coin by hiting ? box
                               map[i][j]=10;
                               Gold_Coin++;
                               score+=100;
                               animation[0][animation_coin_counter].life=1;
                               animation[0][animation_coin_counter].yPos=(i-1)*32;
                               animation[0][animation_coin_counter].xPos=(j)*32+7;
                               animation[0][animation_coin_counter].down=15;
                               Sound_Effect(1);
                           }
                           if(map[i][j]==13 && Up_Collision==1){
                               //This kind of ? box will give you power ups
                               map[i][j]=10;                   
                               Sound_Effect(8); 
                               //score+=200;

                               if(Mario.life==1){ //Mushroom will make you bigger
                                   object[8][mushroom_counter].life=1;
                                   object[8][mushroom_counter].yPos=(i-1)*32;
                                   object[8][mushroom_counter].xPos=(j)*32;
                                   object[8][mushroom_counter].move=2;             
                                   mushroom_counter++;      
                               }
                               else{ //Flower gives the Power to shoot fire ball
                                   object[4][flower_counter].life=1;
                                   object[4][flower_counter].yPos=(i-1)*32;
                                   object[4][flower_counter].xPos=(j)*32+3;
                                   object[4][flower_counter].move=0;             
                                   flower_counter++;                                     
                               }            
                           }     
                        } 
                    }    
                }                
                if(Up_Collision!=0)
                {
                    //excutes when there is a top collision
                    Mario.down=0;
                    Mario.yPos=(i+1)*32;                
                }   
                //BOTTOM================================================================
                //excat same method as top collision
                //added gravity(acceleration force that pulls you downwards)
                i=(Mario.yPos+Mario.height)/32;            
                j1=Mario.xPos/32;
                j2=(Mario.xPos+Mario.width)/32;   
                if((Mario.xPos+Mario.width)%32==0)
                    j2--; 
                for(j=j1;j<=j2;j++){ 
                    if(collide(j*32, i*32,32,32,Mario.xPos,Mario.yPos,Mario.width,Mario.height) && map[i][j]!=0){
                       Down_Collision++;
                    }
                }               
                if(Down_Collision!=0  && Mario.life>0 && (Mario.yPos+Mario.height<480)){
                    Mario.down=0;
                    Mario.yPos=i*32-(Mario.height);         
                    if(map[i][j]==2 && Game_level==2){
                        inertia_amount=30;
                    }       
                }              
                else if(Down_Collision==0 || Mario.life<1){
                       //Mario accelerates when there is no gravity
                        Mario.down-=1;
                }      
                //Mario's X MOVEMENT-------------------------------------------------------------------------------------------------
                Mario.xPos+=Mario.move;   
                //Right =============================================================================================================
                //Same method as the top collision, but with the right side of the tiles
                i1=Mario.yPos/32;              
                i2=(Mario.yPos+Mario.height)/32;
                if((Mario.yPos+Mario.height)%32==0){
                     i2--;
                }            
                j=(Mario.xPos+Mario.width)/32;
    
                for(i=i1;i<=i2;i++){        
                    if(collide(j*32, i*32,32,32,Mario.xPos,Mario.yPos,Mario.width,Mario.height) && map[i][j]!=0 && Mario.yPos>0){
                       Right_Collision++;
                    }                           
                }      
                if(Right_Collision !=0 && Mario.move>0 && Mario.life>0){
                     Mario.xPos=j*32-Mario.width;
                     Mario.move=0;
                     inertia_counter=0;
                }   
                //LEFT =============================================================================================================
                //Same method as the bottom collision, but with the left side of the tiles
                i1=Mario.yPos/32;  
                i2=(Mario.yPos+Mario.height)/32;
                if((Mario.yPos+Mario.height)%32==0){
                    i2--;
                }
                j=(Mario.xPos)/32;
                if(Mario.xPos%32==0 && Mario.move<0)
                    j--;
                for(i=i1;i<=i2;i++){     
                    if(collide(j*32, i*32,32,32,Mario.xPos,Mario.yPos,Mario.width,Mario.height) && map[i][j]!=0 && Mario.yPos>0){
                       Left_Collision++;
                    }  
                }                   
                if(Left_Collision !=0   && Mario.yPos>=0  && Mario.life>0){
                     Mario.xPos=(j+1)*32;
                     Mario.move=0;
                     inertia_counter=0;             
                }

                //=====================================================================================================================
                //Jump!    
                if(key[KEY_X] && Down_Collision!=0 && KeyNotJump==TRUE && Mario.life>0 && (Mario.height+Mario.yPos<480) && (level_change_animation==FALSE && Game_Complete==FALSE)){  
                    Mario.down+=17;
                    KeyNotJump=FALSE;
                    if(Mario.life==1)
                        Sound_Effect(4);
                    else
                        Sound_Effect(5);
                    //Mario gains a upward force if he is standing somewhere
                    if(key[KEY_DOWN])
                    {
                        DuckJump = TRUE;
                    }
                    else
                    {
                        DuckJump = FALSE;
                    }
                }
                if(key[KEY_DOWN] && Down_Collision!=0)
                    DuckJump = TRUE;
                if(!key[KEY_X]) //you cant hold x to jump
                    KeyNotJump=TRUE;
                if(Mario.move>speed){ //Mario's speed is limited by the variable called speed
                    Mario.move=speed;
                }
                else if(Mario.move<-speed){
                    Mario.move=-speed;
                }      
                //FALLING-----------------------------------------
                Mario.yPos-=Mario.down; 
                //--------------------------------------------------
                //MONSTERS===========================================================================================================================================
                On_Hit=TRUE; //Only applies Collision on the monster once so mario wont die if he jumps on 2 enemies
                for(a=9;a>=0;a--){
                    for(b=0;b<100;b++){
                        if(collide(Mario.xPos,Mario.yPos,Mario.width,Mario.height,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && object[a][b].life>0 && Mario.life>0 && On_Hit==TRUE){    
                            if(a==8){ //Mushroom ------------------------
                                if(Mario.life==1){
                                    Mario.yPos-=32;
                                    Mario.xPos-=10;
                                    Sound_Effect(6);
                                    score+=1000;
                                    Mario.life=2;
                                    
                                }
                                else{
                                    score+=1000; //if you are big mario already, you just gain scores
                                }
                                Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,1000);
                                object[a][b].life=0;
                            }
                            if(a==1  && mario_transform==FALSE){ //Goomba=================================================================
                                On_Hit=FALSE;
                                if(Mario.down<0 && Mario.yPos<object[a][b].yPos){ //If mario is falling and he collides with
                                    Sound_Effect(3);                              //Goomba, Mario kills it
                                    score+=200;
                                    Mario.down=15;
                                    object[a][b].life=0;
                                    Mario.yPos=object[a][b].yPos-Mario.height;
                                    //Animation
                                    animation[2][b].life=1;
                                    animation[2][b].xPos=object[a][b].xPos;
                                    animation[2][b].yPos=object[a][b].yPos;
                                    animation[2][b].move=0;
                                    animation[2][b].down=0;
                                    Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,200);
                                }
                                else if((Mario.yPos+Mario.height)<(object[a][b].yPos+object[a][b].height) && Mario.down==0)
                                    object[a][b].move=-object[a][b].move; //Mario will not get hurt by goomba if he is standing above the goomba
                                else if(Mario.down>=0){         //Mario loses health he is collide with a goomba without jumping down
                                        if(Mario.life>1){
                                            Mario.life--;
                                            Sound_Effect(7);
                                        }
                                        else if(Mario.life==1){
                                            Mario.life=0;
                                        }
                                        if(Mario.life==0){ //Mario dies
                                            Mario.down = 15;
                                        }
                                        else{
                                            mario_transform=TRUE; //if he has more than 1 life, he will enter a mode that nothing can harm him
                                        }
                                        object[a][b].move=-object[a][b].move; //Goomba will move backwards once it collided with Mario(and survived)
                                }
                            }
                            if(a==2  && mario_transform==FALSE){ //turtles================================================================
                            //Same method as goomba, but turtle has 2 lifes and its turtle shell is a lot different than the goomba
                                On_Hit=FALSE;                  
                                if(Mario.down<0 && Mario.yPos+Mario.height<object[a][b].yPos+object[a][b].height){ //Mario will make the moving turtle
                                    Mario.down=13;                                                                 //into a turtle shell
                                    
                                    Mario.yPos=object[a][b].yPos-Mario.height;
                                    object[a][b].life=1;
                                    object[a][b].death_counter=0;
                                    if(object[a][b].life==1 && object[a][b].xPos>=x_scroll && object[a][b].xPos<=x_scroll+640)
                                        Sound_Effect(3);
                                    if(object[a][b].life==2)
                                        object[a][b].move=0;
                                }
                                else if((Mario.yPos+Mario.height)<(object[a][b].yPos+object[a][b].height) && (Mario.down==0 && object[a][b].life==2 || (object[a][b].life==1 && object[a][b].move==0)))
                                    object[a][b].move=-object[a][b].move; //Same thing as the goomba part
                                else if(Mario.down>=0){                                
                                    if(object[a][b].life==2 || (object[a][b].life==1 && object[a][b].move!=0)){ //Same thing when goomba collides with Mario
                                        if(Mario.life>1){
                                            Mario.life--;                                        
                                            Sound_Effect(7);
                                        }
                                        else if(Mario.life==1){
                                            Mario.life=0;
                                        }
                                        if(Mario.life==0){
                                            Mario.down = 15;
                                        }
                                        else{
                                            mario_transform=TRUE;
                                        }
                                        object[a][b].move=-object[a][b].move;  
                                    }     
                                }
                                if(object[a][b].life==1 && object[a][b].move==0 && (Mario.move!=0 || Mario.down!=0)){ //Collision in the turtle shell mode
                                    Sound_Effect(2);
                                    score+=50;
                                    //Turtle moves depends on where mario faces
                                    if(mario_facing==1)
                                        object[a][b].move=7;
                                    else
                                        object[a][b].move=-7;
                                    //to prevent kicking turtle shell hurts mario
                                    if(Mario.down==0 && Mario.move<0){
                                        Mario.move=0;
                                        Mario.xPos=object[a][b].xPos+object[a][b].width+1;
                                    }
                                    else if(Mario.down==0 && Mario.move>0){
                                         Mario.move=0;
                                         Mario.xPos=object[a][b].xPos-Mario.width-1;
                                    }
                                    Mario.down=15; //Mario jumps when he hits the turtle, doesnt happen when he is walking because
                                                   //bottom collision cancels it

                                }      
                                else if(object[a][b].life==1 && object[a][b].move!=0){
                                    object[a][b].move=0;
                                    //Mario stops the turtle shell moving when he stands on it
                                }                        
                            } 
                            if(a==3  && mario_transform==FALSE){ //meteroid=================================================================
                                //Excatly the same thing as the goomba collisions and Turtle collisions
                                //However, Mario cant destroy the meteroid, he will only take dmg from it
                                On_Hit=FALSE;
                                if(Mario.life>1){
                                    Mario.life--;
                                    Sound_Effect(7);
                                }
                                else if(Mario.life==1){
                                    Mario.life=0;
                                }
                                if(Mario.life==0){
                                    Mario.down = 15;
                                }
                                else{
                                    mario_transform=TRUE;
                                }
                                //Meteroid Animation-----------------------------------------------------
                                animation[10][animation_ball_counter].move=0;
                                animation[10][animation_ball_counter].down=0;
                                animation[10][animation_ball_counter].life=1;
                                animation[10][animation_ball_counter].death_counter=0;                    
                                animation[10][animation_ball_counter].xPos=object[a][b].xPos;
                                animation[10][animation_ball_counter].yPos=object[a][b].yPos;
                                object[a][b].life=0; //Meteroid is destroied after it hits mario
                                animation_ball_counter++;
                            }
                            if(a==4){ //Flower ------------------------
                                      //Mario gains the ability to shoot fire
                                if(Mario.life==1){
                                    Mario.yPos-=32;
                                    Mario.xPos-=10;
                                                                        
                                }
                                if(Mario.life!=3){
                                    Sound_Effect(6);
                                    score+=1000;
                                }
                                else{
                                    score+=1000;
                                }                                
                                Mario.life=3;
                                object[a][b].life=0;
                                Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,1000);
                                
                            }
                            if(a==5){ //Gold Coin ------------------------
                                      //More Gold Coin
                                      //99 Gold coin = 1 extra life
                                score+=100;
                                Gold_Coin++;
                                Sound_Effect(1);
                                object[a][b].life=0;
                            } 
                            if(a==6  && mario_transform==FALSE){ //Cannon=================================================================
                                On_Hit=FALSE;
                                //Excatly the same thing as goomba
                                if(Mario.down<0 && Mario.yPos<object[a][b].yPos  || ((Mario.yPos+Mario.height)<(object[a][b].yPos+object[a][b].height) && Mario.down==0)){
                                    Sound_Effect(3);
                                    score+=200;
                                    Mario.down=15;
                                    object[a][b].life=0;
                                    Mario.yPos=object[a][b].yPos-Mario.height;
                                    animation[11][b].life=1;
                                    animation[11][b].xPos=object[a][b].xPos;
                                    animation[11][b].yPos=object[a][b].yPos;
                                    animation[11][b].move=object[a][b].move;
                                    animation[11][b].down=0;
                                    Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,200);
                                }
                                else if(Mario.down>=0){
                                        if(Mario.life>1){
                                            Mario.life--;
                                            Sound_Effect(7);
                                        }
                                        else if(Mario.life==1){
                                            Mario.life=0;
                                        }
                                        if(Mario.life==0){
                                            Mario.down = 15;
                                        }
                                        else{
                                            mario_transform=TRUE;
                                        }   
                                }
                            }       
                            if(a==7){//Button---------------------------------------------
                            //Used in the boss fire scene, this button destroys all the bricks within the screen area
                                if(Mario.down<0 && Mario.yPos<object[a][b].yPos){
                                    object[a][b].life=0;
                                    object[9][0].down=6;
                                    Sound_Effect(3);
                                    for(c=x_scroll/32;c<(x_scroll/32+20);c++){
                                        for(d=0;d<15;d++){
                                            if(map[d][c]==3){
                                                map[d][c]=0;  
                                                //Brick Animation---------------------------------------------------------
                                                animation[1][animation_brick_counter].life=1;
                                                animation[1][animation_brick_counter].yPos=(d-1)*32;
                                                animation[1][animation_brick_counter].xPos=(c)*32;
                                                animation[1][animation_brick_counter].down=2;
                                                animation_brick_counter++;
                                                Sound_Effect(9);  
                                                //--------------------------------------------------------------------                                                
                                            }
                                        }
                                    }
                                }
                                else{ //Right & left Collision with the button 
                                    if(Mario.xPos<object[a][b].xPos){
                                        Mario.move=0;
                                        Mario.xPos=object[a][b].xPos-Mario.width;
                                    }
                                    if(Mario.xPos>object[a][b].xPos){
                                        Mario.move=0;
                                        Mario.xPos=object[a][b].xPos+object[a][b].width;
                                    }
                                }                                
                            }       
                            if(a==0  && mario_transform==FALSE){ //Boswer-------------------------------------------------------------
                            //The final boss, he hurts Mario but Mario cant do anything to him
                                if(Mario.life>1){
                                    Mario.life--;
                                    Sound_Effect(7);
                                }
                                else if(Mario.life==1){
                                    Mario.life=0;
                                }
                                if(Mario.life==0){
                                    Mario.down = 15;
                                }
                                else{
                                    mario_transform=TRUE;
                                }
                            }                                 
                            if(a==9){//When Mario touches the flag, level changes                    
                                level_change_animation=TRUE;
                            }      
                        }          
                    if(object[a][b].xPos+object[a][b].width<0 || object[a][b].yPos>=480){
                        object[a][b].life=0; //if the object falls out of the map, object is dead
                    }                                       
                    //Object movement========================================================================================================================================
                    //Please refer to the Mario move collision because it is the same method
                    if(object[a][b].life>0){
                        object[a][b].yPos-=object[a][b].down;
                        object[a][b].xPos+=object[a][b].move;  
                        Right_Collision_2=0;
                        Left_Collision_2=0;
                        Down_Collision_2=0;
                        //BOTTOM================================================================
                        i=(object[a][b].yPos+object[a][b].height)/32;            
                        j1=object[a][b].xPos/32;
                        j2=(object[a][b].xPos+object[a][b].width)/32;   
                        if((object[a][b].xPos+object[a][b].width)%32==0)
                            j2--; 
                        for(j=j1;j<=j2;j++){ 
                              if(collide(j*32, i*32,32,32,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && map[i][j]!=0  && a!=5  && a!=6 && object[a][b].yPos>0){
                                Down_Collision_2++;
                                if(a==3 && object[a][b].life!=0 &&object[a][b].yPos>0){ //When the meteroid hits a game tile, it will destroy the game tile
                                    map[i][j]=0;
                                    animation[10][animation_ball_counter].move=0;
                                    animation[10][animation_ball_counter].down=0;
                                    animation[10][animation_ball_counter].life=1;
                                    animation[10][animation_ball_counter].death_counter=0;            
                                    animation[10][animation_ball_counter].xPos=object[3][b].xPos;
                                    animation[10][animation_ball_counter].yPos=object[3][b].yPos;
                                    object[a][b].life=0;
                                    animation_ball_counter++;
                                }                                              
                            }
                        }   
                        //Some objects has no gavity, such as the coins
                        if((Down_Collision_2==0 || (object[a][b].yPos+object[a][b].height>480)) && a!=5  && a!=6){    
                                object[a][b].down-=1;                                
                        }                
                        else if(Down_Collision_2!=0 || a==5)
                        {
                                object[a][b].down=0;
                                object[a][b].yPos=i*32-(object[a][b].height);                                            
                        }              
                
                        //Right =============================================================================================================
                        i1=object[a][b].yPos/32;
                        i2=(object[a][b].yPos+object[a][b].height)/32;
                        if((object[a][b].yPos+object[a][b].height)%32==0){
                             i2--;
                        }
                        j=(object[a][b].xPos+object[a][b].width)/32;

                        for(i=i1;i<=i2;i++){
                            if(collide(j*32, i*32,32,32,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && map[i][j]!=0  && a!=6){
                                Right_Collision_2++;
                                if(map[i][j-1]!=0  && a!=6){
                                    object[a][b].life=0; //destroys the object if it is stuck in the map
                                }
                             }
                        }
                        if(Right_Collision_2 !=0 && object[a][b].yPos>=0){
                             object[a][b].xPos=j*32-object[a][b].width;

                             object[a][b].move=-object[a][b].move;
                             
                             //Sounds effects when turtle shell hits something(applies with in the screen area)
                             if(object[a][b].life==1 && a==2 && object[a][b].xPos>=x_scroll && object[a][b].xPos<=x_scroll+640 && object[a][b].move>0)
                                 Sound_Effect(2);
                        }
                        //LEFT =============================================================================================================          
                        i1=object[a][b].yPos/32;              
                        i2=(object[a][b].yPos+object[a][b].height)/32;
                        if((object[a][b].yPos+object[a][b].height)%32==0){
                             i2--;
                        }   
                        j=(object[a][b].xPos)/32;
                        if(object[a][b].xPos%32==0 && object[a][b].move<0)
                            j--;
                        for(i=i1;i<=i2;i++){     
                            if(collide(j*32, i*32,32,32,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && map[i][j]!=0  && a!=6){
                                Left_Collision_2++;
                                if(map[i][j+1]!=0  && a!=6){
                                    object[a][b].life=0; //destroys the object if it is stuck in the map
                                }
                            }  
                        }   
                        if(Left_Collision_2 !=0  && object[i][j].yPos>=0){
                             object[a][b].xPos=(j+1)*32;
                             object[a][b].move=-object[a][b].move;
                             //Same as the right one
                             if(object[a][b].life==1 && a==2 && object[a][b].xPos>=x_scroll && object[a][b].xPos<=x_scroll+640 && object[a][b].move>0)
                                 Sound_Effect(2);
                        }
                      }
                   }
                }
                //unit collision
                //========================================================================================================================================
                for(a=1;a<3;a++){       //long loop that accounts for all the goombas and turtles
                    for(b=0;b<100;b++){ 
                        for(c=a;c<3;c++){ 
                            if(c==a)
                                e=b;
                            if(c!=a)
                                e=0;      
                            for(d=e;d<100;d++){  
                                if(!(a==c && d==b)){ 
                                    if(collide(object[c][d].xPos,object[c][d].yPos,object[c][d].width,object[c][d].height,object[a][b].xPos,object[a][b].yPos,object[a][b].width,object[a][b].height) && object[a][b].life!=0 && object[c][d].life!=0){    

                                        //This is the part where turtle shell hits other monsters
                                        turtle_turning=FALSE;
                                        if((object[a][b].life==1 || object[c][d].life==1) && (c==2 || a==2)){
                                                if(c==2 && object[c][d].life==1 && object[c][d].move!=0){                                                
                                                    //Animation----------------------------------
                                                    if(a==1){
                                                        animation[3][b].life=1;
                                                        animation[3][b].xPos=object[a][b].xPos;
                                                        animation[3][b].yPos=object[a][b].yPos;
                                                        animation[3][b].move=object[a][b].move;
                                                        animation[3][d].down=4;
                                                    }
                                                    else if(a==2){
                                                        animation[4][b].life=1;
                                                        animation[4][b].xPos=object[a][b].xPos;
                                                        animation[4][b].yPos=object[a][b].yPos;
                                                        animation[4][b].move=object[a][b].move;      
                                                        animation[4][d].down=4;                                          
                                                    }
                                                    //------------------------------------------
                                                    object[a][b].life=0;
                                                    score+=200;
                                                    Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,200);
                                                    turtle_turning=TRUE;
                                                    if(object[c][d].life==1 &&  object[c][d].xPos>=x_scroll && object[c][d].xPos<=x_scroll+640)
                                                        Sound_Effect(2);
                                                }
                                                if(a==2 && object[a][b].life==1 && object[a][b].move!=0){
                                                    //Animation----------------------------------
                                                    if(c==1){
                                                        animation[3][d].life=1;
                                                        animation[3][d].xPos=object[c][d].xPos;
                                                        animation[3][d].yPos=object[c][d].yPos;
                                                        animation[3][d].move=object[c][d].move;
                                                        animation[3][d].down=4;
                                                    }
                                                    else if(c==2){
                                                        animation[4][d].life=1;
                                                        animation[4][d].xPos=object[c][d].xPos;
                                                        animation[4][d].yPos=object[c][d].yPos;
                                                        animation[4][d].move=object[c][d].move;   
                                                        animation[4][d].down=4;                                             
                                                    }
                                                    //-----------------------------------------------
                                                    object[c][d].life=0;
                                                    score+=200;
                                                    Scoredisplayer(Gamescore,&global_score_counter,object[a][b].xPos,object[a][b].yPos,200);
                                                    turtle_turning=TRUE;
                                                    if(object[a][b].life==1 && object[a][b].xPos>=x_scroll && object[a][b].xPos<=x_scroll+640)
                                                        Sound_Effect(2);
                                                }
                                                
                                        }
                                        if(turtle_turning==FALSE){   //this is how the objects will change their direct when they hit each other
                                            if(object[c][d].move<0 && object[a][b].move<0 && object[c][d].xPos<object[a][b].xPos){
                                                object[a][b].move=-object[a][b].move;
                                            }
                                            else if(object[c][d].move>0 && object[a][b].move>0 && object[c][d].xPos>object[a][b].xPos){
                                                object[a][b].move=-object[a][b].move;
                                            }
                                            else if(object[c][d].move<0 && object[a][b].move<0 && object[c][d].xPos>object[a][b].xPos){
                                                object[c][d].move=-object[c][d].move;
                                            }
                                            else if(object[c][d].move>0 && object[a][b].move>0 && object[c][d].xPos<object[a][b].xPos){
                                                object[c][d].move=-object[c][d].move;                                 
                                            }
                                            else{ 
                                                 if(object[c][d].xPos<object[a][b].xPos)                                                    
                                                     object[a][b].xPos=object[c][d].xPos+object[c][d].width;
                                                 else if(object[c][d].xPos>object[a][b].xPos)                                             
                                                     object[c][d].xPos=object[a][b].xPos+object[a][b].width;  
                                                 object[a][b].move=-object[a][b].move;
                                                 object[c][d].move=-object[c][d].move;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                //Bullets-----------------------------------------------------------------------------------------------------------
                for(a=0;a<100;a++){
                    if(bullet[a].life>0){
                        bullet[a].yPos-=bullet[a].down;
                        bullet[a].xPos+=bullet[a].move;  
                        Right_Collision_3=0;
                        Left_Collision_3=0;
                        Down_Collision_3=0;
                        Up_Collision_3=0;
                        //Collision with the terrian-------------------------------------------
                        //Button has the excat same collision as mario and game objects
                        //However it jumps every time it reaches the ground
                        if(bullet[a].down>0){
                            i=(bullet[a].yPos)/32;
                            j1=bullet[a].xPos/32;
                            j2=(bullet[a].xPos+bullet[a].width)/32; 
                            if((bullet[a].xPos+bullet[a].width)%32==0)
                                j2--;      
                            if(bullet[a].yPos%32==0)
                                i--;
                            for(j=j1;j<=j2;j++){     
                                if(collide(j*32, i*32,32,32,bullet[a].xPos,bullet[a].yPos,bullet[a].width,bullet[a].height) && map[i][j]!=0 && bullet[a].yPos>0)
                                   Up_Collision_3++;
                            }
                            if(Up_Collision_3!=0){  
                                animation[5][a].life=1;
                                animation[5][a].xPos=bullet[a].xPos;
                                animation[5][a].yPos=bullet[a].yPos;
                                bullet[a].life=0;   
                            }   
                        }                     
                        //BOTTOM================================================================
                        i=(bullet[a].yPos+bullet[a].height)/32;            
                        j1=bullet[a].xPos/32;
                        j2=(bullet[a].xPos+bullet[a].width)/32;   
                        if((bullet[a].xPos+bullet[a].width)%32==0)
                            j2--; 
                        for(j=j1;j<=j2;j++){ 
                              if(collide(j*32, i*32,32,32,bullet[a].xPos,bullet[a].yPos,bullet[a].width,bullet[a].height) && map[i][j]!=0){
                                Down_Collision_3++;
                            }
                        }   
                        if(Down_Collision_3==0){
                                bullet[a].down-=1;
                        }                
                        if(Down_Collision_3!=0)
                        {
                            bullet[a].down=8; //Fireball bouncing
                            bullet[a].yPos=i*32-bullet[a].height;                
                        }              
                        if(bullet[a].yPos>480){
                            bullet[a].life=0;
                        }
                
                        //Right =============================================================================================================         
                        i1=bullet[a].yPos/32;              
                        i2=(bullet[a].yPos+bullet[a].height)/32;
                        if((bullet[a].yPos+bullet[a].height)%32==0){
                             i2--;
                        }            
                        j=(bullet[a].xPos+bullet[a].width)/32;
                
                        for(i=i1;i<=i2;i++){        
                            if(collide(j*32, i*32,32,32,bullet[a].xPos,bullet[a].yPos,bullet[a].width,bullet[a].height) && map[i][j]!=0){
                                Right_Collision_3++;
                            }                           
                        }      
                        if(Right_Collision_3 !=0 && bullet[a].yPos>=0){ 
                             animation[5][a].life=1;
                             animation[5][a].xPos=bullet[a].xPos;
                             animation[5][a].yPos=bullet[a].yPos;
                             bullet[a].life=0;
                        }   
                        //LEFT =============================================================================================================          
                        i1=bullet[a].yPos/32;              
                        i2=(bullet[a].yPos+bullet[a].height)/32;
                        if((bullet[a].yPos+bullet[a].height)%32==0){
                             i2--;
                        }   
                        j=(bullet[a].xPos)/32;
                        if(bullet[a].xPos%32==0 && bullet[a].move<0)
                            j--;
                        for(i=i1;i<=i2;i++){     
                            if(collide(j*32, i*32,32,32,bullet[a].xPos,bullet[a].yPos,bullet[a].width,bullet[a].height) && map[i][j]!=0){
                                Left_Collision_3++;
                            }  
                        }   
                        if(Left_Collision_3 !=0  && object[i][j].yPos>=0){            
                             animation[5][a].life=1;
                             animation[5][a].xPos=bullet[a].xPos;
                             animation[5][a].yPos=bullet[a].yPos;
                             bullet[a].life=0;
                        }      
                        //Bullets and the Monsters(Goomba and Turtle Only) Bowser is Immune to this---------------------------------------------
                        for(c=0;c<3;c++){ 
                            for(d=0;d<100;d++){  
                                if(collide(object[c][d].xPos,object[c][d].yPos,object[c][d].width,object[c][d].height,bullet[a].xPos,bullet[a].yPos,bullet[a].width,bullet[a].height) && bullet[a].life!=0 && object[c][d].life!=0){   
                                    animation[5][a].life=1;
                                    animation[5][a].xPos=bullet[a].xPos;
                                    animation[5][a].yPos=bullet[a].yPos;
                                    bullet[a].life=0;     
                                    if(c==1){ //When it applies to a goomba
                                        object[c][d].life=0;
                                        Sound_Effect(2);
                                        score+=100;                                        
                                        animation[3][d].life=1;
                                        animation[3][d].xPos=object[c][d].xPos;
                                        animation[3][d].yPos=object[c][d].yPos;
                                        animation[3][d].move=object[c][d].move;
                                        animation[3][d].move=object[c][d].move;
                                        animation[3][d].down=3;
                                        Scoredisplayer(Gamescore,&global_score_counter,object[c][d].xPos,object[c][d].yPos,100);
                                    }
                                    else if(c==2){ //When it applies to a turtle
                                        object[c][d].life=0;
                                        Sound_Effect(2);
                                        score+=100;   
                                        animation[4][d].life=1;
                                        animation[4][d].xPos=object[c][d].xPos;
                                        animation[4][d].yPos=object[c][d].yPos;
                                        animation[4][d].move=object[c][d].move;   
                                        animation[4][d].down=3;
                                        Scoredisplayer(Gamescore,&global_score_counter,object[c][d].xPos,object[c][d].yPos,100);
                                    }                                                                        
                                }
                            }
                        }                                   
                    } 
                }      
                //END OF NORMAL MOVEMENTS---------------------------------------------------------------------------------------------
                if(Gold_Coin>99){//When you have 100 coins, it will convert 99 coins into 1 life
                    Gold_Coin-=99;
                    Life++;
                    animation[6][0].life=1;
                    animation[6][0].move=0;
                    animation[6][0].xPos=Mario.xPos+60;
                    animation[6][0].yPos=Mario.yPos-60;
                    Sound_Effect(11);
                }
                //Animations--------------------------------------------------------------------------------------------------------
                //Think animations as game objects that has no collision yet with gravity and other movements
                for(a=0;a<12;a++){
                    for(b=0;b<100;b++){     
                        if(animation[a][b].life>0){
                            if(level_change_animation==FALSE){
                                animation[9][0].yPos=object[9][0].yPos+17;
                                animation[9][0].down=0;
                            }                            
                            if(a!=2 && a!=5 && a!=6 && a!=7 && a!=8 && a!=10){ //gravity is only applied to few
                                animation[a][b].yPos-=animation[a][b].down;
                            }                        
                            animation[a][b].xPos+=animation[a][b].move;  
                            animation[a][b].down-=1;   
                            if(animation[a][b].yPos>480 || ((a==0 || a==2 || a==5 || a==6 || a==7 || a==8 || a==10) && animation[a][b].death_counter>30)){
                                animation[a][b].life=0;
                                animation[a][b].death_counter=0;
                                if(a==0)
                                {
                                    Scoredisplayer(Gamescore,&global_score_counter,animation[a][b].xPos,animation[a][b].yPos,100);
                                }
                            }
                        }
                    }
                }                             
                //Counters     
                inertia_counter ++;
                speed_counter --;
                goomba_sprite_counter++;
                brick_counter++;
                mario_sprite_counter+=abs(Mario.move);
                //Bullet Counters--------------------------------------------------------------------------------
                if(bullet_cd==TRUE){ //Cool down time for the bullet
                    bullet_cd_counter++;
                }
                if(bullet_cd_counter>30){
                    bullet_cd_counter=0;
                    bullet_cd=FALSE;
                }
                //Animation Counters=============================================================================
                //Determines how long the animation lasts(if they dont fall off the screen)
                for(b=0;b<100;b++){     
                    if(animation[2][b].life==1){
                        animation[2][b].death_counter++;
                    }                                      
                    if(animation[0][b].life==1){
                        animation[0][b].death_counter++;
                    }                    
                    if(animation[5][b].life==1){
                        animation[5][b].death_counter+=3;
                    }   
                    if(animation[6][b].life==1){
                        animation[6][b].death_counter++;
                    }       
                    if(animation[10][b].life==1){
                        animation[10][b].death_counter+=3;
                    }                             
                }   
                //Falling Rocks!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                if(Game_level==5 || Game_level==4){ //level 4 and level 5 has cannons
                    sky_fire_counter++;
                    for(b=0;b<100;b++){
                        object[3][b].down=-2; //adjust the downward speed of the cannon
                    }
                    //Spawns a cannon ball every few seconds
                    if(sky_fire_counter>85 && Mario.xPos<level[Game_level].ladder_position-600){
                        object[3][sky_fire_ball_counter].life=1;
                        object[3][sky_fire_ball_counter].yPos=-32;
                        object[3][sky_fire_ball_counter].down=0;
                        object[3][sky_fire_ball_counter].xPos=Mario.xPos+rand()%720+90;
                        
                        //this is a random movement function
                        if(object[3][sky_fire_ball_counter].xPos%2==0){
                            object[3][sky_fire_ball_counter].move=1;
                        }
                        else{
                            object[3][sky_fire_ball_counter].move=-1;
                        }
                        sky_fire_counter=0;
                        sky_fire_ball_counter++;
                    }
                }
                
                //Cannons!----------------------------------------------------------------------------------------
                if(Game_level==5 || Game_level==3){ //happens in level 5 and level 3
                    cannon_counter++;
                    //spawns a cannon every few seconds
                    if(cannon_counter>100){
                        object[6][cannon_spawn_counter].life=1;
                        object[6][cannon_spawn_counter].yPos=Mario.yPos-40+rand()%80;
                        object[6][cannon_spawn_counter].down=0;
                        object[6][cannon_spawn_counter].xPos=Mario.xPos+640;
                        object[6][cannon_spawn_counter].move=-2;
                        cannon_counter=0;
                        cannon_spawn_counter++;
                    }                    
                    //if you complete the level, there wont be any cannon ball coming to hurt you
                    if(level_change_animation == TRUE || Game_Complete== TRUE){
                        for(i=0;i<100;i++){
                            object[6][i].life=0;
                        }
                    }
                }

                //game scroll=================================================================================
    			if(Mario.xPos<320){ //if Mario is close to the left end, the scroll position will remain the same
                    mario_scroll_position=Mario.xPos;
                    x_scroll=0;
                }
                else{
                    mario_scroll_position=320; //Mario will stay at the centre of the screen
                    x_scroll=Mario.xPos-320; //x_scroll is the x value at the left most side of the game screen
                }
                //Mario Poistion counter======================================================================
                if(Mario.xPos<=0){
                    Mario.xPos=0; //Mario cant move off the map
                }
                if(Mario.yPos>480){
                    Mario.yPos=481; //He wont go down any further
                    Mario.life=0;
                }
                //Boss Fight !--------------------------------------------------------------------------------
                if(Mario.xPos>5120 && Game_level==6 && Spawn_Browser==FALSE){
                    //Spawns bowser in a designed location
                    object[0][0].life=1;   
                    object[0][0].yPos=0;
                    object[0][0].xPos=5320;
                    object[0][0].move=1;
                    object[0][0].down=0;
                    Browser_Jump=0;
                    Browser_Turtle=0;
                    Spawn_Browser=TRUE;
                    Play_Music(8); //Intense Music
                    Browser_Jump=0;
                    Browser_Turtle=0;
                }
                if(object[0][0].life==1){ //while bowser is alive
                    Browser_Jump++; //his jumping cool down
                    Browser_Turtle++; //his turtle cool down
                    if(object[0][0].xPos>5380){ //bowser will stay between 5380 and 5300
                        object[0][0].move = -object[0][0].move;
                    }
                    else if(object[0][0].xPos<5300){
                        object[0][0].move = -object[0][0].move;
                    }                    
                    if(Browser_Jump>150 && object[7][0].life==1){ //every 150/60 seconds,bowser jumps
                        object[0][0].down=20;
                        Browser_Jump=0;
                    }
                    if(Browser_Turtle>200 && object[7][0].life==1){//every 150/60 seconds,bowser spawns a turtle shell
                        object[2][flower_counter].life=1;          //infront of him
                        object[2][flower_counter].yPos=object[0][0].yPos+20;
                        object[2][flower_counter].xPos=object[0][0].xPos-object[2][flower_counter].width-1;
                        Browser_Turtle=0;
                        object[2][flower_counter].move=-7;
                        flower_counter++; //flower_counter is not used in level 6, 
                                          //so it will be used to count the turtle shells
                    }                    
                    if(x_scroll>4930){ //locking the game scroll so mario has to stay in the boss fight
                        x_scroll=4930;
                    }
                    else if(x_scroll<4800){ //still allows some scrolling
                        x_scroll=4800;
                    }
                    mario_scroll_position=Mario.xPos-x_scroll; //adjust the scroll position again
                    if(Mario.xPos<x_scroll){ //Mario is locked in this battle arena
                        Mario.xPos=x_scroll;
                        Mario.move=0;
                    }
                    if(Mario.xPos>x_scroll+640+Mario.width){ //same as above
                        Mario.xPos=x_scroll+640-Mario.width;
                        Mario.move=0;
                    }
   
                    for(b=0;b<100;b++){  //Bowser and Goomba collision
                        if(collide(object[0][0].xPos,object[0][0].yPos,object[0][0].width,object[0][0].height,object[2][b].xPos,object[2][b].yPos,object[2][b].width,object[2][b].height) && object[2][b].life!=0){    
                            object[2][b].life=0; //Turtle will die
                            animation[4][b].life=1;
                            animation[4][b].xPos=object[2][b].xPos;
                            animation[4][b].yPos=object[2][b].yPos;
                            animation[4][b].move=object[2][b].move;   
                            animation[4][b].down=3;     
                        }
                    }                                                                                                                                                                 
                }
                if(object[0][0].life==0 && Spawn_Browser==TRUE){ //if bowser is spawned and killed
                    Game_Complete = TRUE;                        //Game is complete
                }
                //Map Tiles
                for(i=0;i<10;i++){
                    background_scroll[i]=-(x_scroll/5)+i*1592;  //giving the positions of the background
                }
                //Inertia==============================================================================================
                if(inertia_counter>inertia_amount){ //Makes the ground slippery
                    inertia_counter=0;              //hard, and has a realistic feeling
                    if(Mario.move!=0){              
                        if(Mario.move>0){
                            Mario.move--; //every few counters, mario's velocity slows down
                        }
                        if(Mario.move<0){
                            Mario.move++; // same as above
                        }    
                    }
                }    
                //TIMERS---------------------------------------------------------------------------------
                if(Mario.death==TRUE){
                    Mario.death_counter++; //it is a timer which in charges the song when mario dies 
                }                 
                if(Mario.death_counter>180){ // same as above
                    Mario.death_counter=0;
                    Mario.death=FALSE;
                }
                //score display
                for(i=0;i<1000;i++)
                {
                    if(Gamescore[i].life>0)
                    {
                        Gamescore[i].time_counter--;
                        Gamescore[i].yPos--;
                    }
                    if(Gamescore[i].time_counter<=0)
                    {
                        Gamescore[i].life = 0;
                    }
                }
                //Turtle Regen----------------------------------------------------
                for(b=0;b<100;b++){ 
                    if(object[2][b].life==1 && object[2][b].move==0){
                        object[2][b].death_counter++; //when turtle turns to a shell
                    }                                 //this counter will start
                    else if(object[2][b].life==1 && object[2][b].move!=0){
                        object[2][b].death_counter=0; //if turtle is normal, nothing happens
                    }
                    if(object[2][b].death_counter>300){
                        object[2][b].death_counter=0; //after 5 secs, turtle is able to move again
                        object[2][b].life=2;
                        object[2][b].move=1;
                    }
                }
                //counters-------------------------------------------------------------------------------
                if(brick_counter>15){  //when small mario hits the brick
                    brick_counter=0;   //a slanted brick will be created
                    for(i=0;i<15;i++){
                        for(j=0;j<500;j++){
                            if(map[i][j]==14)
                                map[i][j]=3; //after few milli secs, it return normal
                        }
                    }                    
                    
                }
                //Counters for the Game objects, resets at 99
                if(sky_fire_ball_counter>=99){ //for the meteroid
                    sky_fire_ball_counter=0;
                }
                if(animation_ball_counter>=99){ //for the meteroid animation
                    animation_ball_counter=0;
                }                
                if(mushroom_counter>=99){ //for the mushroom
                    mushroom_counter=0;
                }
                if(flower_counter>=99){ //flower
                    flower_counter=0;
                }
                if(bullet_counter>=100){ //for the fire ball
                    bullet_counter=0;
                }                
                if(animation_coin_counter>=99){ //for the coin animation
                    animation_coin_counter=0;
                }
                if(animation_brick_counter>=96){ //for the brick animation
                    animation_brick_counter=0;
                }            
                if(cannon_spawn_counter>=96){ //for the cannons
                    cannon_spawn_counter=0;
                }
                if(global_score_counter >= 1000){ //for the score display
                    global_score_counter = 0;
                }
                //Seconds Counter-------------------------------------------------------------------
                if(goomba_sprite_counter>60){   //although it is called goomba sprite
                    goomba_sprite_counter=0;    //it actually is in charge of half of the sprite movement
                    if(level_change_animation==FALSE) //it can also act as a second counter
                        time--;               
                }
                if(mario_sprite_counter>80){ //Mario's sprite resetting
                    mario_sprite_counter=0;
                }
                Walking_counter = mario_sprite_counter/20;
                //Mario transform counter-------------------------------------------------------------
                if(mario_transform==TRUE){ //when mario is damaged, he become invulnerable for few 
                    mario_transform_counter++; 
                    if(mario_transform_counter>60){ //After a second, mario returns to normal state
                        mario_transform_counter=0;
                        mario_transform=FALSE;
                    }
                }
                //Game_Complete!--------------------------------------------------------------------
                if(Game_Complete==TRUE){
                    if(complete_song==TRUE){ //play the victory song
                        if(song_counter==0){
                            Play_Music(11);
                        }                        
                        song_counter++;
                        if(song_counter>510){
                            song_counter=0;
                            complete_song=FALSE;
                        }
                    }
                    else if(complete_song==FALSE && Mario.xPos<5850){ //after the song, mario starts to move
                        Play_Music(99);
                        Mario.move=3;
                        mario_facing=1;
                    }
                    if(Mario.xPos>5850 && song_counter==0){ //after he moves to the princess, mario will stop
                         Mario.move=0;
                         Play_Music(7); 
                         song_counter++;                         
                    }
                    else if(Mario.xPos>5850 && song_counter!=0){ //princess will talk to mario
                        animation[8][0].life=1;
                        animation[8][0].yPos=255;
                        animation[8][0].xPos=x_scroll+280;
                        if(key[KEY_X]){ // x key to eject the game loop  
                            Menu_Loop=1;
                            Paused=0;
                            Play_Music(99);
                        }
                    }
                }
                //Level Complete! ------------------------------------------------------------------
                if(level_change_animation==TRUE){ 
                    if(complete_song==TRUE){
                        if(song_counter==0){
                            flag_touch=TRUE;
                            Play_Music(9); //play the level  complete song
                        }                        
                        song_counter++;
                        if(song_counter>360){
                            song_counter=0;
                            complete_song=FALSE;
                        }
                    }
                    else if(complete_song==FALSE){
                        Play_Music(99); //after song is played, the music stops
                    }
                    if(Down_Collision==0 && flag_touch==TRUE){ //mario will fall down
                        score+=time*10;
                        time=0;
                        Mario.xPos=object[9][0].xPos+7-Mario.width;
                        mario_facing=1;
                        Mario.down=-1;
                        Mario.move=0;                        
                    }
                    else{                      
                        flag_touch=FALSE;
                    }
                    // the flag goes down
                    if((animation[9][0].yPos+34)<object[9][0].yPos+object[9][0].height){
                        animation[9][0].down=-3;                        
                    }
                    else{
                    //flag stops
                        flag_down=TRUE;
                        animation[9][0].down=0;
                        animation[9][0].yPos=object[9][0].yPos+object[9][0].height-34;
                    }
                    if(flag_touch==FALSE && flag_down==TRUE){
                    //Mario will move to the castle
                        mario_facing=1;
                        Mario.move=3;
                    }
                    if((Mario.xPos)>level[Game_level].castle_position+75){
                        //he disappears after he enters the castle
                        Mario.move=0;
                        if(complete_song==FALSE){ 
                            level_change=TRUE; //now level actually changes
                            Game_level++;
                        }
                    }                
                }
                //Death----------------------------------------------------------------------------
                if(Mario.life==0 && Death_Song==TRUE && Mario.death==FALSE){
                    Play_Music(0); //Plays the music
                    Death_Song=FALSE;
                    Mario.death=TRUE;
                }
                else if(Mario.life==0 && Death_Song==FALSE && Mario.death==FALSE){
                    Play_Music(99); //Stop playing the music after it is finished
                }
    //========================================================================================================================================
    //SPEED COUNTER ENDS HERE=================================================================================================================
    //========================================================================================================================================
            }
            
            //backgrounds
            //the large background
            blit(Panaroma_Background[Game_level], buffer, 0, 0, 0-(x_scroll/30), 0, 1200, 480);
            for(i=0;i<10;i++){
                //Small backgrounds such as clounds
                masked_blit(Panaroma[Game_level], buffer, 0, 0, background_scroll[i], 0, 1592, 480); 
            }  
            masked_blit(Small_Castle, buffer,  0, 0, level[Game_level].castle_position-x_scroll, 192, 192, 192);
            for(i=0;i<16;i++){
                for(j=x_scroll/32;j<(x_scroll/32+21);j++){                                                                         
                    if (map[i][j] == 1){      
                         //There are 5 types of layer 1(soil)           
                         soil_counter=j%5;          
                         masked_blit(In_Ground[Game_level][soil_counter], buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); 
                    }
                    if (map[i][j] == 2) {
                        //There are 3 types of layer 2(Grass) 
                        grass_counter=j%3;
                        masked_blit(Out_Ground[Game_level][grass_counter], buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); 
                    }
                    if (map[i][j] == 3) 
                        masked_blit(Brick, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //brick
                    if (map[i][j] == 4){  
                            //every 0.333 seconds, the colour of the ? box changes
                            if(goomba_sprite_counter/20==2){
                                masked_blit(Box_D,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            }
                            else if(goomba_sprite_counter/20==1){
                                masked_blit(Box_L,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            }
                            else{
                                masked_blit( Box_M,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            } 
                    }
                    if (map[i][j] == 5)     
                        masked_blit(Tube_Ext_Left, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Tube ext
                    if (map[i][j] == 6)     
                        masked_blit(Tube_Ext_Right, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Tube ext
                    if (map[i][j] == 7)     
                        masked_blit(Tube_Top_Left, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Top of Tube
                    if (map[i][j] == 8)     
                        masked_blit(Tube_Top_Right, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32);//Top of Tube
                    if (map[i][j] == 9)  
                        masked_blit(Block, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Block
                    if (map[i][j] == 10)  
                        masked_blit(Box_Hit, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Exmpty box
                    if (map[i][j] == 11)     
                        masked_blit(Pole, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Useless
                    if (map[i][j] == 12)     
                        masked_blit(Pole_Head, buffer, 0, 0, j*32-x_scroll, i*32, 32, 32); //Useless
                    if (map[i][j] == 13){                              
                            //same as block 4
                            if(goomba_sprite_counter/20==2){
                                masked_blit(Box_D,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            }
                            else if(goomba_sprite_counter/20==1){
                                masked_blit(Box_L,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            }
                            else{
                                masked_blit(Box_M,buffer,  0, 0, j*32-x_scroll, i*32, 32, 32);    
                            } 
                    }
                    if (map[i][j] == 14)     
                        masked_blit(Brick, buffer, 0, 0, j*32-x_scroll, i*32-5, 32, 32); //Brick that is hit by 
                }                                                                        //small Mario
            } 
                  
            //GAME OBJECTS----------------------------------------------------------------
            for(i=0;i<10;i++){
                for(j=0;j<100;j++){         
                    if(object[i][j].life>0){
                        if(i==3){
                            draw_sprite(buffer, Sky_Fire, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            //Falling meteroid                            
                        }                                                
                        if(i==8){ //Mushroom
                            draw_sprite(buffer, ShroomImage, object[i][j].xPos-x_scroll, object[i][j].yPos);        
                        }
                        if(i==1){ //Goomba, contains 2 walking sprites
                            if((goomba_sprite_counter/10)%2==0)
                                draw_sprite(buffer, Goomba_Left, object[i][j].xPos-x_scroll, object[i][j].yPos);   
                            else
                                draw_sprite(buffer, Goomba_Right, object[i][j].xPos-x_scroll, object[i][j].yPos);    
                        }
                        if(i==2){ //Turtle, contains 2 direct movement each with 2 walking sprite
                           if(object[i][j].life==2 && object[i][j].move>0 && (goomba_sprite_counter/10)%2==0){
                                 draw_sprite(buffer, Turtle_Right1, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            }           
                            else if(object[i][j].life==2 && object[i][j].move>0 && (goomba_sprite_counter/10)%2!=0){
                                 draw_sprite(buffer, Turtle_Right2, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            }
                            else if(object[i][j].life==2 && object[i][j].move<0 && (goomba_sprite_counter/10)%2==0){  
                                 draw_sprite(buffer, Turtle_Left1, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            }        
                            else if(object[i][j].life==2 && object[i][j].move<0 && (goomba_sprite_counter/10)%2!=0){
                                 draw_sprite(buffer, Turtle_Left2, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            }
                            if(object[i][j].life==1){
                                 draw_sprite(buffer, Turtle_Shell, object[i][j].xPos-x_scroll, object[i][j].yPos);
                            }                          
                        }
                        if(i==4){ //2 colours for the flower
                            if((goomba_sprite_counter/30)%2==0)
                                draw_sprite(buffer, FlowerImage_2, object[i][j].xPos-x_scroll, object[i][j].yPos);   
                            else
                                draw_sprite(buffer, FlowerImage_1, object[i][j].xPos-x_scroll, object[i][j].yPos);    
                        }   
                        if(i==5){ //3 Colours of the coin like the ? box
                            if(goomba_sprite_counter/20==2){
                                draw_sprite(buffer, Coin_D, object[i][j].xPos-x_scroll, object[i][j].yPos);    
                            }
                            else if(goomba_sprite_counter/20==1){
                                draw_sprite(buffer, Coin_L, object[i][j].xPos-x_scroll, object[i][j].yPos);    
                            }
                            else{
                                draw_sprite(buffer, Coin_M, object[i][j].xPos-x_scroll, object[i][j].yPos);    
                            }       
                        }   
                        if(i==7){ //The button to defeat Bowser
                            draw_sprite(buffer, Button, object[i][j].xPos-x_scroll, object[i][j].yPos);                            
                        }                            
                        if(i==0){ //Boswer sprites, 2 walking 1 jumping 1 falling
                            if(object[0][j].down==0 && (goomba_sprite_counter/10)%2==0)
                                draw_sprite(buffer, Broswer_Walk_1, object[i][j].xPos-x_scroll, object[i][j].yPos);   
                            else if (object[0][j].down==0 && (goomba_sprite_counter/10)%2!=0)
                                draw_sprite(buffer, Broswer_Walk_2, object[i][j].xPos-x_scroll, object[i][j].yPos);   
                            else if (object[0][j].down!=0 && object[i][j].yPos<=352)
                                draw_sprite(buffer, Broswer_Jump, object[i][j].xPos-x_scroll, object[i][j].yPos);             
                            else if (object[0][j].down!=0 && object[i][j].yPos>352)
                                draw_sprite(buffer, Broswer_Fall, object[i][j].xPos-x_scroll, object[i][j].yPos);          
                        }                          
                        if(i==9){ //the Pole
                            draw_sprite(buffer, Flag_Pole, object[i][j].xPos-x_scroll, object[i][j].yPos);                            
                        }                          
                        if(i==6){ //the cannon
                            draw_sprite(buffer, Cannon, object[i][j].xPos-x_scroll, object[i][j].yPos);                            
                        }          
                   }
                }
            }
            //Animations (they have no collision)
            for(i=0;i<12;i++){
                for(j=0;j<100;j++){
                    if(animation[i][j].life>0){
                        if(i==0){ //the coin that flies when you hit the ? box
                            draw_sprite(buffer, Box_Coin, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==1){ //the parts of brick that is falling when you hit the brick
                            draw_sprite(buffer, Broken_Brick, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==2){ //the goomba that get squashed
                            draw_sprite(buffer, Goomba_Squash, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==3){ //The falling goomba
                            draw_sprite(buffer, Goomba_Dead, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==4){ //the falling turtle
                            draw_sprite(buffer, Turtle_Dead, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==5){ //the explosion created by the fireball
                            draw_sprite(buffer, Explosion, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==6){ //the 1 UP notification
                            draw_sprite(buffer, UPONE, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==7){ //the princess is a picture instead of a game object
                            draw_sprite(buffer, Princess, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==8){ //the messege after you beat the game
                            draw_sprite(buffer, Messege, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==9){ //the flag that falling when you complete level
                            draw_sprite(buffer, Flag, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==10){ //the explosion created by the meteroid
                            draw_sprite(buffer, Boom, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                        if(i==11){ //the falling cannon ball
                            draw_sprite(buffer, Cannon, animation[i][j].xPos-x_scroll, animation[i][j].yPos);
                        }
                    }
               }
            }
            //the sprite for fireball
            //there are 4 different animations in 1 second
            for(j=0;j<100;j++){      
                if(bullet[j].life>0){
                    if(goomba_sprite_counter/15==1){
                         draw_sprite(buffer, Fire_ball1, bullet[j].xPos-x_scroll, bullet[j].yPos);   
                    }                                     
                    if(goomba_sprite_counter/15==2){
                         draw_sprite(buffer, Fire_ball2, bullet[j].xPos-x_scroll, bullet[j].yPos);   
                    }      
                    if(goomba_sprite_counter/15==3){
                         draw_sprite(buffer, Fire_ball3, bullet[j].xPos-x_scroll, bullet[j].yPos);   
                    }      
                    else{
                         draw_sprite(buffer, Fire_ball4, bullet[j].xPos-x_scroll, bullet[j].yPos);   
                    }                         
                }
            }      
            //Mario facing definitions
            //1=Right
            //0=Left
            if((Mario.xPos+Mario.width)<level[Game_level].castle_position+95){
                //if mario is hurt, he will flash
                if(mario_transform==TRUE && (goomba_sprite_counter/10)%2!=0){
                    draw_sprite(buffer, Flash_Mario , mario_scroll_position, Mario.yPos); 
                }
                else if(Mario.life==0) //if he is dead, he is dead
                    draw_sprite(buffer, Dead_Small_Mario , mario_scroll_position, Mario.yPos);   
                else if(Mario.life==1){ //small mario
                    if(mario_facing==1){ //facing right
                        if(Down_Collision==0)
                            //jumping
                            draw_sprite(buffer, Small_Jump_R, mario_scroll_position, Mario.yPos); 
                        else if(Mario.move>1 && Walking_counter==0)
                            //walking 1
                            draw_sprite(buffer, Small_Walk_R_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>1 && (Walking_counter==1 || Walking_counter==3))
                            //walking 2
                            draw_sprite(buffer, Small_Walk_R_2 , mario_scroll_position, Mario.yPos); 
                        else if(Mario.move<0)
                            //turning
                            draw_sprite(buffer, Small_Turn_R , mario_scroll_position, Mario.yPos);
                        else
                            //standing
                            draw_sprite(buffer, Small_Stand_R , mario_scroll_position, Mario.yPos); 
                    }
                    else if(mario_facing==0){ //facing left
                    //Same as above
                        if(Down_Collision==0) 
                           draw_sprite(buffer, Small_Jump_L, mario_scroll_position, Mario.yPos);   
                        else if(Mario.move<-1 && Walking_counter==0)
                            draw_sprite(buffer, Small_Walk_L_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move<-1 && (Walking_counter==1 || Walking_counter==3))
                            draw_sprite(buffer, Small_Walk_L_2 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>0) 
                            draw_sprite(buffer, Small_Turn_L , mario_scroll_position, Mario.yPos); 
                        else
                            draw_sprite(buffer, Small_Stand_L , mario_scroll_position, Mario.yPos); 
                    }   
                } 
                else if(Mario.life==2){ //Big Red Mario
                    if(mario_facing==1){ //Same as the small mario
                        if(DuckMode == TRUE)
                            draw_sprite(buffer, Large_Duck_R, mario_scroll_position, Mario.yPos);
                        else if(Down_Collision==0)
                            draw_sprite(buffer, Large_Jump_R, mario_scroll_position, Mario.yPos);                                       
                        else if(Mario.move>1 && Walking_counter==0)
                            draw_sprite(buffer, Large_Walk_R_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>1 && (Walking_counter==1 || Walking_counter==3))
                            draw_sprite(buffer, Large_Walk_R_2 , mario_scroll_position, Mario.yPos); 
                        else if(Mario.move<0) 
                            draw_sprite(buffer, Large_Turn_R , mario_scroll_position, Mario.yPos);
                        else 
                            draw_sprite(buffer, Large_Stand_R , mario_scroll_position, Mario.yPos); 
                    }
                    else if(mario_facing==0){ 
                        if(DuckMode == TRUE)
                            draw_sprite(buffer, Large_Duck_L, mario_scroll_position, Mario.yPos);
                        else if(Down_Collision==0)
                           draw_sprite(buffer, Large_Jump_L, mario_scroll_position, Mario.yPos);                        
                        else if(Mario.move<-1 && Walking_counter==0)
                            draw_sprite(buffer, Large_Walk_L_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move<-1 && (Walking_counter==1 || Walking_counter==3))
                            draw_sprite(buffer, Large_Walk_L_2 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>0) 
                            draw_sprite(buffer, Large_Turn_L , mario_scroll_position, Mario.yPos); 
 
                        else 
                            draw_sprite(buffer, Large_Stand_L , mario_scroll_position, Mario.yPos); 
                    }     
                }    
                else if(Mario.life==3){ //Fire mario
                    if(mario_facing==1){ //same as the small mario and big red mario
                        if(DuckMode == TRUE)
                            draw_sprite(buffer, Fire_Duck_R, mario_scroll_position, Mario.yPos);
                        else if(Down_Collision==0)
                            draw_sprite(buffer, Fire_Jump_R, mario_scroll_position, Mario.yPos); 
                        else if(Mario.move>1 && Walking_counter==0)
                            draw_sprite(buffer, Fire_Walk_R_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>1 && (Walking_counter==1 || Walking_counter==3))
                            draw_sprite(buffer, Fire_Walk_R_2 , mario_scroll_position, Mario.yPos); 
                        else if(Mario.move<0) 
                            draw_sprite(buffer, Fire_Turn_R , mario_scroll_position, Mario.yPos); 
 
                        else 
                            draw_sprite(buffer, Fire_Stand_R , mario_scroll_position, Mario.yPos);   
                    }
                    else if(mario_facing==0){ 
                        if(DuckMode == TRUE)
                            draw_sprite(buffer, Fire_Duck_L, mario_scroll_position, Mario.yPos);
                        else if(Down_Collision==0)
                           draw_sprite(buffer, Fire_Jump_L, mario_scroll_position, Mario.yPos);   
                        else if(Mario.move<-1 && Walking_counter==0)
                            draw_sprite(buffer, Fire_Walk_L_1 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move<-1 && (Walking_counter==1 || Walking_counter==3))
                            draw_sprite(buffer, Fire_Walk_L_2 , mario_scroll_position, Mario.yPos);   
                        else if(Mario.move>0) 
                            draw_sprite(buffer, Fire_Turn_L , mario_scroll_position, Mario.yPos); 
                        else 
                            draw_sprite(buffer, Fire_Stand_L , mario_scroll_position, Mario.yPos); 
                    }     
                }        
            }
            //score------------------------------------------------------------------------------------------------------------------------------------------------
            for(i=0;i<1000;i++)
            {
                if(Gamescore[i].life > 0)
                {
                    alfont_textprintf_aa_ex(buffer, arial2, Gamescore[i].xPos-x_scroll+2, Gamescore[i].yPos, makecol(0,0,0), -1, "%d",Gamescore[i].score);
                    alfont_textprintf_aa_ex(buffer, arial2, Gamescore[i].xPos-x_scroll, Gamescore[i].yPos, makecol(255,255,255), -1, "%d",Gamescore[i].score);
                }
            }
            //Game information display
            alfont_textprintf_aa_ex(buffer, arial, 15, 10, makecol(255,255,0),   -1, "Score"); 
            alfont_textprintf_aa_ex(buffer, arial, 400, 10, makecol(255,255,0),   -1, "World"); 
            alfont_textprintf_aa_ex(buffer, arial, 565, 10, makecol(255,255,0),   -1, "Time");  
            alfont_textprintf_aa_ex(buffer, arial, 10, 30, makecol(255,255,255), -1, "%6d",score);  
            alfont_textprintf_aa_ex(buffer, arial, 255, 30, makecol(255,255,255), -1, "%2d",Gold_Coin);  
            alfont_textprintf_aa_ex(buffer, arial, 420, 30, makecol(255,255,255), -1, "%2d",Game_level);  
            alfont_textprintf_aa_ex(buffer, arial, 575, 30, makecol(255,255,255), -1, "%3d",time);  
            //Coin symbol changes every of few seconds 
            if((goomba_sprite_counter/10)%2==0)
                draw_sprite(buffer, Coin_Multi_L, 230,33);
            else
                draw_sprite(buffer, Coin_Multi_D, 230,33);
                
            blit(buffer, screen, 0,0,0,0,640,480); 
            clear_bitmap(buffer);
        }   
    }
    //Destroy the BITMAP
    //Mario-----------------------------------------------
    //Fire------------------------
    destroy_bitmap (Fire_Stand_R);
    destroy_bitmap (Fire_Walk_R_1);
    destroy_bitmap (Fire_Walk_R_2);
    destroy_bitmap (Fire_Jump_R);
    destroy_bitmap (Fire_Turn_R);
    destroy_bitmap (Fire_Stand_L);
    destroy_bitmap (Fire_Walk_L_1);
    destroy_bitmap (Fire_Walk_L_2);
    destroy_bitmap (Fire_Jump_L);
    destroy_bitmap (Fire_Turn_L);
    //Large ----------------------
    destroy_bitmap (Large_Stand_R);
    destroy_bitmap (Large_Walk_R_1);
    destroy_bitmap (Large_Walk_R_2);
    destroy_bitmap (Large_Jump_R);
    destroy_bitmap (Large_Turn_R);
    destroy_bitmap (Large_Stand_L);
    destroy_bitmap (Large_Walk_L_1);
    destroy_bitmap (Large_Walk_L_2);
    destroy_bitmap (Large_Jump_L);
    destroy_bitmap (Large_Turn_L);
   //Small-----------------------
    destroy_bitmap (Small_Stand_R);
    destroy_bitmap (Small_Walk_R_1);
    destroy_bitmap (Small_Walk_R_2);
    destroy_bitmap (Small_Jump_R);
    destroy_bitmap (Small_Turn_R);
    destroy_bitmap (Small_Stand_L);
    destroy_bitmap (Small_Walk_L_1);
    destroy_bitmap (Small_Walk_L_2);
    destroy_bitmap (Small_Jump_L);
    destroy_bitmap (Small_Turn_L);
    //Dead-------------------------
    destroy_bitmap (Dead_Small_Mario);
    destroy_bitmap (Flash_Mario);
    //Goomba----------------------------------------------
    destroy_bitmap (GoombaImage);
    destroy_bitmap (Goomba_Right);
    destroy_bitmap (Goomba_Left);
    //Turtle----------------------------------------------
    destroy_bitmap (TurtleImage);
    destroy_bitmap (Turtle_Left1);
    destroy_bitmap (Turtle_Left2);
    destroy_bitmap (Turtle_Right1);
    destroy_bitmap (Turtle_Right2);
    destroy_bitmap (Turtle_Shell);
    //Button---------------------------------------------
    destroy_bitmap (Button);
    //Cannon---------------------------------------------
    destroy_bitmap (Cannon);
    //Broswer--------------------------------------------
    destroy_bitmap (Broswer_Fall);
    destroy_bitmap (Broswer_Walk_2);
    destroy_bitmap (Broswer_Walk_1);
    destroy_bitmap (Broswer_Jump);
    //Princess--------------------------------------------
    destroy_bitmap (Princess);
    //Coin Multiplier------------------------------------
    destroy_bitmap (Coin_Multi_L);
    destroy_bitmap (Coin_Multi_D);
    //Coin Image-----------------------------------------
    destroy_bitmap (CoinImage);
    destroy_bitmap (Coin_D);
    destroy_bitmap (Coin_L);
    destroy_bitmap (Coin_M);
    //Power Ups-------------------------------------------
    destroy_bitmap (ShroomImage);
    destroy_bitmap (FlowerImage_1);
    destroy_bitmap (FlowerImage_2);
    //Flag Pole----------------------------------------------------
    destroy_bitmap (Flag_Pole);
    destroy_bitmap (Flag);
    //Animations------------------------------------------
    destroy_bitmap (Box_Coin);
    destroy_bitmap (Broken_Brick);
    destroy_bitmap (Goomba_Squash);
    destroy_bitmap (Goomba_Dead);
    destroy_bitmap (Turtle_Dead);
    destroy_bitmap (Explosion);
    destroy_bitmap (UPONE);
    destroy_bitmap (Messege);
    //Fire Ball-----------------------------------------------------
    destroy_bitmap (Fire_ball1);
    destroy_bitmap (Fire_ball2);
    destroy_bitmap (Fire_ball3);
    destroy_bitmap (Fire_ball4);
    //Falling Fire Ball-----------------------------------------------
    destroy_bitmap (Sky_Fire);
    destroy_bitmap (Boom );
    //Game Tiles------------------------------------------
    destroy_bitmap (Small_Castle);
    destroy_bitmap (Brick);
    destroy_bitmap (Night_Brick);
    destroy_bitmap (Brick_Up);
    destroy_bitmap (Tube_Ext_Left);
    destroy_bitmap (Tube_Ext_Right);
    destroy_bitmap (Tube_Top_Left);
    destroy_bitmap (Tube_Top_Right);
    destroy_bitmap (Box);
    destroy_bitmap (Box_D);
    destroy_bitmap (Box_M);
    destroy_bitmap (Box_L);
    destroy_bitmap (Box_Hit);
    destroy_bitmap (Block);
    destroy_bitmap (Night_Block);
    destroy_bitmap (Pole);
    destroy_bitmap (Pole_Head);
    return 0;

}
END_OF_MAIN()

//the collision of the game
bool collide(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
    //the code is taken from allegro tutorial and made into a function
    int collision = FALSE;
    int image1_bb_left = x1;
    int image1_bb_top = y1;
    int image1_bb_right = (image1_bb_left + w1);
    int image1_bb_bottom = (image1_bb_top + h1);
    int image2_bb_left = x2;
    int image2_bb_top = y2;
    int image2_bb_right = (image2_bb_left + w2);
    int image2_bb_bottom = (image2_bb_top + h2);
    collision = TRUE;
    if(image1_bb_bottom < image2_bb_top)
    {
        collision = FALSE;
    }
    else if(image1_bb_top > image2_bb_bottom)
    {
        collision = FALSE;
        
    }
    else if(image1_bb_right < image2_bb_left)
    {
        collision = FALSE;
    }
    else if(image1_bb_left > image2_bb_right)
    {
        collision = FALSE;
    }
    return(collision); //determines if it collides or not
}
END_OF_FUNCTION(collide)

//function used to load maps
void Load_Map(int Level){
//same as the textfile program used to load matrix
    FILE *fptr;
    char sFile[10];
    int b,c;
    sprintf(sFile,"data/Level %d.txt",Level); //loads the map base on the level
    fptr = fopen(sFile, "r");  
    if (fptr == NULL){ //Error checking                     
        set_gfx_mode(GFX_TEXT,0,0,0,0);
        allegro_message("Could not Open Level!");
        exit(EXIT_FAILURE);
    }   
    else{ //read from file
        for(b=0;b<15;b++){
            for(c=0;c<500;c++){
                fscanf(fptr,"%d",&map[b][c]);
            }
        }
        fclose(fptr);
    }    
}
END_OF_FUNCTION(Load_Map)

//The title screen
void Menu_Screen(){
     SAMPLE *MenuSong = load_wav("music/Title Theme.wav"); //the title music
     BITMAP *buffer2 = create_bitmap(640,480); //the buffer
     BITMAP *Border = load_bitmap("img/Main Menu.bmp", NULL);
     BITMAP *Scroll = load_bitmap("img/Main Menu scroll.bmp", NULL);
     BITMAP *Title = load_bitmap("img/MenuTitle.bmp", NULL);
     BITMAP *Enter_Key = load_bitmap("img/Main Menu Enter.bmp", NULL);
     BITMAP *Option = load_bitmap("img/MenuOption.bmp", NULL);
     BITMAP *Select = load_bitmap("img/Selected.bmp", NULL);
     BITMAP *Instruction_Pic = load_bitmap("img/Instruction Menu.bmp", NULL);
     int map_scroll=0; //background map scroll
     int scroll_mode=0; //the direction
     int title_y=-167; //the position of title's y axis
     int start=FALSE; 
     int start_counter=FALSE;
     int Hit_twice=FALSE; //force user to hit twice to start the game
     int Loop=0; //the mean while loop
     int second_counter=0; //counts the amount of time passed
     int y_move=260; //movement of the background scroll
     int y_speed=1;
     int Selected=1; //Menu select
     int cd_counter=30; //
     int keynotup=FALSE; //prevent holding input
     int keynotdown=FALSE; //same as ^
     int instruction=FALSE;
     
     play_sample(MenuSong, 255, 128, 1000, 1); // plays the song when menu starts
     while(Loop==0){
         while(speed_counter > 0)
         {         
             if(key[KEY_ENTER] && Hit_twice==FALSE){
                 start=TRUE; //press enter to continue thing at the menu
             }
             else if(key[KEY_ENTER] && Hit_twice==TRUE && Selected==1 && cd_counter==30){
                 if(instruction==TRUE)
                     instruction=FALSE; //display instruction
                 else
                     instruction=TRUE;
                 cd_counter=0; //cool down restarts
             }             
             else if(key[KEY_ENTER] && Hit_twice==TRUE && Selected==2 && cd_counter==30){
                 Loop=1;     //exit the menu(starts the game)
             }
             else if(key[KEY_ENTER] && Hit_twice==TRUE && Selected==3 && cd_counter==30){
                 exit(EXIT_FAILURE); //exit the game
             }             
             speed_counter --;
             cd_counter++;//cool down counter
             if(cd_counter>30){
                 cd_counter=30; //if cd_counter=30, you can select!
             }
             if(start==TRUE)
                 start_counter++; //prevent user click enter when game starts and 2 thing comes out at once
             if(start_counter>20){     
                 start_counter=0;  
                 Hit_twice=TRUE; //when hit_twice is true, you can select options in the menu
             }
             //a counter used for the menu spriting
            second_counter++;
            if(second_counter>60){
                second_counter=0; 
            }                 
                        
             //Movements-----------------
             if(second_counter%4==0) 
                 y_move+=y_speed; //the movement of the background map  
             //y move and y speed are for press enter key to continue          
             if(y_move>270){
                 y_speed=-1; //changes the direction 
             }
             else if(y_move<250){
                 y_speed=1; //change the direction
             }                 
             if(scroll_mode==0) //if it is going front
                 map_scroll--; //background scroll
             else
                 map_scroll++; //same as above
             
             if(title_y<45)
                 title_y+=2; //
             if(map_scroll<-4000) //changes the direction upon the end of the background
                 scroll_mode=1;
             else if(map_scroll>=0) //same as above
                 scroll_mode=0;
             
             //Inputs----------------------   
             //Option selection
             if(key[KEY_UP] && keynotup==FALSE && instruction==FALSE){
                 keynotup=TRUE; 
                 Selected--;
                 if(Selected<1){ //allow the player to loop from top to bottom
                     Selected=3;
                 }
             }     
             if(key[KEY_DOWN] && keynotdown==FALSE && instruction==FALSE){
                 keynotdown=TRUE;
                 Selected++;
                 if(Selected>3){ //allow the player to loop from bottom to top
                     Selected=1;
                 }
             } 
             //---------------------------
             if(key[KEY_F1]) //windowed mode
                 set_gfx_mode(GFX_AUTODETECT_WINDOWED, scrx, scry,0,0);
             if(key[KEY_F2]) //full screen mode
                 set_gfx_mode(GFX_AUTODETECT, scrx, scry,0,0);                    
                 
             if(!key[KEY_UP]) //disables holding keys
                 keynotup=FALSE;
             if(!key[KEY_DOWN])
                 keynotdown=FALSE;
         }
         blit(Scroll, buffer2, 0,0,map_scroll,0,4775,480);      
         masked_blit(Title, buffer2, 0, 0, 55, title_y, 544, 167);      
         if(start==TRUE){ //Press enter to continue
              masked_blit(Option, buffer2, 0, 0, 220, 240, 231, 115);   
         }
         if(start==FALSE){ //Game Menu
             masked_blit(Enter_Key, buffer2, 0, 0, 220, y_move, 231, 115);      
         } 
         if(Selected==1 && start==TRUE) //mushrooms that indicate which option is selected
             masked_blit(Select, buffer2, 0, 0, 200, 245, 26, 26);
         else if(Selected==2 && start==TRUE)
             masked_blit(Select, buffer2, 0, 0, 200, 285, 26, 26);
         else if(Selected==3 && start==TRUE)
             masked_blit(Select, buffer2, 0, 0, 200, 320, 26, 26);
         masked_blit(Border, buffer2, 0, 0, 0, 0, 640, 480);
         
         //display instruction menu
         if(instruction==TRUE)
             masked_blit(Instruction_Pic, buffer2, 0, 0, 180, 220, 320, 190);  
         blit(buffer2, screen, 0,0,0,0,640,480); 
         clear_bitmap(buffer2);
     }
     destroy_sample(MenuSong); //stops music from playing
     destroy_bitmap (buffer2); //the buffer
     destroy_bitmap (Border); 
     destroy_bitmap (Scroll);
     destroy_bitmap (Title);
     destroy_bitmap (Enter_Key);
     destroy_bitmap (Option);
     destroy_bitmap (Select);
     destroy_bitmap (Instruction_Pic);   
}
END_OF_FUNCTION(Menu_Screen)

void Paused_Screen(int *mode,BITMAP *buf){ //the paused menu
                               //Baiscly the same as the menu 
     BITMAP *buffer5  = create_bitmap(320,190);
     BITMAP *BackGround = load_bitmap("img/Pause Menu.bmp", NULL);
     BITMAP *Select = load_bitmap("img/Selected.bmp", NULL);
     BITMAP *Instruction_Pic = load_bitmap("img/Instruction Menu.bmp", NULL);
     int Selected=1;
     int keynotup=FALSE;
     int keynotdown=FALSE;
     int Loop=0;
     int cd_counter = 0;
     int instruction=FALSE;
     Play_Music(98); //pause the game music
     Sound_Effect(12); //sound effect when game is paused
     while(Loop==0){
         while(speed_counter > 0)
         {         
             if(key[KEY_ENTER] && Selected==1 && cd_counter==30){ //when you press resume
                 Loop=1;
                 *mode=1;
                 Play_Music(97);
                 Sound_Effect(12);
             }
             else if(key[KEY_ENTER] && Selected==2  && cd_counter==30){ //display instruction
                 if(instruction==TRUE)
                     instruction=FALSE;
                 else
                     instruction=TRUE;
                 cd_counter=0;
             }      
             else if(key[KEY_ENTER] && Selected==3  && cd_counter==30){ //exit the game to the menu
                 Loop=1;
                 *mode=0;
                 Play_Music(99);
             }                 
             speed_counter --;        
             cd_counter++; //0.5 second cooldown
             if(cd_counter>30){
                 cd_counter=30;
             }
             //Inputs----------------------   
             if(key[KEY_UP] && keynotup==FALSE  && instruction==FALSE){
                 keynotup=TRUE;
                 Selected--;
                 if(Selected<1){
                     Selected=3; //changing which one is selected
                 }
             }     
             if(key[KEY_DOWN] && keynotdown==FALSE  && instruction==FALSE){
                 keynotdown=TRUE;
                 Selected++;
                 if(Selected>3){
                     Selected=1;
                 }
             }               
             if(!key[KEY_UP]) //prevent user from holding the key
                 keynotup=FALSE;
             if(!key[KEY_DOWN])
                 keynotdown=FALSE;
         }    
         //print to buffer
         masked_blit(BackGround, buffer5,  0, 0, 0, 0, 320, 190); 
         if(Selected==1) //print the selected mushroom
             masked_blit(Select, buffer5, 0, 0, 50, 65, 26, 26);
         else if(Selected==2)
             masked_blit(Select, buffer5, 0, 0, 50, 108, 26, 26);
         else if(Selected==3)
             masked_blit(Select, buffer5, 0, 0, 50, 149, 26, 26);
             
         
         if(instruction==TRUE){
             masked_blit(Instruction_Pic, buffer5, 0, 0, 0, 0, 320, 190);   
         }
         blit(buffer5, screen, 0,0,160, 100,320,190);          
         clear_bitmap(buffer5);
     }

}
END_OF_FUNCTION(Paused_Screen)

//In game display before the game starts
void Display_Info(int life, int coin, int score, int level){
     Play_Music(99); //stop music
     BITMAP *buffer3=create_bitmap(640,480);
     BITMAP *Mario=load_bitmap("img/Mario.bmp", NULL);
     BITMAP *Coin_X = load_bitmap("img/Multiplier L.bmp", NULL);
     ALFONT_FONT *arial = alfont_load_font("C:/Windows/Fonts/arial.ttf"); 
     alfont_set_font_size(arial, 30); 
     int Loop=0;
     int counter=0;

     while(Loop==0){
         while(speed_counter > 0)
         {         
                counter++;
                speed_counter --;    
                if(counter>120){
                    Loop=1; //the function will exit automaticly after 2 seconds
                }                          
         }
         //print to screen
        draw_sprite(buffer3, Mario, 275, 240); 
        alfont_textprintf_aa_ex(buffer3, arial, 250, 195, makecol(255,255,255),   -1, "WORLD %2d",level); 
        alfont_textprintf_aa_ex(buffer3, arial, 310, 240, makecol(255,255,255),   -1, "X");
        alfont_textprintf_aa_ex(buffer3, arial, 340, 240, makecol(255,255,255),   -1, "%d",life); 
        alfont_textprintf_aa_ex(buffer3, arial, 15, 10, makecol(255,255,0),   -1, "Score"); 
        alfont_textprintf_aa_ex(buffer3, arial, 400, 10, makecol(255,255,0),   -1, "World"); 
        alfont_textprintf_aa_ex(buffer3, arial, 570, 10, makecol(255,255,0),   -1, "Time");  
        alfont_textprintf_aa_ex(buffer3, arial, 10, 30, makecol(255,255,255), -1, "%6d",score);  
        alfont_textprintf_aa_ex(buffer3, arial, 255, 30, makecol(255,255,255), -1, "%2d",coin);  
        alfont_textprintf_aa_ex(buffer3, arial, 420, 30, makecol(255,255,255), -1, "%2d",level);     
        draw_sprite(buffer3, Coin_X, 230,33);              
        blit(buffer3, screen, 0,0,0,0,640,480); 
        clear_bitmap(buffer3);
     }       
}
END_OF_FUNCTION(Menu_Screen)

//Game over display
void Display_GamerOver(int coin, int score, int level){
    ALFONT_FONT *arial = alfont_load_font("C:/Windows/Fonts/arial.ttf");
    alfont_set_font_size(arial, 30);
    BITMAP *buffer4=create_bitmap(640,480);
    BITMAP *Coin_X = load_bitmap("img/Multiplier L.bmp", NULL);
    int Loop=0;
    int counter=0;
    Play_Music(10);//plays the game over music
    while(Loop==0){
        while(speed_counter > 0)
        {
        counter++;
        speed_counter --;
        if(counter>330){
            Loop=1; //exit after 5.5 seconds (the length of the song)
        }
    }
         //display information
        alfont_textprintf_aa_ex(buffer4, arial, 15, 10, makecol(255,255,0),   -1, "Score");
        alfont_textprintf_aa_ex(buffer4, arial, 400, 10, makecol(255,255,0),   -1, "World");
        alfont_textprintf_aa_ex(buffer4, arial, 565, 10, makecol(255,255,0),   -1, "Time");
        alfont_textprintf_aa_ex(buffer4, arial, 5, 30, makecol(255,255,255), -1, "%6d",score);
        alfont_textprintf_aa_ex(buffer4, arial, 255, 30, makecol(255,255,255), -1, "%2d",coin);
        alfont_textprintf_aa_ex(buffer4, arial, 420, 30, makecol(255,255,255), -1, "%2d",level);
        alfont_textprintf_aa_ex(buffer4, arial, 245, 195, makecol(255,255,0),   -1, "GAME OVER!");
        draw_sprite(buffer4, Coin_X, 230,33);
        blit(buffer4, screen, 0,0,0,0,640,480);
        clear_bitmap(buffer4);
     }   
     Play_Music(99); //stops music from playing
}
END_OF_FUNCTION(Menu_Screen)

//function used to be called in other functions to play music
//for midi
int Play_Music(int song){
    MIDI *music;    
    switch (song){ 
        case 0: music = load_midi("music/Death.mid");//death
                break;
        case 1: music = load_midi("music/Overworld (Remix).mid"); //level 1
                break;
        case 2: music = load_midi("music/Overworld.mid"); //level 2
                break;
        case 3: music = load_midi("music/Underground.mid"); //level 3
                break;
        case 4: music = load_midi("music/Level 4.mid"); //level 4
                break;
        case 5: music = load_midi("music/Melty Molten Galaxy.mid"); //level 5
                break;
        case 6: music = load_midi("music/Castle.mid"); //level 6
                break;
        case 7: music = load_midi("music/Ending.mid"); //ending song
                break;
        case 8: music = load_midi("music/Boss Attack!.mid"); //bowser appears
                break;
        case 9: music = load_midi("music/Level Complete.mid"); //level complete
                break;               
        case 10: music = load_midi("music/Game Over.mid"); //game over
                break;
        case 11: music = load_midi("music/Castle Complete.mid"); //game complete
                break;
        case 97: midi_resume(); 
                break;
        case 98: midi_pause();
                break;
        case 99: music = NULL; //stops music
                break;    
    }
    if(song==98 || song==97){}
    else if(song!=0)
        play_looped_midi(music, 1,-1);
    else
       play_midi(music, 1);
}
END_OF_FUNCTION(Play_Music)

//in game sound effects
//wav file can be find on internet much easier than midi
int Sound_Effect(int type){
    SAMPLE *sound;
    switch (type){ 
        case 0: sound =  load_wav("music/Can't Brick.wav");//Coin
                break;        
        case 1: sound =  load_wav("music/coin.wav");//Coin
                break;
        case 2: sound =  load_wav("music/Shell Kick.wav");//Shell Kick
                break;
        case 3: sound =  load_wav("music/Enemy Stomp.wav");//Jumping on Enemy
                break;
        case 4: sound =  load_wav("music/Mario Jump 1.wav");//Small Mario Jump
                break;
        case 5: sound =  load_wav("music/Mario Jump 2.wav");//Big Mario Jump
                break;        
        case 6: sound =  load_wav("music/Power Up.wav");//Power Up
                break;
        case 7: sound =  load_wav("music/Mario Hurt.wav");//Mario got Hurt!
                break;
        case 8: sound =  load_wav("music/Mushroom Sound.wav");//Mushroom Spawn
                break; 
        case 9: sound =  load_wav("music/Brick Break.wav");//Brick Break
                break;                     
        case 10: sound =  load_wav("music/FireballThrow.wav");//Brick Break
                break; 
        case 11: sound =  load_wav("music/1UP.wav");//Brick Break
                break; 
        case 12: sound =  load_wav("music/Pause Sound Effect.wav");//Brick Break
                break; 
    }  
    play_sample(sound, 255, 128, 1000, 0);

}
END_OF_FUNCTION(Sound_Effect)

//--------------------------------------------------------------


int Scoredisplayer(struct scoredisplay *Gamescore,int *i, int x, int y, int score)
{
    Gamescore[*i].life = 1;
    Gamescore[*i].xPos = x;
    Gamescore[*i].yPos = y;
    Gamescore[*i].score = score;
    Gamescore[*i].time_counter = 60;
    *i++;
    return 0;
}
END_OF_FUNCTION(Scoredisplayer)

