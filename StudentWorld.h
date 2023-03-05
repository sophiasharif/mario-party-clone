#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Board.h"
#include <vector>
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    bool isValidPos(int x, int y);
    void handlePlayerLanding(Player* player);
    void handlePlayerCrossing(Player* player);
    void deposit(int numCoins) { m_bank +=numCoins; }
    int getBankMoney() { return m_bank; }
    void resetBank() { m_bank=0; }
    void swapPlayers();
    void swapPlayerCoins();
    void swapPlayerStars();
    std::vector<int> getRandomPos();
    bool isFork(int x, int y, bool considerDirectionalSquares);
    std::vector<int> getValidActions(int x, int y);
    void createDroppingSquare(int x, int y);
    void createVortex(Player* player);
    bool vortexImpactedBaddie(Vortex* votex);

private:
    std::string generateGameStatText();
    std::vector<Actor*> getActorsAtPos(int x, int y);
    std::vector<Actor*> m_actors;
    Player* m_peach;
    Player* m_yoshi;
    int m_bank=0;
    Actor* getSquareAtPos(int x, int y);
    Player* getWinner();
    bool actorsOverlap(Actor* actor1, Actor* actor2);
    std::vector<Actor*> actorsThatOverlapWith(Actor* actor);
    
};

#endif // STUDENTWORLD_H_
