#pragma once

#include <cocos2d.h>
#include "scoreManager.h"

USING_NS_CC;

enum actionTag {
    BIRDFLY,
    EASEIN,
    EASEOUT,
    MOVEBY,
    MOVETO
};

enum bodyType {
    BIRD,
    LAND,
    PIPE
};

class gameMain : public Layer {
public:
    virtual bool init() override;
    static Scene* createScene();
    CREATE_FUNC(gameMain);

protected:
    virtual void update(float delta) override;
    virtual void onEnter() override;
    virtual bool onTouchBegan(Touch *touch, Event *unused_event) override;
    virtual bool onContactBegin(PhysicsContact& contact);

private:
    bool night = false;
    Sprite* land[2], *pipeUp[3], *pipeDown[3];
    float scale;
    Size visibleSize;
    Vec2 landCurrentPosition[2];
    PhysicsBody* playerBody, *pipeUpBody[3], *pipeDownBody[3];
    Sprite* player;
    bool gameOver = false, adLoaded = false;
    float pipeDistance = 0;
    scoreManager userGrade;
    Label* scoreLabel;
    int lastGradePipe = -1, birdId;

    void onMouseDown(Event* event);
    void birdFly();
    void setGameTime();
    void loadGame(float delta = 0.0);
    void setupPhysics();
    float getPipeLength();
    void setPipePositionY(Sprite* pipeUp, Sprite* pipeDown, float upLength);
    void restartGame(Ref *sender);
    void GameOver();
    std::string whichBird(int i);
};
