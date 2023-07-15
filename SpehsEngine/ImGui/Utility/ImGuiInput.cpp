#include "stdafx.h"
#include "SpehsEngine/ImGui/Utility/ImGuiInput.h"
#include "SpehsEngine/ImGui/Utility/ImGuiUtility.h"

namespace ImGui
{
	// Scalars
#define SE_SCALAR(p_ScalarType, p_ImGuiDataType, p_DefaultFormat, p_DefaultImGuiTextFlags) \
	template<> template<> \
	bool Impl<p_ScalarType>::input(const Label label, p_ScalarType& value, p_ScalarType step, p_ScalarType stepFast, const char* format, ImGuiInputTextFlags flags) \
	{ \
		return InputScalar(label.string, p_ImGuiDataType, &value, &step, &stepFast, format, flags); \
	} \
	template<> template<> \
	bool Impl<p_ScalarType>::input(const Label label, p_ScalarType& value) \
	{ \
		const p_ScalarType step = 1; \
		const p_ScalarType stepFast = 100; \
		const char* const format = p_DefaultFormat; \
		const ImGuiInputTextFlags flags = p_DefaultImGuiTextFlags; \
		return InputScalar(label.string, p_ImGuiDataType, &value, &step, &stepFast, format, flags); \
	}
	SE_SCALAR(uint8_t, ImGuiDataType_U8, "%hhu", 0)
	SE_SCALAR(uint16_t, ImGuiDataType_U16, "%hu", 0)
	SE_SCALAR(uint32_t, ImGuiDataType_U32, "%u", 0)
	SE_SCALAR(uint64_t, ImGuiDataType_U64, "%llu", 0)
	SE_SCALAR(int8_t, ImGuiDataType_S8, "%hhi", 0)
	SE_SCALAR(int16_t, ImGuiDataType_S16, "%hi", 0)
	SE_SCALAR(int32_t, ImGuiDataType_S32, "%i", 0)
	SE_SCALAR(int64_t, ImGuiDataType_S64, "%lli", 0)
	SE_SCALAR(float, ImGuiDataType_Float, "%.3f", ImGuiInputTextFlags_CharsScientific)
	SE_SCALAR(double, ImGuiDataType_Double, "%.3f", ImGuiInputTextFlags_CharsScientific)
#undef SE_SCALAR

