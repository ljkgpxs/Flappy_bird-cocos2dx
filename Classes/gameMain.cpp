#include "gameMain.h"
#ifdef SDKBOX_ENABLED
#include "PluginAdMob/PluginAdMob.h"
#endif

bool gameMain::init()
{
	if(!Layer::init())
		return false;
	setGameTime();

	this->scheduleUpdate();

	visibleSize = Director::getInstance()->getVisibleSize();
	pipeDistance = (float) (visibleSize.height / 5.0);
	birdId = (int)random(0.0f, 2.0f);

	//scheduleOnce(schedule_selector(gameMain::loadGame), 2.0);
	loadGame();


	return true;
}

Scene *gameMain::createScene()
{
	auto scene = Scene::createWithPhysics();
	auto graveity = Vec2(0, -430);
	scene->getPhysicsWorld()->setGravity(graveity);
	//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	auto layer = gameMain::create();
	scene->addChild(layer);
	return scene;
}


void gameMain::update(float )
{
#ifdef SDKBOX_ENABLED
	if(!adLoaded) {
		if(sdkbox::PluginAdMob::isAvailable("testad")) {
			sdkbox::PluginAdMob::show("testad");
			adLoaded = true;
		} else {
		    CCLOG("----Ad not loaded\n");
		}
	}
#endif

	//if(gameReady)
	//	return;

	if(gameOver) {
		//playerBody->applyImpulse(Vec2(0, -200));
		this->unscheduleUpdate();
		GameOver();
		player->stopAllActions();
		return;
		//CCLOG("Game Over");
	}
	if(landCurrentPosition[0].x <= -land[0]->getContentSize().width * scale + 3.0)
		landCurrentPosition[0].x = visibleSize.width;
	else
		land[0]->setPosition(landCurrentPosition[0].x -= 3.0, 0);

	if(landCurrentPosition[1].x <= -land[1]->getContentSize().width * scale + 3.0)
		landCurrentPosition[1].x = visibleSize.width;
	else
		land[1]->setPosition(landCurrentPosition[1].x -= 3.0, 0);
	if(player->getPosition().y + player->getContentSize().height / 2.0 >= visibleSize.height) {
		player->stopActionByTag(EASEIN);
		player->setPosition(player->getPosition().x,
				    (float) (visibleSize.height - player->getContentSize().height / 2.0));
		CCLOG("Warning");
	}

	for(int i = 0; i < 3; ++i) {
		if(pipeUp[i]->getPosition().x <= -pipeUp[i]->getContentSize().width * scale + 3) {
			pipeUp[i]->setPositionX((float) (visibleSize.width + pipeUp[i]->getContentSize().width + visibleSize.width / 4.0));
			pipeDown[i]->setPositionX((float) (visibleSize.width + pipeDown[i]->getContentSize().width + visibleSize.width / 4.0));
			setPipePositionY(pipeUp[i], pipeDown[i], getPipeLength());
		} else {
			pipeUp[i]->setPositionX(pipeUp[i]->getPosition().x - 3.0);
			pipeDown[i]->setPositionX(pipeDown[i]->getPosition().x - 3.0);
		}


		if(pipeUp[i]->getPosition().x + pipeUp[i]->getContentSize().width * scale < player->getPosition().x - player->getContentSize().width * scale / 2.0 && lastGradePipe != i) {
			++userGrade;
			char scoreText[20];
			sprintf(scoreText, "%ld", userGrade.getScore());
			scoreLabel->setString(scoreText);
			lastGradePipe = i;
			CCLOG("Current score %ld", userGrade.getScore());
		}
	}
}

void gameMain::onEnter()
{
	Layer::onEnter();
	//auto mouseListener = EventListenerMouse::create();
	auto keyListener = EventListenerKeyboard::create();
	auto touchListener = EventListenerTouchOneByOne::create();
	auto physicsContact = EventListenerPhysicsContact::create();
	physicsContact->onContactBegin = CC_CALLBACK_1(gameMain::onContactBegin, this);
	//mouseListener->onMouseDown = CC_CALLBACK_1(gameMain::onMouseDown, this);
	touchListener->onTouchBegan = CC_CALLBACK_2(gameMain::onTouchBegan, this);

	//_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(physicsContact, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
}


