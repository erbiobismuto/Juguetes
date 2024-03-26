#include "Game.h"
#include <iostream>

Game::Game() : mWindow(sf::VideoMode(WindowWidth, WindowHeight), "Siege", sf::Style::Fullscreen)
{
	mPlayer1.setSize(PlayerSize);
	mPlayer1.setOrigin(mPlayer1.getSize().x / 2, mPlayer1.getSize().y / 2);
	mPlayer1.setOutlineThickness(1);
	mPlayer1.setOutlineColor(sf::Color::Blue);
	mPlayer1Pos.push_back(sf::Vector2f(50.f,50.f));

	mPlayer2.setSize(PlayerSize);
	mPlayer2.setOrigin(mPlayer2.getSize().x / 2, mPlayer2.getSize().y / 2);
	mPlayer2.setOutlineThickness(1);
	mPlayer2.setOutlineColor(sf::Color::Red);
	mPlayer2Pos.push_back(sf::Vector2f(WindowWidth - 60.f, WindowHeight - 60.f));

	leftMargin.setSize(sf::Vector2f(20.f, WindowHeight - 20.f));
	leftMargin.setPosition(sf::Vector2f(10.f, 10.f));

	rightMargin.setSize(sf::Vector2f(20.f, WindowHeight - 20.f));
	rightMargin.setPosition(sf::Vector2f(WindowWidth - 30.f, 10.f));

	topMargin.setSize(sf::Vector2f(WindowWidth - 20.f, 20.f));
	topMargin.setPosition(sf::Vector2f(10.f, 10.f));

	bottomMargin.setSize(sf::Vector2f(WindowWidth - 20.f, 20.f));
	bottomMargin.setPosition(sf::Vector2f(10.f, WindowHeight - 30.f));
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;
		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}


void Game::update(sf::Time deltaTime)
{
	float x = PlayerSize.x + deltaTime.asSeconds();
	float y = PlayerSize.y + deltaTime.asSeconds();

	switch(mDirp1)
	{
	case Direction::D_RIGHT:	
		mPlayer1.move(sf::Vector2f(x, 0.f));
		break;
	case Direction::D_LEFT:
		mPlayer1.move(sf::Vector2f(-x, 0.f));
		break;
	case Direction::D_DOWN:
		mPlayer1.move(sf::Vector2f(0.f, y));
		break;
	case Direction::D_UP:
		mPlayer1.move(sf::Vector2f(0.f, -y));
		break;
	default:
		break;
	}

	mPlayer1Pos.push_back(mPlayer1.getPosition());

	float x2 = PlayerSize.x + deltaTime.asSeconds();
	float y2 = PlayerSize.y + deltaTime.asSeconds();
	switch (mDirp2)
	{
	case Direction::D_RIGHT:
		mPlayer2.move(sf::Vector2f(x2, 0.f));
		break;
	case Direction::D_LEFT:
		mPlayer2.move(sf::Vector2f(-x2, 0.f));
		break;
	case Direction::D_DOWN:
		mPlayer2.move(sf::Vector2f(0.f, y2));
		break;
	case Direction::D_UP:
		mPlayer2.move(sf::Vector2f(0.f, -y2));
		break;
	default:
		break;
	}

	mPlayer2Pos.push_back(mPlayer2.getPosition());

	collision();
}



void Game::render()
{
	mWindow.clear();
	for(auto pos: mPlayer1Pos) 
	{
		mPlayer1.setPosition(pos);
		mWindow.draw(mPlayer1);
	}
	for (auto pos : mPlayer2Pos)
	{
		mPlayer2.setPosition(pos);
		mWindow.draw(mPlayer2);
	}

	mWindow.draw(leftMargin);
	mWindow.draw(rightMargin);
	mWindow.draw(topMargin);
	mWindow.draw(bottomMargin);
	mWindow.display();
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{	
	
	Direction prevDir1 = mDirp1;
	Direction prevDir2 = mDirp2;

	if (isPressed) {
		switch (key)
		{
		case sf::Keyboard::A:
			if (prevDir1 != Direction::D_RIGHT)
			mDirp1 = Direction::D_LEFT;
			break;
		case sf::Keyboard::S:
			if (prevDir1 != Direction::D_UP)
			mDirp1 = Direction::D_DOWN;
			break;
		case sf::Keyboard::W:
			if (prevDir1 != Direction::D_DOWN)
			mDirp1 = Direction::D_UP;
			break;
		case sf::Keyboard::D:
			if (prevDir1 != Direction::D_LEFT)
			mDirp1 = Direction::D_RIGHT;
			break;

		case sf::Keyboard::Left:
			if (prevDir2 != Direction::D_RIGHT)
			mDirp2 = Direction::D_LEFT;
			break;
		case sf::Keyboard::Down:
			if (prevDir2 != Direction::D_UP)
				mDirp2 = Direction::D_DOWN;
			break;
		case sf::Keyboard::Up:
			if (prevDir2 != Direction::D_DOWN)
				mDirp2 = Direction::D_UP;
			break;
		case sf::Keyboard::Right:
			if (prevDir2 != Direction::D_LEFT)
				mDirp2 = Direction::D_RIGHT;
			break;

		default:
			break;
		}
	}
    
}

void Game::collision() 
{
	
	checkBounds(mPlayer1);
	checkBounds(mPlayer2);
	auto bounds1 = mPlayer1.getGlobalBounds();
	auto bounds2 = mPlayer2.getGlobalBounds();
	auto it1 = mPlayer1Pos.begin();
	auto it2 = mPlayer2Pos.begin();
	auto endIt1 = mPlayer1Pos.end() - 3;
	auto endIt2 = mPlayer2Pos.end() - 3;
	
	while (it1 != endIt1 && it2 != endIt2)
	{
		sf::RectangleShape tmp;
		tmp.setPosition(*it1);
		tmp.setSize(PlayerSize);
		sf::RectangleShape tmp2;
		tmp2.setPosition(*it2);
		tmp2.setSize(PlayerSize);
		if(bounds1.intersects(tmp.getGlobalBounds()) || bounds1.intersects(tmp2.getGlobalBounds()) ||
		   bounds2.intersects(tmp.getGlobalBounds()) || bounds2.intersects(tmp2.getGlobalBounds()))
			mWindow.close();
		++it1;
		++it2;
	}

}

void Game::checkBounds(const sf::RectangleShape& p)
{
	auto bounds = p.getGlobalBounds();
	if (bounds.intersects(rightMargin.getGlobalBounds()) || bounds.intersects(leftMargin.getGlobalBounds()) ||
		bounds.intersects(topMargin.getGlobalBounds()) || bounds.intersects(bottomMargin.getGlobalBounds()))
		mWindow.close();
}
