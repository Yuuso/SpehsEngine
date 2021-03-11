#include "stdafx.h"
#include "SpehsEngine/Graphics/Internal/Animation.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/transform.hpp"


namespace se
{
	namespace graphics
	{
		glm::vec3 getPosition(const AnimationNode& _node, const float _time)
		{
			if (_node.positionKeys.size() == 1)
				return _node.positionKeys[0].second;

			for (size_t p = 0; p < _node.positionKeys.size() - 1; p++)
			{
				if (_time < _node.positionKeys[p + 1].first)
				{
					const float delta = _node.positionKeys[p + 1].first - _node.positionKeys[p].first;
					const float timeSinceFirstKey = _time - _node.positionKeys[p].first;
					return glm::mix(_node.positionKeys[p].second, _node.positionKeys[p + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::vec3(0.0f);
		}

		glm::quat getRotation(const AnimationNode& _node, const float _time)
		{
			if (_node.rotationKeys.size() == 1)
				return _node.rotationKeys[0].second;

			for (size_t r = 0; r < _node.rotationKeys.size() - 1; r++)
			{
				if (_time < _node.rotationKeys[r + 1].first)
				{
					const float delta = _node.rotationKeys[r + 1].first - _node.rotationKeys[r].first;
					const float timeSinceFirstKey = _time - _node.rotationKeys[r].first;
					return glm::mix(_node.rotationKeys[r].second, _node.rotationKeys[r + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::quat(glm::vec3(0.0f));
		}

		glm::vec3 getScale(const AnimationNode& _node, const float _time)
		{
			if (_node.scalingKeys.size() == 1)
				return _node.scalingKeys[0].second;

			for (size_t s = 0; s < _node.scalingKeys.size() - 1; s++)
			{
				if (_time < _node.scalingKeys[s + 1].first)
				{
					const float delta = _node.scalingKeys[s + 1].first - _node.scalingKeys[s].first;
					const float timeSinceFirstKey = _time - _node.scalingKeys[s].first;
					return glm::mix(_node.scalingKeys[s].second, _node.scalingKeys[s + 1].second, timeSinceFirstKey / delta);
				}
			}

			se_assert_m(false, "Invalid animation time!");
			return glm::vec3(1.0f);
		}

		glm::mat4 makeTransform(const AnimationNode& _node, const float _time)
		{
			return glm::translate(getPosition(_node, _time)) * glm::mat4_cast(getRotation(_node, _time)) * glm::scale(getScale(_node, _time));
		}
	}
}
