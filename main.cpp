#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = {137 , 204 , 96 , 255};
Color darkgreen = {34 , 51 , 24 , 255};

int cellsize = 30;
int cellcount = 25 ;
int offset = 75;

double lastUpdaTetime = 0 ;

bool ElementInDeque(Vector2 element , deque<Vector2> deque){
    for(unsigned int i = 0 ; i < deque.size() ; i++){
        if(Vector2Equals(deque[i] , element)){
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - lastUpdaTetime >= interval){
        lastUpdaTetime = currentTime;
        return true;
    }
    return false;
}

class Snake {
public :

    deque<Vector2> body = {Vector2{6 , 9} , Vector2{5 , 9} , Vector2{4 , 9}};
    Vector2 direction = {1 , 0};
    bool addsegment = false;

    void Draw(){
        for(unsigned int i = 0 ; i< body.size() ; i++){
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset+x*cellsize ,offset+ y*cellsize ,(float)cellsize ,(float)cellsize};
            DrawRectangleRounded(segment , 0.5 , 6 , darkgreen);
        }
    }
    void Update(){
        body.push_front(Vector2Add(body[0] , direction));
        if(addsegment){
            addsegment = false;
        }else{
            body.pop_back();
        }
       
    }

    void reset(){
        body = {Vector2{6 , 9} , Vector2{5 , 9} , Vector2{4 , 9}};
        direction = {1 ,0};
    }
};

class Food {
public:
    Vector2 position ;
    Texture2D texture;

    Food(deque<Vector2> snakebody){
        Image image = LoadImage("Graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakebody) ;
    }
    ~Food () {
        UnloadTexture(texture);
    }

    void Draw(){
        DrawTexture(texture ,offset+position.x * cellsize,offset + position.y *cellsize, WHITE);
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0 , cellcount - 1);
        float y = GetRandomValue(0 , cellcount - 1) ;
        return Vector2{ x , y};
    }

    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {

        Vector2 position = GenerateRandomCell();

        while(ElementInDeque(position , snakeBody)){
            position = GenerateRandomCell();
        }
        return position;
        
    }
};

class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0 ;
    Sound eatSound;
    Sound wallSound;

    Game(){
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }
    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    void Draw(){
        food.Draw();
        snake.Draw();

    }

    void Update(){
        if(running){
            snake.Update();
            eatfood();
            hitedge();
            slefhit();
        }
    }

    void eatfood(){
        if(Vector2Equals(snake.body[0] , food.position)){
            food.position = food.GenerateRandomPos(snake.body);
            snake.addsegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    void hitedge(){
        if(snake.body[0].x == cellcount || snake.body[0].x == -1){
            GameOver();
        }
        if(snake.body[0].y == cellcount || snake.body[0].y == -1){
            GameOver();
        }
    }

    void GameOver(){
        snake.reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    void slefhit(){
        deque<Vector2> headlessbody = snake.body;
        headlessbody.pop_front();
        if(ElementInDeque(snake.body[0] , headlessbody)){
            GameOver();
        }
    }

};
int main () {

    InitWindow(2*offset + cellsize * cellcount , 2*offset + cellsize * cellcount , "Retro Snake");
    SetTargetFPS(60);

    Game game = Game();
    
    while(!WindowShouldClose()){
        BeginDrawing() ;

        if(eventTriggered(0.2)){
            game.Update();
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0 , -1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0 , 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1 , 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1 , 0};
            game.running = true;
        }

    
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset - 5 , (float)offset - 5 , (float)cellsize * cellcount + 10 , (float)cellsize*cellcount+10}, 5 , darkgreen);
        
        DrawText("Snake Game" , offset - 5 , 20 , 40 , darkgreen);
        DrawText(TextFormat("%i" ,game.score) , offset - 5 , offset + cellsize * cellcount +10, 40 , darkgreen);
        
        game.Draw();


        EndDrawing();
    }

    CloseWindow();
    return 0;
}