// Notes:
// credit to Erick Skiff
// Resistor Anthems / Music

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <conio.h>

#define _WIN32_WINNT 0x0500 // make sure its windows higher than XP
#include <windows.h>
#include <stdexcept>

//define key word for clear
#define clear system("CLS")

using namespace std;

// Windows API struct and methods
// credit to Duoas
// http://www.cplusplus.com/forum/windows/10731/
// colorattribute = foreground_color + background_color * 16
struct console
{
    console( unsigned width, unsigned height )
    {
        SMALL_RECT r;
        COORD      c;
        hConOut = GetStdHandle( STD_OUTPUT_HANDLE );
        if (!GetConsoleScreenBufferInfo( hConOut, &csbi ))
            throw runtime_error( "You must be attached to a human." );

        r.Left   =
            r.Top    = 0;
        r.Right  = width -1;
        r.Bottom = height -1;
        SetConsoleWindowInfo( hConOut, TRUE, &r );

        c.X = width;
        c.Y = height;
        SetConsoleScreenBufferSize( hConOut, c );
    }

    ~console()
    {
        SetConsoleTextAttribute(    hConOut,        csbi.wAttributes );
        SetConsoleScreenBufferSize( hConOut,        csbi.dwSize      );
        SetConsoleWindowInfo(       hConOut, TRUE, &csbi.srWindow    );
    }

    void color( WORD color = 0x07 )
    {
        SetConsoleTextAttribute( hConOut, color );
    }

    HANDLE                     hConOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
};

// console set size
console con(80, 47);

// global variables                {
const unsigned int Y_SIZE = 10;
const unsigned int X_SIZE = 15;
const unsigned int KATS = 5;
const unsigned int OBSTACLES = 15;

// pertange chance
const double KAT_KAPTURE = 0.65;
const double SELF_KAPTURE = 0.02;
const double HAS_TUNA = 0.10;
const double EYES_SUBMISSION = 0.60;
const double PURRS_DOOM = 0.70;
const double IS_SMART = 0.60;

// declare structures
struct Human_t
{
    static const char HUNTER_C = 233; // character representation

    int x; // x position
    int y; // y position
    int health;
    bool has_glasses; // is it on
    bool has_muffs; // is it on
    bool is_stunned; // is he stunned
};

struct Cat_t
{
    static const char KATS_C = 236; // character representation

    int x; // x position on board
    int y; // y position on board
    bool is_kaptured; // check if kaptured
    bool is_smart;
    bool has_tuna; // if cat carries tuna
};

// function prototypes
void instructions(const char, const char);
bool success(double percent_chance);
unsigned int rangeCheck(const char [][X_SIZE], const unsigned int, const unsigned int, int &, int &, const char);
unsigned int directionCheck(const Human_t, const Cat_t);
bool won(Cat_t []);
void initializeBoard(char [][X_SIZE]);
void setupGame(char [][X_SIZE], Human_t &, Cat_t []);
void printBoard(const char [][X_SIZE], const Human_t, const Cat_t [], const unsigned int);
void userMovementAction(char [][X_SIZE], Human_t &, Cat_t [], const unsigned int);
void katsMovementAction(char [][X_SIZE], Human_t &, Cat_t []);

