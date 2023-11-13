#pragma once

#include "Rhombus.h"

namespace rhombus
{
	class EditorLayer : public Layer
	{

	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void OnUpdate(DeltaTime dt) override;
		virtual void OnImGuiRender() override;
		void OnEvent(Event& e) override;

	private:
		// Temp (These needs to be abstracted away)
		Ref<Shader> m_FlatColourShader;
		Ref<VertexArray> m_SquareVA;
		Ref<Framebuffer> m_Framebuffer;

		Ref<Texture2D> m_CheckerboardTexture;

		OrthographicCameraController m_CameraController;
		glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	};
}
