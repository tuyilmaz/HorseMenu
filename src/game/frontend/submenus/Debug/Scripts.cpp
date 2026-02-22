#include "Scripts.hpp"
#include "game/backend/Self.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/backend/ScriptMgr.hpp"
#include "game/rdr/Scripts.hpp"
#include "game/rdr/data/ScriptNames.hpp"
#include "game/rdr/data/StackSizes.hpp"
#include "game/rdr/Natives.hpp"
#include <script/scrProgram.hpp>
#include <script/scrThread.hpp>
#include <script/scriptHandlerNetComponent.hpp>

static rage::scrThread* s_SelectedThread;
static rage::scrProgram* s_SelectedProgram;
static int s_SelectedStackSize             = 128;
static int s_NumFreeStacks                 = -1;
static const char* s_SelectedScriptName    = "(Select)";
static const char* s_SelectedNewScriptName = "(Select)";
static const char* s_SelectedStackSizeStr  = "MICRO";
static std::chrono::high_resolution_clock::time_point s_LastStackUpdateTime{};

namespace
{
	static void UpdateFreeStackSizeCount()
	{
		s_NumFreeStacks = MISC::GET_NUMBER_OF_FREE_STACKS_OF_THIS_SIZE(s_SelectedStackSize);
	}
}

namespace YimMenu::Submenus
{

