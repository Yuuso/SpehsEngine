#pragma once

#include "SpehsEngine/Core/Color.h"
#include "SpehsEngine/Core/StringUtilityFunctions.h"
#include "SpehsEngine/ImGui/imgui.h"
#include "SpehsEngine/ImGui/imgui_stdlib.h"
#include "glm/vec4.hpp"
#include <stdint.h>

// Used to implement a dropdown selector for an enum. p_RangeStart value is inclusive, but p_RangeEnd is exclusive.
#define SE_IMGUI_INPUT_T_ENUM_RANGE_DROPDOWN(p_EnumType, p_RangeStart, p_RangeEnd, p_ToStringFunction) \
    inline bool InputT(const std::string_view label, p_EnumType& p_value) \
    { \
	    struct Callback { static bool callback(void* data, int n, const char** out_str) { *out_str = p_ToStringFunction(p_EnumType(n + int(p_RangeStart))); return true; } }; \
	    int p_current = int(p_value); \
	    if (ImGui::Combo(#p_EnumType, &p_current, &Callback::callback, nullptr, int(p_RangeEnd) - int(p_RangeStart))) \
	    { \
		    p_value = p_EnumType(p_current); \
		    return true; \
	    } \
	    else \
	    { \
		    return false; \
	    } \
    }

namespace ImGui
{
    bool fileSelector(const std::string_view label, std::string& filepath, const std::string_view directory);
    bool textureSelector(const std::string_view label, std::string& filepath, const std::string_view directory);

    std::optional<bool> confirmationDialog(const std::string_view label);
    template<typename A, typename ... Arguments>
    inline std::optional<bool> confirmationDialogV(const std::string_view format, const A& argument, const Arguments&... arguments)
    {
        const std::string formatedString = se::formatString(format, argument, arguments...);
        return confirmationDialog(std::string_view(formatedString));
    }

    inline bool InputT(const std::string_view label, bool& value)
    {
        return Checkbox(label.data(), &value);
    }
    inline bool InputT(const std::string_view label, int8_t& value, int8_t step = 1, int8_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_S8, &value, &step, &stepFast, "%hhi", flags);
    }
    inline bool InputT(const std::string_view label, int16_t& value, int16_t step = 1, int16_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_S16, &value, &step, &stepFast, "%hi", flags);
    }
    inline bool InputT(const std::string_view label, int32_t& value, int32_t step = 1, int32_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_S32, &value, &step, &stepFast, "%i", flags);
    }
    inline bool InputT(const std::string_view label, int64_t& value, int64_t step = 1, int64_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_S64, &value, &step, &stepFast, "%lli", flags);
    }
    inline bool InputT(const std::string_view label, uint8_t& value, uint8_t step = 1, uint8_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_U8, &value, &step, &stepFast, "%hhu", flags);
    }
    inline bool InputT(const std::string_view label, uint16_t& value, uint16_t step = 1, uint16_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_U16, &value, &step, &stepFast, "%hu", flags);
    }
    inline bool InputT(const std::string_view label, uint32_t& value, uint32_t step = 1, uint32_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_U32, &value, &step, &stepFast, "%u", flags);
    }
    inline bool InputT(const std::string_view label, uint64_t& value, uint64_t step = 1, uint64_t stepFast = 100, ImGuiInputTextFlags flags = 0)
    {
        return InputScalar(label.data(), ImGuiDataType_U64, &value, &step, &stepFast, "%llu", flags);
    }
    inline bool InputT(const std::string_view label, float& value, float step = 1.0f, float stepFast = 100.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
    {
        return InputFloat(label.data(), &value, step, stepFast, format, flags);
    }
    inline bool InputT(const std::string_view label, double& value, double step = 1.0f, double stepFast = 100.0f, const char* format = "%.6f", ImGuiInputTextFlags flags = 0)
    {
        return InputDouble(label.data(), &value, step, stepFast, format, flags);
    }
    inline bool InputT(const std::string_view label, std::string& value, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
    {
        return InputText(label.data(), &value, flags, callback, user_data);
    }
    inline bool InputT(const std::string_view label, glm::ivec2& value, ImGuiInputTextFlags flags = 0)
    {
        return InputInt2(label.data(), &value.x, flags);
    }
    inline bool InputT(const std::string_view label, glm::ivec3& value, ImGuiInputTextFlags flags = 0)
    {
        return InputInt3(label.data(), &value.x, flags);
    }
    inline bool InputT(const std::string_view label, glm::ivec4& value, ImGuiInputTextFlags flags = 0)
    {
        return InputInt4(label.data(), &value.x, flags);
    }
    inline bool InputT(const std::string_view label, glm::vec2& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
    {
        return InputFloat2(label.data(), &value.x, format, flags);
    }
    inline bool InputT(const std::string_view label, glm::vec3& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
    {
        return InputFloat3(label.data(), &value.x, format, flags);
    }
    inline bool InputT(const std::string_view label, glm::vec4& value, const char* format = "%.3f", ImGuiInputTextFlags flags = 0)
    {
        return InputFloat4(label.data(), &value.x, format, flags);
    }
    inline bool InputT(const std::string_view label, se::Color& value, ImGuiInputTextFlags flags = 0)
    {
        return ColorEdit4(label.data(), &(value)[0], flags);
    }

    template<typename T>
    void InputVector(std::vector<T>& vector, size_t &selectedIndex)
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
}