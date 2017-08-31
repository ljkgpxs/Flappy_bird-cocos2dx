//
// Created by ljkgpxs on 17-4-19.
//

#pragma once
#include <cocos2d.h>

USING_NS_CC;

class scoreManager : public UserDefault {
public:
    unsigned long getScore();
    unsigned long getBestScore();
    scoreManager();
    ~scoreManager();
    scoreManager operator++(int a); // i++
    scoreManager operator++(); // ++i
    bool currentIsBestScore();

private:
    unsigned long bestScore = 0;
    unsigned long score = 0;
    bool setBestScore = false;

    unsigned long _getBestScore();


};
