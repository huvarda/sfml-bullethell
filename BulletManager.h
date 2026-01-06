#pragma once
#include <vector>
#include <SFML\Graphics.hpp>

struct Bullet {
	sf::Vector2f pos;
	sf::Vector2f vel;
	int texId;
	bool active;

	void update(float timeElapsed);
};

enum ShotType {
	ABSOLUTE,
	RELATIVE,
	ABSOLUTE_W_ACCELERATION,
	RELATIVE_W_ACCELERATION,
};

struct ShotData {
	ShotType shotType;
	sf::Vector2f pos;
	float speed;
	float angle;
	float spawnDelay;
	float acceleration;
	float maxSpeed;

};


namespace BulletManager {
	extern size_t maxBullets;
	extern std::vector<Bullet> bullets;

	extern size_t boundW;
	extern size_t boundH;

	extern sf::Texture bulletSheet;
	extern sf::VertexArray vertices;

	extern float counterTime;
	extern float bulletSpeed;
	extern float timeBetweenShots;
	extern float angleStep;
	extern float angle;
	extern float variance;

	void init(size_t maxBullets, size_t boundW, size_t boundH);
	void update(float dt);

	void shotA1(float x, float y, float speed, float angle, int graphic, int delay);
};