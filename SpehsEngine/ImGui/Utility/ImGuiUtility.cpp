#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

#include "SpehsEngine/Core/File/DirectoryState.h"
#include "SpehsEngine/Input/EventSignaler.h"


#pragma optimize("", off) // nocommit
namespace ImGui
{
	bool renderDirectoryStateRecursive(const se::DirectoryState& directoryState, std::string& filepath)
	{
		bool changed = false;
		ImGui::Button(directoryState.path.c_str());
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
		return changed;
	}

	bool fileSelector(const std::string_view label, std::string& filepath, const std::string_view directory)
	{
		bool changed = false;
		ImGui::Button("Browse");
		const std::string browseContextMenuId = std::string(label) + filepath + std::string(directory);
		if (ImGui::BeginPopupContextItem(browseContextMenuId.c_str(), ImGuiPopupFlags_MouseButtonLeft))
		{
			se::DirectoryState directoryState;
			se::getDirectoryState(directoryState, directory, se::DirectoryState::Flag::none, 0);
			if (renderDirectoryStateRecursive(directoryState, filepath))
			{
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

	bool textureSelector(const std::string_view label, std::string& filepath, const std::string_view directory)
	{
		return fileSelector(label, filepath, directory);
	}

	std::optional<bool> confirmationDialog(const std::string_view header, const std::string_view message)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header.data());

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(header.data(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(message.data());
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

	std::optional<bool> stringDialog(const std::string_view header, const std::string_view message, std::string& output)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header.data());

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(header.data(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (!message.empty())
			{
				ImGui::Text(message.data());
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

	void keyBindButton(const std::string_view label, se::input::Key& key, se::input::EventSignaler& eventSignaler, boost::signals2::scoped_connection& scopedConnection)
	{
		ImGui::Text(label.data());
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
	}
}
