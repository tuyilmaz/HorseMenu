#pragma once
#include "game/rdr/Natives.hpp"
#include "game/rdr/RuleService.hpp"

namespace YimMenu
{

	namespace award_hooks
	{
		inline void NETWORK_AWARD_HAS_REACHED_MAXCLAIM(rage::scrNativeCallContext* src)
		{
			Hash awardHash = src->get_arg<Hash>(0);
			BOOL ret       = NETWORK::NETWORK_AWARD_HAS_REACHED_MAXCLAIM(awardHash);
			g_award_service->get_rule(awardHash, &ret);
			src->set_return_value<BOOL>(std::move(ret));
		}
	}
}