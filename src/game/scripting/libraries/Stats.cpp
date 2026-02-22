#include "core/scripting/LuaLibrary.hpp"
#include "core/scripting/LuaScript.hpp"
#include "core/scripting/LuaUtils.hpp"

#include "game/rdr/Stats.hpp"

namespace YimMenu::Lua
{
	class Stats : LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

		static int IsValid(lua_State* state)
		{
			lua_pushboolean(state, YimMenu::Stats::IsValid(GetHashArgument(state, 1), GetHashArgument(state, 2)));
			return 1;
		}

		static int GetBool(lua_State* state)
		{
			lua_pushboolean(state, YimMenu::Stats::GetBool(GetHashArgument(state, 1), GetHashArgument(state, 2)));
			return 1;
		}

		static int GetInt(lua_State* state)
		{
			lua_pushinteger(state, YimMenu::Stats::GetInt(GetHashArgument(state, 1), GetHashArgument(state, 2)));
			return 1;
		}

		static int GetFloat(lua_State* state)
		{
			lua_pushnumber(state, YimMenu::Stats::GetFloat(GetHashArgument(state, 1), GetHashArgument(state, 2)));
			return 1;
		}

		static int GetDate(lua_State* state)
		{
			CopyObject<YimMenu::Stats::Date>(state, YimMenu::Stats::GetDate(GetHashArgument(state, 1), GetHashArgument(state, 2)));
			return 1;
		}

		static int SetBool(lua_State* state)
		{
			YimMenu::Stats::SetBool(GetHashArgument(state, 1), GetHashArgument(state, 2), lua_toboolean(state, 3));
			return 0;
		}

		static int SetInt(lua_State* state)
		{
			YimMenu::Stats::SetInt(GetHashArgument(state, 1), GetHashArgument(state, 2), luaL_checkinteger(state, 3));
			return 0;
		}

		static int SetFloat(lua_State* state)
		{
			YimMenu::Stats::SetFloat(GetHashArgument(state, 1), GetHashArgument(state, 2), luaL_checknumber(state, 3));
			return 0;
		}

		static int SetDate(lua_State* state)
		{
			YimMenu::Stats::SetDate(GetHashArgument(state, 1), GetHashArgument(state, 2), &GetObject<YimMenu::Stats::Date>(state, 3));
			return 0;
		}

		static int IncrementInt(lua_State* state)
		{
			YimMenu::Stats::IncrementInt(GetHashArgument(state, 1), GetHashArgument(state, 2), luaL_checkinteger(state, 3));
			return 0;
		}

		static int IncrementFloat(lua_State* state)
		{
			YimMenu::Stats::IncrementFloat(GetHashArgument(state, 1), GetHashArgument(state, 2), luaL_checknumber(state, 3));
			return 0;
		}


		virtual void Register(lua_State* state) override
		{
			lua_newtable(state);
			SetFunction(state, IsValid, "is_valid");
			SetFunction(state, GetBool, "get_bool");
			SetFunction(state, GetInt, "get_int");
			SetFunction(state, GetFloat, "get_float");
			SetFunction(state, GetDate, "get_date");
			SetFunction(state, SetBool, "set_bool");
			SetFunction(state, SetInt, "set_int");
			SetFunction(state, SetFloat, "set_float");
			SetFunction(state, SetDate, "set_date");
			SetFunction(state, IncrementInt, "increment_int");
			SetFunction(state, IncrementFloat, "increment_float");
			lua_setglobal(state, "Stats");
		}
	};

	Stats _Stats;
}