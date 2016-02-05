#include <cmath>//Std::floor

#include "ApplicationData.h"
#include "LengthConversion.h"



//Screen coordinate / GL screen dimension conversions
std::pair<int, int> getAsWindowCoordinates(float xValue, float yValue)
{
	return std::make_pair<int, int>(getAsWindowXCoordinate(xValue), getAsWindowYCoordinate(yValue));
}
int getAsWindowXCoordinate(float xValue)
{
	xValue += 1.0f;
	xValue /= 2.0f;
	return int(std::floor(xValue * applicationData->getWindowWidth()));
}
int getAsWindowYCoordinate(float yValue)
{
	yValue -= 1.0f;
	yValue /= -2.0f;
	return int(std::floor(yValue * applicationData->getWindowHeight()));
}
std::pair<float, float> getAsGLCoordinates(int xValue, int yValue)
{
	return std::make_pair<float, float>(getAsGLXCoordinate(xValue), getAsGLYCoordinate(yValue));
}
float getAsGLXCoordinate(int xValue)
{
	float returnValue = float(xValue) / applicationData->getWindowWidth();
	returnValue *= 2.0f;
	return --returnValue;
}
float getAsGLYCoordinate(int yValue)
{
	float returnValue = float(yValue) / applicationData->getWindowHeight();
	returnValue *= -2.0f;
	return ++returnValue;
}
float getAsGLWidth(int widthInWindowPixels)
{
	return 2.0f * (float(widthInWindowPixels) / applicationData->getWindowWidth());
}
float getAsGLHeight(int heightInWindowPixels)
{
	return 2.0f * (float(heightInWindowPixels) / applicationData->getWindowHeight());
}