int main()
{
    // play music
    PlaySound("music.wav", NULL, SND_FILENAME | SND_ASYNC);

    // variable declarations
    // consts
    const unsigned int ACTIONS_PER_TURN = 2;
    const unsigned int ENTER = 13;

    // non-const
    char board[Y_SIZE][X_SIZE] = { {' '} };
    Human_t hunter;
    Cat_t kats[KATS];
    bool have_won = false;

    // set console color
    con.color(15);
    clear;

    // tell user the instruction
    instructions(hunter.HUNTER_C,kats[0].KATS_C);

    // initialize board
    initializeBoard(board);

    // setup board
    setupGame(board, hunter, kats);

    // print board
    printBoard(board, hunter, kats, 0);

    // start the game
    while ( !(hunter.health <= 0) )
    {
        // user performs two action
        for (unsigned int i = 0; (i < ACTIONS_PER_TURN) && !hunter.is_stunned; i++)
        {
            userMovementAction(board, hunter, kats, i);

            // reprint to screen
            printBoard(board, hunter, kats, i+1);

            // check to see if i've won
            have_won = won(kats);

            // if you won break
            if (have_won)
                break;
        }

        // if you won break second loop
        if (have_won)
            break;

        // tell user it's kats turn
        if (!hunter.is_stunned)
            cout << "Kats will now make their move.." << endl << endl;
        else
            cout << "You are stunned, Kats will move again.." << endl << endl;

        // reset stun
        hunter.is_stunned = false;

        // so user can seoption = 1 + rand() % 4;e movement
        // make user wait
        Sleep(750);

        // make Kats move
        for (unsigned int i = 0; i < ACTIONS_PER_TURN; i++)
        {
            // make user wait
            Sleep(750);

            // kats choose their action
            katsMovementAction(board, hunter, kats);

            // reprint to screen
            printBoard(board, hunter, kats, 0);

            // check to see if you lost
            if (hunter.health <= 0)
                break;
        }

        // reset hunter's protection
        hunter.has_glasses = false;
        hunter.has_muffs = false;

        // reprint to screen
        printBoard(board, hunter, kats, 0);

        // check to see if you lost
        if (hunter.health <= 0)
            break;
    }

    if (hunter.health <= 0)
    {
        // tell then they lost
        con.color(12);
        cout << "YOU LOSE! TRY AGAIN..." << endl;
        con.color(15);
    }
    else if (have_won)
    {
        // tell then they lost
        con.color(10);
        cout << "CONGRATULATIONS! YOU WIN!" << endl;
        con.color(15);
    }

    cout << "Press enter to play again!" << endl;
    while ((unsigned)getch() != ENTER);

    main(); // start game again

    return 0;
}

void instructions(const char hunter, const char kat)
{
    // variable declaration
    // consts
    const char EMPTY_C = 176;
    const char OBSTACLE_C = 219;
    const char BULLET = 237;
    const unsigned int ENTER = 13;

    // explain instruction
    // characters
    cout << "Characters: " << endl << endl;
    cout << "\t" << hunter << " - HUNTER" << endl;
    cout << "\t" << kat << " - KAT" << endl;
    cout << "\t" << OBSTACLE_C << " - HOUSE" << endl;
    cout << "\t" << EMPTY_C << " - EMPTY" << endl << endl;

    // background
    cout << "Background:" << endl << endl;
    cout << "You are in a town that is crawling with Krazy Kats!! You a hunter sent to\nthis town to kapture them. The Kats wander in";
    cout << " the town and attack their\nvictims. Once an attack is executed successfully the hunter has been subued\nand the kat steals";
    cout << " their Tuna Of Health. The hunter has equipment to protect\nagainst these types of attacks and can kapture kats using a Kat";
    cout << " Kapture system." << endl << endl;

    // controls
    cout << "Controls: " << endl << endl;
    cout << "\tUp - Move Up" << endl;
    cout << "\tDown - Move Down" << endl;
    cout << "\tLeft - Move Left" << endl;
    cout << "\tRight - Move Right" << endl;
    cout << "\tF1 - Reflective Glasses" << endl;
    cout << "\tF2 - Earmuffs of Happiness" << endl;
    cout << "\tF3 - Kat Kapture" << endl << endl;

    //  mechanics
    cout << "Mechanics: " << endl << endl;
    cout << "\t" << BULLET << " Each the hunter and the kats get two actions per turn" << endl;
    cout << "\t" << BULLET << " The hunter and kats can move up, down, left or right" << endl;
    cout << "\t" << BULLET << " Any attack by the hunter or kats take one action" << endl;
    cout << "\t" << BULLET << " The hunter and the kat may not occupy the same square" << endl << endl;

    // hunter attacks
    cout << "Attack and Defenses:" << endl << endl;
    cout << "Kats use Eyes of Submission to take a hunter's Tuna from one space away and\nhave a 60% chance of success. ";
    cout << "They also use Purr of Doom to stun a hunter from\ntwo spaces away and have a 80% chance of success. ";
    cout << "Hunter's may use Reflective\nGlasses and Earmuffs of Happiness respectively and cut those chances in half.\n";
    cout << "To defeat the Kats the hunter must use the Kat Kapture System to kapture them\nwith a 10% of finding health! ";
    cout << "Careful with some kats! They may not be as stupid\nas they seem. Good luck." << endl << endl;

    cout << "You can press ESC at anytime to view instructions again!\nPress enter to continue to the game...";
    while ((unsigned)getch() != ENTER);

    // clear screen
    clear;

    // go back to main function
    return;
}

