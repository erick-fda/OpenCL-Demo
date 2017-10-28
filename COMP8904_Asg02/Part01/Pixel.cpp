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
	Returns a float4 with random RGBA values;
*/
float4 Pixel::MakeRandomPixel()
{
	return float4{Random0to1(), Random0to1(), Random0to1(), Random0to1()};
}

/**
	Returns a random float value in the range [0-1].
*/
float Pixel::Random0to1()
{
	return ((float)rand() / RAND_MAX);
}

/**
	Gets the average color from an array of pixels.
*/
float4 Pixel::GetAverageColor(std::vector<float4> pixels)
{
	float4 total { 0, 0, 0, 0 };
	float4 average { 0, 0, 0, 0 };

	for each (float4 eachPixel in pixels)
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
