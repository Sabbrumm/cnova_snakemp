#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
const int SCREENWIDTH = 80;
const int SCREENHEIGHT = 22;
int screen = 0;
int screen1_point = 1;

struct SnakeCoord{
    int X;
    int Y;
    int sn_type; //0 - голова, 1 - тело
};

struct Snake{
    struct SnakeCoord *dots;
    int FruitCount;
};

enum SnakeCrawlDir {Left, Right, Up, Down};


//0 - главное меню
//1 - меню выбора
//2 - конструктор уровня
//3 - игра
//4 - ошибка

int game_board[10][10] = {{0}};
//0 - пусто
//1 - стена
//2 - фрукт
void cleanboard(){
    for (int i=0;i<10;i++){
        for (int j=0;j<10;j++){
            game_board[i][j]=0;
        }
    }
}

void cleanfruits(){
    for (int i=0;i<10;i++){
        for (int j=0;j<10;j++){
            if (game_board[i][j]==2)
                game_board[i][j]=0;
        }
    }
}

int RESULTRED = 0;
int RESULTBLUE = 0;

int last_err_code = 0;

int gamewidth = 0;
int gameheight = 0;

int freecells_count(){
    int count = 0;
    for (int i = 0; i<gamewidth; i++){
        for (int j = 0; j<gameheight; j++){
            if (game_board[i][j]==0){
                count++;
            }
        }
    }
    return count;
}

COORD get_random_free_coord(){
    int t_cou = freecells_count();
    COORD *tempmas = malloc(sizeof(COORD)*t_cou);
    int cntr = 0;
    for (int i = 0; i<gamewidth; i++){
        for (int j = 0; j<gameheight; j++){
            if (game_board[i][j]==0){
                COORD tempco;
                tempco.X = i;
                tempco.Y = j;
                tempmas[cntr] = tempco;
                cntr++;
            }
        }
    }
    t_cou = rand()%t_cou;
    return tempmas[t_cou];
}

COORD get_random_free_coord_snakes(struct Snake *pl1, struct Snake *pl2){
    int t_cou = freecells_count()-pl1->FruitCount-pl2->FruitCount-2;
    COORD *tempmas = malloc(sizeof(COORD)*t_cou);
    if (t_cou<=0){
        COORD tempco;
        tempco.X = -1;
        tempco.Y = -1;
        return tempco;
    }
    int cntr = 0;
    int doit = 1;
    for (int i = 0; i<gamewidth; i++){
        for (int j = 0; j<gameheight; j++){
            if (game_board[i][j]==0){
                doit = 1;
                for (int k=0; k<pl1->FruitCount+1; k++){
                    if (i==pl1->dots[k].X && j==pl1->dots[k].Y)
                        doit=0;
                }
                for (int k=0; k<pl2->FruitCount+1; k++){
                    if (i==pl2->dots[k].X && j==pl2->dots[k].Y)
                        doit=0;
                }
                if (doit){
                    COORD tempco;
                    tempco.X = i;
                    tempco.Y = j;
                    tempmas[cntr] = tempco;
                    cntr++;
                }
            }
        }
    }
    t_cou = rand()%t_cou;
    return tempmas[t_cou];
}

int CanWePlay(){
    if (gamewidth<3 || gameheight<3)
        return 2;
    if (gamewidth>10 || gameheight>10)
        return 3;
    if (freecells_count()<3)
        return 4;
    return 1;
}
//2 - слишком мелкие параметры
//3 - слишком большие параметры
//4 - слишком мало свободных клеток

//#include "fmod.h"

// Создание змеи
struct Snake make_snake(int x, int y){
    struct Snake NewSnake;

    NewSnake.FruitCount=0;

    struct SnakeCoord NewHead;
    NewHead.sn_type = 0;
    NewHead.X = x;
    NewHead.Y = y;

    NewSnake.dots = malloc(sizeof(NewHead));
    NewSnake.dots[0] = NewHead;

    return NewSnake;
}
//TODO Фрукты остаются на поле после первой катки

// Рост и движение змеи

void snake_grow(struct Snake *snake, enum SnakeCrawlDir last_s_mov){

    // Создаём новую координату змеи

    struct SnakeCoord NewCoord;
    NewCoord.sn_type = 0;

    //В зависимости от последнего движения задаем координату
    switch (last_s_mov){
        case Left:
            NewCoord.X = snake->dots[0].X-1,
                    NewCoord.Y = snake->dots[0].Y;
            break;
        case Right:
            NewCoord.X = snake->dots[0].X+1,
                    NewCoord.Y = snake->dots[0].Y;
            break;
        case Up:
            NewCoord.X = snake->dots[0].X,
                    NewCoord.Y = snake->dots[0].Y-1;
            break;
        case Down:
            NewCoord.X = snake->dots[0].X,
                    NewCoord.Y = snake->dots[0].Y+1;
            break;
    }

    // Добавляем змейке фрукт
    snake->FruitCount++;

    // Перевыделяем место под память координат змеи
    snake->dots = realloc(snake->dots, sizeof(struct SnakeCoord)*(snake->FruitCount+1));

    for (int i = snake->FruitCount; i>0; i--){
        snake->dots[i] = snake->dots[i-1];
        snake->dots[i].sn_type = 1;
    }

    // Назначаем новую координату
    snake->dots[0] = NewCoord;
}

