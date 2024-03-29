#include "StudentWorld.h"
#include "GameConstants.h"
#include "time.h"
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
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
    // load board
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
                
                // calculate x,y position
                int x = col*SPRITE_WIDTH;
                int y = row*SPRITE_HEIGHT;
                
                Board::GridEntry ge = bd.getContentsOf(col, row);
                
                // create the appropriate player
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
        
	startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_peach->doSomething();
    m_yoshi->doSomething();
    for (int i=0; i<m_actors.size(); i++) {
        if (m_actors[i]->isActive()) {
            // allow each player to do something
            m_actors[i]->doSomething();
        } else {
            // if player is inactive, delete its memory and remove its pointer from the m_actors vector
            delete m_actors[i];
            m_actors.erase(m_actors.begin()+i);
        }
    }
    

    
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
    // iterate through actors and add actors with matching (x,y) to vector
    for (int i=0; i<m_actors.size(); i++)
        if (x == m_actors[i]->getX() && y == m_actors[i]->getY())
            res.push_back(m_actors[i]);
    return res;
}

void StudentWorld::handlePlayerLanding(Player* player) {
    // call handlePlayerLanding method of all the actors at the player's (x,y) position
    std::vector<Actor*> actorsAtPos = getActorsAtPos(player->getX(), player->getY());
    for (int i=0; i<actorsAtPos.size(); i++) {
        actorsAtPos[i]->handlePlayerLanding(player);
    }
}

void StudentWorld::handlePlayerCrossing(Player* player) {
    // only run function if player is exactly over a square
    if (!isValidPos(player->getX(), player->getY()))
        return;
    
    // call handlePlayerCrossing method of all actors at player's (x,y)
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

void StudentWorld::swapPlayers() {
//    i. x, y coordinates
    int x = m_peach->getX(), y=m_peach->getY();
    m_peach->moveTo(m_yoshi->getX(), m_yoshi->getY());
    m_yoshi->moveTo(x, y);
    
//    ii. the number of ticks left that the player has to move before
//    completing their roll
    int ticks = m_peach->getTicks();
    m_peach->setTicks(m_yoshi->getTicks());
    m_yoshi->setTicks(ticks);
    
//    iii. the player's walk direction
//    iv. the player's sprite direction
    int dir = m_peach->getWalkingDirection();
    m_peach->setWalkingDirection(m_yoshi->getWalkingDirection());
    m_yoshi->setWalkingDirection(dir);

//    v. the player's roll/walk state
    bool state = m_peach->getWaitingState();
    m_peach->setWaitingState(m_yoshi->getWaitingState());
    m_yoshi->setWaitingState(state);
    
}

std::vector<int> StudentWorld::getRandomPos() {
    std::vector<int> coords;
    
    int rand = randInt(0, m_actors.size()-1);
    coords.push_back(m_actors[rand]->getX());
    coords.push_back(m_actors[rand]->getY());
    
    return coords;
}

Actor* StudentWorld::getSquareAtPos(int x, int y) {
    std::vector<Actor*> actorsAtPos = getActorsAtPos(x, y);
    for (int i=0; i<actorsAtPos.size(); i++)
        if (actorsAtPos[i]->isSquare())
            return actorsAtPos[i];
    
    return nullptr;
}

bool StudentWorld::isFork(int x, int y, bool considerDirectionalSquares) {
    
    if (!isValidPos(x, y))
        return false;
    
    // ignore fork if directional square
    if (considerDirectionalSquares && getSquareAtPos(x, y) &&  getSquareAtPos(x, y)->managesDirection())
        return false;
    
    return getValidActions(x, y).size() > 2;
}

std::vector<int> StudentWorld::getValidActions(int x, int y) {
    std::vector<int> validActions;
    int dirs[4][3] = {{SPRITE_WIDTH, 0, ACTION_RIGHT}, {-SPRITE_WIDTH, 0, ACTION_LEFT}, {0, SPRITE_HEIGHT, ACTION_UP}, {0,-SPRITE_HEIGHT, ACTION_DOWN}};
    for (int i=0; i<4; i++) {
        int dx = dirs[i][0];
        int dy = dirs[i][1];
        Actor* square = getSquareAtPos(x+dx, y+dy);
        if (square)
            validActions.push_back(dirs[i][2]);
    }
    
    return validActions;
}

void StudentWorld::swapPlayerCoins() {
    int temp = m_yoshi->getCoins();
    m_yoshi->addCoins(-m_yoshi->getCoins()+m_peach->getCoins());
    m_peach->addCoins(-m_peach->getCoins()+temp);
};

void StudentWorld::swapPlayerStars() {
    int temp = m_yoshi->getStars();
    m_yoshi->addStars(-m_yoshi->getStars()+m_peach->getStars());
    m_peach->addStars(-m_peach->getStars()+temp);
};


void StudentWorld::createDroppingSquare(int x, int y) {
    // find square with those coords
    Actor* square = nullptr;
    for (int i=0; i<m_actors.size(); i++) {
        if (m_actors[i]->getX() == x && m_actors[i]->getY() == y && m_actors[i]->isSquare()) {
            square = m_actors[i];
            break;
        }
    }
    if (!square){
        std::cerr << "SQUARE DOES NOT EXIST" << std::endl;
        exit(1);
    }
    square->setInactive();
    m_actors.push_back(new DroppingSquare(this, IID_DROPPING_SQUARE, x, y));
    playSound(SOUND_DROPPING_SQUARE_CREATED);
    
}

void StudentWorld::createVortex(Player *player) {
    int dir = player->getWalkingDirection();
    int x = player->getX();
    int y = player->getY();
    m_actors.push_back(new Vortex(this, IID_VORTEX, x, y, dir));
    playSound(SOUND_PLAYER_FIRE);
}

bool StudentWorld::actorsOverlap(Actor* actor1, Actor* actor2) {
    int bigX = std::max(actor1->getX(), actor2->getX());
    int smallX = std::min(actor1->getX(), actor2->getX());
    int bigY = std::max(actor1->getY(), actor2->getY());
    int smallY = std::min(actor1->getY(), actor2->getY());
    
    // overlap horizontally
    if (bigY == smallY && (bigX - smallX < SPRITE_WIDTH)) return true;
    // overlap vertially
    if (bigX == smallX && (bigY - smallY < SPRITE_HEIGHT)) return true;
    
    return false;
}

std::vector<Actor*> StudentWorld::actorsThatOverlapWith(Actor* actor) {
    vector<Actor*> res;
    for (int i = 0; i<m_actors.size(); i++) {
        if (actorsOverlap(m_actors[i], actor))
            res.push_back(m_actors[i]);
    }
    return res;
}

bool StudentWorld::vortexImpactedBaddie(Vortex* votex) {
    vector<Actor*> overlappingActors = actorsThatOverlapWith(votex);
    for (int i=0; i<overlappingActors.size(); i++) {
        Actor* actor = overlappingActors[i];
        if (actor->isImpactable()) {
            actor->handleVortex();
            return true;
        }
    }
    return false;
}