	template<> template<>
	bool Impl<bool>::input(const Label label, bool& value)
	{
		return Checkbox(label, &value);
	}
	template<> template<>
	bool Impl<std::string>::input(const Label label, std::string& value, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
	{
		return InputText(label, &value, flags, callback, user_data);
	}
	template<> template<>
	bool Impl<std::string>::input(const Label label, std::string& value)
	{
		ImGuiInputTextFlags flags = 0;
		ImGuiInputTextCallback callback = NULL;
		void* user_data = NULL;
		return InputText(label, &value, flags, callback, user_data);
	}
	template<> template<>
	bool Impl<glm::ivec2>::input(const Label label, glm::ivec2& value, ImGuiInputTextFlags flags)
	{
		return InputInt2(label, &value.x, flags);
	}
	template<> template<>
	bool Impl<glm::ivec2>::input(const Label label, glm::ivec2& value)
	{
		return InputInt2(label, &value.x, 0);
	}
	template<> template<>
	bool Impl<glm::ivec3>::input(const Label label, glm::ivec3& value, ImGuiInputTextFlags flags)
	{
		return InputInt2(label, &value.x, flags);
	}
	template<> template<>
	bool Impl<glm::ivec3>::input(const Label label, glm::ivec3& value)
	{
		return InputInt2(label, &value.x, 0);
	}
	template<> template<>
	bool Impl<glm::ivec4>::input(const Label label, glm::ivec4& value, ImGuiInputTextFlags flags)
	{
		return InputInt2(label, &value.x, flags);
	}
	template<> template<>
	bool Impl<glm::ivec4>::input(const Label label, glm::ivec4& value)
	{
		return InputInt2(label, &value.x, 0);
	}
	template<> template<>
	bool Impl<glm::vec2>::input(const Label label, glm::vec2& value)
	{
		const char* const format = "%.3f";
		ImGuiInputTextFlags flags = 0;
		return InputFloat2(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::vec2>::input(const Label label, glm::vec2& value, const char* format, ImGuiInputTextFlags flags)
	{
		return InputFloat2(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::vec3>::input(const Label label, glm::vec3& value)
	{
		const char* const format = "%.3f";
		ImGuiInputTextFlags flags = 0;
		return InputFloat3(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::vec3>::input(const Label label, glm::vec3& value, const char* format, ImGuiInputTextFlags flags)
	{
		return InputFloat3(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::vec4>::input(const Label label, glm::vec4& value)
	{
		const char* const format = "%.3f";
		ImGuiInputTextFlags flags = 0;
		return InputFloat4(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::vec4>::input(const Label label, glm::vec4& value, const char* format, ImGuiInputTextFlags flags)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::quat>::input(const Label label, glm::quat& value)
	{
		const char* const format = "%.3f";
		const ImGuiInputTextFlags flags = 0;
		return InputFloat4(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<glm::quat>::input(const Label label, glm::quat& value, const char* format, ImGuiInputTextFlags flags)
	{
		return InputFloat4(label, &value.x, format, flags);
	}
	template<> template<>
	bool Impl<se::Color>::input(const Label label, se::Color& value)
	{
		const ImGuiColorEditFlags flags = 0;
		return ColorEdit4(label, &(value)[0], flags);
	}
	template<> template<>
	bool Impl<se::Color>::input(const Label label, se::Color& value, ImGuiColorEditFlags flags)
	{
		return ColorEdit4(label, &(value)[0], flags);
	}
	template<> template<>
	bool Impl<se::time::Time>::input(const Label label, se::time::Time& value)
	{
		const se::time::Time step = se::time::Time(1);
		const se::time::Time stepFast = se::time::Time(1000);
		const ImGuiInputTextFlags flags = 0;
		const bool result = Input(label, value.value, step.value, stepFast.value, "%llu", flags);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(se::toTimeLengthString(value, 2).c_str());
		}
		return result;
	}
	template<> template<>
	bool Impl<se::time::Time>::input(const Label label, se::time::Time& value, const se::time::Time step, const se::time::Time stepFast, ImGuiInputTextFlags flags)
	{
		const bool result = Input(label, value.value, step.value, stepFast.value, "%llu", flags);
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(se::toTimeLengthString(value, 2).c_str());
		}
		return result;
	}
	template<> template<>
	bool Impl<se::time::TimeInfo>::input(const Label label, se::time::TimeInfo& timeInfo)
	{
		bool changed = false;
		if (ImGui::CollapsingHeader(label))
		{
			changed |= ImGui::Input("Day", timeInfo.monthDay);
			if (ImGui::IsItemHovered())
			{
				ImGui::SetTooltip("Day of the month");
			}
			changed |= ImGui::Input("Month", timeInfo.month);
			changed |= ImGui::Input("Year", timeInfo.year);
			changed |= ImGui::Input("Hour", timeInfo.hour);
			changed |= ImGui::Input("Minute", timeInfo.minute);
			changed |= ImGui::Input("Second", timeInfo.second);
			changed |= ImGui::Input("Dayligt savings flag", timeInfo.dayligtSavingsFlag);
		}
		return changed;
	}

	struct InputTagVector {};
	template<typename T> struct InputTag<std::vector<T>> { typedef InputTagVector type; };
	template<>
	template<typename U, typename ... Args>
	bool Impl<InputTagVector>::input(const Label label, U& vector, Args ... args)
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
				changed |= Input(("[" + std::to_string(i) + "]").c_str(), vector[i], std::forward<Args>(args)...);
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

	struct InputTagOptional {};
	template<typename T> struct InputTag<std::optional<T>> { typedef InputTagOptional type; };
	template<> template<typename V, typename ... Args>
	bool Impl<InputTagOptional>::input(const Label label, V& optional, Args ... args)
	{
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
			changed = Input(label, *optional, std::forward<Args>(args)...);
			ImGui::PopID();
			ImGui::Unindent();
		}
		return changed;
	}
}
