#include "LuaManager.h"


namespace LuaManager {
	std::optional<Attack*> attack_list;
	std::optional<Attack*> active_attack;
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
	}

	void unload_folder(const std::string& folder_name) {
		return;
	}

	sol::protected_function_result continue_attack_script() {
		return (active_attack.value()->cr)();
	}

	void update(float dt) {
		if (!active_attack) {
			return;
		}
		if (attack_counter_time >= active_attack.value()->timer) {
			auto next_attack = active_attack.value()->next;
			if (!next_attack) {
				attack_counter_time = 0;
				return;
			}
			active_attack = next_attack;
		}

		if (wait_counter_time >= wait_time) {
			wait_counter_time -= wait_time;
			auto result = continue_attack_script();
			std::cout << result.valid() << std::endl;

			if (result.valid()) {
				switch (result.status()) {
				case sol::call_status::ok: {
					auto next_attack = active_attack.value()->next;
					if (!next_attack) {
						attack_counter_time = 0;
						return;
					}
					active_attack = next_attack;
					break;
				}
				case sol::call_status::yielded: {
					wait_time = result;
					}
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

		sol::coroutine attack_cr = attack_data["attack"];

		Attack *prev = new Attack();
		prev->cr = attack_data["attack"];
		prev->timer = attack_data.get_or("duration", 0.0) * 1000;
		attack_list = prev;
		active_attack = attack_list;

		if (attack_list_lua.size() == 1) {
			return;
		}

		for (std::size_t i = 2; i <= attack_list_lua.size(); i++) {
			const sol::table attack_data = attack_list_lua[i];

			sol::coroutine attack_cr = attack_data["attack"];

			Attack *cur = new Attack();
			cur->cr = attack_data["attack"];
			cur->timer = attack_data.get_or("duration", 0.0) * 1000;
			prev->next = std::optional<Attack*>(cur);
			prev = cur;
		}

		attack_counter_time = 0;
		wait_counter_time = 0;
	}

}