bool success(double percent_chance)
{
    // delcare and initialize variable
    unsigned int random = 0;
    unsigned int bin = 100 * percent_chance; // store in int even if trancates

    random = 1 + rand() % 100; // number between 1 - 100

    // determine wether it is success or not
    if (random <= bin)
        return true;
    else
        return false;
}

unsigned int rangeCheck(const char board [][X_SIZE], const unsigned int y, const unsigned int x, unsigned int & y_find, unsigned int & x_find, const char find)
{
    // check to see where we can find char "find"
    // range of 1
    for (unsigned int j = y - 1; j <= y + 1; j++)
        for (unsigned int i = x - 1; i <= x + 1; i++)
            if (board[j][i] == find)
            {
                // save coordinates
                y_find = j;
                x_find = i;
                return 1;
            }

    // range of 2
    for (unsigned int j = y - 2; j <= y + 2; j++)
        for (unsigned int i = x - 2; i <= x + 2; i++)
            if (board[j][i] == find)
            {
                // save coordinates
                y_find = j;
                x_find = i;
                return 2;
            }

    // if we didnt find anything retunr 0
    return 0;
}

void initializeBoard(char arr_board[][X_SIZE])
{
    // variable declaration
    // consts
    const char HORI = 205;
    const char VERT = 186;
    const char CORNER1 = 201;
    const char CORNER2 = 187;
    const char CORNER3 = 200;
    const char CORNER4 = 188;
    const char EMPTY_C = 176;

    // do top fence
    arr_board[0][0] = CORNER1;
    arr_board[0][X_SIZE - 1] = CORNER2;

    for (unsigned int i = 1; i < (X_SIZE - 1); i++)
        arr_board[0][i] =  HORI;

    // everything in middle
    for (unsigned int i = 1; i < Y_SIZE; i++)
    {
        for (unsigned int j = 0; j < X_SIZE; j++)
        {
            if (j == 0 || j == (X_SIZE - 1) )
                arr_board[i][j] = VERT;
            else
                arr_board[i][j] = EMPTY_C;
        }
    }

    // do bottom fence
    arr_board[Y_SIZE - 1][0] = CORNER3;
    arr_board[Y_SIZE - 1][X_SIZE - 1] = CORNER4;

    for (unsigned int i = 1; i < (X_SIZE - 1); i++)
        arr_board[Y_SIZE - 1][i] = HORI;

    // go back to main
    return;
}

void setupGame(char arr_board[][X_SIZE], Human_t & hunter, Cat_t kats[])
{
    // variable declaration
    // consts
    const char EMPTY_C = 176;
    const char OBSTACLE_C = 219;

    // non consts
    unsigned int bin_x = 0;
    unsigned int bin_y = 0;
    unsigned int dummy_y = 0;
    unsigned int dummy_x = 0;
    bool hunter_generated = false;

    // seed random function
    srand( (unsigned)time(NULL) );

    // intitalize values
    // hunter
    hunter.x = 0;
    hunter.y = 0;
    hunter.health = 5;
    hunter.has_glasses = 0;
    hunter.has_muffs = 0;
    hunter.is_stunned = 0;

    // kats
    for (unsigned int i = 0; i < KATS; i++)
    {
        kats[i].x = 0;
        kats[i].y = 0;
        kats[i].is_kaptured = 0;
        kats[i].is_smart = 0;
        kats[i].has_tuna = 0;
    }

    // give kats tuna 10% chance and 50% if smart
    for (unsigned int i = 0; i < KATS; i++)
    {
        if ( success(HAS_TUNA) )
            kats[i].has_tuna = 1;

        if ( success(IS_SMART) )
            kats[i].is_smart = 1;
    }

    // generate obstacles
    for (unsigned int i = 0; i < OBSTACLES; i++)
    {
        // generate a coordinate and make sure neither x or y are zero
        bin_y = 1 + rand() % (Y_SIZE - 2);

        bin_x = 1 + rand() % (X_SIZE - 2);

        // put in board
        if (arr_board[bin_y][bin_x] == EMPTY_C && rangeCheck(arr_board, bin_y, bin_x, dummy_y, dummy_x, OBSTACLE_C) != 1 )
            arr_board[bin_y][bin_x] = OBSTACLE_C;
        else
            i--;
    }

    // generate kats
    for (unsigned int i = 0; i < KATS; i++)
    {
        kats[i].y = 1 + rand() % (Y_SIZE - 2);

        kats[i].x = 1 + rand() % (X_SIZE - 2);

        // put on board
        if (arr_board[ kats[i].y ][ kats[i].x ] == EMPTY_C)
            arr_board[ kats[i].y ][ kats[i].x ] = kats[0].KATS_C;
        else
            i--;
    }

    // generate hunter
    while(!hunter_generated)
    {
        hunter.y = 1 + rand() % (Y_SIZE - 2);

        hunter.x = 1 + rand() % (X_SIZE - 2);

        // put on board
        if (arr_board[ hunter.y ][ hunter.x ] == EMPTY_C && rangeCheck(arr_board, hunter.y, hunter.x, dummy_y, dummy_x, kats[0].KATS_C) != 1)
        {
            arr_board[ hunter.y ][ hunter.x ] = hunter.HUNTER_C;
            hunter_generated = true;
        }
    }

    // go back to main
    return;
}

