#include "rbpch.h"
#include "PlatformerPlayerControllerSystem.h"
#include "Rhombus/ECS/Components/PlatformerPlayerControllerComponent.h"
#include "Rhombus/ECS/Components/PixelPlatformerBodyComponent.h"
#include "Rhombus/ECS/Components/TransformComponent.h"
#include "Rhombus/Core/Input.h"
#include "Rhombus/Core/KeyCodes.h"

namespace rhombus
{
	void PlatformerPlayerControllerSystem::Update(DeltaTime dt)
	{
		float input = Input::IsKeyPressed(RB_KEY_D) - Input::IsKeyPressed(RB_KEY_A);

		for (Entity entity : GetEntities())
		{
			const PlatformerPlayerControllerComponent& playerController = entity.GetComponentRead<PlatformerPlayerControllerComponent>();
			float speed = 0.0f;

			if (input != 0.0f)
			{
				speed = input * playerController.m_speed;
			}
			else
			{
				speed = 0.0f;
			}

			if (entity.HasComponent<PixelPlatformerBodyComponent>())
			{
				PixelPlatformerBodyComponent& physicsBody = entity.GetComponent<PixelPlatformerBodyComponent>();
				physicsBody.m_velocity.x = speed;
			}
		}
	}

	void PlatformerPlayerControllerSystem::OnKeyPressed(int keycode, bool isRepeat)
	{
		bool shouldJump = !isRepeat && keycode == RB_KEY_SPACE;	// Check if on ground too
		for (Entity entity : GetEntities())
		{
			if (shouldJump)
			{
				// Jump code
				const PlatformerPlayerControllerComponent& playerController = entity.GetComponentRead<PlatformerPlayerControllerComponent>();
				if (playerController.m_jumpHeight > 0.0f)
				{
					// Linear equation of motion v^2 = u^2 + 2as
					float jumpVelocity = math::Sqrt(-2 * GRAVITY * playerController.m_jumpHeight);

					if (entity.HasComponent<PixelPlatformerBodyComponent>())
					{
						PixelPlatformerBodyComponent& physicsBody = entity.GetComponent<PixelPlatformerBodyComponent>();
						physicsBody.m_velocity.y = jumpVelocity;
					}
				}
			}
		}
	}
}