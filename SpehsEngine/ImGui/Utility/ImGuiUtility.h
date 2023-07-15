#pragma once

#include "boost/signals2/connection.hpp"
#include "glm/ext/quaternion_float.hpp"
#include "glm/vec4.hpp"
#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/SE_Time.h"
#include "SpehsEngine/Core/TimeUtilityFunctions.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_stdlib.h"
#include "SpehsEngine/ImGui/ImGuiTypes.h"
#include "SpehsEngine/ImGui/Utility/IState.h"
#include "SpehsEngine/ImGui/Utility/ImGuiInput.h"
#include <stdint.h>

namespace se
{
	namespace input
	{
		class EventSignaler;
		struct CustomEventParameters;
		enum class Key : uint32_t;
	}

	namespace graphics
	{
		class Texture;
	}
}

// Do not use directly, see macros below. Used to implement a dropdown selector for an enum. p_RangeStart value is inclusive, but p_RangeEnd is exclusive.
#define SE_IMGUI_INPUT_ENUM_RANGE_2(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction, p_InlineAndBool) \
	p_InlineAndBool InputT(const char* const p_label, p_EnumType& p_value) \
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
	p_InlineAndBool InputT(const std::string& p_label, p_EnumType& p_value) \
	{ \
		return InputT(p_label.c_str(), p_value); \
	} \
	p_InlineAndBool InputT(const char* const p_label, p_EnumType& p_value, const std::function<bool(const p_EnumType)>& p_isVisible) \
	{ \
		bool changed = false; \
		if (ImGui::BeginCombo(p_label, p_ToStringFunction(p_value))) \
		{ \
			for (int i = int(p_RangeStart); i < int(p_RangeEnd); i++) \
			{ \
				const p_EnumType p_v = p_EnumType(i); \
				if (!p_isVisible(p_v)) \
					continue; \
				const bool p_selected = p_v == p_value; \
				if (ImGui::Selectable(p_ToStringFunction(p_v), p_selected, 0)) \
				{ \
					p_value = p_v; \
					changed = true; \
				} \
				if (p_selected) \
					ImGui::SetItemDefaultFocus(); \
			} \
			ImGui::EndCombo(); \
		} \
		return changed; \
	}

// Implementation is inlined
#define SE_IMGUI_INPUT_ENUM_RANGE_INLINE(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction) \
	SE_IMGUI_INPUT_ENUM_RANGE_2(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction, inline bool)

// Implementation is not inlined
#define SE_IMGUI_INPUT_ENUM_RANGE(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction) \
	SE_IMGUI_INPUT_ENUM_RANGE_2(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction, bool)

namespace se
{
	inline ImVec4 toImVec4(const Color& color)
	{
		return ImVec4(color.r, color.g, color.b, color.a);
	}
	inline ImVec2 toImVec2(const glm::vec2& vec)
	{
		return ImVec2(vec.x, vec.y);
	}
	inline Color fromImVec4(const ImVec4& imVec4)
	{
		return Color(&imVec4.x);
	}
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

	inline bool Button(const std::string& label, const ImVec2& size = ImVec2(0, 0))
	{
		return Button(label.c_str(), size);
	}

	inline void Text(const std::string& label)
	{
		Text(label.c_str());
	}

	template<typename ... Args>
	inline void TextColored(const se::Color& color, const char* const format, Args ... args)
	{
		TextColored((ImVec4&)color, format, args...);
	}
	template<typename ... Args>
	inline void TextColored(const se::Color& color, const std::string& format, Args ... args)
	{
		TextColored((ImVec4&)color, format.c_str(), args...);
	}

	ImVec2 SizeToScale(const se::graphics::Texture& _texture, const ImVec2 _size);

	void Image(
		const se::graphics::Texture& _texture,
		const ImVec2 scale			= ImVec2(1.0f, 1.0f),
		const ImVec2 uv0				= ImVec2(0.0f, 0.0f),
		const ImVec2 uv1				= ImVec2(1.0f, 1.0f),
		const se::Color tintColor		= se::Color(1.0f, 1.0f, 1.0f, 1.0f),
		const se::Color borderColor	= se::Color(0.0f, 0.0f, 0.0f, 0.0f));

	bool ImageButton(
		const se::graphics::Texture& _texture,
		const ImVec2 scale				= ImVec2(1.0f, 1.0f),
		const ImVec2 uv0				= ImVec2(0.0f, 0.0f),
		const ImVec2 uv1				= ImVec2(1.0f, 1.0f),
		const int framePadding			= 1,
		const se::Color tintColor		= se::Color(1.0f, 1.0f, 1.0f, 1.0f),
		const se::Color backgroundColor	= se::Color(0.0f, 0.0f, 0.0f, 0.0f));

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

	std::optional<bool> confirmationDialog(const char* const header, const char* const message, const std::function<void()>& customRender = std::function<void()>());
	inline std::optional<bool> confirmationDialog(const std::string& header, const std::string& message, const std::function<void()>& customRender = std::function<void()>())
	{
		return confirmationDialog(header.c_str(), message.c_str(), customRender);
	}

	std::optional<bool> stringDialog(const char* const header, const char* const message, std::string& output);
	inline std::optional<bool> stringDialog(const std::string& header, const std::string& message, std::string& output)
	{
		return stringDialog(header.c_str(), message.c_str(), output);
	}

	inline bool InputAngle(const Label label, float &radians)
	{
		float degrees = (radians / se::PI<float>) * 180.0f;
		const bool changed = Input(label, degrees);
		if (changed)
		{
			radians = (degrees / 180.0f) * se::PI<float>;
		}
		return changed;
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

	inline bool BeginCentered(const char* const label, bool* const open, const ImGuiWindowFlags flags = 0, const ImGuiCond centerCondition = ImGuiCond_Appearing)
	{
		const ImGuiIO& io = ImGui::GetIO();
		const ImVec2 displayCenter(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f);
		ImGui::SetNextWindowPos(displayCenter, centerCondition, ImVec2(0.5f, 0.5f));
		return ImGui::Begin(label, open, flags);
	}
	inline bool BeginCentered(const std::string& label, bool* const open, const ImGuiWindowFlags flags = 0, const ImGuiCond centerCondition = ImGuiCond_Appearing)
	{
		return BeginCentered(label.c_str(), open, flags, centerCondition);
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

	void PushFont(const se::imgui::ImGuiFont _font);

	class ScopedFont
	{
	public:
		ScopedFont() = delete;
		ScopedFont(const ScopedFont& copy) = delete;
		ScopedFont(ScopedFont&& other)
			: pop(other.pop)
		{
			other.pop = false;
		}
		ScopedFont(const se::imgui::ImGuiFont _font)
			: pop(true)
		{
			PushFont(_font);
		}
		~ScopedFont()
		{
			if (pop)
				PopFont();
		}
	private:
		bool pop = false;
	};
}