void printBoard(const char arr_board[][X_SIZE], const Human_t hunter, const Cat_t kats[], const unsigned int turn)
{
    // clear screen before printing
    clear;

    // print the header (health and game name)
    con.color(10);
    cout << "\t\t\t" << setw( (X_SIZE) + 5 ) << right << "KrAzY KAts!" << endl << endl;
    con.color(15);

    cout << "\t\t\tTURN: " << turn << endl;
    cout << "\t\t\tHEALTH: ";

    if ( !(hunter.health < 0) ) // if health becomes less than 0 make it 0
    {
        for (int i = 0; i < hunter.health; i++)
        {
            con.color(14);
            cout << "* ";
            con.color(15);
        }
    }
    cout << "\n";

    // print board to screen
    for (unsigned int i = 0; i < Y_SIZE; i++)
    {
        // tab eveything over and center the board
        cout << "\t\t\t";
        for (unsigned int j = 0; j < X_SIZE; j++)
        {
            // add top and bottom fences to spaces if needed
            if ( ( i == 0 || i == (Y_SIZE - 1) ) && j != (X_SIZE - 1) )
                cout << arr_board[i][j] << arr_board[0][1];
            else
            {
                if (arr_board[i][j] == hunter.HUNTER_C)
                {
                    // this will make the hunter green
                    con.color(10);
                    cout << arr_board[i][j] << " ";
                    con.color(15);
                }
                else if (arr_board[i][j] == kats[0].KATS_C)
                {
                    // this will make the the kats red
                    con.color(12);
                    cout << arr_board[i][j] << " ";
                    con.color(15);
                }
                else
                    cout << arr_board[i][j] << " ";
            }
        }
        // after the newline tab everything over and center
        // add the side fence to the spaces
        if ( i != (Y_SIZE - 1) )
        {
            cout << "\n\t\t\t";
            cout << arr_board[1][0] << setw( (X_SIZE * 2) - 2 ) << right << arr_board[1][0] << endl;
        }
        else
            cout << "\n";
    }

    // print hunter information
    cout << "\t\t\tReflective Glasses: ";
    if (hunter.has_glasses)
    {
        con.color(10);
        cout << "ON";
        con.color(15);
    }
    else
    {
        con.color(12);
        cout << "OFF";
        con.color(15);
    }
    cout << "\n";

    cout << "\t\t\tEarmuffs of Happiness: ";
    if (hunter.has_muffs)
    {
        con.color(10);
        cout << "ON";
        con.color(15);
    }
    else
    {
        con.color(12);
        cout << "OFF";
        con.color(15);
    }
    cout << "\n\n";

    // log part
    cout << "LOG:" << endl;

    // go back to main
    return;
}

