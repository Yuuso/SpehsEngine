
#include "SpehsEngine/Game/Collider.h"

#include "SpehsEngine/Game/GameObject.h"


#define VISUALISE_COLLIDERS false


namespace se
{
	Collider::Collider()
	{
	}
	Collider::Collider(GameObject& _owner) : Component(_owner)
	{
	}
	Collider::~Collider()
	{
	}
}