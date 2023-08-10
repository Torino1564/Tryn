#pragma once
#include <bitset>
#include <queue>

namespace tryn::win
{
	class IWindow;
	class Keyboard
	{
		friend class IWindow;
	public:
		// User Interface
		// Keystrokes
		class KeyboardEvent
		{
		public:
			enum class Type
			{
				Press, Release, Invalid
			};
		public:
			KeyboardEvent();
			KeyboardEvent(Type type, uint8_t code);
			bool IsTypePress() const;
			bool IsTypeRelease() const;
			bool IsValid() const;
			uint8_t GetCode() const;
		private:
			Type type;
			uint8_t code;

		};
		bool IsKeyPressed(uint8_t keyCode);
		bool IsKeyQueueEmpty();
		KeyboardEvent ReadKey();
		void ClearKeyQueue();
		// Char
		bool IsCharQueueEmpty();
		char ReadChar();
		void ClearCharQueue();
		void ClearQueues();
		// Autorepeat
		void EnableAutoRepeat();
		void DisableAutoRepeat();
		bool IsAutoRepeatEnabled() const;

		template <typename T>
		static void CullQueue(std::queue<T>& queue)
		{
			while (queue.size() > queueSize)
			{
				queue.pop();
			}
		}

	private:
		// Window side interface
		void OnKeyPress(uint8_t keyCode);
		void OnKeyRelease(uint8_t keyCode);
		void OnChar(char character);
		void Clear();

		static constexpr uint16_t numKeys = 256;
		static constexpr uint16_t queueSize = 16;
		bool autorepeatEnabled = false;
		std::bitset<numKeys> keyStates;
		std::queue<KeyboardEvent> keyEventQueue;
		std::queue<char> charQueue;
	};

}