void userMovementAction(char board[][X_SIZE], Human_t & hunter, Cat_t kats[], const unsigned int turn)
{
    // variable declaration
    // consts
    const unsigned int UP = 72;
    const unsigned int DOWN = 80;
    const unsigned int LEFT = 75;
    const unsigned int RIGHT = 77;
    const unsigned int F1 = 59;
    const unsigned int F2 = 60;
    const unsigned int F3 = 61;
    const unsigned int ESC = 27;
    const unsigned int ENTER = 13;
    const char EMPTY_C = 176;


    // non-consts
    bool action_success = false;
    unsigned int bin_y = hunter.y;
    unsigned int bin_x = hunter.x;
    unsigned int kat_y = 0;
    unsigned int kat_x = 0;
    bool bin_glasses = false;
    bool bin_muffs = false;
    bool kat_kapture = false;
    bool instructions_bool = false;

    while(!action_success)
    {
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // flush input buffer
        // start to take input
        switch ( getch() )
        {
        case UP: // move up
            bin_y--;
            break;
        case DOWN: // move down
            bin_y++;
            break;
        case LEFT: // move left
            bin_x--;
            break;
        case RIGHT: // move right
            bin_x++;
            break;
        case F1: // put glasses on
            bin_glasses = true;
            break;
        case F2: // put glasses on
            bin_muffs = true;
            break;
        case F3:
            kat_kapture = true;
            break;
        case ESC:
            // show user the instruction without reseting the game
            instructions_bool = true;
            break;
        default:
            break;
        }

        // if we moved then check to see if there is an empty space
        if (bin_x != (unsigned)hunter.x || bin_y != (unsigned)hunter.y)
        {
            if (board[bin_y][bin_x] == EMPTY_C)
            {
                // replace old spot with empty space
                board[hunter.y][hunter.x] = EMPTY_C;

                // set new coordinates to hunter
                hunter.x = bin_x;
                hunter.y = bin_y;

                // put hunter in new spot
                board[hunter.y][hunter.x] = hunter.HUNTER_C;

                // make action successful
                action_success = true;
            }
            else
            {
                // let the user know they may not go in that direction
                cout << "This direction is blocked! Try Again." << endl;

                // return to original position
                bin_x = hunter.x;
                bin_y = hunter.y;
            }
        }
        // if user wants to put glasses on makes sure they aren't on already
        else if (bin_glasses)
        {
            // check to see if hunter has glasses on already if not put the on
            if (!hunter.has_glasses)
            {
                // put glasses on
                hunter.has_glasses = true;

                // tell user glasses on
                con.color(10);
                cout << "You have put on the Reflective Glasses." << endl;
                con.color(15);
                // ask the user to press enter so they may end their turn
                cout << "Press enter to continue..." << endl << endl;
                while ((unsigned)getch() != ENTER);

                // reset bin
                bin_glasses = false;

                // make action successful
                action_success = true;
            }
            else
            {
                // tell user he already has glasses on
                cout << "You already have the glasses on." << endl;

                // reset bin
                bin_glasses = false;
            }
        }
        // if user wants to put muffs on makes sure they aren't on already
        else if (bin_muffs)
        {
            // check to see if hunter has glasses on already if not put the on
            if (!hunter.has_muffs)
            {
                // put glasses on
                hunter.has_muffs = true;

                // tell user earmuffs on
                con.color(10);
                cout << "You have put on the Earmuffs of Happiness." << endl;
                con.color(15);
                // ask the user to press enter so they may end their turn
                cout << "Press enter to continue..." << endl << endl;
                while ((unsigned)getch() != ENTER);

                // reset bin
                bin_muffs = false;

                // make action successful
                action_success = true;
            }
            else
            {
                // tell user he already has glasses on
                cout << "You already have the earmuffs on." << endl;

                // reset bin
                bin_muffs = false;
            }
        }
        // run code if user wants to kapture kat
        else if (kat_kapture)
        {
            if ( rangeCheck(board, hunter.y, hunter.x, kat_y, kat_x, kats[0].KATS_C) == 1 )
            {
                // check to see if attack successful
                if( success(KAT_KAPTURE) )
                {
                    // find kat that was kaptured and set to true
                    for (unsigned int i = 0; i < KATS; i++)
                    {
                        if( (unsigned)kats[i].y == kat_y && (unsigned)kats[i].x == kat_x)
                        {
                            kats[i].is_kaptured = true;
                            // if kat has tuna increase health
                            if (kats[i].has_tuna)
                            {
                                con.color(10);
                                cout << "You have found a tuna of health!" << endl;
                                con.color(15);
                                hunter.health++;
                            }

                        }

                    }

                    // set space to empty
                    board[kat_y][kat_x] = EMPTY_C;

                    // tell user they were successful
                    con.color(10);
                    cout << "You have kaptured the kat successfully!!" << endl;
                    con.color(15);
                    // ask the user to press enter so they may end their turn
                    cout << "Press enter to continue..." << endl;
                    while ((unsigned)getch() != ENTER);
                }
                else if ( success(SELF_KAPTURE) )
                {
                    // take health
                    hunter.health--;

                    // tell user kapture was unsuccessful
                    con.color(4);
                    cout << "You have hurt yourself with the Kat Kapture.. -.-" << endl;
                    con.color(15);

                    cout << "Press enter to continue..." << endl;
                    while ((unsigned)getch() != ENTER);
                }
                else
                {
                    // tell user kapture was unsuccessful
                    con.color(12);
                    cout << "Kapture was unsuccessful. :'(" << endl;
                    con.color(15);

                    cout << "Press enter to continue..." << endl;
                    while ((unsigned)getch() != ENTER);
                }

                // reset bin
                    kat_y = 0;
                    kat_x = 0;

                // reset action
                kat_kapture = false;

                // set action successful to true
                action_success = true;
            }
            else
            {
                // tell user there are no kats near by
                cout << "You are not within one space of a kat. Try again." << endl;

                // reset action
                kat_kapture = false;

                // reset bin
                kat_y = 0;
                kat_x = 0;
            }
        }
        // run code to show instructions
        else if(instructions_bool)
        {
            // clear current screen
            clear;

            // show instruction
            instructions(hunter.HUNTER_C, kats[0].KATS_C);

            // clear instruction screen
            clear;

            // return to game
            printBoard(board, hunter, kats, turn);

            // reset instructions
            instructions_bool = false;
        }
    }

    // go back to main
    return;
}

