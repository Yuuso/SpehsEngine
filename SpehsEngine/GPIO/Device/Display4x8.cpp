#include "stdafx.h"
#include "SpehsEngine/GPIO/Device/Display4x8.h"
#include "SpehsEngine/GPIO/Pin.h"
#include "SpehsEngine/Core/SE_Time.h"


namespace se
{
	namespace device
	{
		Display4x8::Display4x8()
		{

		}
		
		Display4x8::~Display4x8()
		{

		}

		void Display4x8::setString(const std::string& str)
		{
			const se::gpio::Pin digitPins[4] =
			{
				se::gpio::pin_19,
				se::gpio::pin_23,
				se::gpio::pin_21,
				se::gpio::pin_24,
			};
			const se::gpio::Pin ledPins[8] =
			{
				se::gpio::pin_38,//top
				se::gpio::pin_40,//top left
				se::gpio::pin_36,//top right
				se::gpio::pin_37,//middle
				se::gpio::pin_29,//bottom left
				se::gpio::pin_35,//bottom right
				se::gpio::pin_31,//bottom
				se::gpio::pin_33,//.
			};
			const int symbols[12][8] =
			{
				{ /*top*/1,/*topL*/1,/*topR*/1,/*middle*/0,/*bottomL*/1,/*bottomR*/1,/*bottom*/1,/*.*/0 },//0
				{ /*top*/0,/*topL*/0,/*topR*/1,/*middle*/0,/*bottomL*/0,/*bottomR*/1,/*bottom*/0,/*.*/0 },//1
				{ /*top*/1,/*topL*/0,/*topR*/1,/*middle*/1,/*bottomL*/1,/*bottomR*/0,/*bottom*/1,/*.*/0 },//2
				{ /*top*/1,/*topL*/0,/*topR*/1,/*middle*/1,/*bottomL*/0,/*bottomR*/1,/*bottom*/1,/*.*/0 },//3
				{ /*top*/0,/*topL*/1,/*topR*/1,/*middle*/1,/*bottomL*/0,/*bottomR*/1,/*bottom*/0,/*.*/0 },//4
				{ /*top*/1,/*topL*/1,/*topR*/0,/*middle*/1,/*bottomL*/0,/*bottomR*/1,/*bottom*/1,/*.*/0 },//5
				{ /*top*/1,/*topL*/1,/*topR*/0,/*middle*/1,/*bottomL*/1,/*bottomR*/1,/*bottom*/1,/*.*/0 },//6
				{ /*top*/1,/*topL*/0,/*topR*/1,/*middle*/0,/*bottomL*/0,/*bottomR*/1,/*bottom*/0,/*.*/0 },//7
				{ /*top*/1,/*topL*/1,/*topR*/1,/*middle*/1,/*bottomL*/1,/*bottomR*/1,/*bottom*/1,/*.*/0 },//8
				{ /*top*/1,/*topL*/1,/*topR*/1,/*middle*/1,/*bottomL*/0,/*bottomR*/1,/*bottom*/1,/*.*/0 },//9
				{ /*top*/0,/*topL*/0,/*topR*/0,/*middle*/0,/*bottomL*/0,/*bottomR*/0,/*bottom*/0,/*.*/1 },//.
				{ /*top*/0,/*topL*/0,/*topR*/0,/*middle*/0,/*bottomL*/0,/*bottomR*/0,/*bottom*/0,/*.*/0 },//<empty>
			};

			for (size_t i = 0; i < 4; i++)
				se::gpio::setPinAsOutput(digitPins[i]);
			for (size_t i = 0; i < 8; i++)
				se::gpio::setPinAsOutput(ledPins[i]);
			for (size_t i = 0; i < 4; i++)
				se::gpio::enable(digitPins[i]);
			for (size_t i = 0; i < 8; i++)
				se::gpio::disable(ledPins[i]);

			int number = 0;
			int digitIndex = 0;
			for (size_t d = 0; d < 4; d++)
			{//For each digit

				for (size_t d2 = 0; d2 < 4; d2++)
				{
					if (d == d2)
						se::gpio::disable(digitPins[d2]);
					else
						se::gpio::enable(digitPins[d2]);
				}

				for (size_t s = 0; s < 8; s++)
					se::gpio::write(ledPins[s], (se::gpio::PinState)symbols[number][s]);

				se::time::delay(se::time::fromMilliseconds(100));
				if (++number == 11)
				{
					number = 0;
					if (++digitIndex == 4)
						digitIndex = 0;
				}
			}
		}
	}
}