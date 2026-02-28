#include "BulletManager.h"

void Bullet::update(float timeElapsed) {
	pos = { pos.x + timeElapsed * vel.x, pos.y + timeElapsed * vel.y };
}


namespace BulletManager {

	size_t maxBullets;

	std::vector<Bullet> bullets;
	std::vector<ShotData> shotData;

	size_t boundH;
	size_t boundW;

	float counterTime = 0;
	float timeBetweenShots = 1;

	size_t numBullet = 0;
	float bulletSpeed = 10;
	float angleStep = 0.4;
	float angle = 0;
	float variance = 0;

	sf::Texture bulletSheet;
	sf::VertexArray vertices;

	void init(size_t maxBullets, size_t boundW, size_t boundH)
	{
		bullets.resize(maxBullets);
		vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
		vertices.resize(6 * maxBullets);

		BulletManager::maxBullets = maxBullets;
		BulletManager::boundW = boundW;
		BulletManager::boundH = boundH;

		bulletSheet.loadFromFile("data/default_shots.png");
	}

	//sf::IntRect({ 0, 64 }, { 16, 16 })

	void update(float dt) {
		for (size_t i = 0; i < maxBullets; i++) {
			if (bullets[i].active) {
				bullets[i].update(dt);
				sf::Vertex* triangles = &vertices[i * 6];

				ShotData shot_data = shotData[bullets[i].texId];

				triangles[0].position = bullets[i].pos + sf::Vector2f(-shot_data.width / 2, -shot_data.height / 2);
				triangles[1].position = bullets[i].pos + sf::Vector2f(shot_data.width / 2, -shot_data.height / 2);
				triangles[2].position = bullets[i].pos + sf::Vector2f(-shot_data.width / 2, shot_data.height / 2);
				triangles[3].position = bullets[i].pos + sf::Vector2f(-shot_data.width / 2, shot_data.height / 2);
				triangles[4].position = bullets[i].pos + sf::Vector2f(shot_data.width / 2, -shot_data.height / 2);
				triangles[5].position = bullets[i].pos + sf::Vector2f(shot_data.width / 2, shot_data.height / 2);

				//Make inactive if out of bounds
				if (bullets[i].pos.x > boundW || bullets[i].pos.y > boundH || (bullets[i].pos.x+16 < 0) || (bullets[i].pos.y+16 < 0)) {
					bullets[i].active = false;
				}
			}
		} 
		counterTime += dt;
	}

	void shotA1(float x, float y, float speed, float angle, int shot_type, int delay) {
		// Reset the number of bullet when the max is reached
		if (numBullet == maxBullets) {
			numBullet = 0;
		}

		//New bullet information
		bullets[numBullet].pos = sf::Vector2f(x, y);
		bullets[numBullet].vel = sf::Vector2f(speed * cos(angle), speed * sin(angle));
		bullets[numBullet].texId = shot_type;
		bullets[numBullet].active = true;

		//New bullet sprite information
		sf::Vertex* triangles = &(vertices)[numBullet * 6];

		ShotData shot_data = shotData[shot_type];

		triangles[0].position = bullets[numBullet].pos + sf::Vector2f(-shot_data.width / 2, -shot_data.height / 2);
		triangles[1].position = bullets[numBullet].pos + sf::Vector2f(shot_data.width / 2, -shot_data.height / 2);
		triangles[2].position = bullets[numBullet].pos + sf::Vector2f(-shot_data.width / 2, shot_data.height / 2);
		triangles[3].position = bullets[numBullet].pos + sf::Vector2f(-shot_data.width / 2, shot_data.height / 2);
		triangles[4].position = bullets[numBullet].pos + sf::Vector2f(shot_data.width / 2, -shot_data.height / 2);
		triangles[5].position = bullets[numBullet].pos + sf::Vector2f(shot_data.width / 2, shot_data.height / 2);

		triangles[0].texCoords = shot_data.top_left; //top left
		triangles[1].texCoords = shot_data.top_left + sf::Vector2f(shot_data.width, 0); // top right
		triangles[2].texCoords = shot_data.top_left + sf::Vector2f(0, shot_data.height); // bottom left
		triangles[3].texCoords = shot_data.top_left + sf::Vector2f(0, shot_data.height); // bottom left
		triangles[4].texCoords = shot_data.top_left + sf::Vector2f(shot_data.width, 0);
		triangles[5].texCoords = shot_data.top_left + sf::Vector2f(shot_data.width, shot_data.height); // bottom right

		numBullet++;
	}
}