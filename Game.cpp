#include <iostream>
#include <random>
#include <optional>

#include "imgui.h"
#include "imgui-SFML.h"

#include "BulletManager.h"
#include "LuaManager.h"
#include "Game.h"

#define Pi 3.14159265358979323846

struct Player {
	float vel;
	float velFocus;
	sf::Vector2f pos;
	bool focused;
	sf::Vector2f moveDir;

	void update(float dt);
};

void Player::update(float dt) {
	pos += dt * moveDir * (focused ? velFocus : vel);
}

void lua_shotA1(float x, float y, float speed, float angle, int graphic, int delay) {
	BulletManager::shotA1(x, y, speed, angle/360*(2*Pi), graphic, delay);
}

namespace Game {
	Player player = {200, 100, {0, 0}, false, {0, 0} };

	sf::Texture playerTex("data/player.png");
	sf::Sprite playerSprite(playerTex);

	sf::RenderWindow window(sf::VideoMode({ 1440, 1080 }), "BullHell");

	sf::Clock clock;
	sf::Time timeElapsed;
	sol::state lua;
	float wait_time = 0;

	void init(size_t w, size_t h)
	{
		srand(time(NULL));
		BulletManager::init(6000, w, h);
		LuaManager::init();

		LuaManager::add_function_binding("shotA1", lua_shotA1);

		LuaManager::load_folder("testfolder");

		ImGui::SFML::Init(window);
		window.setSize(sf::Vector2u(w, h));
		window.setTitle("Bullet Hell");
		window.setFramerateLimit(60);

	}

	void HandleInput() {
		while (const std::optional event = window.pollEvent()) {
			ImGui::SFML::ProcessEvent(window, *event);
			if (event->is<sf::Event::Closed>())
				window.close();

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					window.close();
				}
		}

		// Player input
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
			player.focused = true;
		}
		else {
			player.focused = false;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
			player.moveDir.x = -1.0;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
			player.moveDir.x = 1.0;
		}
		else {
			player.moveDir.x = 0;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
			player.moveDir.y = -1.0;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
			player.moveDir.y = 1.0;
		}
		else {
			player.moveDir.y = 0;
		}

	}

	void Render() {
		window.clear();

		sf::Transform transform;
		sf::RenderStates states;
		states.transform = transform;
		states.texture = &BulletManager::bulletSheet;

		window.draw(BulletManager::vertices, states);
		window.draw(playerSprite);


		ImGui::SFML::Update(window, timeElapsed);

		ImGui::Begin("Title");
		ImGui::Text("Bruh");
		ImGui::SliderFloat("Speed", &BulletManager::bulletSpeed, 0.1f, 100);
		ImGui::SliderFloat("Time between shots", &BulletManager::timeBetweenShots, 1, 100);
		ImGui::SliderFloat("Angle Between Shots", &BulletManager::angleStep, 0, 2 * Pi);
		ImGui::SliderFloat("Variance", &BulletManager::variance, 0, 0.3f);

		ImGui::End();


		ImGui::SFML::Render(window);

		window.display();
	}

	void run() {

		while (window.isOpen()) {
			timeElapsed = clock.restart();

			HandleInput();

			// Update player
			player.update(timeElapsed.asSeconds());
			playerSprite.setPosition(player.pos);

			// Update bullets
			BulletManager::update(timeElapsed.asMilliseconds());

			// Add new bullet(s) if should
			LuaManager::update(timeElapsed.asMilliseconds());

			Render();		
		}

		ImGui::SFML::Shutdown();
	}
}