bool gameMain::onTouchBegan(Touch *, Event *) {
	//if(gameReady)
	//	return true;

	if(!gameOver)
		birdFly();
	return true;
}

bool gameMain::onContactBegin(PhysicsContact &contact)
{
	CCLOG("Contacting");
	if(gameOver)
		playerBody->setVelocity(Vec2(0, 0));
	int shapeATag = contact.getShapeA()->getBody()->getTag();
	int shapeBTag = contact.getShapeB()->getBody()->getTag();
	CCLOG("%d %d", shapeATag, contact.getShapeB()->getTag());
	if(shapeATag == LAND || shapeBTag == LAND || shapeATag == PIPE || shapeBTag == PIPE)
		gameOver = true;
	return true;
}

void gameMain::birdFly()
{
	playerBody->getNode()->stopActionByTag(EASEIN);
	//playerBody->setEnabled(false);
	playerBody->setGravityEnable(false);
	playerBody->setVelocity(Vec2(0, 0));
	auto moveBy = MoveBy::create(0.53f, Vec2(0, (float) (visibleSize.height / 6.5)));
	auto easeIn = EaseIn::create(moveBy, 0.6f);
	easeIn->setTag(EASEIN);
	playerBody->getNode()->runAction(easeIn);
	//playerBody->setEnabled(true);
	playerBody->setGravityEnable(true);
}

void gameMain::setGameTime()
{
	time_t timeStamp = time(nullptr);
	tm *time = localtime(&timeStamp);
	//CCLOG("%d\n", time->tm_hour);
	if(time->tm_hour >= 18 || time->tm_hour <= 7)
		night = true;
}

void gameMain::loadGame(float )
{
	Sprite* background;
	if(night) {
		CCLOG("Is night now\n");
		background = Sprite::create("bg_night.png");
	} else {
		background = Sprite::create("bg_day.png");
	}

	scale = visibleSize.height / background->getContentSize().height;

	land[0] = Sprite::create("land.png");
	land[1] = Sprite::create("land.png");
	player = Sprite::create("bird1_0.png");
	scoreLabel = Label::createWithBMFont("number_font.fnt", "0");

	for(int i = 0; i < 3; i++) {
		if(!night) {
			pipeUp[i] = Sprite::create("pipe_up.png");
			pipeDown[i] = Sprite::create("pipe_down.png");
		} else {
			pipeUp[i] = Sprite::create("pipe2_up.png");
			pipeDown[i] = Sprite::create("pipe2_down.png");
		}
		pipeUp[i]->setScale(scale);
		pipeDown[i]->setScale(scale);
	}

	CCLOG("Scale %f\n", scale);
	if(!land[0])
		CCLOG("Unable to create land sprite");


	land[0]->setScale(scale);
	land[1]->setScale(scale);
	background->setScale(scale);
	player->setScale(scale);
	scoreLabel->setScale(scale);

	land[0]->setAnchorPoint(Vec2(0, 0));
	land[1]->setAnchorPoint(Vec2(0, 0));
	landCurrentPosition[0] = Vec2(0, 0);
	landCurrentPosition[1] = Vec2(visibleSize.width, 0);
	land[0]->setPosition(landCurrentPosition[0]);
	land[1]->setPosition(landCurrentPosition[1]);
	background->setPosition(Vec2((float) (visibleSize.width / 2.0), (float) (visibleSize.height / 2.0)));
	player->setPosition(Vec2((float) (visibleSize.width / 6.0 * 1), (float) (visibleSize.height / 8.0 * 3)));
	scoreLabel->setPosition(Vec2((float) (visibleSize.width / 2.0), (float) (visibleSize.height / 6.0 * 5.0)));

	for(int i = 0; i < 3; i++) {
		pipeUp[i]->setAnchorPoint(Vec2(0, 1));
		pipeDown[i]->setAnchorPoint(Vec2(0, 0));
		pipeUp[i]->setPositionX(visibleSize.width / 4 * (2 * i + 3));
		pipeDown[i]->setPositionX(visibleSize.width / 4 * (2 * i + 3));
		setPipePositionY(pipeUp[i], pipeDown[i], getPipeLength());
	}

	auto playerAnimation = Animation::create();
	playerAnimation->addSpriteFrameWithFile(whichBird(0));
	playerAnimation->addSpriteFrameWithFile(whichBird(1));
	playerAnimation->addSpriteFrameWithFile(whichBird(2));
	playerAnimation->setDelayPerUnit((float) (1.0 / 5.0));
	//playerAnimation->setLoops(true);

	auto playerAnimate = Animate::create(playerAnimation);
	//playerAnimate->setTag(BIRDFLY);
	auto repeatPlayerAnimate = RepeatForever::create(playerAnimate);
	repeatPlayerAnimate->setTag(BIRDFLY);
	player->runAction(repeatPlayerAnimate);

	setupPhysics();

	this->addChild(background);

	for(int i = 0; i < 3; i++) {
		//CCLOG("%d", i);
		this->addChild(pipeUp[i]);
		this->addChild(pipeDown[i]);
	}



	this->addChild(land[0]);
	this->addChild(land[1]);
	this->addChild(player);
	this->addChild(scoreLabel);


	/*
	gameLayer->addChild(background);
	gameLayer->addChild(land[0]);
	gameLayer->addChild(land[1]);
	gameLayer->addChild(player);
	gameLayer->addChild(scoreLabel);
	 */
	//gameReady = false;
	//this->addChild(gameLayer);
}

