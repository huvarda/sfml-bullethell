#include "LuaManager.h"


namespace LuaManager {
	std::optional<Attack*> attack_list;
	std::optional<Attack*> active_attack;

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
		active_thread = sol::thread::create(lua);
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
		auto next_attack = active_attack.value()->next;
		if (!next_attack) {
			std::cout << "no next attack";
			return;
		}
		active_attack = next_attack;
		attack_counter_time = 0;
		wait_counter_time = 0;
		wait_time = 0;

		active_coroutine = sol::coroutine(active_attack.value()->fn);
	}

	void update(float dt) {
		if (!active_coroutine) {
			return;
		}
		if (attack_counter_time >= active_attack.value()->timer) {
			load_next_attack();
			std::cout << "past 2 seconds";
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

		sol::table attack_list_lua = lua["attack_list"];

		if (attack_list_lua.size() == 0) {
			unload_folder(folder_name);
		}

		const sol::table attack_data = attack_list_lua[1];

		sol::function attack_fn = attack_data["attack"];

		Attack *prev = new Attack();
		prev->fn = attack_data["attack"];
		prev->timer = attack_data.get_or("duration", 0.0) * 1000;
		attack_list = prev;
		active_attack = attack_list;
		active_coroutine = sol::coroutine(attack_fn);

		if (attack_list_lua.size() == 1) {
			return;
		}

		for (std::size_t i = 2; i <= attack_list_lua.size(); i++) {
			const sol::table attack_data = attack_list_lua[i];

			sol::function attack_fn = attack_data["attack"];

			Attack *cur = new Attack();
			cur->fn = attack_data["attack"];
			cur->timer = attack_data.get_or("duration", 0.0) * 1000;
			prev->next = std::optional<Attack*>(cur);
			prev = cur;
		}

		attack_counter_time = 0;
		wait_counter_time = 0;
	}

}