#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

#include "SpehsEngine/Core/File/DirectoryState.h"
#include "SpehsEngine/Input/EventSignaler.h"


#pragma optimize("", off) // nocommit
namespace ImGui
{
	namespace
	{
		bool renderDirectoryStateRecursive(const se::DirectoryState& directoryState, std::string& filepath)
		{
			bool changed = false;
			if (!directoryState.path.empty())
			{
				ImGui::Button(directoryState.path.c_str());
				ImGui::Indent();
			}
			for (const se::DirectoryState& child : directoryState.directories)
			{
				changed = renderDirectoryStateRecursive(child, filepath) || changed;
			}
			for (const se::DirectoryState::FileState& fileState : directoryState.files)
			{
				if (ImGui::Button(fileState.path.c_str()))
				{
					filepath = fileState.path;
					changed = true;
				}
			}
			if (!directoryState.path.empty())
			{
				ImGui::Unindent();
			}
			return changed;
		}
	}

	bool fileSelector(const char* const label, std::string& filepath, const char* const directoryPtr)
	{
		const std::string_view directory(directoryPtr);
		bool changed = false;
		ImGui::Button("Browse");
		const std::string browseContextMenuId = std::string(label) + filepath + std::string(directory);
		if (ImGui::BeginPopupContextItem(browseContextMenuId.c_str(), ImGuiPopupFlags_MouseButtonLeft))
		{
			se::DirectoryState directoryState;
			se::getDirectoryState(directoryState, directory, se::DirectoryState::Flag::none, 0);
			if (renderDirectoryStateRecursive(directoryState, filepath))
			{
				if (!directory.empty())
				{
					if (directory.back() == '/')
					{
						filepath.insert(filepath.begin(), directory.begin(), directory.end());
					}
					else
					{
						filepath = std::string(directory) + "/" + filepath;
					}
				}
				changed = true;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (InputT(label, filepath))
		{
			changed = true;
		}

		return changed;
	}

	std::optional<bool> fileDialog(const char* const label, const char* const message, const char* const _directory, std::string& output)
	{
		std::optional<bool> result;
		const ImGuiWindowFlags windowFlags =
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoCollapse;
		ImGui::PushID("se::fileSelector()");
		if (strlen(message) > 0)
		{
			ImGui::Text(message);
		}
		if (ImGui::Begin(label, nullptr, windowFlags))
		{
			se::DirectoryState directoryState;
			se::getDirectoryState(directoryState, _directory, se::DirectoryState::Flag::none, 0);
			if (renderDirectoryStateRecursive(directoryState, output))
			{
				std::string_view directory(_directory);
				if (!directory.empty())
				{
					if (directory.back() == '/')
					{
						output.insert(output.begin(), directory.begin(), directory.end());
					}
					else
					{
						output = std::string(directory) + "/" + output;
					}
				}
				result = true;
			}
			if (ImGui::Button("Cancel"))
			{
				result = false;
			}
		}
		ImGui::PopID();
		return result;
	}

	bool textureSelector(const char* const label, std::string& filepath, const char* const directory)
	{
		return fileSelector(label, filepath, directory);
	}

	std::optional<bool> confirmationDialog(const char* const header, const char* const message)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header);

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(header, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(message);
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				result.emplace(true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				result.emplace(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return result;
	}

	std::optional<bool> stringDialog(const char* const header, const char* const message, std::string& output)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header);

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(header, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			
			if (!std::string_view(message).empty())
			{
				ImGui::Text(message);
			}
			/*
			TODO: finish dialog if the user presses 'enter', below is some previous research on the subject
			struct Callback
			{
				static int callback(ImGuiInputTextCallbackData* const data)
				{
					if (data->EventKey == ImGuiKey_Enter)
					{

					}
					return 0;
				}
			};
			ImGui::InputT("", output, inputTextFlags, &Callback::callback);
			const bool inputDeactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit(); // Returns true also on 'esc' and on clicking away
			*/
			ImGuiInputTextFlags inputTextFlags = ImGuiInputTextFlags_EnterReturnsTrue; // Works otherwise than pressing 'esc' which clears the buffer
			if (ImGui::InputT("", output, inputTextFlags))
			{
				result.emplace(true);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				result.emplace(true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetItemDefaultFocus();
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				result.emplace(false);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		return result;
	}

	void keyBindButton(const char* const label, se::input::Key& key, se::input::EventSignaler& eventSignaler, boost::signals2::scoped_connection& scopedConnection)
	{
		ImGui::PushID(&scopedConnection);
		ImGui::Text(label);
		ImGui::SameLine();
		if (ImGui::Button(scopedConnection.connected() ? "press any key" : se::input::toString(key)))
		{
			eventSignaler.connectToKeyboardPressSignal(
				scopedConnection,
				[&](const se::input::KeyboardPressEvent& event) -> bool
				{
					if (event.key != se::input::Key(se::input::Key::ESCAPE))
					{
						key = event.key;
					}
					scopedConnection.disconnect();
					return true;
				}, INT_MAX);
		}
		ImGui::PopID();
	}

	std::string getImGuiFormatString(const std::string_view string)
	{
		std::string imGuiFormatString = std::string(string);

		// NOTE: because imgui only accepts format strings in tooltips, we have to replace each "%" with "%%"
		for (std::string::iterator it = imGuiFormatString.begin(); it != imGuiFormatString.end(); it++)
		{
			if (*it == '%')
			{
				std::string::iterator nextIt = it;
				nextIt++;
				it = imGuiFormatString.insert(nextIt, '%');
			}
		}

		return imGuiFormatString;
	}
}
