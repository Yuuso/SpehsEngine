#pragma once
namespace se
{
	namespace rendering
	{
		class Window;
		class GLContext
		{
		public:

			bool isValid() const { return valid; }

			Window& window;

			void* getImpl() const { return glContext; }

		private:
			friend class Window;
			GLContext(Window& window);
			~GLContext();
			void* glContext;
			bool valid;
		};
	}
}