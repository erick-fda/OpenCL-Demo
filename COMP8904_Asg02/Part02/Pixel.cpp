/*===================================================================================*//**
	Pixel
	
	Static class for pixel utilities.
    
    Copyright 2017 Erick Fernandez de Arteaga. All rights reserved.
        https://www.linkedin.com/in/erick-fda
        https://bitbucket.org/erick-fda

    @author Erick Fernandez de Arteaga
	@version 0.0.0
	@file
	
	@see Pixel
	@see Pixel.h
	
*//*====================================================================================*/

/*========================================================================================
	Dependencies
========================================================================================*/
#include "Pixel.h"
#include <random>
#include <vector>

/*----------------------------------------------------------------------------------------
	Class Fields
----------------------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------------------
	Class Methods
----------------------------------------------------------------------------------------*/
/**
	Returns a cl_float4 with random RGBA values;
*/
cl_float4 Pixel::MakeRandomPixel()
{
	return cl_float4{Random0to1(), Random0to1(), Random0to1(), Random0to1()};
}

/**
	Returns a random float value in the range [0-1].
*/
float Pixel::Random0to1()
{
	return ((float)rand() / RAND_MAX);
}

/**
	Simulates halving the screen brightness by halving each cl_float4.	
*/
void Pixel::HalveBrightness(std::vector<cl_float4>& pixels, std::vector<cl_float4>& resultPixels)
{
	resultPixels = std::vector<cl_float4>();

	for each (cl_float4 eachPixel in pixels)
	{
		cl_float4 half = cl_float4();
		half.x = eachPixel.x / 2;
		half.y = eachPixel.y / 2;
		half.z = eachPixel.z / 2;
		half.w = eachPixel.w / 2;

		resultPixels.push_back(half);
	}
}

/**
	Gets the average color from a vector of pixels.
*/
cl_float4 Pixel::GetAverageColor(std::vector<cl_float4> pixels)
{
	cl_float4 total { 0, 0, 0, 0 };
	cl_float4 average { 0, 0, 0, 0 };

	for each (cl_float4 eachPixel in pixels)
	{
		total.x += eachPixel.x;
		total.y += eachPixel.y;
		total.z += eachPixel.z;
		total.w += eachPixel.w;
	}

	average.x = total.x / pixels.size();
	average.y = total.y / pixels.size();
	average.z = total.z / pixels.size();
	average.w = total.w / pixels.size();

	return average;
}
