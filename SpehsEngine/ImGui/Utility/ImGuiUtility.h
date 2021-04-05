#pragma once

#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_stdlib.h"
#include "glm/vec4.hpp"
#include "glm/ext/quaternion_float.hpp"
#include <stdint.h>

namespace se
{
	namespace input
	{
		class EventSignaler;
		enum class Key;
	}
}


// Used to implement a dropdown selector for an enum. p_RangeStart value is inclusive, but p_RangeEnd is exclusive.
#define SE_IMGUI_INPUT_T_ENUM_RANGE_DROPDOWN(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction) \
	inline bool InputT(const char* const p_label, p_EnumType& p_value) \
	{ \
		struct Callback { static bool callback(void* data, int n, const char** out_str) { *out_str = p_ToStringFunction(p_EnumType(n + int(p_RangeStart))); return true; } }; \
		int p_current = int(p_value) - int(p_RangeStart); \
		if (ImGui::Combo(p_label, &p_current, &Callback::callback, nullptr, int(p_RangeEnd) - int(p_RangeStart))) \
		{ \
			p_value = p_EnumType(p_current + int(p_RangeStart)); \
			return true; \
		} \
		else \
		{ \
			return false; \
		} \
	} \
	inline bool InputT(const std::string& p_label, p_EnumType& p_value) \
	{ \
		return InputT(p_label.c_str(), p_value); \
	}

namespace ImGui
{
	// Drag scalars
#define SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, p_Components, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	inline bool DragScalar##p_Components(const char* const label, p_ScalarType& i, float speed, p_ScalarType min, p_ScalarType max, const char* format = p_DefaultFormat, ImGuiSliderFlags flags = p_DefaultImGuiSliderFlags) \
	{ \
		return DragScalarN(label, p_ImGuiDataType, &i, p_Components, speed, &min, &max, format, flags); \
	} \
	inline bool DragScalar##p_Components(const std::string& label, p_ScalarType& i, float speed, p_ScalarType min, p_ScalarType max, const char* format = p_DefaultFormat, ImGuiSliderFlags flags = p_DefaultImGuiSliderFlags) \
	{ \
		return DragScalarN(label.c_str(), p_ImGuiDataType, &i, p_Components, speed, &min, &max, format, flags); \
	}
#define SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(p_ScalarType, p_ImGuiDataType, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 1, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 2, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 3, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 4, p_DefaultFormat, p_DefaultImGuiSliderFlags)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint8_t, ImGuiDataType_U8, "%hhu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint16_t, ImGuiDataType_U16, "%hu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint32_t, ImGuiDataType_U32, "%u", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint64_t, ImGuiDataType_U64, "%llu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int8_t, ImGuiDataType_S8, "%hhi", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int16_t, ImGuiDataType_S16, "%hi", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int32_t, ImGuiDataType_S32, "%i", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int64_t, ImGuiDataType_S64, "%lli", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(float, ImGuiDataType_Float, "%f", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(double, ImGuiDataType_Double, "%f", 0)
#undef SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS
#undef SE_IMGUI_DRAG_SCALAR

	// Input scalars
#define SE_IMGUI_INPUT_SCALAR(p_ScalarType, p_ImGuiDataType, p_DefaultFormat, p_DefaultImGuiTextFlags) \
	inline bool InputT(const char* const label, p_ScalarType& value, p_ScalarType step = 1, p_ScalarType stepFast = 100, const char* format = p_DefaultFormat, ImGuiInputTextFlags flags = p_DefaultImGuiTextFlags) \
	{ \
		return InputScalar(label, p_ImGuiDataType, &value, &step, &stepFast, format, flags); \
	} \
	inline bool InputT(const std::string& label, p_ScalarType& value, p_ScalarType step = 1, p_ScalarType stepFast = 100, const char* format = p_DefaultFormat, ImGuiInputTextFlags flags = p_DefaultImGuiTextFlags) \
	{ \
		return InputScalar(label.c_str(), p_ImGuiDataType, &value, &step, &stepFast, format, flags); \
	}
	SE_IMGUI_INPUT_SCALAR(uint8_t, ImGuiDataType_U8, "%hhu", 0)
	SE_IMGUI_INPUT_SCALAR(uint16_t, ImGuiDataType_U16, "%hu", 0)
	SE_IMGUI_INPUT_SCALAR(uint32_t, ImGuiDataType_U32, "%u", 0)
	SE_IMGUI_INPUT_SCALAR(uint64_t, ImGuiDataType_U64, "%llu", 0)
	SE_IMGUI_INPUT_SCALAR(int8_t, ImGuiDataType_S8, "%hhi", 0)
	SE_IMGUI_INPUT_SCALAR(int16_t, ImGuiDataType_S16, "%hi", 0)
	SE_IMGUI_INPUT_SCALAR(int32_t, ImGuiDataType_S32, "%i", 0)
	SE_IMGUI_INPUT_SCALAR(int64_t, ImGuiDataType_S64, "%lli", 0)
	SE_IMGUI_INPUT_SCALAR(float, ImGuiDataType_Float, "%.3f", ImGuiInputTextFlags_CharsScientific)
	SE_IMGUI_INPUT_SCALAR(double, ImGuiDataType_Double, "%.3f", ImGuiInputTextFlags_CharsScientific)
#undef SE_IMGUI_INPUT_SCALAR