void gameMain::setupPhysics()
{
	PhysicsBody* landBody[2];
	for(int i = 0; i < 2; ++i) {
		landBody[i] = PhysicsBody::createBox(land[i]->getContentSize(), PHYSICSBODY_MATERIAL_DEFAULT);
		landBody[i]->setTag(LAND);
		landBody[i]->setDynamic(false);
		landBody[i]->setCategoryBitmask(0x10);
		landBody[i]->setContactTestBitmask(0x01);
		landBody[i]->setCollisionBitmask(0x01);
		land[i]->setPhysicsBody(landBody[i]);
	}

	for(int i = 0; i < 3; ++i) {
		pipeDownBody[i] = PhysicsBody::createBox(pipeDown[i]->getContentSize(), PhysicsMaterial(1.0f, 0.0f, 0.0f));
		pipeUpBody[i] = PhysicsBody::createBox(pipeUp[i]->getContentSize(), PhysicsMaterial(1.0f, 0, 0));
		pipeUpBody[i]->setDynamic(false);
		pipeDownBody[i]->setDynamic(false);
		pipeDown[i]->setPhysicsBody(pipeDownBody[i]);
		pipeUp[i]->setPhysicsBody(pipeUpBody[i]);
		pipeUpBody[i]->setCategoryBitmask(0x100);
		pipeDownBody[i]->setCategoryBitmask(0x100);
		pipeUpBody[i]->setContactTestBitmask(0x101);
		pipeDownBody[i]->setContactTestBitmask(0x101);
		pipeUpBody[i]->setCollisionBitmask(0x101);
		pipeDownBody[i]->setCollisionBitmask(0x101);
		pipeDownBody[i]->setTag(PIPE);
		pipeUpBody[i]->setTag(PIPE);
	}

	playerBody = PhysicsBody::createCircle((float) (player->getContentSize().width / 4.0), PhysicsMaterial(0.001f, 0.0f, 0.0f));
	playerBody->setGravityEnable(true);
	playerBody->setTag(BIRD);
	playerBody->setCategoryBitmask(0x01);
	playerBody->setContactTestBitmask(0x110);
	playerBody->setCollisionBitmask(0x110);
	player->addComponent(playerBody);
	//CCLOG("land0 %d", landBody[0]->getTag());
}

float gameMain::getPipeLength()
{
	return float(random(pipeUp[0]->getContentSize().height * scale / 6.0,
			    pipeUp[0]->getContentSize().height * scale - pipeUp[0]->getContentSize().height * scale / 6.0));
}

void gameMain::setPipePositionY(Sprite *pipeUp, Sprite *pipeDown, float upLength)
{
	//pipeDown->setAnchorPoint(Vec2(0, 0));
	pipeDown->setPositionY(visibleSize.height - upLength);
	//pipeUp->setAnchorPoint(Vec2(0, 1));
	pipeUp->setPositionY(visibleSize.height - upLength - pipeDistance);
}

