/*===================================================================================*//**
	Part 01
	
	Example of reducing brightness for a collection of pixels serially.

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
#include <CL/cl.h>
#include "Pixel.h"


/*========================================================================================
	Fields
========================================================================================*/
const int NUM_PIXELS = 1000000;
std::vector<cl_float4> _startPixels;
std::vector<cl_float4> _resultPixels;
int _startTime;
int _endTime;
int _timeToRun;

/*========================================================================================
	Main Function
========================================================================================*/
/**
	Creates a collection of "pixels" (represented by cl_float4s indicating RGBA values) and 
	halves them to represent reducing the screen brightness.
*/
int main()
{
	/* Initialize fields. */
	_startPixels = std::vector<cl_float4>();
	_resultPixels = std::vector<cl_float4>();

	/* Seed the random number generator. */
	srand((unsigned int) time(nullptr));

	/* Generate pixels. */
	std::cout << "Generating pixels...\n\n";
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		_startPixels.push_back(Pixel::MakeRandomPixel());
	}

	/* Calculate the average color, timing how long it takes to calculate this. */
	std::cout << "Reducing brightness. Timer starts now!\n\n";
	_startTime = clock();
	Pixel::HalveBrightness(_startPixels, _resultPixels);
	_endTime = clock();
	_timeToRun = (_endTime - _startTime) / (double) CLOCKS_PER_SEC * 1000;

	std::cout << "Done. Time to calculate: " << _timeToRun << " ms\n\n";

	/* Print out results.*/
	std::cout << "Sample initial pixel: \n" << 
		"\tX: " << _startPixels[0].x << "\n"
		"\tY: " << _startPixels[0].y << "\n"
		"\tZ: " << _startPixels[0].z << "\n"
		"\tW: " << _startPixels[0].w << "\n\n";
	std::cout << "Corresponding result pixel: \n" <<
		"\tX: " << _resultPixels[0].x << "\n"
		"\tY: " << _resultPixels[0].y << "\n"
		"\tZ: " << _resultPixels[0].z << "\n"
		"\tW: " << _resultPixels[0].w << "\n\n";

	std::cout << "Press any key to exit...\n\n";

	std::cin.ignore();
}
