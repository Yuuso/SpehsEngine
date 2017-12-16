
#include "SpehsEngine/Game/Collider.h"

#include "SpehsEngine/Game/GameObject.h"


#define VISUALISE_COLLIDERS false


namespace spehs
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