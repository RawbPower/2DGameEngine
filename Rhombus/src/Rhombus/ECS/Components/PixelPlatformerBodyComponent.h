#pragma once

#include "Component.h"

namespace rhombus
{
	class PixelPlatformerBodyComponent : public ComponentBase
	{
	public:
		enum class BodyType { Static = 0, Dynamic, Kinematic };
		BodyType m_type = BodyType::Static;
		bool m_fixedRotation = true;

		PixelPlatformerBodyComponent() = default;
		PixelPlatformerBodyComponent(const PixelPlatformerBodyComponent& other) = default;
	private:
		friend class PixelPlatformerPhysicsSystem;

		Vec2 m_velocity = Vec2(0.0f);
		Vec2 m_translationRemainder = Vec2(0.0f);
	};
}