	inline bool Button(const std::string& label, const ImVec2& size = ImVec2(0, 0))
	{
		return Button(label.c_str(), size);
	}

	inline void Text(const std::string& label)
	{
		Text(label.c_str());
	}

	inline void TextColored(const se::Color& color, const char* const label)
	{
		TextColored((ImVec4&)color, label);
	}
	inline void TextColored(const se::Color& color, const std::string& label)
	{
		TextColored((ImVec4&)color, label.c_str());
	}

	bool fileSelector(const char* const label, std::string& filepath, const char* const directory);
	inline bool fileSelector(const std::string& label, std::string& filepath, const std::string& directory)
	{
		return fileSelector(label.c_str(), filepath, directory.c_str());
	}

	std::optional<bool> fileDialog(const char* const header, const char* const message, const char* const directory, std::string& output);
	inline std::optional<bool> fileDialog(const std::string& header, const std::string& message, const std::string& directory, std::string& output)
	{
		return fileDialog(header.c_str(), message.c_str(), directory.c_str(), output);
	}

	bool textureSelector(const char* const label, std::string& filepath, const char* const directory);
	inline bool textureSelector(const std::string& label, std::string& filepath, const std::string& directory)
	{
		return textureSelector(label.c_str(), filepath, directory.c_str());
	}

	std::optional<bool> confirmationDialog(const char* const header, const char* const message);
	inline std::optional<bool> confirmationDialog(const std::string& header, const std::string& message)
	{
		return confirmationDialog(header.c_str(), message.c_str());
	}
	template<typename A, typename ... Arguments>
	inline std::optional<bool> confirmationDialogV(const std::string& header, const std::string& formatMessage, const A& argument, const Arguments&... arguments)
	{
		return confirmationDialog(header.c_str(), formatMessage.c_str(), argument, arguments...);
	}
	template<typename A, typename ... Arguments>
	inline std::optional<bool> confirmationDialogV(const char* const header, const char* const formatMessage, const A& argument, const Arguments&... arguments)
	{
		return confirmationDialog(header, se::formatString(formatMessage, argument, arguments...).c_str());
	}

	std::optional<bool> stringDialog(const char* const header, const char* const message, std::string& output);
	inline std::optional<bool> stringDialog(const std::string& header, const std::string& message, std::string& output)
	{
		return stringDialog(header.c_str(), message.c_str(), output);
	}