	static void RenderBytecode(rage::scrProgram* program)
	{
		constexpr int bytesPerRow = 16;
		const std::uint32_t codeSize = program->GetFullCodeSize();
		const std::uint32_t totalRows = (codeSize + bytesPerRow - 1) / bytesPerRow;

		static bool shouldJump = false;
		static float targetScroll = -1.0f;
		static char offsetInput[9] = "";

		ImGui::SetNextItemWidth(150);
		ImGui::InputText("##jumpoffset", offsetInput, IM_ARRAYSIZE(offsetInput));
		ImGui::SameLine();
		if (ImGui::Button("Jump to Offset"))
		{
			char* end = nullptr;
			std::uint32_t offset = strtoul(offsetInput, &end, 0);
			if (end != offsetInput && offset < codeSize)
			{
				std::uint32_t row = offset / bytesPerRow;
				targetScroll = row * ImGui::GetFrameHeightWithSpacing();
				shouldJump = true;
			}
		}

		ImGui::BeginChild("##bytecode", ImVec2(610, 400), ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);

		if (shouldJump && targetScroll >= 0.0f)
		{
			ImGui::SetScrollY(targetScroll);
			shouldJump = false;
		}

		ImGuiListClipper clipper;
		clipper.Begin(totalRows);
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				std::uint32_t offset = row * bytesPerRow;

				ImGui::Text("%08X: ", offset);
				ImGui::SameLine(80);

				for (int i = 0; i < bytesPerRow; ++i)
				{
					std::uint32_t index = offset + i;
					if (index >= codeSize)
						break;

					if (auto byte = program->GetCodeAddress(index))
					{
						char hexStr[3];
						snprintf(hexStr, sizeof(hexStr), "%02X", *byte);

						ImGui::SetNextItemWidth(24);
						ImGui::PushID(index);
						if (ImGui::InputText("##byte", hexStr, sizeof(hexStr), ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue))
						{
							char* end = nullptr;
							std::uint8_t newVal = static_cast<std::uint8_t>(strtoul(hexStr, &end, 16));
							if (end != hexStr)
								*byte = newVal; // this will conflict with the ScriptPatches class, but still useful to keep for quick testings
						}
						ImGui::PopID();
						if (ImGui::IsItemActive() && ImGui::IsItemHovered())
							ImGui::SetTooltip("Press ENTER to write.");

						if (i < bytesPerRow - 1)
							ImGui::SameLine();
					}
				}
			}
		}

		clipper.End();
		ImGui::EndChild();
	}

	std::shared_ptr<Category> BuildScriptsMenu()
	{
		auto scripts = std::make_unique<Category>("Scripts");

		auto threads = std::make_unique<Group>("Threads");
		threads->AddItem(std::make_unique<ImGuiItem>([] {
			if (!Pointers.ScriptThreads || Pointers.ScriptThreads->size() == 0)
			{
				ImGui::TextDisabled("None");
				s_SelectedThread = nullptr;
				s_SelectedProgram = nullptr;
				return;
			}

			ImGui::SetNextItemWidth(225.0f);
			if (ImGui::BeginCombo("Thread", s_SelectedThread ? s_SelectedScriptName : "(Select)"))
			{
				for (auto script : *Pointers.ScriptThreads)
				{
					if (script)
					{
						if (script->m_Context.m_State != rage::eThreadState::killed && script->m_Context.m_StackSize == 0)
							continue;

						ImGui::PushID(script->m_Context.m_ThreadId);

						if (script->m_Context.m_State == rage::eThreadState::killed)
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.1f, 0.1f, 1.f));

						if (ImGui::Selectable(Scripts::GetScriptName(script->m_Context.m_ScriptHash), s_SelectedThread == script))
						{
							s_SelectedThread = script;
							s_SelectedProgram = Scripts::FindScriptProgram(script->m_Context.m_ScriptHash);
							s_SelectedScriptName = Scripts::GetScriptName(script->m_Context.m_ScriptHash);
						}

						if (s_SelectedThread == script)
							ImGui::SetItemDefaultFocus();

						if (script->m_Context.m_State == rage::eThreadState::killed)
							ImGui::PopStyleColor();

						ImGui::PopID();
					}
				}

				ImGui::EndCombo();
			}

			if (!s_SelectedThread || !s_SelectedProgram || !s_SelectedProgram->IsValid() || s_SelectedProgram->m_RefCount == 0)
			{
				s_SelectedThread = nullptr;
				s_SelectedProgram = nullptr;
				return;
			}

			constexpr auto s_ThreadStateNames = std::to_array({"Idle", "Running", "Killed", "Paused", "Unk"});
			ImGui::SetNextItemWidth(95.0f);
			ImGui::Combo(
			    "State", (int*)&s_SelectedThread->m_Context.m_State, s_ThreadStateNames.data(), s_ThreadStateNames.size(), -1);

			if (s_SelectedThread->m_Context.m_State == rage::eThreadState::killed)
			{
				ImGui::Text(std::format("Exit Reason: {}", s_SelectedThread->m_ExitMessage).c_str());
			}
			else
			{
				if (ImGui::Button("Kill"))
				{
					FiberPool::Push([] {
						if (s_SelectedThread->m_Context.m_StackSize != 0)
							s_SelectedThread->Kill();

						s_SelectedThread->m_Context.m_State = rage::eThreadState::killed;
					});
				}
				ImGui::SameLine();
				if (ImGui::Button("Log Labels"))
				{
					FiberPool::Push([] {
						for (int i = 0; i < s_SelectedProgram->m_StringsCount; i++)
						{
							if (auto str = s_SelectedProgram->GetString(i))
							{
								if (HUD::DOES_TEXT_LABEL_EXIST(str))
								{
									LOGF(INFO, "{} - {} (0x{:X}): {}", i, str, Joaat(str), HUD::GET_FILENAME_FOR_AUDIO_CONVERSATION(str));
								}
							}
						}
					});
				}

				if (ImGui::TreeNode("Info"))
				{
					if (auto handler = static_cast<rage::scriptHandlerNetComponent*>(s_SelectedThread->m_HandlerNetComponent))
					{
						auto host = handler->GetHost();
						auto hostPlayer = Player(host);
						if (host)
						{
							ImGui::Text("Host: %s", hostPlayer.GetName());
						}
						ImGui::SameLine();
						ImGui::BeginDisabled(hostPlayer == Self::GetPlayer());
						if (ImGui::SmallButton("Take Control"))
						{
							FiberPool::Push([handler] {
								handler->DoHostMigration(Self::GetPlayer().GetHandle(), 0xFFFF, true);
							});
						}
						ImGui::EndDisabled();
					}
					ImGui::BeginGroup();
					ImGui::Text("Thread ID: %d", s_SelectedThread->m_Context.m_ThreadId);
					ImGui::Text("Stack Size: %d", s_SelectedThread->m_Context.m_StackSize);
					ImGui::Text("Stack Pointer: 0x%X", s_SelectedThread->m_Context.m_StackPointer);
					ImGui::Text("Program Counter: 0x%X", s_SelectedThread->m_Context.m_ProgramCounter);
					ImGui::Text("Code Size: %d", s_SelectedProgram->m_CodeSize);
					ImGui::EndGroup();
					ImGui::SameLine();
					ImGui::BeginGroup();
					ImGui::Text("Arg Count: %d", s_SelectedProgram->m_ArgCount);
					ImGui::Text("Local Count: %d", s_SelectedProgram->m_LocalCount);
					ImGui::Text("Global Count: %d", s_SelectedProgram->m_GlobalCount);
					ImGui::Text("Native Count: %d", s_SelectedProgram->m_NativeCount);
					ImGui::Text("String Count: %d", s_SelectedProgram->m_StringsCount);
					ImGui::EndGroup();
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("Bytecode"))
				{
					RenderBytecode(s_SelectedProgram);
					ImGui::TreePop();
				}
			}
		}));

		auto new_ = std::make_unique<Group>("New");
		new_->AddItem(std::make_unique<ImGuiItem>([] {
			ImGui::SetNextItemWidth(225.0f);
			if (ImGui::BeginCombo("Name", s_SelectedNewScriptName))
			{
				auto& map = Scripts::UsingMPScripts() ? Data::g_MpScriptNames : Data::g_SpScriptNames;
				for (auto& el : map)
				{
					if (ImGui::Selectable(el.second, el.second == s_SelectedNewScriptName))
					{
						s_SelectedNewScriptName = el.second;
					}

					if (el.second == s_SelectedNewScriptName)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::SetNextItemWidth(225.0f);
			if (ImGui::BeginCombo("Stack Size", s_SelectedStackSizeStr))
			{
				for (auto& p : Data::g_StackSizes)
				{
					if (ImGui::Selectable(std::format("{} ({})", p.first, p.second).data(), s_SelectedStackSize == p.second))
					{
						s_SelectedStackSizeStr = p.first;
						s_SelectedStackSize    = p.second;

						FiberPool::Push([] {
							UpdateFreeStackSizeCount();
						});
					}

					if (p.second == s_SelectedStackSize)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Text(std::format("Free Stacks: {}", s_NumFreeStacks).c_str());

			if (ImGui::Button("Start"))
			{
				FiberPool::Push([] {
					auto hash = Joaat(s_SelectedNewScriptName);

					if (!SCRIPTS::DOES_SCRIPT_WITH_NAME_HASH_EXIST(hash))
					{
						return;
					}

					if (MISC::GET_NUMBER_OF_FREE_STACKS_OF_THIS_SIZE(s_SelectedStackSize) == 0)
					{
						return;
					}

					while (!SCRIPTS::HAS_SCRIPT_WITH_NAME_HASH_LOADED(hash))
					{
						SCRIPTS::REQUEST_SCRIPT_WITH_NAME_HASH(hash);
						ScriptMgr::Yield();
					}

					SCRIPTS::START_NEW_SCRIPT_WITH_NAME_HASH(hash, s_SelectedStackSize);
					SCRIPTS::SET_SCRIPT_WITH_NAME_HASH_AS_NO_LONGER_NEEDED(hash);

					UpdateFreeStackSizeCount();
				});
			};


			if (Pointers.ScriptThreads && Pointers.ScriptThreads->size() > 0 && std::chrono::high_resolution_clock::now() - s_LastStackUpdateTime > 100ms)
			{
				s_LastStackUpdateTime = std::chrono::high_resolution_clock::now();
				FiberPool::Push([] {
					UpdateFreeStackSizeCount();
				});
			}
		}));

		scripts->AddItem(std::move(threads));
		scripts->AddItem(std::move(new_));

		return scripts;
	}
}