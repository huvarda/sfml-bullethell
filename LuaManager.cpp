#include "LuaManager.h"
#include "BulletManager.h"

#include <vector>

namespace LuaManager {
	std::vector<Attack> attack_list;
	int active_attack_idx;

	sol::thread active_thread;
	std::optional<sol::coroutine> active_coroutine;
	float wait_time;
	float wait_counter_time;
	float attack_counter_time;

	void init()
	{
		lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::package, sol::lib::coroutine, sol::lib::io);

		lua.script("function wait(ms)"
			"coroutine.yield(ms)"
			"end"
		);
		active_thread = sol::thread::create(lua.lua_state());
	}

	void unload_folder(const std::string& folder_name) {
		return;
	}

	std::optional<sol::protected_function_result> continue_attack_script() {
		if (active_coroutine) {
			return active_coroutine.value()();
		}
		return std::nullopt;
	}

	void load_next_attack() {
		active_attack_idx++;
		if (active_attack_idx >= attack_list.size()) {
			std::cout << "no next attack";
			active_attack_idx--;

			return;
		}
		attack_counter_time = 0;
		wait_counter_time = 0;
		wait_time = 0;
		std::cout << active_coroutine.value().lua_state() << std::endl;

		active_thread = sol::thread::create(lua.lua_state());
		active_coroutine = sol::nil;
		active_coroutine = active_thread.state()["attack_list"][active_attack_idx+1]["attack"];
	}

	void update(float dt) {
		if (!active_coroutine) {
			return;
		}
		if (attack_counter_time >= attack_list[active_attack_idx].timer) {
			load_next_attack();
			std::cout << "past 3 seconds";
			return;
		}

		if (wait_counter_time >= wait_time) {
			wait_counter_time -= wait_time;
			auto result = continue_attack_script();

			if (!result || !result->valid()) {
				return;
			}

			switch (result->status()) {
				case sol::call_status::ok: {
					load_next_attack();
					break;
				}
				case sol::call_status::yielded: {
					wait_time = result.value();
				}
			}
		}
		attack_counter_time += dt;
		wait_counter_time += dt;
	}

	void load_folder(const std::string& folder_name) {
		const std::string package_path = lua["package"]["path"];
		lua["package"]["path"] = package_path + (!package_path.empty() ? ";" : "") + folder_name + "/?.lua";

		lua.safe_script_file(folder_name+"/main.lua");

		std::string shot_image = lua["shot_image"];
		BulletManager::bulletSheet.loadFromFile(folder_name+"/"+shot_image);

		sol::table shots_lua = lua["shots"];
		for (int i = 1; i <= shots_lua.size(); i++) {
			sol::table shot_data_lua = shots_lua[i];
			float left = shot_data_lua[1];
			float top = shot_data_lua[2];
			float width = shot_data_lua.get<float>(3) - left;
			float height = shot_data_lua.get<float>(4) - top;
			BulletManager::shotData.push_back({ {left, top}, width, height });
		}

		sol::table attack_list_lua = lua["attack_list"];

		if (attack_list_lua.size() == 0) {
			unload_folder(folder_name);
		}
	

		attack_list.clear();

		for (int i = 1; i <= attack_list_lua.size(); i++) {

			const sol::table attack_data = attack_list_lua[i];
			const float dur = attack_data.get_or("duration", 0.0) * 1000;

			attack_list.push_back({ dur });
		}

		active_attack_idx = 0;
		attack_counter_time = 0;
		wait_counter_time = 0;
		wait_time = 0;

		active_coroutine = active_thread.state()["attack_list"][1]["attack"];
	}
}