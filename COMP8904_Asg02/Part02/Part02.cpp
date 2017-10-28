/*===================================================================================*//**
	Part 02
	
	Example of calculating average colour for a collection of pixels using OpenCL to 
	run on the CPU only.

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
//#include "Pixel.h"

#include <stdio.h>
#include <CL/cl.h>

/*========================================================================================
	Fields
========================================================================================*/
const int NUM_PIXELS = 1000000;
//std::vector<float4> _pixels;
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
	cl_int err;
	cl_uint numPlatforms;

	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (CL_SUCCESS == err)
		printf("\nDetected OpenCL platforms: %d", numPlatforms);
	else
		printf("\nError calling clGetPlatformIDs. Error code: %d", err);

	std::cin.ignore();
}