void katsMovementAction(char board[][X_SIZE], Human_t & hunter, Cat_t kats[])
{
    // variable declaration
    // consts
    const char EMPTY_C = 176;
    const unsigned int ENTER = 13;

    // non-consts
    unsigned int bin_y = 0;
    unsigned int bin_x = 0;
    unsigned int option = 0;
    unsigned int bin_range = 0;
    double chance = 0.0;
    bool submission = false;
    bool purr = false;
    bool blocked = false;
    bool action_success = false;

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // flush input buffer

    // start actions for each kat
    for (unsigned int i = 0; i < KATS; i++)
    {
        // reset action success
        action_success = false;

        // if he is kaptured skip kat
        if ( kats[i].is_kaptured )
            continue;

        while(!action_success) // if counter goes over 5 kat is stuck
        {
            // if kat is smart force to attack if close or move closer
            if( kats[i].is_smart )
            {
                bin_range = rangeCheck(board, kats[i].y, kats[i].x, bin_y, bin_x, hunter.HUNTER_C);

                if (bin_range == 1)
                    option = 6; // eyes of submission
                else if (bin_range == 2 && !hunter.is_stunned)
                    option = 7; // purr of doom
                else if (bin_range == 2 && hunter.is_stunned && !blocked)
                {
                    option = directionCheck(hunter, kats[i]);
                    blocked = true; // make sure kat isnt blocked
                }
                else
                {
                    option = 1 + rand() % 4;
                    blocked = false; // reset blocked since it moved
                }
            }
            else
                // generate random number for stupid kat
                option = 1 + rand() % 7;


            // set coordinates for this kat to bin
            bin_y = kats[i].y;
            bin_x = kats[i].x;

            // decide what cat will do
            switch ( option )
            {
            case 1:
                // up
                bin_y--;
                break;
            case 2:
                // down
                bin_y++;
                break;
            case 3:
                // left
                bin_x--;
                break;
            case 4:
                // right
                bin_x++;
                break;
            case 5: // creating another case will higher the chance of stupid kats attacking
            case 6:
                // eyes of submission
                submission = true;
                break;
            case 7:
                // purr of doom
                purr = true;
                break;
            default:
                break;
            }

            // if coordinates have changed and there is an empty space move
            if ( (bin_y != (unsigned)kats[i].y || bin_x != (unsigned)kats[i].x) )
            {
                if (board[bin_y][bin_x] == EMPTY_C)
                {
                    // put empty space in current space
                    board[kats[i].y][kats[i].x] = EMPTY_C;

                    // move kat
                    kats[i].y = bin_y;
                    kats[i].x = bin_x;

                    // put kat in new space
                    board[kats[i].y][kats[i].x] = kats[i].KATS_C;

                    // make action success
                    action_success = true;
                }

                // reset values
                bin_y = 0;
                bin_x = 0;
            }
            else if (submission)
            {
                // pull range
                bin_range = rangeCheck(board, kats[i].y, kats[i].x, bin_y, bin_x, hunter.HUNTER_C);

                // check to see if hunter is in range
                if (bin_range == 1)
                {
                    // if hunter has glasses on cut chances in half
                    if (hunter.has_glasses)
                        chance = EYES_SUBMISSION / 2;
                    else
                        chance = EYES_SUBMISSION;

                    // check to see if attack is successful
                    if ( success(chance) )
                    {
                        // tell user kat attacked
                        cout << "Kat attacks with Eyes of Submission!!" << endl;

                        con.color(12); // change color
                        if (hunter.has_glasses)
                        {
                            cout << "Reflective Glasses were ineffective and Kat was still successful..." << endl;
                        }
                        else
                        {
                            cout << "Kat attack was successful and you've lost one tuna..." << endl;
                        }
                        con.color(15);

                        hunter.health--; // Subtract one health from hunter

                        // ask user to press enter
                        cout << "Press enter to continue..." << endl << endl;
                        while ((unsigned)getch() != ENTER);
                    }
                    else
                    {
                        if (hunter.has_glasses)
                        {
                            con.color(10);
                            cout << "Reflective Glasses protected you from the attack!" << endl;
                            con.color(15);

                            // ask user to press enter
                            cout << "Press enter to continue..." << endl << endl;
                            while ((unsigned)getch() != ENTER);
                        }
                    }
                }

                // reset submission
                submission = false;

                // reset values
                bin_y = 0;
                bin_x = 0;
                bin_range = 0;
                chance = 0.0;

                // make action success
                action_success = true; // action is always successful whether or not it is in range
            }
            else if (purr)
            {
                // pull range
                bin_range = rangeCheck(board, kats[i].y, kats[i].x, bin_y, bin_x, hunter.HUNTER_C);

                // check to see if he is within range
                if ( (bin_range == 1 || bin_range == 2) && !hunter.is_stunned) // check to see if hunter isn't already stunned
                {
                    // if hunter has glasses on cut chances in half
                    if (hunter.has_muffs)
                        chance = PURRS_DOOM / 2;
                    else
                        chance = PURRS_DOOM;

                    if ( success(chance) )
                    {
                        // tell user kat attacked
                        cout << "Kat attacks with Purr of Doom!!" << endl;

                        con.color(12); // change color
                        if (hunter.has_muffs)
                        {
                            cout << "Earmuffs of Happiness were ineffective and Kat was still successful..." << endl;
                        }
                        else
                        {
                            cout << "Kat attack was successful and you've been stunned for a turn..." << endl;
                        }
                        con.color(15);

                        hunter.is_stunned = true; // stun hunter

                        // ask user to press enter
                        cout << "Press enter to continue..." << endl << endl;
                        while ((unsigned)getch() != ENTER);
                    }
                    else
                    {
                        if (hunter.has_muffs)
                        {
                            con.color(10);
                            cout << "Earmuffs of Happiness protected you from the attack!" << endl;
                            con.color(15);

                            // ask user to press enter
                            cout << "Press enter to continue..." << endl << endl;
                            while ((unsigned)getch() != ENTER);
                        }
                    }
                }

                // reset submission
                purr = false;

                // reset values
                bin_y = 0;
                bin_x = 0;
                bin_range = 0;
                chance = 0.0;

                // make action success
                action_success = true; // action is always successful whether or not it is in range
            }

        }
    }

    // go back to main
    return;
}

bool won(Cat_t kats[])
{
    // variable declarations
    unsigned int kaptured = 0;

    // check to see if they are kaptured
    for (unsigned int i = 0; i < KATS; i++)
    {
        if (kats[i].is_kaptured)
        {
            kaptured++;
        }
    }

    // checkk to see if won
    if (kaptured == KATS)
        return true;
    else
        return false;
}

unsigned int directionCheck(const Human_t hunter, const Cat_t kat)
{
    // variables declaration
    int bin_y = 0;
    int bin_x = 0;

    // check distance
    bin_y = kat.y - hunter.y;
    bin_x = kat.x - hunter.x;

    // check to see whether y or x have a greater gap
    if ( abs(bin_y) > abs(bin_x) )
    {
        if (bin_y > 0)
            return 1; // up
        if (bin_y < 0)
            return 2; // down
    }
    else
    {
        if (bin_x > 0)
            return 3; // left
        if (bin_x < 0)
            return 4; // right
    }

    return 0;
}
