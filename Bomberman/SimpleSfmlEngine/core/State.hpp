#pragma once

namespace sse
{
	class State
	{
	public:
		virtual bool Init() = 0;
		virtual bool Terminate() { return true; }

		virtual bool HandleInput(float dt) = 0;
		virtual bool Update(float dt) = 0;
		virtual bool Render(float dt) = 0;

		virtual bool Pause() { return true; }
		virtual bool Resume() { return true; }
	};
}  // namespace sse