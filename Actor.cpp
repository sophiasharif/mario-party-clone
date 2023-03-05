#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection, int depth, double size)
:GraphObject(imageID, startX, startY, startDirection, depth, size), m_studentWorld(studentWorld)
{};

MoveableActor::MoveableActor(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Actor(studentWorld, imageID, startX, startY) {}

void MoveableActor::doSomething() {
    
    // STATE 1: WAITING TO ROLL
    if (m_waiting)
        handleWaitingToRoll();
    
    // STATE 2: WALKING
    if (!m_waiting) {
        
        // if it's a fork, make actor handle it
        if (!handleFork())
            return;
        
        // move
        manageSpriteDirection();
        moveAtAngle(getWalkingDirection(), 2);
        m_ticks_to_move--;
        
        // handle landing or crossing; landing has precedence over crossing
        if (m_ticks_to_move == 0) {
            setWaitingState(true);
            handleLanding();
        }
        else
            handleCrossing();
    }
}

void MoveableActor::manageSpriteDirection() {
    // if we're facing horizontally and perfectly aligned on a square and there is no square to move on to, change direction
    if (getX() % SPRITE_WIDTH == 0 && (
            (getWalkingDirection() == 0 && !studentWorld()->isValidPos(getX()+SPRITE_WIDTH, getY())) ||
            (getWalkingDirection() == 180 && !studentWorld()->isValidPos(getX()-SPRITE_WIDTH, getY()))
        )) {
        if (studentWorld()->isValidPos(getX(), getY()+SPRITE_WIDTH)) {
            setWalkingDirection(90);
        }
        else {
            setWalkingDirection(270);
        }
    }
    // same for vertical
    else if (getY() % SPRITE_WIDTH == 0 && (
            (getWalkingDirection() == 90 && !studentWorld()->isValidPos(getX(), getY()+SPRITE_HEIGHT)) ||
            (getWalkingDirection() == 270 && !studentWorld()->isValidPos(getX(), getY()-SPRITE_HEIGHT))
        )) {
        if (studentWorld()->isValidPos(getX()+SPRITE_WIDTH, getY())) {
            setWalkingDirection(0);
        }
        else {
            setWalkingDirection(180);
        }
           
    }
}

void MoveableActor::setWalkingDirection(int dir) {
    switch (dir) {
        case 0:
            m_direction=0;
            setDirection(0);
            break;
        case 90:
            m_direction=90;
            setDirection(0);
            break;
        case 180:
            m_direction=180;
            setDirection(180);
            break;
        case 270:
            m_direction=270;
            setDirection(0);
            break;
    }
}

void MoveableActor::teleport() {
    std::vector<int> coords = studentWorld()->getRandomPos();
    moveTo(coords[0], coords[1]);
}

int invalidAction(int dir) {
    switch(dir) {
        case 0:
            return ACTION_LEFT;
        case 90:
            return ACTION_DOWN;
        case 180:
            return ACTION_LEFT;
        case 270:
            return ACTION_UP;
        default:
            std::cerr << "INVALID ACTION IN HELPER" << std::endl;
            exit(1);
    }
}

int actionToDirection(int action) {
    switch(action) {
        case ACTION_RIGHT:
            return 0;
        case ACTION_UP:
            return 90;
        case ACTION_LEFT:
            return 180;
        case ACTION_DOWN:
            return 270;
        default:
            std::cerr << "INVALID ACTION IN HELPER" << std::endl;
            exit(1);
    }
}

// PLAYERS
Player::Player(int playerNum, StudentWorld* studentWorld, int imageID, int startX, int startY)
:MoveableActor(studentWorld, imageID, startX, startY), m_playerNum(playerNum)
{};

void Player::handleCrossing()  { studentWorld()->handlePlayerCrossing(this);
}

void Player::handleLanding() {
    studentWorld()->handlePlayerLanding(this);
}

void Player::handleWaitingToRoll() {
    int action = studentWorld()->getAction(m_playerNum);
    // get action
    if (action == ACTION_ROLL) {
        int die_roll = randInt(1, 10);
        setTicks(die_roll * 8);
        setWaitingState(false);
    } else if (action == ACTION_FIRE && hasVortex()) {
        studentWorld()->createVortex(this);
        m_hasVortex = false;
    }
}

bool Player::handleFork() {
    // if no fork, no need to handle
    if (!studentWorld()->isFork(getX(), getY(), true))
        return true;
    
    // wait for player to choose a direction
    int action = studentWorld()->getAction(m_playerNum);
    
    std::vector<int> validActions = studentWorld()->getValidActions(getX(), getY());
    
    // check action is valid
    for (int i=0; i<validActions.size(); i++) {
        if (action == validActions[i] && !(action == invalidAction(getWalkingDirection()))) {
            setWalkingDirection(actionToDirection(action));
            return true;
        }
    }
    
    return false;
}

// SQUARES
Square::Square(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection, int depth)
:Actor(studentWorld, imageID, startX, startY, startDirection, depth) {}

StarSquare::StarSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

void StarSquare::handlePlayerLanding(Player *player) {
    handlePlayerCrossing(player);
}

void StarSquare::handlePlayerCrossing(Player *player) {
    if (player->getCoins() < 20) return;
    player->addCoins(-20);
    player->addStars(1);
    studentWorld()->playSound(SOUND_GIVE_STAR);
}

DirectionalSquare::DirectionalSquare(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection)
:Square(studentWorld, imageID, startX, startY, startDirection), m_dir(startDirection) {}

void DirectionalSquare::handlePlayerLanding(Player *player) {
    player->setWalkingDirection(m_dir);
}
void DirectionalSquare::handlePlayerCrossing(Player *player) {
    player->setWalkingDirection(m_dir);
}

BankSquare::BankSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

void BankSquare::handlePlayerCrossing(Player *player) {
    int coinsTaken = std::min(player->getCoins(), 5);
    player->addCoins(-coinsTaken);
    studentWorld()->deposit(coinsTaken);
    studentWorld()->playSound(SOUND_DEPOSIT_BANK);
}

void BankSquare::handlePlayerLanding(Player *player) {
    int coinsToAdd = studentWorld()->getBankMoney();
    player->addCoins(coinsToAdd);
    studentWorld()->resetBank();
    studentWorld()->playSound(SOUND_WITHDRAW_BANK);
}

EventSquare::EventSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

void EventSquare::handlePlayerLanding(Player *player) {
    // choose an action
    int action = randInt(1, 3);
    
    if (action == 1) {
        player->teleport();
        studentWorld()->playSound(SOUND_PLAYER_TELEPORT);
    }
    
    else if (action == 2) {
        studentWorld()->swapPlayers();
        studentWorld()->playSound(SOUND_PLAYER_TELEPORT);
    }
        
    else {
        player->giveVortex();
        studentWorld()->playSound(SOUND_GIVE_VORTEX);
    }
}

DroppingSquare::DroppingSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

void DroppingSquare::handlePlayerLanding(Player *player) {
    int action = randInt(1, 2);
    if (action == 1) {
        // update coins
        int coins = player->getCoins();
        if (coins < 10) {
            player->addCoins(-coins);
        } else {
            player->addCoins(-10);
        }
    } else {
        // subtract a star if a player has any
        if (player->getStars() > 0) {
            player->addStars(-1);
        }
    }
    studentWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
}

CoinSquare::CoinSquare(int coinChange, StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY), m_coinChange(coinChange) {}

void CoinSquare::doSomething() {
    if (!isActive()) {
        return;
    }
}

void CoinSquare::handlePlayerLanding(Player *player) {
    player->addCoins(m_coinChange);
    if (m_coinChange > 0)
        studentWorld()->playSound(SOUND_GIVE_COIN);
    else
        studentWorld()->playSound(SOUND_TAKE_COIN);
};


// BADDIES


Baddie::Baddie(StudentWorld* studentWorld, int imageID, int startX, int startY)
:MoveableActor(studentWorld, imageID, startX, startY)
{}

void Baddie::handleVortex() {
    teleport();
    setWalkingDirection(0);
    setWaitingState(true);
    m_pauseCounter = PAUSE_COUNTER;
}

void Baddie::handleWaitingToRoll(){
    m_pauseCounter--;
    if (m_pauseCounter == 0) {
        
        // choose a random direction
        std::vector<int> validActions = studentWorld()->getValidActions(getX(), getY());
        
        int randomAction = validActions[randInt(0, validActions.size()-1)];
        
        setWalkingDirection(actionToDirection(randomAction));
        
        // set moves
        int squaresToMove = randInt(1, 10);
        setTicks(squaresToMove * 8);
        setWaitingState(false);
    }
};

bool Baddie::handleFork(){
    // choose a random direction
    if (!studentWorld()->isFork(getX(), getY(), false))
        return true;
    
    std::vector<int> validActions = studentWorld()->getValidActions(getX(), getY());
    
    int randomAction = validActions[randInt(0, validActions.size()-1)];
    
    setWalkingDirection(actionToDirection(randomAction));
    
    return true;
    
}

void Baddie::handleLanding(){
    m_pauseCounter = PAUSE_COUNTER;
    baddieLandingAction();
};

Bowser::Bowser(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Baddie(studentWorld, imageID, startX, startY)
{}

void Bowser::handlePlayerLanding(Player *player) {
    if (getWaitingState() && (randInt(1, 2) == 1)) {
        player->addCoins(-player->getCoins());
        player->addStars(-player->getStars());
        studentWorld()->playSound(SOUND_BOWSER_ACTIVATE);
    }
}

void Bowser::baddieLandingAction() {
    // 25% chance of creating a dropping square
    if (randInt(1, 4) == 1)
        studentWorld()->createDroppingSquare(getX(), getY());
}

Boo::Boo(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Baddie(studentWorld, imageID, startX, startY)
{}

void Boo::handlePlayerLanding(Player *player) {
    if (getWaitingState()) {
        if (randInt(1, 2) == 1)
            studentWorld()->swapPlayerCoins();
        else
            studentWorld()->swapPlayerStars();
       
        studentWorld()->playSound(SOUND_BOO_ACTIVATE);
    }
}


// VORTEX

Vortex::Vortex(StudentWorld* studentWorld, int imageID, int startX, int startY, int direction)
:Actor(studentWorld, imageID, startX, startY), m_direction(direction)
{}

void Vortex::doSomething() {
    if (!isActive())
        return;
    
    moveAtAngle(m_direction, 2);
    
    // check it's in a valid position on the screen
    if (getX() < 0 || getX() >= VIEW_WIDTH || getY() < 0 || getY() >= VIEW_HEIGHT)
        setInactive();
    
    if (studentWorld()->vortexImpactedBaddie(this)) {
        setInactive();
        studentWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
        
}
