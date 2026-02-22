#include "Recovery.hpp"

#include "core/commands/BoolCommand.hpp"
#include "core/commands/Commands.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/NativeHooks.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/backend/ScriptPatches.hpp"
#include "game/frontend/items/Items.hpp"
#include "game/rdr/data/Stats.hpp"
#include "game/rdr/Natives.hpp"
#include "game/rdr/ScriptFunction.hpp"
#include "game/rdr/Scripts.hpp"
#include "game/rdr/Stats.hpp"
#include "util/Rewards.hpp"

template<size_t len>
struct TransactionArray
{
	uint64_t count = len;
	uint64_t items[len];
};

namespace YimMenu::Submenus
{
	std::shared_ptr<ScriptPatches::Patch> BypassCondition{};
	std::shared_ptr<ScriptPatches::Patch> ModifierHash{};

	void _0xA3B8D31C13CB4239(rage::scrNativeCallContext* ctx)
	{
		int TransactionID = ctx->GetArg<int>(0);
		joaat_t ActionHash = ctx->GetArg<joaat_t>(1);
		uint64_t* ItemInfo1 = ctx->GetArg<uint64_t*>(2);
		int ItemCount = ctx->GetArg<int>(3);
		uint64_t* ItemInfo2 = ctx->GetArg<uint64_t*>(4);

		ctx->SetReturnValue<BOOL>(NETSHOPPING::_0xA3B8D31C13CB4239(TransactionID, ActionHash, ItemInfo1, ItemCount, ItemInfo2, 0));

		std::stringstream iss;
		iss << "_0xA3B8D31C13CB4239 called from " << Scripts::GetScriptName(SCRIPTS::GET_HASH_OF_THIS_SCRIPT_NAME()) << std::endl;
		iss << "TID: " << TransactionID << std::endl;
		iss << "Action Hash: " << HEX(ActionHash) << std::endl;
		for (int i = 0; i < ItemCount; i++)
			iss << "ItemInfo1[" << i << "] = " << HEX(ItemInfo1[i]) << std::endl;
		for (int i = 0; i < ItemCount; i++)
			iss << "ItemInfo2[" << i << "] = " << HEX(ItemInfo2[i]) << std::endl;
		iss << "***************************************" << std::endl;
		LOG(INFO) << iss.str();
	}

	void _CASHINVENTORY_TRANSACTION_FIRE_AND_FORGET_ITEM(rage::scrNativeCallContext* ctx)
	{
		joaat_t ActionHash = ctx->GetArg<joaat_t>(0);
		int* TransactionID = ctx->GetArg<int*>(1);
		uint64_t* ItemInfo = ctx->GetArg<uint64_t*>(2);
		int ItemCount = ctx->GetArg<int>(3);

		ctx->SetReturnValue<int>(NETSHOPPING::_CASHINVENTORY_TRANSACTION_FIRE_AND_FORGET_ITEM(ActionHash, TransactionID, ItemInfo, ItemCount));

		std::stringstream iss;
		iss << "_CASHINVENTORY_TRANSACTION_FIRE_AND_FORGET_ITEM called from " << Scripts::GetScriptName(SCRIPTS::GET_HASH_OF_THIS_SCRIPT_NAME()) << std::endl;
		iss << "TID: " << TransactionID << std::endl;
		iss << "Action Hash: " << HEX(ActionHash) << std::endl;
		for (int i = 0; i < ItemCount; i++)
			iss << "ItemInfo[" << i << "] = " << HEX(ItemInfo[i]) << std::endl;
		iss << "***************************************" << std::endl;
		LOG(INFO) << iss.str();
	}

	/* void GET_EVENT_AT_INDEX(rage::scrNativeCallContext* ctx)
	{
		int eventGroup = ctx->GetArg<int>(0);
		int eventIndex = ctx->GetArg<int>(1);
		Hash eventAtIndex = SCRIPTS::GET_EVENT_AT_INDEX(eventGroup, eventIndex);

		if (eventAtIndex == 1741908893)
		{
			uint64_t items[12];
			if (SCRIPTS::GET_EVENT_DATA(eventGroup, eventIndex, &items, 12))
			{
				LOG(INFO) << "Claimed Award: " << HEX(items[5]);
			}
		}

		ctx->SetReturnValue<Hash>(SCRIPTS::GET_EVENT_AT_INDEX(eventGroup, eventIndex));
	}*/