int snake_move(struct Snake *snake, enum SnakeCrawlDir move_to){

    //Если змейка состоит не только из головы
    if (snake->FruitCount>0){
        // создаем временную координату в которой выражено смещение между головой и первым элементом хвоста
        struct SnakeCoord temp;
        temp.X = snake->dots[0].X-snake->dots[1].X,
                temp.Y = snake->dots[0].Y-snake->dots[1].Y;
        // проверяем что перемещаться можно
        if ((temp.X == -1 && move_to==Right) ||
            (temp.X == 1 && move_to ==Left) ||
            (temp.Y == -1 && move_to==Down) ||
            (temp.Y == 1 && move_to==Up)){
            return 0;
        }


        //Можно! Перемещаемся,
        //создаем новую голову

        struct SnakeCoord NewHead;
        NewHead.sn_type = 0;

        //Задаём координату головы
        switch (move_to){
            case Left:
                NewHead.X = snake->dots[0].X-1,
                        NewHead.Y = snake->dots[0].Y;
                break;
            case Right:
                NewHead.X = snake->dots[0].X+1,
                        NewHead.Y = snake->dots[0].Y;
                break;
            case Up:
                NewHead.X = snake->dots[0].X,
                        NewHead.Y = snake->dots[0].Y-1;
                break;
            case Down:
                NewHead.X = snake->dots[0].X,
                        NewHead.Y = snake->dots[0].Y+1;
                break;
        }
        // все координаты смещаем на 1, последнюю координату удаляем
        for (int i = snake->FruitCount; i>0; i--){
            snake->dots[i] = snake->dots[i-1];
            snake->dots[i].sn_type = 1;
        }

        //новая голова теперь в начале массива точек
        snake->dots[0]=NewHead;
    }
        // если только голова
    else{
        switch (move_to){
            case Left:
                snake->dots[0].X -= 1;
                break;
            case Right:
                snake->dots[0].X += 1;
                break;
            case Up:
                snake->dots[0].Y -= 1;
                break;
            case Down:
                snake->dots[0].Y += 1;
                break;
        }
    }

    return 1;
}



#define SC SetConsoleCursorPosition
#define KEY_ESC 27
#define KEY_CONTROL 224
#define KEY_BACKSPACE 8
#define KEY_ENTER 13
#define KEY_CONTROL_DOWN 80
#define KEY_CONTROL_UP 72

#define KEY_CONTROL_RIGHT 77
#define KEY_CONTROL_LEFT 75
#define KEY_0 48
#define KEY_9 57

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100

int ExitVar = 0;
void SND_MAINMENU(){
    PlaySound("sounds\\THEME_MAINMENU.wav", NULL, SND_ASYNC|SND_LOOP);
}
void SND_GAME(){
    PlaySound("sounds\\THEME_GAME_LOOP.wav", NULL, SND_ASYNC|SND_LOOP);
}
void SND_LVLBUILDER(){
    PlaySound("sounds\\THEME_LEVELBUILDER.wav", NULL, SND_ASYNC|SND_LOOP);
}
void SND_MAINMENU_CHOOSE(){
    PlaySound("sounds\\THEME_MENUCHOOSER.wav", NULL, SND_ASYNC|SND_LOOP);
}
void SND_ERRORSCREEN(){
    PlaySound("sounds\\THEME_ERRORSCREEN.wav", NULL, SND_ASYNC|SND_LOOP);
}

void SND_POSITIVE(){
    PlaySound("sounds\\CHOOSE_POSITIVE.wav", NULL, SND_ASYNC);
    Sleep(400);
}
void SND_NEGATIVE(){
    PlaySound("sounds\\CHOOSE_NEGATIVE.wav", NULL, SND_ASYNC);
    Sleep(400);
}
void SND_NEUTRAL(){
    PlaySound("sounds\\CHOOSE_NEUTRAL.wav", NULL, SND_ASYNC);
    Sleep(400);
}
void SND_GAMEOVER(){
    PlaySound("sounds\\SNAKE_DEATH.wav", NULL, SND_ASYNC);
}

void SND_STOP(){
    PlaySound(NULL, NULL, 0);
}

