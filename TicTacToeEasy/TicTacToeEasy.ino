    ///////////////////////////////////////
   //    VBB BIG-BROWSER-BITS           //
  //        TIC TAC TOE                //
 // http://www.virtualbreadboard.com  //
///////////////////////////////////////

// ** CREDITS **

//Based on the tic-tac-toe project for LCD
//Replaces TouchScreen and LCD display with same game logic

   //////////////////////////////////////////////
  //     2.8" TOUCH SCREEN TIC TAC TOE        //
 //                                          //
//           http://www.educ8s.tv           //
/////////////////////////////////////////////


///VBB Asset Registry
const long VBB_CLEAR_ALL = 0;

 
const int VBB_X0 = 0;
const int VBB_X1 = 1;
const int VBB_X2 = 2;
const int VBB_X3 = 3;
const int VBB_X4 = 4;
const int VBB_X5 = 5;
const int VBB_X6 = 6;
const int VBB_X7 = 7;
const int VBB_X8 = 8;

const int VBB_O0 = 9;
const int VBB_O1 = 10;
const int VBB_O2 = 11;
const int VBB_O3 = 12;
const int VBB_O4 = 13;
const int VBB_O5 = 14;
const int VBB_O6 = 15;
const int VBB_O7 = 16;
const int VBB_O8 = 17;

const int VBB_GAME_OVER   = 18;
const int VBB_DRAW        = 19;
const int VBB_CPU_WINS    = 20;
const int VBB_HUMAN_WINS  = 21;
const int VBB_PLAY_AGAIN  = 22;
const int VBB_YOUR_MOVE   = 23;
 

const int PIN_CLK = 13;
const int PIN_DAT = 11;
const int PIN_CS  = 10;

const int TILE_START  = 9;


long vbbAssetRegister;

void vbbClearAll(){
    vbbAssetRegister = 0;
    vbbSync();
}
void vbbShow(int asset)
{
    long bitmask = 1;

    bitmask = bitmask << asset;
    
    vbbAssetRegister |= bitmask;

    vbbSync();
}

void vbbHide(int assets){

    long bitmask = 1;

    bitmask = bitmask << assets;
    
    vbbAssetRegister |= bitmask;
    vbbAssetRegister ^= bitmask;  

    vbbSync();
}
 
void vbbInit(){
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_DAT, OUTPUT);
    pinMode(PIN_CS, OUTPUT);

    digitalWrite(PIN_CLK, LOW);
    digitalWrite(PIN_CS, HIGH);
  
    vbbAssetRegister = 0;
 
}

void vbbSync(){
    digitalWrite(PIN_CS, LOW);

    shiftOut(PIN_DAT,PIN_CLK, MSBFIRST, vbbAssetRegister );
    shiftOut(PIN_DAT,PIN_CLK, MSBFIRST, vbbAssetRegister>>8);
    shiftOut(PIN_DAT,PIN_CLK, MSBFIRST, vbbAssetRegister>>16);
  
    digitalWrite(PIN_CS, HIGH);
}

/*
    VBB is injecting the tile value onto the ADC 
    10 buttons 
    0 -- 9 buttons for the tiles
    10 for the start button
    16 is used as the divisor so 64 range
    1/2 bit variance.
    Hence button0 maps to 32 : 96
 */
 
int tileTouched(){

  int tile =analogRead(A0);

  int ladder = 1024 / 11;

  if( tile < ( ladder / 2) )
  {
      
      return -1;
  }else{
      int tt = ( tile - ladder/2) / ladder;
   Serial.print("Tile = ");
    Serial.print(tt);

       Serial.print(" Calc = ");
      Serial.println(tile);
        
      return tt;
  }
   
}

///////////////////////////////////////////////
 
int gameScreen = 1;
int moves = 1;

int winner = 0;  //0 = Draw, 1 = Human, 2 = CPU

 
int board[]={0,0,0,0,0,0,0,0,0};// holds position data 0 is blank, 1 human, 2 is computer

void setup() {
  Serial.begin(9600);
  Serial.print("Starting...");
  randomSeed(analogRead(1));
  
  vbbInit() ;
  
  vbbShow(VBB_PLAY_AGAIN);
  

}

void resetGame()
{
 
  for(int i=0;i<9;i++)
  {
   board[i]=0;
  }
  moves = 1;
  winner = 0;
  gameScreen=2;
}


void loop() 
{
  
   int tile = tileTouched();
    
   if(tile == TILE_START)// The user has pressed inside the red rectangle
   {
      vbbClearAll();
      
      Serial.println("Start Pressed");
      resetGame();  
      playGame();
   
   } 
    delay(10);  
   
}


 

void animate(int asset){
    vbbShow(asset);
    delay(1000);
    vbbHide(asset);
    delay(100);
}
 

void drawGameOverScreen()
{
  animate( VBB_GAME_OVER);
  
  if(winner == 0) animate( VBB_DRAW); 
  if(winner == 1) animate(VBB_HUMAN_WINS );
  if(winner == 2) animate(VBB_CPU_WINS );

  vbbShow(VBB_PLAY_AGAIN);
   
}

 

