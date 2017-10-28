/*===================================================================================*//**
	Part 01
	
	Example of calculating average colour for a collection of pixels serially.

    @author Erick Fernandez de Arteaga, John Janzen
	@version 0.0.0
	@file
	
*//*====================================================================================*/

/*========================================================================================
	Dependencies
========================================================================================*/
#include <chrono>
#include <ctime>
#include <iostream>
#include <random>
#include "Pixel.h"


/*========================================================================================
	Fields
========================================================================================*/
const int NUM_PIXELS = 1000000;
std::vector<float4> _pixels;
int _startTime;
int _endTime;
int _timeToRun;

/*========================================================================================
	Main Function
========================================================================================*/
/**
	Creates a collection of "pixels" (represented by float4s indicating RGBA values) and 
	calculates the average color for the pixels.
*/
int main()
{
	/* Initialize fields. */
	_pixels = std::vector<float4>();

	/* Seed the random number generator. */
	srand((unsigned int) time(nullptr));

	/* Generate pixels. */
	std::cout << "Generating pixels...\n\n";
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		_pixels.push_back(Pixel::MakeRandomPixel());
	}

	/* Calculate the average color, timing how long it takes to calculate this. */
	std::cout << "Calculating average color. Timer starts now!\n\n";
	_startTime = clock();
	float4 averageColor = Pixel::GetAverageColor(_pixels);
	_endTime = clock();
	_timeToRun = (_endTime - _startTime) / (double) CLOCKS_PER_SEC * 1000;

	/* Print out results.*/
	std::cout << "Average color: \n" << 
		"\tX: " << averageColor.x << "\n"
		"\tY: " << averageColor.y << "\n"
		"\tZ: " << averageColor.z << "\n"
		"\tW: " << averageColor.w << "\n\n";

	std::cout << "Time to calculate: " << _timeToRun << " ms\n\n";

	std::cout << "Press any key to exit...\n\n";

	std::cin.ignore();
}
