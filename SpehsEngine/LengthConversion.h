#pragma once
#include <utility> //std::pair

std::pair<int, int> getAsWindowCoordinates(float xValue, float yValue);
int getAsWindowXCoordinate(float xValue);
int getAsWindowYCoordinate(float yValue);
std::pair<float, float> getAsGLCoordinates(int xValue, int yValue);
float getAsGLXCoordinate(int xValue);
float getAsGLYCoordinate(int yValue);
float getAsGLWidth(int widthInWindowPixels);
float getAsGLHeight(int heightInWindowPixels);

