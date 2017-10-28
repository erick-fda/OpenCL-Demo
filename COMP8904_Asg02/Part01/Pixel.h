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
	@see Pixel.cpp
	
*//*====================================================================================*/

#ifndef PIXEL_H
#define PIXEL_H

/*========================================================================================
	Dependencies
========================================================================================*/
#include <vector>

/*========================================================================================
	Structs
========================================================================================*/
typedef struct float4
{
	public: 
		float x;
		float y;
		float z;
		float w;
};

/*========================================================================================
	Pixel	
========================================================================================*/
/**
	Static class for pixel utilities.
	
	@see Pixel
	@see Pixel.cpp
*/
static class Pixel
{
    /*------------------------------------------------------------------------------------
		Class Fields
    ------------------------------------------------------------------------------------*/
    private:

	/*------------------------------------------------------------------------------------
		Class Methods
	------------------------------------------------------------------------------------*/
    public:
		static float4 MakeRandomPixel();
		static float4 GetAverageColor(std::vector<float4> pixels);

    private:
		static float Random0to1();

};

#endif
