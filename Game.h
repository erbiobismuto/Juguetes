#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Game
{
public:
	Game();
	void run();
private:
	void processEvents();
	void update(sf::Time deltaTime);
	void render();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
private:
	enum class Direction{ D_RIGHT, D_DOWN, D_LEFT, D_UP };
	const unsigned int WindowWidth{ sf::VideoMode::getDesktopMode().width };
	const unsigned int WindowHeight{ sf::VideoMode::getDesktopMode().height };
	const unsigned int capacity{ 100 };
	sf::RectangleShape leftMargin{ };
	sf::RectangleShape rightMargin{ };
	sf::RectangleShape topMargin{ }; 
	sf::RectangleShape bottomMargin{ };
	const sf::Vector2f PlayerSize{ 15.f,15.f };
	const sf::Time TimePerFrame{ sf::seconds(1.f / 60.f) };
	sf::RenderWindow mWindow;
	sf::RectangleShape mPlayer1{ };
	sf::RectangleShape mPlayer2{ };
	std::vector<sf::Vector2f> mPlayer1Pos{ capacity };
	std::vector<sf::Vector2f> mPlayer2Pos{ capacity };
	Direction mDirp1{ Direction::D_RIGHT };
	Direction mDirp2{ Direction::D_LEFT };
	bool mAi{ false };
	void collision();
	void checkBounds(const sf::RectangleShape& p);
};
