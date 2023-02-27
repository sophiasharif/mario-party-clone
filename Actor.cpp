#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection, int depth, double size)
:GraphObject(imageID, startX, startY, startDirection, depth, size), m_studentWorld(studentWorld)
{};

void Actor::doSomething(){}

void Actor::handlePlayerInteraction(Player* player) {};


// PLAYERS
Player::Player(int playerNum, StudentWorld* studentWorld, int imageID, int startX, int startY)
:Actor(studentWorld, imageID, startX, startY), m_playerNum(playerNum)
{};

void Player::doSomething() {
    if (m_waitingToRoll) {
        int action = studentWorld()->getAction(m_playerNum);
        if (action == ACTION_ROLL) {
            int die_roll = randInt(1, 10);
            m_numRolls = die_roll;
            m_ticks_to_move = die_roll * 8;
            m_waitingToRoll = false;
        }
        else {
            return;
        }
    }
    
    if (!m_waitingToRoll) {
        
        // handle rolls
        if (studentWorld()->isValidPos(getX(), getY()))
            m_numRolls--;
        
        // handle direction change
        if (getX() % SPRITE_WIDTH == 0 && (
                (m_direction == 0 && !studentWorld()->isValidPos(getX()+SPRITE_WIDTH, getY())) ||
                (m_direction == 180 && !studentWorld()->isValidPos(getX()-SPRITE_WIDTH, getY()))
            )) {
            if (studentWorld()->isValidPos(getX(), getY()+SPRITE_WIDTH)) {
                m_direction = 90;
                setDirection(0);
            }
            else {
                m_direction = 270;
                setDirection(0);
            }    
        }
        else if (getY() % SPRITE_WIDTH == 0 && (
                (m_direction == 90 && !studentWorld()->isValidPos(getX(), getY()+SPRITE_HEIGHT)) ||
                (m_direction == 270 && !studentWorld()->isValidPos(getX(), getY()-SPRITE_HEIGHT))
            )) {
            if (studentWorld()->isValidPos(getX()+SPRITE_WIDTH, getY())) {
                m_direction = 0;
                setDirection(0);
            }
            else {
                m_direction = 180;
                setDirection(180);
            }
               
        }

//        if (getDirection() % 180 == 0) {
//            if (getX() % 16 == 0)
//        }
        moveAtAngle(m_direction, 2);
        m_ticks_to_move -= 1;
        if (m_ticks_to_move == 0) {
            studentWorld()->handlePlayerLanding(this);
            m_waitingToRoll = true;
        }
    }
    
//    switch(action) {
//        case ACTION_LEFT:
//            moveTo(getX()-2, getY());
//
//    }
// if the player presses the left arrow key:
    //    set the avatar's direction to left
    //    move the avatar two pixels forward
// if the player presses the right arrow key:
    //    set the avatar's direction to right
    //    move the avatar two pixels forward
    

//    if the player presses the space key:
//    add a new projectile directly in front of the player
//    decrement the ammunition count by one
}


// SQUARES
Square::Square(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection, int depth)
:Actor(studentWorld, imageID, startX, startY, startDirection, depth) {}

StarSquare::StarSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

DirectionalSquare::DirectionalSquare(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection)
:Square(studentWorld, imageID, startX, startY, startDirection) {}

BankSquare::BankSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

EventSquare::EventSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

DroppingSquare::DroppingSquare(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY) {}

CoinSquare::CoinSquare(int coinChange, StudentWorld* studentWorld, int imageID, int startX, int startY)
:Square(studentWorld, imageID, startX, startY), m_coinChange(coinChange) {}

void CoinSquare::doSomething() {
    if (!isActive()) {
        return;
    }
}

void CoinSquare::handlePlayerInteraction(Player *player) {
    player->addCoins(m_coinChange);
    if (m_coinChange > 0)
        studentWorld()->playSound(SOUND_GIVE_COIN);
    else
        studentWorld()->playSound(SOUND_TAKE_COIN);
    std::cerr << "player now has " << player->getCoins() << " coins." << std::endl;
};


// BADDIES
Baddie::Baddie(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Actor(studentWorld, imageID, startX, startY)
{}

Bowser::Bowser(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Baddie(studentWorld, imageID, startX, startY)
{}

Boo::Boo(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Baddie(studentWorld, imageID, startX, startY)
{}

// VORTEX

Vortex::Vortex(StudentWorld* studentWorld, int imageID, int startX, int startY)
:Actor(studentWorld, imageID, startX, startY)
{}
