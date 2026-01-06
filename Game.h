#pragma once

#include <SFML\Graphics.hpp>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

namespace Game {
	extern sf::RenderWindow window;

	extern sf::Clock clock;
	extern sf::Time timeElapsed;
	extern sol::state lua;

	void init(size_t w, size_t h);

	void HandleInput();
	void HandleCollisions();

	void Render();
	void RenderGUI();

	void run();
};
