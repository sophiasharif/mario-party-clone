#include "StudentWorld.h"
#include "GameConstants.h"
#include "time.h"
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

int StudentWorld::init()
{
    Board bd;
    string fileName = "board0";
    fileName += '0' + getBoardNumber();
    fileName += ".txt";
    string board_file = assetPath() + fileName; Board::LoadResult result = bd.loadBoard(board_file);
    if (result == Board::load_fail_file_not_found) {
        cerr << "Could not find " + fileName + " data file\n";
        return GWSTATUS_BOARD_ERROR;
    }
    else if (result == Board::load_fail_bad_format) {
        cerr << "Your board was improperly formatted\n";
        return GWSTATUS_BOARD_ERROR;
    }
    else if (result == Board::load_success) {
        cerr << "Successfully loaded board\n";
        for (int col=0; col<BOARD_WIDTH; col++) {
            for (int row=0; row<BOARD_HEIGHT; row++) {
                
                int x = col*SPRITE_WIDTH;
                int y = row*SPRITE_HEIGHT;
                
                Board::GridEntry ge = bd.getContentsOf(col, row);

                switch (ge) {
                    case Board::empty:
                        break;
                    case Board::player:
                        m_peach = new Player(1, this, IID_PEACH, x, y);
                        m_yoshi = new Player(2, this, IID_YOSHI, x, y);
                        m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x, y));
                        break;
                    case Board::blue_coin_square:
                        m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x, y));
                        break;
                    case Board::red_coin_square:
                        m_actors.push_back(new CoinSquare(-3, this, IID_RED_COIN_SQUARE, x, y));
                        break;
                    case Board::up_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, x, y, 90));
                        break;
                    case Board::down_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, x, y, 270));
                        break;
                    case Board::left_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, x, y, 180));
                        break;
                    case Board::right_dir_square:
                        m_actors.push_back(new DirectionalSquare(this, IID_DIR_SQUARE, x, y, 0));
                        break;
                    case Board::event_square:
                        m_actors.push_back(new EventSquare(this, IID_EVENT_SQUARE, x, y));
                        break;
                    case Board::bank_square:
                        m_actors.push_back(new BankSquare(this, IID_BANK_SQUARE, x, y));
                        break;
                    case Board::star_square:
                        m_actors.push_back(new StarSquare(this, IID_STAR_SQUARE, x, y));
                        break;
                    case Board::bowser:
                        m_actors.push_back(new Bowser(this, IID_BOWSER, x, y));
                        m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x, y));
                        break;
                    case Board::boo:
                        m_actors.push_back(new Boo(this, IID_BOO, x, y));
                        m_actors.push_back(new CoinSquare(3, this, IID_BLUE_COIN_SQUARE, x, y));
                        break;

                    default:
                        cerr << "An error occured in creating the board." << endl;
                }

            }
        }
    }
        
	startCountdownTimer(99);  // this placeholder causes timeout after 5 seconds
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_peach->doSomething();
    m_yoshi->doSomething();
    for (int i=0; i<m_actors.size(); i++) {
        if (m_actors[i]->isActive()) {
            m_actors[i]->doSomething();
        }
    }
    
    // Remove newly-inactive actors after each tick
    // remove inactive/dead game objects
    
    // Update the Game Status Line
    setGameStatText(generateGameStatText());
    
    // end of game
    if (timeRemaining() <= 0) {
        playSound(SOUND_GAME_FINISHED);
        Player* winner = getWinner();
        setFinalScore(winner->getStars(), winner->getCoins());
        if (winner == m_yoshi)
            return GWSTATUS_YOSHI_WON;
        else
            return GWSTATUS_PEACH_WON;
    }
    
	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (int i=0; i<m_actors.size(); i++) {
        delete m_actors[i];
    }
    m_actors.clear();
    
    delete m_peach;
    delete m_yoshi;
    m_peach = nullptr;
    m_yoshi = nullptr;
}

StudentWorld::~StudentWorld() {
    cleanUp();
}

bool StudentWorld::isValidPos(int x, int y) {
    for (int i=0; i<m_actors.size(); i++)
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY())
            return true;
    return false;
}

std::vector<Actor*> StudentWorld::getActorsAtPos(int x, int y) {
    std::vector<Actor*> res;
    for (int i=0; i<m_actors.size(); i++)
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY())
            res.push_back(m_actors[i]);
    return res;
}

void StudentWorld::handlePlayerLanding(Player* player) {
    std::vector<Actor*> actorsAtPos = getActorsAtPos(player->getX(), player->getY());
    for (int i=0; i<actorsAtPos.size(); i++) {
        actorsAtPos[i]->handlePlayerLanding(player);
    }
}

void StudentWorld::handlePlayerCrossing(Player* player) {
    std::vector<Actor*> actorsAtPos = getActorsAtPos(player->getX(), player->getY());
    for (int i=0; i<actorsAtPos.size(); i++) {
        actorsAtPos[i]->handlePlayerCrossing(player);
    }
}

std::string StudentWorld::generateGameStatText() {
    ostringstream gst;
    gst << "P1 Roll: " << m_peach->getRolls() << " Stars: " << m_peach->getStars()
        << " $$: " << m_peach->getCoins() << (m_peach->hasVortex() ? " VOR" : "");
    gst << " | Time: " << timeRemaining() << " | Bank: " << m_bank << " | ";
    gst << "P2 Roll: " << m_yoshi->getRolls() << " Stars: " << m_yoshi->getStars()
    << " $$: " << m_yoshi->getCoins() << (m_yoshi->hasVortex() ? " VOR" : "");
    return gst.str();
}

Player* StudentWorld::getWinner() {
    if (m_peach->getStars() > m_yoshi->getStars())
        return m_peach;
    else if (m_yoshi->getStars() > m_peach->getStars())
        return m_yoshi;
    else if (m_peach->getCoins() > m_yoshi->getCoins())
        return m_peach;
    else if (m_yoshi->getCoins() > m_peach->getCoins())
        return m_yoshi;
    else {
        return (randInt(0, 1) == 0) ? m_yoshi : m_peach;
    }
       
}
