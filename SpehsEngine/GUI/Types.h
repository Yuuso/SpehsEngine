#pragma once

#include "SpehsEngine/GUI/Units.h"
#include <stdint.h>


namespace se
{
	namespace gui
	{
		// TODO: Move to Core? C++ Language features?
	#define SE_INTERFACE __declspec(novtable)
	#define SE_NO_COPY(CLASS) \
		CLASS(const CLASS& _other) = delete; \
		CLASS& operator=(const CLASS& _other) = delete; \
		CLASS(CLASS&& _other) = delete; \
		CLASS& operator=(CLASS&& _other) = delete;
		template<typename Type> using unique_ptr_vector = std::vector<std::unique_ptr<Type>>;
		//

		class CallOnDelete
		{
		public:
			SE_NO_COPY(CallOnDelete)
			CallOnDelete(const std::function<void()>& _fn) : func(_fn) {}
			~CallOnDelete() { func(); }
		private:
			const std::function<void()> func;
		};

		typedef int16_t ZIndex;

		template<typename Signature> using Signal = boost::signals2::signal<Signature>;
		using Connection = boost::signals2::connection;
		using ScopedConnection = boost::signals2::scoped_connection;
		using ConnectionBlock = boost::signals2::shared_connection_block;

		enum Alignment
		{
			Center		=				0,

			Vertical	=				1 << 0,
			Top			=	 Vertical | 1 << 1,
			Bottom		=	 Vertical | 1 << 2,

			Horizontal	=				1 << 3,
			Left		=  Horizontal | 1 << 4,
			Right		=  Horizontal | 1 << 5,
		};
	}

	namespace legacygui
	{
		typedef gui::ZIndex ZIndex;
		typedef uint16_t GUIElementUpdateFlagsType;

		enum class VerticalAlignment
		{
			Top,
			Center,
			Middle = Center,
			Bottom
		};
		enum class HorizontalAlignment
		{
			Left,
			Center,
			Middle = Center,
			Right
		};

		typedef VerticalAlignment VerticalAnchor;
		typedef HorizontalAlignment HorizontalAnchor;

		enum class StackOrientation
		{
			Vertical,
			Horizontal
		};
	}
}
