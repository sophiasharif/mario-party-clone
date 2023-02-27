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
    Player* getWinner();
    std::vector<Actor*> getActorsAtPos(int x, int y);

private:
    std::string generateGameStatText();
    std::vector<Actor*> m_actors;
    Player* m_peach;
    Player* m_yoshi;
    int m_bank=0;
};

#endif // STUDENTWORLD_H_
