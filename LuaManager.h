#pragma once

#include <SFML/Graphics.hpp>

#include <optional>
#include <string>

#define SOL_ALL_SAFETIES_ON 1
#include "sol/sol.hpp"

struct Attack {
	sol::function fn;
	float timer;
	std::optional<Attack*> next;
};

namespace LuaManager {
	inline sol::state lua;

	void init();

	template<typename T>
	void add_function_binding(const std::string& fn_name, T fn) {
		lua.set_function(fn_name, fn);
	}

	void update(float dt);

	void load_folder(const std::string& folder_name);
}