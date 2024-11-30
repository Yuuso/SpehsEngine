#pragma once

namespace se::gfx
{
	class Light;
}

namespace se::gfx::impl
{
	class LightInternal
	{
	public:

		LightInternal(Light& _light);
		~LightInternal() = default;

		LightInternal(const LightInternal& _other) = delete;
		LightInternal& operator=(const LightInternal& _other) = delete;

		LightInternal(LightInternal&& _other) = delete;
		LightInternal& operator=(LightInternal&& _other) = delete;

		bool operator==(const Light& _other) const;


		const bool wasDestroyed() const;
		Light& getLight();

	private:

		void lightDestroyed();

		ScopedConnection lightDestroyedConnection;

		Light* light = nullptr;
	};
}
