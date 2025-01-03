#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

#include "SpehsEngine/Core/File/DirectoryState.h"
#include "SpehsEngine/Graphics/Texture.h"
#include "SpehsEngine/Input/EventSignaler.h"
#include "SpehsEngine/Input/CustomEventParametersRecorder.h"
#include "SpehsEngine/ImGui/Utility/BackendWrapper.h"


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
	
	ImVec2 SizeToScale(const se::graphics::Texture& _texture, const ImVec2 _size)
	{
		return ImVec2(_size.x / float(_texture.getWidth()), _size.y / float(_texture.getHeight()));
	}

	bool ButtonWithConfirm(const String label, const ImVec2& size)
	{
		bool result = false;
		const bool wasOpen = ImGui::GetStateStorage()->GetInt(ImGui::GetID(label)) != 0;
		std::optional<int> newState;
		if (Button(label))
		{
			if (wasOpen)
			{
				newState.emplace(0);
			}
			else
			{
				newState.emplace(1);
			}
		}
		if (wasOpen)
		{
			Indent();
			if (Button("Confirm", size))
			{
				ImGui::GetStateStorage()->SetInt(ImGui::GetID(label), 0);
				result = true;
				newState.emplace(0);
			}
			else if (Button("Cancel", size))
			{
				newState.emplace(0);
			}
			Unindent();
		}
		if (newState)
		{
			ImGui::GetStateStorage()->SetInt(ImGui::GetID(label), *newState);
		}
		return result;
	}

	void Image(
		const se::graphics::Texture& _texture,
		const ImVec2 scale,
		const ImVec2 uv0,
		const ImVec2 uv1,
		const se::Color tintColor,
		const se::Color borderColor)
	{
		se::imgui::ImGuiUserTextureData userTextureData;
		userTextureData.resourceHandle = _texture.getHandle();
		ImGui::Image(
			userTextureData.id,
			ImVec2(float(_texture.getWidth()) * scale.x, float(_texture.getHeight()) * scale.y),
			uv0,
			uv1,
			se::toImVec4(tintColor),
			se::toImVec4(borderColor));
	}

	bool ImageButton(
		const se::graphics::Texture& _texture,
		const ImVec2 scale,
		const ImVec2 uv0,
		const ImVec2 uv1,
		const int framePadding,
		const se::Color tintColor,
		const se::Color backgroundColor)
	{
		se::imgui::ImGuiUserTextureData userTextureData;
		userTextureData.resourceHandle = _texture.getHandle();
		return ImGui::ImageButton(
			userTextureData.id,
			ImVec2(float(_texture.getWidth()) * scale.x, float(_texture.getHeight()) * scale.y),
			uv0,
			uv1,
			framePadding,
			se::toImVec4(backgroundColor),
			se::toImVec4(tintColor));
	}

	bool fileSelector(const String _label, std::string& _filepath, const String _directory)
	{
		const std::string directory(_directory);
		bool changed = false;
		ImGui::Button("Browse");
		const std::string browseContextMenuId = std::string(_label) + _filepath + directory;
		if (ImGui::BeginPopupContextItem(browseContextMenuId.c_str(), ImGuiPopupFlags_MouseButtonLeft))
		{
			se::DirectoryState directoryState;
			se::getDirectoryState(directoryState, directory, se::DirectoryState::Flag::None, 0);
			if (renderDirectoryStateRecursive(directoryState, _filepath))
			{
				if (!directory.empty())
				{
					if (directory.back() == '/')
					{
						_filepath.insert(_filepath.begin(), directory.begin(), directory.end());
					}
					else
					{
						_filepath = std::string(directory) + "/" + _filepath;
					}
				}
				changed = true;
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (InputT(_label, _filepath))
		{
			changed = true;
		}

		return changed;
	}

	std::optional<bool> fileDialog(const String label, const String message, const String _directory, std::string& output)
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
		bool open = true;
		if (ImGui::Begin(label, &open, windowFlags))
		{
			if (!open)
			{
				result = false;
			}
			se::DirectoryState directoryState;
			se::getDirectoryState(directoryState, std::string(_directory.pointer), se::DirectoryState::Flag::None, 0);
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

	bool textureSelector(const String label, std::string& filepath, const String directory)
	{
		return fileSelector(label, filepath, directory);
	}

	std::optional<bool> confirmationDialog(const String header, const String message, const std::function<void()>& customRender)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header.pointer);

		ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(header, NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(message);
			if (customRender)
			{
				customRender();
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

	std::optional<bool> stringDialog(const String header, const String message, std::string& output)
	{
		std::optional<bool> result;

		ImGui::OpenPopup(header.pointer);

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

	void keyBindButton(const String label, se::input::Key& key, se::input::EventSignaler& eventSignaler, se::ScopedConnection& scopedConnection)
	{
		ImGui::PushID(&scopedConnection);
		ImGui::Text(label);
		ImGui::SameLine();
		if (ImGui::Button(scopedConnection ? "press any key" : se::input::toString(key)))
		{
			eventSignaler.connectToKeyboardSignal(
				scopedConnection,
				[&](const se::input::KeyboardEvent& event) -> bool
				{
					if (event.type == se::input::KeyboardEvent::Type::Press)
					{
						if (event.key != se::input::Key(se::input::Key::ESCAPE))
						{
							key = event.key;
						}
						scopedConnection.disconnect();
						return true;
					}
					else	
					{
						return false;
					}
				}, INT_MAX);
		}
		ImGui::PopID();
	}

	bool InputT(const String label, se::input::CustomEventParameters& customEventParameters, se::input::EventSignaler& eventSignaler, StateWrapper& stateWrapper)
	{
		struct State : public IState
		{
			std::unique_ptr<se::input::CustomEventParametersRecorder> customEventParametersRecorder;
		};
		State& state = stateWrapper.get<State>();

		bool changed = false;
		if (state.customEventParametersRecorder)
		{
			ImGui::Text(se::formatString("%s: press any key...", label));
			if (const std::optional<se::input::CustomEventParameters> result = state.customEventParametersRecorder->getCustomEventParameters())
			{
				customEventParameters = *result;
				state.customEventParametersRecorder.reset();
				changed = true;
			}
		}
		else if (ImGui::Button(se::formatString("%s: %s", label, customEventParameters.toString().c_str())))
		{
			state.customEventParametersRecorder.reset(new se::input::CustomEventParametersRecorder(eventSignaler));
		}

		return changed;
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

	void PushFont(const se::imgui::ImGuiFont _font)
	{
		PushFont(getFont(_font));
	}

	bool InputT(const String label, se::Time& value,
		const se::Time step,
		const se::Time stepFast, ImGuiInputTextFlags flags)
	{
		const bool result = InputT(label, value.value, step.value, stepFast.value, "%llu", flags);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(se::toTimeLengthString(value, 2).c_str());
		}
		return result;
	}

	bool InputAngle(const String label, float& radians)
	{
		float degrees = (radians / se::PI<float>) * 180.0f;
		const bool changed = InputT(label, degrees);
		if (changed)
		{
			radians = (degrees / 180.0f) * se::PI<float>;
		}
		return changed;
	}

	bool InputT(const String label, se::TimeInfo& timeInfo)
	{
		bool changed = false;
		if (ImGui::CollapsingHeader(label))
		{
			changed |= ImGui::InputT("Day", timeInfo.monthDay);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Day of the month");
			}
			changed |= ImGui::InputT("Month", timeInfo.month);
			changed |= ImGui::InputT("Year", timeInfo.year);
			changed |= ImGui::InputT("Hour", timeInfo.hour);
			changed |= ImGui::InputT("Minute", timeInfo.minute);
			changed |= ImGui::InputT("Second", timeInfo.second);
			changed |= ImGui::InputT("Dayligt savings flag", timeInfo.dayligtSavingsFlag);
		}
		return changed;
	}

	bool CollapsingHeader2(const String idLabel, const String mutableLabel, const ImGuiTreeNodeFlags flags)
	{
		const bool result = ImGui::CollapsingHeader(idLabel, flags);
		ImGui::SameLine();
		ImGui::Text("");
		ImGui::SameLine();
		ImGui::Text(mutableLabel);
		return result;
	}

	bool BeginCentered(const String label, bool* const open, const ImGuiWindowFlags flags, const ImGuiCond centerCondition)
	{
		const ImGuiIO& io = ImGui::GetIO();
		const ImVec2 displayCenter(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(displayCenter, centerCondition, ImVec2(0.5f, 0.5f));
		return ImGui::Begin(label, open, flags);
	}
}
