//#include <iostream>
#include <iostream>
#include "scoreManager.h"

scoreManager scoreManager::operator++(int a)
{
	scoreManager i = *this;
	this->score++;
	if(score > bestScore) {
		setBestScore = true;
		bestScore = score;
	}
	return  i;
}

scoreManager scoreManager::operator++()
{
	this->score++;
	if(score > bestScore) {
		setBestScore = true;
		bestScore = score;
	}
	return *this;
}

scoreManager::~scoreManager()
{
	if(!setBestScore)
		return;
	this->getInstance()->setIntegerForKey("bestScore", (int)bestScore);
	this->getInstance()->flush();
}

scoreManager::scoreManager() : score(0)
{
	std::cout << "-----" << this->getInstance()->getXMLFilePath() << std::endl;
	bestScore = _getBestScore();
}

unsigned long scoreManager::getScore()
{
	return score;
}

unsigned long scoreManager::_getBestScore()
{
	return (unsigned long)(this->getInstance()->getIntegerForKey("bestScore", 0));
}

unsigned long scoreManager::getBestScore()
{
	return bestScore;
}

bool scoreManager::currentIsBestScore()
{
	return setBestScore;
}