void SetCur(int X, int Y){
    COORD cursor;
    cursor.X = X;
    cursor.Y = Y;
    SC(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
}


void IntroAnimation(){
    SetCursorPos(0,0);
    HANDLE handler = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD cursor;
    cursor.X = 0;
    cursor.Y = 0;

    for (int printer = 0; printer < SCREENWIDTH; printer++){
        cursor.Y = 0;
        SC(handler, cursor);
        printf("=");
        cursor.Y = SCREENHEIGHT-1;
        SC(handler, cursor);
        printf("=");
        cursor.X = printer;
        Sleep(10);
        if (printer%7==0){
            PlaySound("sounds\\SOUND_INTRO_SHAPER.wav", NULL, SND_ASYNC);
        }
    }
    DWORD timing = 375;

    cursor.X=13;
    cursor.Y=13;

    SC(handler, cursor);
    printf("########          ##    ##  ########     ####  ##    ##");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(timing);
    cursor.Y=12;

    SC(handler, cursor);
    printf("##                ##    ##  ##    ##   ##  ##  ##    ##");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(timing);
    cursor.Y=11;

    SC(handler, cursor);
    printf("##                ##    ##  ##    ## ##    ##  ##    ##");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(timing);
    cursor.Y=10;

    SC(handler, cursor);
    printf("##        ######  ##  ####  ##    ## ##    ##  ########");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(timing);
    cursor.Y=9;

    SC(handler, cursor);
    printf("##                ####  ##  ##    ## ##    ##  ##    ##");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(timing);
    cursor.Y=8;

    SC(handler, cursor);
    printf("########          ##    ##  ######## ##    ##  ########");
    PlaySound("sounds\\SOUND_INTRO_BOOM.wav", NULL, SND_ASYNC);
    Sleep(500);

    PlaySound("sounds\\CNOVA_INTRO.wav", NULL, SND_ASYNC);
    system("color 2");
    Sleep(300);
    system("color 3");
    Sleep(300);
    system("color 4");
    Sleep(300);
    system("color F");
}

void BorderPrint(){
    SetCur(0,0);
    printf("================================================================================\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "================================================================================");
}

void ColorBlinkSnake(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_INTENSITY);
}

void ConsoleColorBlank(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
}

void ConsoleColorRed(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
}

void ConsoleColorBlue(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE);
}

void WallColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED);
}
void EmptyFieldColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), BACKGROUND_GREEN);
}

void ChosenColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|
                            BACKGROUND_RED);
}

void FruitColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            BACKGROUND_RED|BACKGROUND_BLUE|
                            FOREGROUND_GREEN);
}

void Pl1HeadColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                           BACKGROUND_RED);
}

void Pl1BodyColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            BACKGROUND_RED|BACKGROUND_INTENSITY);
}

void Pl2HeadColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                           BACKGROUND_BLUE);
}

void Pl2BodyColor(){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                            BACKGROUND_BLUE|BACKGROUND_INTENSITY);
}





