#pragma once

#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_stdlib.h"
#include "SpehsEngine/ImGui/ImGuiTypes.h"
#include "SpehsEngine/ImGui/Utility/IState.h"
#include "SpehsEngine/ImGui/Utility/String.h"


// Do not use directly, see macros below. Used to implement a dropdown selector for an enum. p_RangeStart value is inclusive, but p_RangeEnd is exclusive.
#define SE_IMGUI_INPUT_ENUM_RANGE_2(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction, p_InlineAndBool) \
	p_InlineAndBool InputT(const String p_label, p_EnumType& p_value) \
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
	p_InlineAndBool InputT(const String p_label, p_EnumType& p_value, const std::function<bool(const p_EnumType)>& p_isVisible) \
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

// You can use this to declare header functions with both label types
#define SE_IMGUI_INPUT_DECL(p_Type, ...) \
	bool InputT(const String p_label, p_Type& p_value, __VA_ARGS__);

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
	inline bool DragScalar##p_Components(const String label, p_ScalarType& i, float speed, p_ScalarType min, p_ScalarType max, const String format = p_DefaultFormat, ImGuiSliderFlags flags = p_DefaultImGuiSliderFlags) \
	{ \
		return DragScalarN(label, p_ImGuiDataType, &i, p_Components, speed, &min, &max, format, flags); \
	}
