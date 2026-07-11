#pragma once

#include "Sign/Layer.h"

#include "Sign/Renderer/Renderer.h"
#include "Sign/D3D12Utils.h"
#include "Sign/Events/Event.h"
#include "Sign/Buffers/FrameBuffer.h"

namespace Sign {
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();
		virtual void OnAttach() override;
		virtual void OnDettach() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;

		void Begin();
		void End();

		void BlockEvents(bool block) { m_BlockEvents = block; }


	private:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};
}