void gameMain::GameOver()
{
	char scoreFont[20], bestScoreFont[20];
	sprintf(scoreFont, "%ld", userGrade.getScore());
	sprintf(bestScoreFont, "%ld", userGrade.getBestScore());
	auto scorePanel = Sprite::create("score_panel.png");
	auto userScoreLabel = Label::createWithBMFont("number_score_font.fnt", scoreFont);
	auto bestScoreLabel = Label::createWithBMFont("number_score_font.fnt", bestScoreFont);
	Sprite *medal = nullptr, *star = nullptr, *gameOverText = nullptr, *newImg = nullptr;
	auto restartButton = MenuItemImage::create("button_play.png", "button_play.png", CC_CALLBACK_1(gameMain::restartGame, this));
	auto restartMenu = Menu::create(restartButton, nullptr);
	restartButton->setScale(scale);
	this->removeChild(scoreLabel);

	gameOverText = Sprite::create("text_game_over.png");
	gameOverText->setScale(scale);
	scorePanel->setScale(scale);
	scorePanel->setPosition(Vec2((float) (visibleSize.width / 2.0), (float) (visibleSize.height / 2.0)));
	gameOverText->setPosition(Vec2((float) (visibleSize.width / 2.0), (float) (visibleSize.height / 5.0 * 4.0)));
	restartMenu->setPosition(Vec2((float) (visibleSize.width / 2.0), (float) (visibleSize.height / 4.0)));

	scorePanel->addChild(userScoreLabel);
	scorePanel->addChild(bestScoreLabel);
	userScoreLabel->setAnchorPoint(Vec2(1, 0.5));
	userScoreLabel->setPosition((float) (scorePanel->getContentSize().width * 0.9),
				    (float) (scorePanel->getContentSize().height * 0.6));
	bestScoreLabel->setAnchorPoint(Vec2(1, 0.5));
	bestScoreLabel->setPosition((float) (scorePanel->getContentSize().width * 0.9),
				    (float) (scorePanel->getContentSize().height * 0.25));
	//userScoreLabel->setPosition(0.5, 0.5);

	if (userGrade.getScore() >= 200)
		medal = Sprite::create("medals_0.png");
	else if (userGrade.getScore() >= 80)
		medal = Sprite::create("medals_1.png");
	else if (userGrade.getScore() >= 40)
		medal = Sprite::create("medals_2.png");
	else if (userGrade.getScore() >= 20)
		medal = Sprite::create("medals_3.png");
	if (medal) {
		star = Sprite::create();
		auto startAnimation = Animation::create();
		for (int i = 0; i < 3; ++i) {
			char path[15];
			sprintf(path, "blink_0%d.png", i);
			startAnimation->addSpriteFrameWithFile(path);
		}
		//startAnimation->setLoops(1);
		startAnimation->setDelayPerUnit(1.0f / 2.0f);
		auto starAnimate = RepeatForever::create(Animate::create(startAnimation));
		star->runAction(starAnimate);
		star->setPosition(
			Vec2((float) (medal->getContentSize().width / 10.0 * 9.0),
			     (float) (medal->getContentSize().height / 10.0 * 9.0)));
		//star->setScale(scale);
		//medal->setScale(scale);
		medal->addChild(star);
		medal->setPosition(
			Vec2((float) (scorePanel->getContentSize().width / 4.5),
			     (float) (scorePanel->getContentSize().height / 2.1)));
		scorePanel->addChild(medal);
	}

	if(userGrade.currentIsBestScore()) {
		newImg = Sprite::create("new.png");
		//newImg->setScale(scale);
		userScoreLabel->addChild(newImg);
		newImg->setPosition((float) (-newImg->getContentSize().width / 2.0),
				    (float) (userScoreLabel->getContentSize().height / 2.0));
	}

	this->addChild(scorePanel);
	this->addChild(gameOverText);
	this->addChild(restartMenu);
}

void gameMain::restartGame(Ref *)
{
	auto scene = gameMain::createScene();
	Director::getInstance()->replaceScene(scene);
}

std::string gameMain::whichBird(int i)
{
	char filename[15] = { 0 };
	sprintf(filename, "bird%d_%d.png", birdId, i);
	CCLOG("%s", filename);
	return filename;
}
