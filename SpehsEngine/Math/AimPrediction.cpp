#include "stdafx.h"
#include "SpehsEngine/Math/AimPrediction.h"

namespace se
{
	// Source: https://www.gamasutra.com/blogs/KainShin/20090515/83954/Predictive_Aim_Mathematics_for_AI_Targeting.php
	std::optional<glm::vec2> calculateAimPrediction(const glm::vec2& Pbi, const glm::vec2& Pti, const glm::vec2& Vt, const float Sb)
	{
		std::optional<glm::vec2> result;
		const float St = glm::length(Vt);
		if (St > 0.0f)
		{
			const float D = glm::length(Pti - Pbi);
			const float cosTheta = glm::dot(glm::normalize(Pbi - Pti), glm::normalize(Vt));
			const float Sb2 = Sb * Sb;
			const float St2 = St * St;
			const float D2 = D * D;

			const float val1 = std::powf(2.0f * D * St * cosTheta, 2.0f);
			const float t1 = (-2.0f * D * St * cosTheta + std::powf(val1 + 4.0f * (Sb2 - St2) * D2, 0.5f)) / (2.0f * (Sb2 - St2));
			const float t2 = (-2.0f * D * St * cosTheta - std::powf(val1 + 4.0f * (Sb2 - St2) * D2, 0.5f)) / (2.0f * (Sb2 - St2));

			float t = -1.0f;
			if (t1 >= 0.0f && t2 >= 0.0f)
			{
				t = std::min(t1, t2);
			}
			else if (t1 >= 0.0f && t2 < 0.0f)
			{
				t = t1;
			}
			else if (t1 < 0.0f && t2 >= 0.0f)
			{
				t = t2;
			}

			if (t >= 0.0f)
			{
				const glm::vec2 Vb = (Pti + Vt * t - Pbi) / t;
				result = Pbi + Vb * t;
			}
		}
		else
		{
			result = Pti;
		}

		return result;
	}
}
