#include "startScene.h"
#include "gameMain.h"

Scene* startScene::createScene()
{
	auto scene = Scene::create();
	auto layer = startScene::create();
	scene->addChild(layer);
	return scene;
}

bool startScene::init()
{
	if(!Layer::init())
		return false;
	time_t timeStamp = time(nullptr);
	tm *time = localtime(&timeStamp);
	//CCLOG("%d\n", time->tm_hour);
	if(time->tm_hour >= 18)
		night = true;

	this->scheduleUpdate(); // Enable to call startScene::update every frame

	visibleSize = Director::getInstance()->getVisibleSize();

	Sprite* background;
	if(night) {
		CCLOG("Is night now\n");
		background = Sprite::create("bg_night.png");
	} else {
		background = Sprite::create("bg_day.png");
	}

	scale = visibleSize.height / background->getContentSize().height;
	CCLOG("%f\n", scale);

	background->setScale(scale);
	background->setPosition(Vec2(visibleSize.width / 2.0, visibleSize.height /2.0));
	this->addChild(background);

	auto playButton = MenuItemImage::create("button_play.png", "button_play.png", CC_CALLBACK_1(startScene::playGame, this));
	auto playMenu = Menu::create(playButton, nullptr);
	auto title = Sprite::create("title.png");
	auto tutorial = Sprite::create();
	auto bird = Sprite::create();
	land[0] = Sprite::create("land.png");
	land[1] = Sprite::create("land.png");

	playButton->setScale(scale);
	title->setScale(scale);
	tutorial->setScale(scale);
	bird->setScale(scale);
	land[0]->setScale(scale);
	land[1]->setScale(scale);
	title->setPosition(Vec2(visibleSize.width / 2.0, visibleSize.height * 4.0 / 5.0));
	playMenu->setPosition(Vec2(visibleSize.width / 2.0, visibleSize.height / 4));
	tutorial->setPosition(Vec2(visibleSize.width / 2.0, visibleSize.height / 2.0));
	bird->setPosition(Vec2(visibleSize.width / 6.0 * 1, visibleSize.height / 8.0 * 3));
	land[0]->setAnchorPoint(Vec2(0, 0));
	land[1]->setAnchorPoint(Vec2(0, 0));
	landCurrentPosition[0] = Vec2(0, 0);
	landCurrentPosition[1] = Vec2(visibleSize.width, 0);
	land[0]->setPosition(landCurrentPosition[0]);
	land[1]->setPosition(landCurrentPosition[1]);

	//CCLOG("%f\n", land[1]->getContentSize().width);

	auto tutorualAnimation = Animation::create();
	auto birdAnimation = Animation::create();
	tutorualAnimation->addSpriteFrameWithFile("tutorial_0.png");
	tutorualAnimation->addSpriteFrameWithFile("tutorial_1.png");
	tutorualAnimation->setLoops(true);
	tutorualAnimation->setDelayPerUnit(1.0f / 2.0f);
	birdAnimation->addSpriteFrameWithFile("bird1_0.png");
	birdAnimation->addSpriteFrameWithFile("bird1_1.png");
	birdAnimation->addSpriteFrameWithFile("bird1_2.png");
	birdAnimation->setLoops(true);
	birdAnimation->setDelayPerUnit(1.0 / 5.0f);

	auto tutorialAnimate = Animate::create(tutorualAnimation);
	auto birdAnimate = Animate::create(birdAnimation);
	bird->runAction(RepeatForever::create(birdAnimate));
	tutorial->runAction(RepeatForever::create(tutorialAnimate));

	this->addChild(title);
	this->addChild(land[0]);
	this->addChild(land[1]);
	this->addChild(playMenu);
	this->addChild(tutorial);
	this->addChild(bird);

	return true;
}

void startScene::playGame(Ref *sender)
{
	CCLOG("Start Game");
	auto gameScene = gameMain::createScene();
	Director::getInstance()->replaceScene(gameScene);
}

void startScene::onEnter() {
	Layer::onEnter();
}

void startScene::update(float delta) {
	if(landCurrentPosition[0].x <= -land[0]->getContentSize().width * scale + 3)
		landCurrentPosition[0].x = visibleSize.width;
	else
		land[0]->setPosition(landCurrentPosition[0].x -= 3, 0);

	if(landCurrentPosition[1].x <= -land[1]->getContentSize().width * scale + 3)
		landCurrentPosition[1].x = visibleSize.width;
	else
		land[1]->setPosition(landCurrentPosition[1].x -= 3, 0);
}
