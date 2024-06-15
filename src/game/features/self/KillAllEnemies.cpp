#include "core/commands/LoopedCommand.hpp"
#include "game/features/Features.hpp"
#include "game/rdr/Natives.hpp"
#include "util/Helpers.hpp"

namespace YimMenu::Features
{
	class KillEnemies : public LoopedCommand
	{
		using LoopedCommand::LoopedCommand;

		virtual void OnTick() override
		{
			for (Ped ped : Helpers::GetAllPeds())
			{
				int relation = PED::GET_RELATIONSHIP_BETWEEN_PEDS(ped, Self::PlayerPed);
				if (relation == 4 || relation == 5)
				{
					PED::APPLY_DAMAGE_TO_PED(ped, std::numeric_limits<int>::max(), 1, 0, Self::PlayerPed);
				}
			}
		}
	};

	static KillEnemies _KillEnemies{"killenemies", "Kill Enemies", "Kills all enemies instantly!"};
}