void playGame()
{
  do
  {
    if(moves%2==1)
    {
     arduinoMove();
     printBoard();
     checkWinner();
    }else
    {
      playerMove(); 
      printBoard();
      checkWinner();  
    }
    moves++;
  }
  while (winner==0 && moves<10); 
  if(winner == 1)
  {
    Serial.println("HUMAN WINS");
    delay(1000);
    gameScreen=3;
    drawGameOverScreen();
  }else if(winner ==2)
  {
     Serial.println("CPU WINS");
     delay(1000);
     gameScreen=3;
     drawGameOverScreen();
  }else
  {
    Serial.println("DRAW");
    delay(1000);
    gameScreen=3;
    drawGameOverScreen();
  }
  
}


void playerMove()
{

  Serial.print("\nPlayer Move:");
  vbbShow(VBB_YOUR_MOVE);
  while(true)
  {    
    int tile = tileTouched();
     
    if (tile >= 0)
    {
      if(board[tile]==0){
         Serial.print("Player Moved: ");
         vbbHide(VBB_YOUR_MOVE);
         Serial.print(tile);
         board[tile]=1;
         drawPlayerMove(tile);  
         Serial.println("Drawing player move");
         break;
      }
       
    }
   } 
}

void printBoard()
{
  int i=0;
  Serial.println("Board: [");
  for(i=0;i<9;i++)
  {
    Serial.print(board[i]);
    Serial.print(",");
  }
  Serial.print("]");
}

int checkOpponent()
{
  if(board[0]==1 && board[1]==1 && board[2]==0)
  return 2;
  else if(board[0]==1 && board[1]==0 && board[2]==1)
  return 1;
  else if (board[1]==1 && board [2]==1 && board[0]==0)
  return 0;
  else if (board[3]==1 && board[4]==1 && board[5]==0)
  return 5;
  else if (board[4]==1 && board[5]==1&& board[3]==0)
  return 3;
  else if (board[3]==1 && board[4]==0&& board[5]==1)
  return 4;
  else if (board[1]==0 && board[4]==1&& board[7]==1)
  return 1;
  else
  return 100;
}

void arduinoMove()
{
  int b = 0;
  int counter =0;
  int movesPlayed = 0;
  Serial.print("\nArduino Move:");

  int firstMoves[]={0,2,6,8}; // will use these positions first

  for(counter=0;counter<4;counter++) //Count first moves played
  {
    if(board[firstMoves[counter]]!=0) // First move is played by someone
    {
      movesPlayed++;
    }
  }  
  do{
    if(moves<=2)
    {
      int randomMove =random(4); 
      int c=firstMoves[randomMove];
      
      if (board[c]==0)
      {  
        delay(1000);
        board[c]=2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b=1;
      }   
        
    }else
    {
    int nextMove = checkOpponent();
    if(nextMove == 100)
    {  
    if(movesPlayed == 4) //All first moves are played
    {
      int randomMove =random(9); 
      if (board[randomMove]==0)
      {  
        delay(1000);
        board[randomMove]=2;
        Serial.print(randomMove);
        Serial.println();
        drawCpuMove(randomMove);
        b=1;
      }   
    }else
    {
      int randomMove =random(4); 
      int c=firstMoves[randomMove];
      
      if (board[c]==0)
      {  
        delay(1000);
        board[c]=2;
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b=1;
      }   
  }
    }else
    {
       delay(1000);
       board[nextMove]=2;
       drawCpuMove(nextMove);
       b=1;
    }
  }
  }
  while (b<1);
}

 
 
void drawCpuMove(int move)
{
    vbbShow( VBB_O0 + move  ); 
}

void drawPlayerMove(int move)
{
   vbbShow( VBB_X0 + move  );
}

void checkWinner() 
// checks board to see if there is a winner
// places result in the global variable 'winner'
{
  int qq=0;
  // noughts win?
  if (board[0]==1 && board[1]==1 && board[2]==1) { 
    winner=1; 
  }
  if (board[3]==1 && board[4]==1 && board[5]==1) { 
    winner=1; 
  }
  if (board[6]==1 && board[7]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[3]==1 && board[6]==1) { 
    winner=1; 
  }
  if (board[1]==1 && board[4]==1 && board[7]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[5]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[4]==1 && board[8]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[4]==1 && board[6]==1) { 
    winner=1; 
  }
  // crosses win?
  if (board[0]==2 && board[1]==2 && board[2]==2) { 
    winner=2; 
  }
  if (board[3]==2 && board[4]==2 && board[5]==2) { 
    winner=2; 
  }
  if (board[6]==2 && board[7]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[3]==2 && board[6]==2) { 
    winner=2; 
  }
  if (board[1]==2 && board[4]==2 && board[7]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[5]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[4]==2 && board[8]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[4]==2 && board[6]==2) { 
    winner=2; 
  }
 
}
 
