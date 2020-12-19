#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

#include "SpehsEngine/Core/File/DirectoryState.h"


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
		se::DirectoryState directoryState;
		se::getDirectoryState(directoryState, directory, se::DirectoryState::Flag::none, 0);
		bool changed = false;
		ImGui::Button("Browse");
		const std::string browseContextMenuId = std::string(label) + filepath + std::string(directory);
		if (ImGui::BeginPopupContextItem(browseContextMenuId.c_str(), ImGuiPopupFlags_MouseButtonLeft))
		{
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

	std::optional<bool> confirmationDialog(const std::string_view label)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(label.data());

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(label.data(), NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(label.data());
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
}