	inline bool InputT(const std::string& label, bool& value)
	{
		return Checkbox(label.c_str(), &value);
	}
	inline bool InputT(const char* const label, bool& value)
	{
		return Checkbox(label, &value);
	}
	inline bool InputT(const char* const label, std::string& value, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
	{
		return InputText(label, &value, flags, callback, user_data);
	}
	inline bool InputT(const std::string& label, std::string& value, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
	{
		return InputText(label.c_str(), &value, flags, callback, user_data);
	}
	inline bool InputT(const char* const label, glm::ivec2& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt2(label, &value.x, flags);
	}
	inline bool InputT(const std::string& label, glm::ivec2& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt2(label.c_str(), &value.x, flags);
	}
	inline bool InputT(const char* const label, glm::ivec3& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt3(label, &value.x, flags);
	}
	inline bool InputT(const std::string& label, glm::ivec3& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt3(label.c_str(), &value.x, flags);
	}
	inline bool InputT(const char* const label, glm::ivec4& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt4(label, &value.x, flags);
	}
	inline bool InputT(const std::string& label, glm::ivec4& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt4(label.c_str(), &value.x, flags);
	}
	inline bool InputT(const char* const label, glm::vec2& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat2(label, &value.x, format, flags);
	}
	inline bool InputT(const std::string& label, glm::vec2& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat2(label.c_str(), &value.x, format, flags);
	}
	inline bool InputT(const char* const label, glm::vec3& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat3(label, &value.x, format, flags);
	}
	inline bool InputT(const std::string& label, glm::vec3& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat3(label.c_str(), &value.x, format, flags);
	}
	inline bool InputT(const char* const label, glm::vec4& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	inline bool InputT(const std::string& label, glm::vec4& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label.c_str(), &value.x, format, flags);
	}
	inline bool InputT(const char* const label, glm::quat& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	inline bool InputT(const std::string& label, glm::quat& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label.c_str(), &value.x, format, flags);
	}
	inline bool InputT(const char* const label, se::Color& value, ImGuiInputTextFlags flags = 0)
	{
		return ColorEdit4(label, &(value)[0], flags);
	}
	inline bool InputT(const std::string& label, se::Color& value, ImGuiInputTextFlags flags = 0)
	{
		return ColorEdit4(label.c_str(), &(value)[0], flags);
	}
	inline bool InputT(const char* const label, se::time::Time& value,
		const se::time::Time step = se::time::Time(1),
		const se::time::Time stepFast = se::time::Time(1000), ImGuiInputTextFlags flags = 0)
	{
		const bool result = InputT(label, value.value, step.value, stepFast.value, "%llu", flags);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(se::toTimeLengthString(value, 2).c_str());
		}
		return result;
	}
	inline bool InputT(const std::string& label, se::time::Time& value,
		const se::time::Time step = se::time::Time(1),
		const se::time::Time stepFast = se::time::Time(1000), ImGuiInputTextFlags flags = 0)
	{
		return InputT(label.c_str(), value, step, stepFast, flags);
	}

	template<typename T>
	bool InputT(const char* const label, std::vector<T>& vector, const std::function<bool(T&, const size_t)>& renderElement)
	{
		se_assert(renderElement);
		bool changed = false;
		if (!label || ImGui::CollapsingHeader(label))
		{
			if (label)
			{
				ImGui::Indent();
			}
			ImGui::PushID(&vector);
			for (size_t i = 0; i < vector.size(); i++)
			{
				ImGui::PushID(&vector[i]);
				if (ImGui::Button("Delete"))
				{
					vector.erase(vector.begin() + i--);
					changed = true;
				}
				const bool canMoveUp = i > 0;
				if (canMoveUp)
				{
					ImGui::SameLine();
					if (ImGui::ArrowButton("Move up", ImGuiDir_Up))
					{
						if (i > 0)
						{
							std::swap(vector[i], vector[i - 1]);
							changed = true;
						}
					}
				}
				const bool canMoveDown = i < vector.size() - 1;
				if (canMoveDown)
				{
					ImGui::SameLine();
					if (ImGui::ArrowButton("Move down", ImGuiDir_Down))
					{
						std::swap(vector[i], vector[i + 1]);
						changed = true;
					}
				}
				changed = renderElement(vector[i], i) || changed;
				ImGui::PopID();
			}
			if (ImGui::Button("New"))
			{
				vector.push_back(T());
				changed = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
			{
				vector.clear();
				changed = true;
			}
			ImGui::PopID();
			if (!label)
			{
				ImGui::Unindent();
			}
		}
		return changed;
	}
	template<typename T>
	inline bool InputT(const std::string& label, std::vector<T>& vector, const std::function<bool(T&, const size_t)>& renderElement)
	{
		return InputT<T>(label.c_str(), vector, renderElement);
	}

	template<typename T>
	bool InputT(const char* const label, std::optional<T>& optional, const std::function<bool(T&)>& render)
	{
		se_assert(render);
		bool changed = false;
		bool enabled = optional.has_value();
		if (ImGui::Checkbox(label, &enabled))
		{
			if (enabled)
			{
				optional.emplace();
			}
			else
			{
				optional.reset();
			}
			changed = true;
		}
		if (optional)
		{
			ImGui::Indent();
			ImGui::PushID(&optional);
			changed = render(*optional);
			ImGui::PopID();
			ImGui::Unindent();
		}
		return changed;
	}
	template<typename T>
	inline bool InputT(const std::string& label, std::optional<T>& optional, const std::function<bool(T&)>& render)
	{
		return InputT<T>(label.c_str(), optional, render);
	}

	template<typename T>
	inline bool InputT(const char* const label, std::optional<T>& optional)
	{
		return ImGui::InputT<T>(label, optional, [&](T& t)
			{
				return ImGui::InputT(label, t);
			});
	}
	template<typename T>
	inline bool InputT(const std::string& label, std::optional<T>& optional)
	{
		return ImGui::InputT<T>(label.c_str(), optional);
	}

	/* Vector overload for vectors with parameterless InputT() implemented for their element type. */
	template<typename T>
	inline bool InputT(const char* const label, std::vector<T>& vector)
	{
		return ImGui::InputT<T>(label, vector,
			[](T& t, const size_t index)
			{
				return ImGui::InputT(t);
			});
	}
	template<typename T>
	inline bool InputT(const std::string& label, std::vector<T>& vector)
	{
		return ImGui::InputT<T>(label.c_str(), vector);
	}

	template<typename T>
	void InputVector(std::vector<T>& vector, size_t& selectedIndex)
	{
		if (ImGui::Button("New"))
		{
			selectedIndex = vector.size();
			vector.push_back(T());
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			vector.clear();
			selectedIndex = ~0u;
		}
		if (selectedIndex < vector.size())
		{
			ImGui::SameLine();
			if (ImGui::Button("Delete"))
			{
				vector.erase(vector.begin() + selectedIndex);
				if (selectedIndex > 0)
				{
					selectedIndex--;
				}
			}
		}
		for (size_t i = 0; i < vector.size(); i++)
		{
			if (ImGui::Selectable(std::to_string(i).c_str(), selectedIndex == i))
			{
				if (selectedIndex == i)
				{
					selectedIndex = ~0u;
				}
				else
				{
					selectedIndex = i;
				}
			}
		}
	}

	// Splits shown string into two parts, id part and mutable part. Useful when the contents of the header edit the shown header string.
	inline bool CollapsingHeader2(const char* const idLabel, const char* const mutableLabel, const ImGuiTreeNodeFlags flags = 0)
	{
		const bool result = ImGui::CollapsingHeader(idLabel, flags);
		ImGui::SameLine();
		ImGui::Text("");
		ImGui::SameLine();
		ImGui::Text(mutableLabel);
		return result;
	}
	inline bool CollapsingHeader2(const std::string& idLabel, const std::string& mutableLabel, const ImGuiTreeNodeFlags flags = 0)
	{
		return CollapsingHeader2(idLabel.c_str(), mutableLabel.c_str(), flags);
	}

	// NOTE: 'key' value must be valid in the scope of 'scopedConnection'
	void keyBindButton(const char* const label, se::input::Key& key, se::input::EventSignaler& eventSignaler, boost::signals2::scoped_connection& scopedConnection);
	inline void keyBindButton(const std::string& label, se::input::Key& key, se::input::EventSignaler& eventSignaler, boost::signals2::scoped_connection& scopedConnection)
	{
		keyBindButton(label.c_str(), key, eventSignaler, scopedConnection);
	}

	inline void SetTooltip(const std::string& tooltip)
	{
		ImGui::SetTooltip(tooltip.c_str());
	}

	std::string getImGuiFormatString(const std::string_view string);

	class ScopedStyleColor
	{
	public:
		ScopedStyleColor() = default;
		ScopedStyleColor(const ScopedStyleColor& copy) = delete;
		ScopedStyleColor(ScopedStyleColor&& move)
			: pop(move.pop)
		{
			move.pop = false;
		}
		ScopedStyleColor(const ImGuiCol_ imGuiCol_, const se::Color& color)
			: pop(true)
		{
			PushStyleColor(imGuiCol_, ImVec4(color.r, color.g, color.b, color.a));
		}
		ScopedStyleColor(const ImGuiCol_ imGuiCol_, const ImVec4& _imVec4)
			: pop(true)
		{
			PushStyleColor(imGuiCol_, _imVec4);
		}
		~ScopedStyleColor()
		{
			if (pop)
			{
				PopStyleColor();
			}
		}
	private:
		bool pop = false;
	};
}

namespace se
{
	inline ImVec4 toImVec4(const Color& color)
	{
		return ImVec4(color.r, color.g, color.b, color.a);
	}

	inline Color fromImVec4(const ImVec4& imVec4)
	{
		return Color(&imVec4.x);
	}
}
