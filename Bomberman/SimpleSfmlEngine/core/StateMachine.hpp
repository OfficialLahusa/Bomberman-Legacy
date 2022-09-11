#pragma once

#include <memory>
#include <stack>

#include "State.hpp"

namespace sse
{
	using StateRef = std::unique_ptr<State>;

	class StateMachine
	{
	public:
		StateMachine() = default;
		~StateMachine() = default;

		void AddState(StateRef newState, bool isReplacing = true)
		{
			this->m_isAdding = true;
			this->m_isReplacing = isReplacing;

			this->m_newState = std::move(newState);
		}

		void Pop_back()
		{
			this->m_isRemoving = true;
		}

		bool ProcessStateChanges()
		{
			if (this->m_isRemoving && !this->m_states.empty())
			{
				if (!this->m_states.top()->Terminate()) { return false; }
				
				this->m_states.pop();

				if (!this->m_states.empty())
				{
					if (!this->m_states.top()->Resume()) { return false; }
				}

				this->m_isRemoving = false;
			}

			if (this->m_isAdding)
			{
				if (!this->m_states.empty())
				{
					if (this->m_isReplacing)
					{
						if (!this->m_states.top()->Terminate()) { return false; }

						this->m_states.pop();
					}
					else
					{
						if(!this->m_states.top()->Pause()) { return false; }
					}
				}

				this->m_states.push(std::move(this->m_newState));
				if (!this->m_states.top()->Init()) { return false; }
				this->m_isAdding = false;
			}
			return true;
		}

		StateRef &GetActiveState()
		{
			return this->m_states.top();
		}

	private:
		std::stack<StateRef> m_states;
		StateRef m_newState;

		bool m_isRemoving = false;
		bool m_isAdding = false;
		bool m_isReplacing = true;
	};
}  // namespace sse