void TransparentPrinter(char* string, int Xstart, int Ystart){
    COORD cursor;
    cursor.X = Xstart;
    cursor.Y = Ystart;
    long long counter = Ystart*SCREENWIDTH+Xstart;
    for (long long int i = 0; ;i++){
        if (string[i]=='\0')
            break;
        else if (string[i]==' '){
            counter++;
            continue;
        }
        else if (string[i]=='\n'){
            counter = (counter/SCREENWIDTH + 1)*SCREENWIDTH;
        }
        else {
            cursor.X=counter%SCREENWIDTH;
            cursor.Y=counter/SCREENWIDTH;
            SC(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
            printf("%c", string[i]);
            counter++;
        }
    }
}

void IntransparentPrinter(char* string, int Xstart, int Ystart){
    COORD cursor;
    cursor.X = Xstart;
    cursor.Y = Ystart;
    long long counter = Ystart*SCREENWIDTH+Xstart;
    for (long long int i = 0; ;i++){
        if (string[i]=='\0')
            break;
        else if (string[i]=='\n'){
            counter = (counter/SCREENWIDTH + 1)*SCREENWIDTH;
        }
        else {
            cursor.X=counter%SCREENWIDTH;
            cursor.Y=counter/SCREENWIDTH;
            SC(GetStdHandle(STD_OUTPUT_HANDLE), cursor);
            printf("%c", string[i]);
            counter++;
        }
    }
}


void ReadySetGoAnimation(){
    BorderPrint();
    SetCur(0, 8);
    printf("                        ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n"
           "                        ┃                                  ┃\n"
           "                        ┃                                  ┃\n"
           "                        ┃                                  ┃\n"
           "                        ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    SetCur(38, 10);
    printf("На старт!");

    SND_NEGATIVE();

    SetCur(38, 10);
    printf("         ");

    SetCur(38, 10);
    printf("Внимание!");

    SND_NEUTRAL();

    SetCur(38, 10);
    printf("         ");

    SetCur(40, 10);
    printf("Марш!");

    SND_POSITIVE();

    SetCur(0, 8);
    printf("                                                            \n"
           "                                                            \n"
           "                                                            \n"
           "                                                            \n"
           "                                                            ");
}

void StartMenuLayerBack(){
     printf("================================================================================\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "                        Нажмите [Enter] для продолжения\n"
           "                            Нажмите [ESC] для выхода\n"
           "\n"
           "\n"
           "\n"
           "\n"
           "================================================================================");
    ConsoleColorRed();
    TransparentPrinter("      ############                            ############      ##  ######\n"
                       "      ##        ####                      ######        ####  ####  ##    ##\n"
                       "      ########      ######################                  ##  ##  ##    ##\n"
                       "            ##                                                  ##  ##  ##\n"
                       "            ##                                                  ##  ##\n"
                       "     <########                                                  ######",
                       0,4);
    ConsoleColorBlank();
    TransparentPrinter("                ##    ##  ########  ##    ##\n"
                       "                      ##  ##    ##  ##\n"
                       "                ##                            ######    ##\n"
                       "                ##    ##  ##    ##  ##    ##  ##        ##\n"
                       "                ##    ##  ##    ##  ##    ##  ##        ##\n"
                       "                ##    ##  ##    ##  ##    ##  ########  ##",
                       0,4);
    ConsoleColorBlank();
}

unsigned short anim_counter = 0; //max-6

void StartMenuAnimFront(){
    while (!kbhit()){
        if (anim_counter==0){
            ConsoleColorRed();
            IntransparentPrinter("     ",
                                 0,9);
            anim_counter++;
        }
        else if (anim_counter == 1 || anim_counter==5){
            ConsoleColorRed();
            IntransparentPrinter("    -",
                               0,9);
            anim_counter++;
        }
        else if (anim_counter == 2 || anim_counter==4){
            ConsoleColorRed();
            IntransparentPrinter("   --",
                               0,9);
            anim_counter++;
        }
        else if (anim_counter == 3){
            ConsoleColorRed();
            IntransparentPrinter("  ---",
                               0,9);
            anim_counter++;
        }
        if (anim_counter >= 6){
            anim_counter=0;
        }
        Sleep(1000);
        COORD C;
        C.X =0;
        C.Y = 0;
        SC(GetStdHandle(STD_OUTPUT_HANDLE), C);
    }
    int key = _getch();
    if (key==KEY_ESC){
        SND_NEGATIVE();
        ExitVar = 1;
    }
    if (key==KEY_ENTER){
        SND_POSITIVE();
        SND_MAINMENU_CHOOSE();
        screen = 1;
    }
}

int PlaceFruit(int X, int Y){
    if (X<0 && Y<0)
        return 0;
    game_board[X][Y] = 2;
    return 1;
}

void SnakeBlinkOne(struct Snake *snake){
    const int start_x = (SCREENWIDTH-(gamewidth*2))/2;
    const int start_y = 4;
    for (int j = 0; j<gameheight; j++) {
        for (int i = 0; i < gamewidth; i++) {
            SetCur(start_x+i*2, start_y+j);
            for (int k = 0; k<snake->FruitCount+1;k++){
                if (snake->dots[k].X==i && snake->dots[k].Y==j){
                    ColorBlinkSnake();
                    printf("  ");
                }
            }
        }
    }
}



void PrintBuildBoard(int X, int Y){
    const int start_x = (SCREENWIDTH-(gamewidth*2))/2;
    const int start_y = 8;
    for (int j = 0; j<gameheight; j++){
        SetCur(start_x,j+start_y);
        for (int i = 0; i<gamewidth; i++){


            if (!(X==i && Y==j)){
                EmptyFieldColor();
            }
            else {
                ChosenColor();
            }
            if (game_board[i][j]==0){
                printf("  ");
            }
            if (game_board[i][j] == 1){
                if (!(X==i && Y==j)){
                    WallColor();
                }
                printf("##");
            }
        }
    }

}
void EraseBuildBoard(){
    const int start_x = (SCREENWIDTH-20)/2;
    const int start_y = 8;
    ConsoleColorBlank();
    for (int j = 0; j<10; j++){
        SetCur(start_x,j+start_y);
        printf("                    ");
    }

}

void PrintGameBoard(struct Snake *pl1, struct Snake *pl2){
    const int start_x = (SCREENWIDTH-(gamewidth*2))/2;
    const int start_y = 4;
    int snakehere = 0;
    for (int j = 0; j<gameheight; j++){
        SetCur(start_x,j+start_y);
        for (int i = 0; i<gamewidth; i++){
            snakehere = 0;
            for (int k = 0; k<pl1->FruitCount+1; k++){
                if (pl1->dots[k].X==i && pl1->dots[k].Y==j){
                    if (pl1->dots[k].sn_type==0){
                        Pl1HeadColor();
                        printf("  ");
                    }
                    if (pl1->dots[k].sn_type==1){
                        Pl1BodyColor();
                        printf("  ");
                    }
                    snakehere = 1;
                }
            }
            for (int k = 0; k<pl2->FruitCount+1; k++){
                if (pl2->dots[k].X==i && pl2->dots[k].Y==j){
                    if (pl2->dots[k].sn_type==0){
                        Pl2HeadColor();
                        printf("  ");
                    }
                    if (pl2->dots[k].sn_type==1){
                        Pl2BodyColor();
                        printf("  ");
                    }
                    snakehere = 1;
                }
            }

            if (!snakehere){
                EmptyFieldColor();

                if (game_board[i][j]==0){
                    printf("  ");
                }
                if (game_board[i][j] == 1){
                    WallColor();
                    printf("##");
                }
                if (game_board[i][j] == 2){
                    FruitColor();
                    printf("><");
                }
            }

        }


    }
}

void SnakeBlinker(int whotoblink, struct Snake *sn1, struct Snake *sn2){
    for (int i = 0; i<3; i++){
        if (whotoblink==1){
            SnakeBlinkOne(sn1);
        }
        else{
            SnakeBlinkOne(sn2);
        }
        Sleep(300);
        PrintGameBoard(sn1, sn2);
        Sleep(300);
    }
}


void BuildLvlUI(int point, int in_editor, int sel_x, int sel_y){
    // Отрисовка стрелочек кареткой
    if (point == 1){
        SetCur(28, 3);
        printf(">>");
        SetCur(49, 3);
        printf("<<");
    }
    else {
        SetCur(28, 3);
        printf("  ");
        SetCur(49, 3);
        printf("  ");
    }
    if (point == 2){
        SetCur(28, 4);
        printf(">>");
        SetCur(49, 4);
        printf("<<");
    }
    else{
        SetCur(28, 4);
        printf("  ");
        SetCur(49, 4);
        printf("  ");
    }
    if (point == 3 && !in_editor){
        SetCur(22, 6);
        printf(">>");
        SetCur(55, 6);
        printf("<<");
    }
    else{
        SetCur(22, 6);
        printf("  ");
        SetCur(55, 6);
        printf("  ");
    }

    // Отрисовка чисел кареткой
    SetCur(45, 3);
    printf("%2d", gamewidth);
    SetCur(45, 4);
    printf("%2d", gameheight);

    //Печать поля
    if (CanWePlay()==2){
        cleanboard();
        EraseBuildBoard();
        SetCur(24, 7);
        printf("--- Поле слишком маленькое! ---");
    } else{
        SetCur(24, 7);
        printf("                               ");
    }
    if (CanWePlay()==1 || CanWePlay()==4){
        PrintBuildBoard(sel_x, sel_y);
        ConsoleColorBlank();
    }


    SetCur(0,0);
}

void BuildLvlIntercept(){
    int key = 0;
    int in_editor = 0;
    int selected_point = 1;
    int selected_x=11, selected_y=11;
    while (key!=KEY_ESC){
        BuildLvlUI(selected_point, in_editor, selected_x, selected_y);
        key = _getch();
        if (key==KEY_CONTROL){
            key = _getch();
            if (!in_editor){
                if (key == KEY_CONTROL_DOWN){
                    selected_point++;
                    if (selected_point>3){
                        selected_point=1;
                    }
                }
                if (key == KEY_CONTROL_UP){
                    selected_point--;
                    if (selected_point<1){
                        selected_point=3;
                    }
                }
            }
            else {
                if (key == KEY_CONTROL_DOWN){
                    if (selected_y<gameheight-1)
                        selected_y++;
                }
                if (key == KEY_CONTROL_UP){
                    if (selected_y>0)
                        selected_y--;
                }
                if (key == KEY_CONTROL_LEFT){
                    if (selected_x>0)
                        selected_x--;
                }
                if (key == KEY_CONTROL_RIGHT){
                    if (selected_x<gamewidth-1)
                        selected_x++;
                }
            }
        }
        if (key>=KEY_0 && key<=KEY_9){
            int num = key-48;
            if (!in_editor){
                if (selected_point==1){
                    if (gamewidth*10+num<=10){
                        gamewidth = gamewidth*10+num;
                    }
                }
                if (selected_point==2){
                    if (gameheight*10+num<=10){
                        gameheight = gameheight*10+num;
                    }
                }
            }
        }
        if (key==KEY_BACKSPACE){
            if (!in_editor){
                if (selected_point==1){
                    gamewidth/=10;
                }
                if (selected_point==2){
                    gameheight/=10;
                }
            }
        }
        if (key==KEY_ENTER){
            if (!in_editor){
                if (selected_point==3){
                    if (CanWePlay()==1 || CanWePlay()==4){
                        in_editor=1;
                        selected_x=0, selected_y=0;
                    }
                }
            }
            else {
                if (game_board[selected_x][selected_y] == 1){
                    game_board[selected_x][selected_y] = 0;
                }
                else{
                    game_board[selected_x][selected_y] = 1;
                }

            }
        }
        if (key==KEY_ESC){
            if (in_editor){
                in_editor = 0;
                key = 0;
                selected_x=11, selected_y=11;
            }
        }
    }
    SND_POSITIVE();
    SND_MAINMENU_CHOOSE();
}

int CheckFruitEating(struct Snake *snake, enum SnakeCrawlDir dir){
    //если змея на клетке с фруктом
    int X = snake->dots[0].X;
    int Y = snake->dots[0].Y;
    switch (dir){
        case Left:
            if (game_board[X-1][Y]==2){
                snake_grow(snake, Left);
                game_board[X-1][Y]=0;
                return 1;
            }
            break;
        case Right:
            if (game_board[X+1][Y]==2){
                snake_grow(snake, Right);
                game_board[X+1][Y]=0;
                return 1;
            }
            break;
        case Up:
            if (game_board[X][Y-1]==2){
                snake_grow(snake, Up);
                game_board[X][Y-1]=0;
                return 1;
            }
            break;
        case Down:
            if (game_board[X][Y+1]==2){
                snake_grow(snake, Down);
                game_board[X][Y+1]=0;
                return 1;
            }
            break;
    }
    return 0;
}
int CanIMoveHere(struct Snake *snake, enum SnakeCrawlDir dir){
    //если 0 - игра заканчивается
    int X = snake->dots[0].X;
    int Y = snake->dots[0].Y;
    switch (dir){
        case Left:
            if (X==0){
                return 0;
            }
            else{
                if (game_board[X-1][Y]==1){
                    return 0;
                }
            }
            break;
        case Right:
            if (X==gamewidth-1){
                return 0;
            }
            else{
                if (game_board[X+1][Y]==1){
                    return 0;
                }
            }
            break;
        case Up:
            if (Y==0){
                return 0;
            }
            else{
                if (game_board[X][Y-1]==1){
                    return 0;
                }
            }
            break;
        case Down:
            if (Y==gameheight-1){
                return 0;
            }
            else{
                if (game_board[X][Y+1]==1){
                    return 0;
                }
            }
            break;
    }
    return 1;
}
int DoCollision(struct Snake *t_snake, struct Snake *r_snake, enum SnakeCrawlDir dir){
    //если 1 - произошла коллизия и нужно закончить игру
    int X = t_snake->dots[0].X;
    int Y = t_snake->dots[0].Y;
    switch(dir){
        case Left:
            X-=1;
            break;
        case Right:
            X+=1;
            break;
        case Up:
            Y-=1;
            break;
        case Down:
            Y+=1;
            break;
    }
    if (t_snake->FruitCount>0 && t_snake->dots[1].X==X && t_snake->dots[1].Y==Y){
        return 0;
    }
    for (int i =0; i<t_snake->FruitCount+1; i++){
        if (t_snake->dots[i].X==X && t_snake->dots[i].Y==Y){
            return 1;
        }
    }
    for (int i =0; i<r_snake->FruitCount+1; i++){
        if (r_snake->dots[i].X==X && r_snake->dots[i].Y==Y){
            return 1;
        }
    }
    return 0;
}

void ResultSplash(){
    ConsoleColorBlank();
    SetCur(23, 8);
    printf("┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓\n");
    SetCur(23, 9);
    printf("┃                                  ┃");
    SetCur(23, 10);
    printf("┃                                  ┃");
    SetCur(23, 11);
    printf("┃                                  ┃");
    SetCur(23, 12);
    printf("┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛");
    SetCur(38, 9);
    printf("СЧЁТ:");
    SND_NEUTRAL();
    SetCur(38, 10);
    ConsoleColorRed();
    printf("%02d",RESULTRED);
    ConsoleColorBlank();
    printf(":");
    ConsoleColorBlue();
    printf("%02d",RESULTBLUE);
    SND_NEUTRAL();
    SetCur(38, 11);
    if (RESULTRED>RESULTBLUE){
        SetCur(33, 11);
        ConsoleColorRed();
        printf("Победил красный!");
    }
    else if (RESULTBLUE>RESULTRED){
        SetCur(34, 11);
        ConsoleColorBlue();
        printf("Победил синий!");
    }
    else{
        SetCur(38, 11);
        ConsoleColorBlank();
        printf("Ничья");
    }
    SND_POSITIVE();
    ConsoleColorBlank();
}

void GameStart(){
    SND_GAME();
    int key = 0;
    int end_causeof = 0;

    COORD rfree = get_random_free_coord();
    struct Snake snake1 = make_snake(rfree.X, rfree.Y);

    COORD rfree2 = get_random_free_coord();

    while (rfree2.X==rfree.X && rfree2.Y==rfree.Y){
        rfree2 = get_random_free_coord();
    }
    struct Snake snake2 = make_snake(rfree2.X, rfree2.Y);

    cleanfruits();
    COORD testfr = get_random_free_coord_snakes(&snake1, &snake2);
    PlaceFruit(testfr.X, testfr.Y);

    while (key!=KEY_ESC){
        PrintGameBoard(&snake1, &snake2);

        key = _getch();

        if (key==KEY_CONTROL){
            key = _getch();
            if (key==KEY_CONTROL_UP){
                if (CanIMoveHere(&snake1, Up)  && !DoCollision(&snake1, &snake2, Up)){
                    if (!CheckFruitEating(&snake1, Up))
                        snake_move(&snake1, Up);
                    else{
                        COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                        if (!PlaceFruit(r.X, r.Y)){
                            end_causeof=1;
                            break;
                        }

                    }
                }
                else{
                    end_causeof=1;
                    break;
                }

            }
            if (key==KEY_CONTROL_DOWN){
                if (CanIMoveHere(&snake1, Down) && !DoCollision(&snake1, &snake2, Down)){
                    if (!CheckFruitEating(&snake1, Down))
                        snake_move(&snake1, Down);
                    else{
                        COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                        if (!PlaceFruit(r.X, r.Y)){
                            end_causeof=1;
                            break;
                        }
                    }
                }
                else{
                    end_causeof=1;
                    break;
                }

            }
            if (key==KEY_CONTROL_LEFT){
                if (CanIMoveHere(&snake1, Left) && !DoCollision(&snake1, &snake2, Left)){
                    if (!CheckFruitEating(&snake1, Left))
                        snake_move(&snake1, Left);
                    else{
                        COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                        if (!PlaceFruit(r.X, r.Y)){
                            end_causeof=1;
                            break;
                        }
                    }
                }
                else{
                    end_causeof=1;
                    break;
                }
            }
            if (key==KEY_CONTROL_RIGHT){
                if (CanIMoveHere(&snake1, Right) && !DoCollision(&snake1, &snake2, Right)){
                    if (!CheckFruitEating(&snake1, Right))
                        snake_move(&snake1, Right);
                    else{
                        COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                        if (!PlaceFruit(r.X, r.Y)){
                            end_causeof=1;
                            break;
                        }
                    }
                }
                else{
                    end_causeof=1;
                    break;
                }
            }
        }
        if (key==KEY_W){
            if (CanIMoveHere(&snake2, Up) && !DoCollision(&snake2, &snake1, Up)){
                if (!CheckFruitEating(&snake2, Up))
                    snake_move(&snake2, Up);
                else{
                    COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                    if (!PlaceFruit(r.X, r.Y)){
                        end_causeof=2;
                        break;
                    }
                }
            }
            else{
                end_causeof=2;
                break;
            }
        }
        if (key==KEY_S){
            if (CanIMoveHere(&snake2, Down) && !DoCollision(&snake2, &snake1, Down)){
                if (!CheckFruitEating(&snake2, Down))
                    snake_move(&snake2, Down);
                else{
                    COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                    if (!PlaceFruit(r.X, r.Y)){
                        end_causeof=2;
                        break;
                    }
                }
            }
            else{
                end_causeof=2;
                break;
            }

        }
        if (key==KEY_A){
            if (CanIMoveHere(&snake2, Left) && !DoCollision(&snake2, &snake1, Left)){
                if (!CheckFruitEating(&snake2, Left))
                    snake_move(&snake2, Left);
                else{
                    COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                    if (!PlaceFruit(r.X, r.Y)){
                        end_causeof=2;
                        break;
                    }
                }
            }
            else{
                end_causeof=2;
                break;
            }
        }
        if (key==KEY_D){

            if (CanIMoveHere(&snake2, Right) && !DoCollision(&snake2, &snake1, Right)){
                if (!CheckFruitEating(&snake2, Right))
                    snake_move(&snake2, Right);
                else{
                    COORD r = get_random_free_coord_snakes(&snake1, &snake2);
                    if (!PlaceFruit(r.X, r.Y)){
                        end_causeof=2;
                        break;
                    }
                }
            }
            else{
                end_causeof=2;
                break;
            }
        }
    }



    cleanfruits();

    if (key==KEY_ESC)
        SND_NEGATIVE();
    else{
        SND_GAMEOVER();
        if (end_causeof==1){
            SnakeBlinker(1, &snake1, &snake2);
        }
        else{
            SnakeBlinker(2, &snake1, &snake2);
        }
        RESULTRED = snake1.FruitCount;
        RESULTBLUE = snake2.FruitCount;
        ResultSplash();
        _getch();
    }
}

void UI(){
    if (screen==1){
        printf("================================================================================\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n");
        if (screen1_point==1)
        printf("                              >> [Начать игру] <<\n");
        else
        printf("                                 [Начать игру]\n");
        printf("\n");
        if (screen1_point==2)
        printf("                           >> [Конструктор уровня] <<\n");
        else
        printf("                              [Конструктор уровня]\n");
        printf("\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "================================================================================");
    }
    if (screen==2){
        printf("================================================================================\n"
               "\n"
               "\n"
               "                                Ширина поля: \n"
               "                                Высота поля: \n"
               "\n"
               "                          [Расставить стенки на поле]\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "================================================================================");
        BuildLvlIntercept();
    }

    if (screen==3){
        BorderPrint();
        ReadySetGoAnimation();
        GameStart();
    }
    if (screen==4){
        printf("================================================================================\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n");
        printf("                                Ой-Ой! Ошибка!\n");
        printf("\n");
        if (last_err_code==2)
        printf("               Слишком маленькие значения размеров игрового поля\n");
        else if (last_err_code==3)
        printf("                Слишком большие значения размеров игрового поля\n");
        else if (last_err_code==4)
        printf("                  Слишком мало свободных клеток на игровом поле\n");
        printf("\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "\n"
               "================================================================================");
    }
}

void Intercept(){
    if (screen!=2 && screen!=3){
        int key = _getch();
        if (key==KEY_ESC){
            if (screen==1){
                screen=0;
                SND_NEUTRAL();
                ConsoleColorBlank();
                system("cls");
                StartMenuLayerBack();
                SND_MAINMENU();
            }
            if (screen==2 || screen==3 || screen==4){
                screen=1;
                SND_NEUTRAL();
                SND_MAINMENU_CHOOSE();
            }
        }
        if (key==KEY_CONTROL){
            key = _getch();
            if (key==KEY_CONTROL_DOWN){
                if (screen==1){
                    screen1_point+=1;
                    if (screen1_point>2){
                        screen1_point=1;
                    }
                }
            }
            if (key==KEY_CONTROL_UP){
                if (screen==1){
                    screen1_point-=1;
                    if (screen1_point<1){
                        screen1_point=2;
                    }
                }
            }
        }
        if (key==KEY_ENTER){
            if (screen==1){
                if (screen1_point==1){
                    if (CanWePlay()==1){
                        screen=3;
                    }
                    else {
                        screen=4;
                        SND_NEGATIVE();
                        SND_ERRORSCREEN();
                        last_err_code = CanWePlay();
                    }
                }
                if (screen1_point==2){
                    screen=2;
                    SND_POSITIVE();
                    SND_LVLBUILDER();
                }
            }
            else if (screen==4){
                screen=1;
                SND_NEUTRAL();
                SND_MAINMENU_CHOOSE();
            }

        }
    }
    else{
        screen=1;
        SND_MAINMENU_CHOOSE();
    }

}

void OptimiseConsoleWindow(){
    //Вывод в UTF-8
    SetConsoleOutputCP(CP_UTF8);

    //Фуллскрин режим
    SendMessage(GetConsoleWindow(), WM_SYSKEYDOWN, VK_RETURN, 0x20000000);

    //Увеличение шрифта
    CONSOLE_FONT_INFOEX finf;
    GetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 1, &finf);
    finf.dwFontSize.Y = 37;
    finf.cbSize = sizeof(finf);
    lstrcpyW(finf.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), 1, &finf);

    //спрятать курсор
    CONSOLE_CURSOR_INFO cinf;
    cinf.dwSize = 100;
    cinf.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cinf);

    //Название окошка
    const unsigned short * myTitle = L"C-NOVA SNAKEMP [v0.1a]";
    SetConsoleTitleW(myTitle);
}


int main() {
    srand(time(NULL));

    OptimiseConsoleWindow();

    IntroAnimation();
    system("cls");
    StartMenuLayerBack();
    SND_MAINMENU();
    while (!ExitVar){
        while (screen==0 && !ExitVar){
            StartMenuAnimFront();
        }
        if (screen!=0){
            ConsoleColorBlank();
            system("cls");
            UI();
            Intercept();
        }
    }
    return 0;
}