	Recovery::Recovery() :
		#define ICON_FA_SACK_DOLLAR "\xef\xa0\x9d"
	    Submenu::Submenu("Recovery", ICON_FA_SACK_DOLLAR)
	{
		auto recovery               = std::make_shared<Category>("Recovery");
		auto spawnCollectiblesGroup = std::make_shared<Group>("Spawn Collectibles");
		auto spawnHerbsGroup		= std::make_shared<Group>("Spawn Herbs");
		auto recoveryOptions        = std::make_shared<Group>("Options");

		static auto recoveryCommand = Commands::GetCommand<BoolCommand>("recoveryenabled"_J);

		spawnCollectiblesGroup->AddItem(std::make_shared<ImGuiItem>([=] {
			if (recoveryCommand->GetState())
			{
				static Rewards::eRewardType selected{};
				static int awardamount = 1;
				std::map<Rewards::eRewardType, std::string> reward_translations = {
				    {Rewards::eRewardType::HEIRLOOMS, "Heirlooms"},
				    {Rewards::eRewardType::COINS, "Coins"},
				    {Rewards::eRewardType::ALCBOTTLES, "Alcohol Bottles"},
				    {Rewards::eRewardType::ARROWHEADS, "Arrowheads"},
				    {Rewards::eRewardType::BRACELETS, "Bracelets"},
				    {Rewards::eRewardType::EARRINGS, "Earrings"},
				    {Rewards::eRewardType::NECKLACES, "Necklaces"},
				    {Rewards::eRewardType::RINGS, "Rings"},
				    {Rewards::eRewardType::TAROTCARDS_CUPS, "Tarot Cards - Cups"},
				    {Rewards::eRewardType::TAROTCARDS_PENTACLES, "Tarot Cards - Pentacles"},
				    {Rewards::eRewardType::TAROTCARDS_SWORDS, "Tarot Cards - Swords"},
				    {Rewards::eRewardType::TAROTCARDS_WANDS, "Tarot Cards - Wands"},
				    {Rewards::eRewardType::FOSSILS, "Fossils"},
				    {Rewards::eRewardType::EGGS, "Eggs"},
				    {Rewards::eRewardType::TREASURE, "Treasure Reward"},
				    {Rewards::eRewardType::CAPITALE, "Capitale"},
				    {Rewards::eRewardType::XP, "25K XP"},
				    {Rewards::eRewardType::HEALTHXP, "Health XP"},
				    {Rewards::eRewardType::STAMINAXP, "Stamina XP"},
				    {Rewards::eRewardType::DEADEYEXP, "Dead Eye XP"},
				    {Rewards::eRewardType::MOONSHINERXP, "200 Moonshiner XP"},
				    {Rewards::eRewardType::TRADERXP, "200 Trader XP"},
				    {Rewards::eRewardType::COLLECTORXP, "200 Collector XP"},
				    {Rewards::eRewardType::NATURALISTXP, "300 Naturalist XP"},
				    {Rewards::eRewardType::BOUNTYHUNTERXP, "200 Bounty Hunter XP"},
				    {Rewards::eRewardType::TRADERGOODS, "Max Trader Goods"},
				};

				if (ImGui::BeginCombo("Rewards", reward_translations[selected].c_str()))
				{
					for (auto& [type, translation] : reward_translations)
					{
						if (ImGui::Selectable(std::string(translation).c_str(), type == selected, ImGuiSelectableFlags_AllowDoubleClick))
						{
							selected = type;
						}
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
						{
							FiberPool::Push([] {
								for (int i = 0; i < awardamount; i++)
									Rewards::GiveRequestedRewards({selected});
							});
						}
					}
					ImGui::EndCombo();
				}

				ImGui::SliderInt("Amount", &awardamount, 1, 10);

				ImGui::SameLine();

				if (ImGui::Button("Add Selected"))
				{
					FiberPool::Push([] {
						for (int i = 0; i < awardamount; i++)
							Rewards::GiveRequestedRewards({selected});
					});
				}

				ImGui::NewLine();

				static std::uint32_t awardHash{};
				ImGui::InputScalar("Award Hash", ImGuiDataType_U32, &awardHash);
				static std::uint32_t modifierHash = 0;
				ImGui::InputScalar("Modifier Hash", ImGuiDataType_U32, &modifierHash);
				ImGui::SameLine();
				static bool method = true;
				ImGui::Checkbox("Loop", &method);
				if (ImGui::Button("Give Award with Modifier"))
				{
					FiberPool::Push([] {
						std::uint32_t AwardHash_unk[2];
						AwardHash_unk[0] = awardHash;
						AwardHash_unk[1] = modifierHash;

						if (method)
						{
							for (int i = 0; i < awardamount; i++)
								Pointers.GiveAwardWithHash(reinterpret_cast<std::uint32_t*>(&AwardHash_unk));
						}
						else
						{
							int oldVal = 0;
							memcpy(&oldVal, Pointers.GiveAwardAmount, sizeof(oldVal));
							memcpy(Pointers.GiveAwardAmount, &awardamount, sizeof(awardamount));
							Pointers.GiveAwardWithHash(reinterpret_cast<std::uint32_t*>(&AwardHash_unk));
							memcpy(Pointers.GiveAwardAmount, &oldVal, sizeof(oldVal));
						}

						//if (!Scripts::RequestScript("interactive_campfire"_J))
							//return;

						/* if (modifierHash)
						{
							if (!BypassCondition)
								BypassCondition = ScriptPatches::AddPatch("interactive_campfire"_J, true, "22 08 1E", 90, {0x0, 0x0});

							std::vector<uint8_t> patch(sizeof(uint32_t));
							memcpy(patch.data(), &modifierHash, sizeof(uint32_t));
							ModifierHash = ScriptPatches::AddPatch("interactive_campfire"_J, true, "22 08 1E", 95, patch);

							BypassCondition->Enable();
							ModifierHash->Enable();

							ScriptFunctions::GiveItemDatabaseAward.StaticCall(awardHash, false, 255, 0, true);

							BypassCondition->Disable();
							ModifierHash->Disable();
							return;
						}*/

						/* TransactionArray<10> unk;
						TransactionArray<11> unk2;
						uint64_t unk3[4]{};

						if (modifierHash != 0)
							ScriptFunctions::AddAwardModifier.StaticCall(&unk2, modifierHash);

						ScriptFunctions::GiveItemDatabaseAward_.StaticCall(awardHash, unk3, &unk2, &unk, false, 255, 0, modifierHash != 0);*/
					});
				}
				ImGui::SameLine();
				if (ImGui::Button("Give Loot Table"))
				{
					FiberPool::Push([] {
						if (!Scripts::RequestScript("interactive_campfire"_J))
							return;

						ScriptFunctions::GiveLootTableAward.StaticCall(awardHash, 0);
					});
				}
				static bool hooked = false;
				if (!hooked && ImGui::Button("Hook Netshopping"))
				{
					FiberPool::Push([] {
						NativeHooks::AddHook("ALL_SCRIPTS"_J, NativeIndex::_0xA3B8D31C13CB4239, _0xA3B8D31C13CB4239);
						NativeHooks::AddHook("ALL_SCRIPTS"_J, NativeIndex::_CASHINVENTORY_TRANSACTION_FIRE_AND_FORGET_ITEM, _CASHINVENTORY_TRANSACTION_FIRE_AND_FORGET_ITEM);
						//NativeHooks::AddHook("ALL_SCRIPTS"_J, NativeIndex::GET_EVENT_AT_INDEX, GET_EVENT_AT_INDEX);
						hooked = true;
					});
				}
				static bool freeBuy = false;
				if (ImGui::Button("Buy M1899"))
				{
					FiberPool::Push([] {
						int txnId;

						// 1. Start basket
						NETSHOPPING::_CASHINVENTORY_TRANSACTION_START(&txnId, "basket"_J, 2113164098);

						// 2. Build item
						uint64_t item[18]{};
						item[8] = "WEAPON_PISTOL_M1899"_J;
						item[9] = 1;
						item[11] = -1591664384;
						item[12] = 1248274121;   // buy context
						item[13] = freeBuy ? 0x3B79583B : 0xC96FEC6B; //997808187
						item[17] = item[8];

						// 3. Add to basket (skips shop UI unlock checks entirely)
						NETSHOPPING::_0xA3B8D31C13CB4239(txnId, 2113164098, &item, 18, &item, 0);

						// 4. Checkout — sends directly to server
						NETSHOPPING::_CASHINVENTORY_TRANSACTION_CHECKOUT(txnId);
					});
				}
				ImGui::SameLine();
				ImGui::Checkbox("Free Buy", &freeBuy);
			}
			else
			{
				ImGui::Text("Recovery Feature Restricted");
				ImGui::Text("The recovery/collectibles feature is risky and you might face a ban for using it. You are responsible for what you do with this feature. None of the developers or the YimMenu organization are responsible for any damages to your account.");
				if (ImGui::Button("Enable Recovery"))
				{
					recoveryCommand->SetState(true);
				}
			}
		}));
		spawnHerbsGroup->AddItem(std::make_shared<ImGuiItem>([=] {
			if (recoveryCommand->GetState())
			{
				static joaat_t selectedHerb;
				std::map<joaat_t, std::string> herb_translations = {{"HERB_LOOT_ALASKAN_GINSENG"_J, "Alaskan Ginseng"},{"HERB_LOOT_AMERICAN_GINSENG"_J, "American Ginseng"},{"HERB_LOOT_BAY_BOLETE"_J, "Bay Bolete"},{"HERB_LOOT_BLACK_BERRY"_J, "Black Berry"},{"HERB_LOOT_BLACK_CURRANT"_J, "Black Currant"},{"HERB_LOOT_BURDOCK_ROOT"_J, "Burdock Root"},{"HERB_LOOT_CHANTERELLES"_J, "Chanterelles"},{"HERB_LOOT_COMMON_BULRUSH"_J, "Common Bulrush"},{"HERB_LOOT_CREEPING_THYME"_J, "Creeping Thyme"},{"HERB_LOOT_DESERT_SAGE"_J, "Desert Sage"},{"HERB_LOOT_ENGLISH_MACE"_J, "English Mace"},{"HERB_LOOT_EVERGREEN_HUCKLEBERRY"_J, "Evergreen Huckleberry"},{"HERB_LOOT_GOLDEN_CURRANT"_J, "Golden Currant"},{"HERB_LOOT_HUMMINGBIRD_SAGE"_J, "Hummingbird Sage"},{"HERB_LOOT_INDIAN_TOBACCO"_J, "Indian Tobacco"},{"HERB_LOOT_MILKWEED"_J, "Milkweed"},{"HERB_LOOT_OLEANDER_SAGE"_J, "Oleander Sage"},{"HERB_LOOT_OREGANO"_J, "Oregano"},{"HERB_LOOT_PARASOL_MUSHROOM"_J, "Parasol Mushroom"},{"HERB_LOOT_PRAIRIE_POPPY"_J, "Prairie Poppy"},{"HERB_LOOT_RAMS_HEAD"_J, "Rams Head"},{"HERB_LOOT_RED_RASPBERRY"_J, "Red Raspberry"},{"HERB_LOOT_RED_SAGE"_J, "Red Sage"},{"HERB_LOOT_VANILLA_FLOWER"_J, "Vanilla Flower"},{"HERB_LOOT_VIOLET_SNOWDROP"_J, "Violet Snowdrop"},{"HERB_LOOT_WILD_CARROTS"_J, "Wild Carrots"},{"HERB_LOOT_WILD_FEVERFEW"_J, "Wild Feverfew"},{"HERB_LOOT_WILD_MINT"_J, "Wild Mint"},{"HERB_LOOT_WINTERGREEN_BERRY"_J, "Wintergreen Berry"},{"HERB_LOOT_YARROW"_J, "Yarrow"},{"HERB_LOOT_AGARITA"_J, "Agarita"},{"HERB_LOOT_BITTERWEED"_J, "Bitterweed"},{"HERB_LOOT_BLUE_BONNET"_J, "Blue Bonnet"},{"HERB_LOOT_BLOOD_FLOWER"_J, "Blood Flower"},{"HERB_LOOT_CARDINAL_FLOWER"_J, "Cardinal Flower"},{"HERB_LOOT_CHOCOLATE_DAISY"_J, "Chocolate Daisy"},{"HERB_LOOT_CREEK_PLUM"_J, "Creek Plum"},{"HERB_LOOT_RHUBARB"_J, "Rhubarb"},{"HERB_LOOT_WISTERIA"_J, "Wisteria"},{"HERB_LOOT_HARRIETUM"_J, "Harrietum"},};
				if (ImGui::BeginCombo("Herbs", herb_translations[selectedHerb].c_str()))
				{
					for (auto& [herb, translation] : herb_translations)
					{
						if (ImGui::Selectable(std::string(translation).c_str(), herb == selectedHerb))
						{
							selectedHerb = herb;
						}
					}
					ImGui::EndCombo();
				}

				static int amount = 1;
				ImGui::SliderInt("Amount", &amount, 1, 10);

				if (ImGui::Button("Give Selected"))
				{
					FiberPool::Push([] {
						if (!Scripts::RequestScript("interactive_campfire"_J))
							return;

						for (int i = 0; i < amount; i++)
							ScriptFunctions::GiveLootTableAward.StaticCall(selectedHerb, 0);
					});
				}
			}
		}));
		recoveryOptions->AddItem(std::make_shared<BoolCommandItem>("unlimiteditems"_J));
		recoveryOptions->AddItem(std::make_shared<BoolCommandItem>("fastmoonshine"_J));
		recovery->AddItem(spawnCollectiblesGroup);
		recovery->AddItem(spawnHerbsGroup);
		recovery->AddItem(recoveryOptions);

		auto stats = std::make_shared<Category>("Stats");
		auto statEditorGroup = std::make_shared<Group>("Stat Editor");
		statEditorGroup->AddItem(std::make_shared<ImGuiItem>([=] {
			static char baseidBuffer[255]{};
			static char permutationidBuffer[255]{};
			static int StatType = 0;
			static bool BoolValue = false;
			static int IntValue = 0;
			static float FloatValue = 0;
			static bool updateValue = false;
			static bool isStatValid = false;

			ImGui::SetNextItemWidth(225.0f);
			if (ImGui::InputText("Base ID", baseidBuffer, sizeof(baseidBuffer)))
				updateValue = true;

			ImGui::SetNextItemWidth(225.0f);
			if (ImGui::InputText("Permutation ID", permutationidBuffer, sizeof(permutationidBuffer)))
				updateValue = true;

			ImGui::SameLine();

			ImGui::SetNextItemWidth(175.f);
			if (ImGui::Combo("##stat_type", &StatType, "Bool\0Int\0Float"))
				updateValue = true;

			if (updateValue)
			{
				FiberPool::Push([] {
					joaat_t baseid = Joaat(baseidBuffer), permutationid = Joaat(permutationidBuffer);
					if (!Stats::IsValid(baseid, permutationid))
					{
						isStatValid = false;
						return;
					}
					isStatValid = true;
					switch (StatType)
					{
					case 0:
					{
						BoolValue = Stats::GetBool(baseid, permutationid);
						break;
					}
					case 1:
					{
						IntValue = Stats::GetInt(baseid, permutationid);
						break;
					}
					case 2:
					{
						FloatValue = Stats::GetFloat(baseid, permutationid);
						break;
					}
					}
				});
				updateValue = false;
			}

			if (isStatValid)
			{
				switch (StatType)
				{
				case 0:
				{
					ImGui::Checkbox("Value", &BoolValue);
					break;
				}
				case 1:
				{
					ImGui::SetNextItemWidth(200.f);
					ImGui::InputInt("Value", &IntValue);
					break;
				}
				case 2:
				{
					ImGui::SetNextItemWidth(200.f);
					ImGui::InputFloat("Value", &FloatValue);
					break;
				}
				}

				if (ImGui::Button("Set Stat"))
				{
					FiberPool::Push([] {
						joaat_t baseid = Joaat(baseidBuffer), permutationid = Joaat(permutationidBuffer);
						switch (StatType)
						{
						case 0:
						{
							Stats::SetBool(baseid, permutationid, BoolValue);
							break;
						}
						case 1:
						{
							Stats::SetInt(baseid, permutationid, IntValue);
							break;
						}
						case 2:
						{
							Stats::SetFloat(baseid, permutationid, FloatValue);
							break;
						}
						}
					});
				}
			}
		}));

		auto allStatsGroup = std::make_shared<Group>("Set All Stats");
		allStatsGroup->AddItem(std::make_shared<ImGuiItem>([=] {
			if (ImGui::Button("Bool Stats"))
			{
				FiberPool::Push([] {
					for (auto& stat : Data::bool_stats)
					{
						Stats::SetBool(stat.BaseId, stat.PermutationId, stat.desiredValue);
					}
				});
			}
			ImGui::SameLine();
			if (ImGui::Button("Int Stats"))
			{
				FiberPool::Push([] {
					for (auto& stat : Data::int_stats)
					{
						Stats::IncrementInt(stat.BaseId, stat.PermutationId, stat.desiredValue);
					}
				});
			}
			ImGui::SameLine();
			if (ImGui::Button("Float Stats"))
			{
				FiberPool::Push([] {
					for (auto& stat : Data::float_stats)
					{
						Stats::IncrementFloat(stat.BaseId, stat.PermutationId, stat.desiredValue);
					}
				});
			}
		}));
		stats->AddItem(statEditorGroup);
		stats->AddItem(allStatsGroup);

		AddCategory(std::move(recovery));
		AddCategory(std::move(stats));
	}
}