#define SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(p_ScalarType, p_ImGuiDataType, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 1, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 2, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 3, p_DefaultFormat, p_DefaultImGuiSliderFlags) \
	SE_IMGUI_DRAG_SCALAR(p_ScalarType, p_ImGuiDataType, 4, p_DefaultFormat, p_DefaultImGuiSliderFlags)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint8_t,	ImGuiDataType_::ImGuiDataType_U8, "%hhu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint16_t,	ImGuiDataType_::ImGuiDataType_U16, "%hu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint32_t,	ImGuiDataType_::ImGuiDataType_U32, "%u", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(uint64_t,	ImGuiDataType_::ImGuiDataType_U64, "%llu", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int8_t,		ImGuiDataType_::ImGuiDataType_S8, "%hhi", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int16_t,	ImGuiDataType_::ImGuiDataType_S16, "%hi", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int32_t,	ImGuiDataType_::ImGuiDataType_S32, "%i", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(int64_t,	ImGuiDataType_::ImGuiDataType_S64, "%lli", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(float,		ImGuiDataType_::ImGuiDataType_Float, "%f", 0)
	SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS(double,		ImGuiDataType_::ImGuiDataType_Double, "%f", 0)
#undef SE_IMGUI_DRAG_SCALAR_ONE_TO_FOUR_COMPONENTS
#undef SE_IMGUI_DRAG_SCALAR

	// Input scalars
#define SE_IMGUI_INPUT_SCALAR(p_ScalarType, p_ImGuiDataType, p_DefaultFormat, p_DefaultImGuiTextFlags) \
	inline bool InputT(const String label, p_ScalarType& value, p_ScalarType step = 1, p_ScalarType stepFast = 100, const String format = p_DefaultFormat, ImGuiInputTextFlags flags = p_DefaultImGuiTextFlags) \
	{ \
		return InputScalar(label, p_ImGuiDataType, &value, &step, &stepFast, format, flags); \
	}
	SE_IMGUI_INPUT_SCALAR(uint8_t,	ImGuiDataType_::ImGuiDataType_U8,		"%hhu",	0)
	SE_IMGUI_INPUT_SCALAR(uint16_t,	ImGuiDataType_::ImGuiDataType_U16,		"%hu",	0)
	SE_IMGUI_INPUT_SCALAR(uint32_t,	ImGuiDataType_::ImGuiDataType_U32,		"%u",	0)
	SE_IMGUI_INPUT_SCALAR(uint64_t,	ImGuiDataType_::ImGuiDataType_U64,		"%llu",	0)
	SE_IMGUI_INPUT_SCALAR(int8_t,	ImGuiDataType_::ImGuiDataType_S8,		"%hhi",	0)
	SE_IMGUI_INPUT_SCALAR(int16_t,	ImGuiDataType_::ImGuiDataType_S16,		"%hi",	0)
	SE_IMGUI_INPUT_SCALAR(int32_t,	ImGuiDataType_::ImGuiDataType_S32,		"%i",	0)
	SE_IMGUI_INPUT_SCALAR(int64_t,	ImGuiDataType_::ImGuiDataType_S64,		"%lli",	0)
	SE_IMGUI_INPUT_SCALAR(float,	ImGuiDataType_::ImGuiDataType_Float,	"%.3f",	ImGuiInputTextFlags_CharsScientific)
	SE_IMGUI_INPUT_SCALAR(double,	ImGuiDataType_::ImGuiDataType_Double,	"%.3f",	ImGuiInputTextFlags_CharsScientific)
#undef SE_IMGUI_INPUT_SCALAR

	inline bool Button(const String _label, const ImVec2& _size = ImVec2(0, 0))
	{
		return Button(_label.pointer, _size);
	}
	
	bool ButtonWithConfirm(const String _label, const ImVec2& _size = ImVec2(0, 0));

	inline void Text(const String _label)
	{
		Text(_label.pointer);
	}

	template<typename ... Args>
	inline void TextColored(const se::Color& _color, const String _format, Args ... _args)
	{
		TextColored((ImVec4&)_color, _format, _args...);
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

	bool fileSelector(const String label, std::string& filepath, const String directory);

	std::optional<bool> fileDialog(const String header, const String message, const String directory, std::string& output);

	bool textureSelector(const String label, std::string& filepath, const String directory);

	std::optional<bool> confirmationDialog(const String header, const String message, const std::function<void()>& customRender = std::function<void()>());

	std::optional<bool> stringDialog(const String header, const String message, std::string& output);

	inline bool InputT(const String label, bool& value)
	{
		return Checkbox(label, &value);
	}
	inline bool InputT(const String label, std::string& value, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
	{
		return InputText(label, &value, flags, callback, user_data);
	}
	inline bool InputT(const String label, glm::ivec2& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt2(label, &value.x, flags);
	}
	inline bool InputT(const String label, glm::ivec3& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt3(label, &value.x, flags);
	}
	inline bool InputT(const String label, glm::ivec4& value, ImGuiInputTextFlags flags = 0)
	{
		return InputInt4(label, &value.x, flags);
	}
	inline bool InputT(const String label, glm::vec2& value, const String format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat2(label, &value.x, format, flags);
	}
	inline bool InputT(const String label, glm::vec3& value, const String format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat3(label, &value.x, format, flags);
	}
	inline bool InputT(const String label, glm::vec4& value, const String format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	inline bool InputT(const String label, glm::quat& value, const String format = "%.3f", ImGuiInputTextFlags flags = 0)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	inline bool InputT(const String label, se::Color& value, ImGuiColorEditFlags flags = 0)
	{
		return ColorEdit4(label, &(value)[0], flags);
	}
	bool InputT(const String label, se::Time& value,
		const se::Time step = se::Time(1),
		const se::Time stepFast = se::Time(1000), ImGuiInputTextFlags flags = 0);
	bool InputAngle(const String label, float& radians);

	template<typename T, typename ... Args>
	bool InputT(const String label, std::vector<T>& vector, Args&& ... args)
	{
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
				bool remove = false;
				if (ImGui::Button("Delete"))
				{
					remove = true;
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
				changed |= InputT(String("[" + std::to_string(i) + "]"), (T&)vector[i], std::forward<Args>(args)...);
				if (remove)
				{
					vector.erase(vector.begin() + i--);
				}
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
			if (label)
			{
				ImGui::Unindent();
			}
		}
		return changed;
	}

	template<typename T, typename ... Args>
	inline bool InputT(const String label, std::optional<T>& optional, Args&& ... args)
	{
		bool changed = false;
		bool enabled = optional.has_value();
		ImGui::PushID(&optional);
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
			changed |= InputT(label, (T&)optional.value(), std::forward<Args>(args)...);
			ImGui::Unindent();
		}
		ImGui::PopID();
		return changed;
	}

	bool InputT(const String label, se::TimeInfo& timeInfo);

	inline bool CollapsingHeader(const String _idLabel, const ImGuiTreeNodeFlags _flags = 0)
	{
		return ImGui::CollapsingHeader(_idLabel.pointer, _flags);
	}
	// Splits shown string into two parts, id part and mutable part. Useful when the contents of the header edit the shown header string.
	bool CollapsingHeader2(const String idLabel, const String mutableLabel, const ImGuiTreeNodeFlags flags = 0);

	bool BeginCentered(const String label, bool* const open, const ImGuiWindowFlags flags = 0, const ImGuiCond centerCondition = ImGuiCond_Appearing);

	// NOTE: 'key' value must be valid in the scope of 'scopedConnection'
	void keyBindButton(const String label, se::input::Key& key, se::input::EventSignaler& eventSignaler, se::ScopedConnection& scopedConnection);

	bool InputT(const String label, se::input::CustomEventParameters& customEventParameters, se::input::EventSignaler& eventSignaler, StateWrapper& stateWrapper);

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
		ScopedStyleColor(ScopedStyleColor&& move) noexcept
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
		ScopedFont(ScopedFont&& other) noexcept
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
