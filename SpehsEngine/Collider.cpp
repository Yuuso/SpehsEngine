
#include "Collider.h"

#include "GameObject.h"


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