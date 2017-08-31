#pragma once

#include <cocos2d.h>

USING_NS_CC;

class startScene : public Layer {
public:
    static  Scene* createScene();
    virtual bool init() override;
    virtual void onEnter() override;
    virtual void update(float delta) override;

    CREATE_FUNC(startScene);

private:
    bool night = false;
    Vec2 landCurrentPosition[2];
    Size visibleSize;
    float scale;
    Sprite* land[2];

    void playGame(Ref *sender);
};