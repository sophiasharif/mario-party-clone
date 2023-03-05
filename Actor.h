#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Player;

class Actor: public GraphObject {
public:
    Actor(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection=0, int depth=0, double size=1.0);
    virtual ~Actor() {};
    virtual void doSomething() {};
    bool isActive() { return m_isActive; }
    void setInactive() { m_isActive = false; }
    virtual void handlePlayerLanding(Player* player) {};
    virtual void handlePlayerCrossing (Player* player) {};
    virtual void handleVortex() {};
    StudentWorld* studentWorld() { return m_studentWorld; }
    // properties
    virtual bool isSquare() { return false; }
    virtual bool isEvil() { return false; }
    virtual bool isImpactable() { return false; }
    virtual bool managesDirection() { return false; }

private:
    bool m_isActive = true;
    StudentWorld* m_studentWorld;
};

class MoveableActor: public Actor {
public:
    MoveableActor(StudentWorld* studentWorld, int imageID, int startX, int startY);
    int getWalkingDirection() { return m_direction; }
    void setWalkingDirection(int dir);
    void manageSpriteDirection();
    int getTicks() { return m_ticks_to_move; }
    void setTicks(int ticks) { m_ticks_to_move = ticks; }
    bool getWaitingState() { return m_waiting; }
    void setWaitingState(bool rollState) { m_waiting = rollState; }
    virtual void doSomething();
    void teleport();

private:
    int m_direction = 0;
    int m_ticks_to_move = 0;
    bool m_waiting = true;
    virtual void handleWaitingToRoll()=0;
    virtual bool handleFork()=0;
    virtual void handleCrossing()=0;
    virtual void handleLanding()=0;
};

class Player: public MoveableActor {
public:
    Player(int playerNum, StudentWorld* studentWorld, int imageID, int startX, int startY);
    
    int getCoins() { return m_numCoins; }
    void addCoins(int numCoins) { m_numCoins += numCoins; }
   
    int getStars() { return m_numStars; }
    void addStars(int numStars) {m_numStars += numStars; }
    
    int getRolls() { return getTicks() / 8; }
    
    bool hasVortex() { return m_hasVortex; }
    void giveVortex() { m_hasVortex = true; }
    
private:
    int m_playerNum;
    int m_numCoins = 0;
    int m_numStars = 0;
    int m_numRolls = 0;
    bool m_hasVortex = false;
    void handleWaitingToRoll();
    bool handleFork();
    void handleCrossing();
    void handleLanding();
};

class Yoshi: public Player {
public:
    Yoshi(StudentWorld* studentWorld, int imageID, int startX, int startY);
};


class Square: public Actor {
public:
    Square(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection=0, int depth=1);
    virtual bool isSquare() {return true;}
};

class CoinSquare: public Square {
public:
    CoinSquare(int coinChange, StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void doSomething();
    virtual void handlePlayerLanding(Player* player);
private:
    int m_coinChange;
};

class StarSquare: public Square {
public:
    StarSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
    virtual void handlePlayerCrossing(Player *player);
};

class DirectionalSquare: public Square {
public:
    DirectionalSquare(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection);
    virtual void handlePlayerLanding(Player *player);
    virtual void handlePlayerCrossing(Player *player);
    virtual bool managesDirection() { return true; }
private:
    int m_dir;
};

class BankSquare: public Square {
public:
    BankSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
    virtual void handlePlayerCrossing(Player *player);
};

class EventSquare: public Square {
public:
    EventSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
};

class DroppingSquare: public Square {
public:
    DroppingSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
};


const int PAUSE_COUNTER = 180;

class Baddie: public MoveableActor {
public:
    Baddie(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual bool isEvil() { return true; }
    virtual bool isImpactable() { return true; }
    int getPauseCounter() {return m_pauseCounter;}
    void setPauseCounter(int num) {m_pauseCounter = num;}
    virtual void handleVortex();
    
private:
    virtual void handleWaitingToRoll();
    virtual bool handleFork();
    virtual void handleCrossing() {};
    virtual void handleLanding();
    int m_pauseCounter = PAUSE_COUNTER;
    virtual void baddieLandingAction() = 0;
};

class Bowser: public Baddie {
public:
    Bowser(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
private:
    virtual void baddieLandingAction();

};

class Boo: public Baddie {
public:
    Boo(StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void handlePlayerLanding(Player *player);
private:
    virtual void baddieLandingAction() {};
};

class Vortex: public Actor {
public:
    Vortex(StudentWorld* studentWorld, int imageID, int startX, int startY, int direction);
    virtual void doSomething();
    
private:
    int m_direction;
};

#endif // ACTOR_H_
