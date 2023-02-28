#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Player;

class Actor: public GraphObject {
public:
    Actor(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection=0, int depth=0, double size=1.0);
    virtual ~Actor() {}; // implement!
    virtual void doSomething() {};
    bool isActive() { return m_isActive; }
    void setInactive() { m_isActive = false; }
    virtual void handlePlayerLanding(Player* player) {};
    virtual void handlePlayerCrossing (Player* player) {};
    StudentWorld* studentWorld() { return m_studentWorld; }

private:
    bool m_isActive = true;
    StudentWorld* m_studentWorld;
};

class Player: public Actor {
public:
    Player(int playerNum, StudentWorld* studentWorld, int imageID, int startX, int startY);
    virtual void doSomething();
    void addCoins(int numCoins) { m_numCoins += numCoins; }
    void addStarts(int numStars) {m_numStars += numStars; }
    int getCoins() { return m_numCoins; }
    int getRolls() { return m_numRolls; }
    int getStars() { return m_numStars; }
    bool hasVortex() { return m_hasVortex; }
    void setWalkingDirection(int dir);
    
private:
    int m_ticks_to_move = 0;
    bool m_waitingToRoll = true;
    int m_direction = 0;
    int m_playerNum;
    int m_numCoins = 0;
    int m_numStars = 0;
    int m_numRolls = 0;
    bool m_hasVortex = false;
//    bool canMoveInDir(int dir);
};

class Yoshi: public Player {
public:
    Yoshi(StudentWorld* studentWorld, int imageID, int startX, int startY);
};


class Square: public Actor {
public:
    Square(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection=0, int depth=1);
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
private:
    int m_dir;
};

class BankSquare: public Square {
public:
    BankSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class EventSquare: public Square {
public:
    EventSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class DroppingSquare: public Square {
public:
    DroppingSquare(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class Baddie: public Actor {
public:
    Baddie(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class Bowser: public Baddie {
public:
    Bowser(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class Boo: public Baddie {
public:
    Boo(StudentWorld* studentWorld, int imageID, int startX, int startY);
};

class Vortex: public Actor {
public:
    Vortex(StudentWorld* studentWorld, int imageID, int startX, int startY);
};
// important things to implement for part 1!

#endif // ACTOR_H_
