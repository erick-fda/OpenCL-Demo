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
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include "Pixel.h"

/*========================================================================================
	Forward Declarations
========================================================================================*/
void GeneratePixels();
void GetAvailablePlatforms();
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind);
cl_device_id GetFirstDeviceOfTypeFromPlatform(cl_platform_id platform, cl_device_type typeToCheck);
bool GetCpuAndGpu();
bool SetUpCL(cl_platform_id platform, cl_device_id device);
bool ReadKernelFile();

/*========================================================================================
	Fields
========================================================================================*/
const int NUM_PIXELS = 10;
std::vector<cl_float4> _startPixels;
std::vector<cl_float4> _resultPixels;

cl_uint _numPlatforms;
std::vector<cl_platform_id> _platformIds;
std::map<cl_platform_id, std::vector<cl_device_id>> _platforms;

cl_platform_id _cpuPlatform;
cl_platform_id _gpuPlatform;
cl_device_id _cpuDevice;
cl_device_id _gpuDevice;

cl_context _context;
cl_command_queue _commandQueue;
std::string _kernelFilePath = "Kernel.cl";
std::string _kernelString = "";
cl_program _program;
cl_kernel _kernel;
size_t _bufferSize = sizeof(cl_float4) * NUM_PIXELS;
cl_mem _clStartPixels;
cl_mem _clResultPixels;

/*========================================================================================
	Main Function
========================================================================================*/
/**
	Example of calculating average colour for a collection of pixels using OpenCL to 
	run on the CPU only.
*/
int main()
{
	GeneratePixels();

	GetAvailablePlatforms();

	if (!GetCpuAndGpu())
	{
		std::cin.ignore();
		return 1;
	}

	if (!SetUpCL(_cpuPlatform, _cpuDevice))
	{
		std::cin.ignore();
		return 1;
	}

	std::cin.ignore();
	return 0;
}

/**
	Create the OpenCL context.
*/
bool SetUpCL(cl_platform_id platform, cl_device_id device)
{
	cl_int result = 0;

	///* Initialize properties for the context. */
	//const cl_context_properties contextProperties[] =
	//{
	//	CL_CONTEXT_PLATFORM,
	//	reinterpret_cast<cl_context_properties>(platform),
	//	0,
	//	0
	//};

	//_context = clCreateContext(
	//	contextProperties, _platforms[platform].size(),
	//	_platforms[platform].data(), nullptr,
	//	nullptr, &result
	//);

	/* Read the kernel source. */
	if (!ReadKernelFile())
	{
		std::cout << "Failed to read kernel file.\n\n";
		return false;
	}

	/* Create the context. */
	_context = clCreateContext(0, 1, &device, NULL, NULL, &result);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create context.\n\n";
		return false;
	}

	/* Create the command queue. */
	_commandQueue = clCreateCommandQueue(_context, device, 0, &result);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create command queue.\n\n";
		return false;
	}

	/* Create and build the CL program. */
	const char* kernelAsChar = new char[_kernelString.size()];
	kernelAsChar = _kernelString.c_str();
	_program = clCreateProgramWithSource(
		_context, 1,
		(const char **)& kernelAsChar, NULL,
		&result);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create program.\n\n";
		return false;
	}

	result = clBuildProgram(_program, 0, NULL, NULL, NULL, NULL);

	/* Print the error log if there are build errors. */
	if (result == CL_BUILD_PROGRAM_FAILURE)
	{
		std::cout << "There were build errors: \n";

		/* Determine CL error log size. */
		size_t errorLogSize;
		clGetProgramBuildInfo(_program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &errorLogSize);

		/* Get and print the CL error log. */
		char *errorLog = (char *)malloc(errorLogSize);
		clGetProgramBuildInfo(_program, device, CL_PROGRAM_BUILD_LOG, errorLogSize, errorLog, NULL);
		std::cout << errorLog << "\n\n";
	}
	else if (result != CL_SUCCESS)
	{
		std::cout << "Failed to build program.\n\n";
		return false;
	}

	///* Create the kernel. */
	//_kernel = clCreateKernel(_program, "halveBrightness", &result);

	//if (result != CL_SUCCESS)
	//{
	//	std::cout << "Failed to create kernel.\n\n";
	//	return false;
	//}

	///* Create input and output buffers. */
	//_clStartPixels = clCreateBuffer(
	//	_context, CL_MEM_READ_ONLY,
	//	_bufferSize, _startPixels.data(),
	//	&result
	//);

	//if (result != CL_SUCCESS)
	//{
	//	std::cout << "Failed to create input buffer.\n\n";
	//	return false;
	//}

	//_clResultPixels = clCreateBuffer(
	//	_context, CL_MEM_WRITE_ONLY,
	//	_bufferSize, _resultPixels.data(),
	//	&result
	//);

	//if (result != CL_SUCCESS)
	//{
	//	std::cout << "Failed to create output buffer.\n\n";
	//	return false;
	//}

	return (result == CL_SUCCESS);
}

/**
	Reads in the contents of the kernel file.
*/
bool ReadKernelFile()
{
	std::ifstream fileStream;
	fileStream.open(_kernelFilePath);

	if(!fileStream)
	{
		std::cout << "Failed to open kernel file.";
		return false;
	}

	/* Get each line. */
	std::string eachLine;
	while (std::getline(fileStream, eachLine))
	{
		_kernelString += eachLine;
	}
	fileStream.close();

	/* Check that the file was not empty. */
	if (_kernelString.length() < 1)
	{
		std::cout << "Kernel file is empty.";
		return false;
	}

	//std::cout << "Kernel file contents: \n" << _kernel;

	return true;
}

/**
	Get the CPU and GPU.
*/
bool GetCpuAndGpu()
{
	/* Find the platform with the GPU. */
	_gpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_GPU);
	if (_gpuPlatform)
	{
		_gpuDevice = GetFirstDeviceOfTypeFromPlatform(_gpuPlatform, CL_DEVICE_TYPE_GPU);
	}
	else
	{
		std::cout << "ERROR: No GPU could be detected on any available platform.\n\n";
		return false;
	}

	/* Check the GPU platform for a CPU device. */
	_cpuDevice = GetFirstDeviceOfTypeFromPlatform(_gpuPlatform, CL_DEVICE_TYPE_CPU);

	if (_cpuDevice)
	{
		_cpuPlatform = _gpuPlatform;
	}
	else
	{
		/* If the CPU is not on the same platform as the GPU, check the other platforms. */
		_cpuPlatform = GetFirstPlatformWithDeviceOfType(CL_DEVICE_TYPE_CPU);

		/* Get the CPU device if we found a platform for it and error out if we didn't. */
		if (_cpuPlatform)
		{
			_cpuDevice = GetFirstDeviceOfTypeFromPlatform(_cpuPlatform, CL_DEVICE_TYPE_CPU);
		}
		else
		{
			std::cout << "ERROR: No CPU could be detected on any available platform.\n\n";
			return false;
		}
	}

	/* Print out info on CPU and GPU. */
	std::cout << "Platform containing CPU is " << _cpuPlatform << "\n";
	std::cout << "CPU is device " << _cpuDevice << "\n\n";
	std::cout << "Platform containing GPU is " << _gpuPlatform << "\n";
	std::cout << "GPU is device " << _gpuDevice << "\n\n";

	return true;
}

/**
	Returns the first available platform that contains a device of the given type.
	Returns 0 if no platform has a device of the given type.
*/
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind)
{
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in _platforms)
	{
		if (GetFirstDeviceOfTypeFromPlatform(eachPlatform.first, typeToFind))
		{
			return eachPlatform.first;
		}
	}

	return 0;
}

/**
	Returns the ID of the first device of the given type on the given platform.
	Returns 0 if there is no device of the given type on the given platform.
*/
cl_device_id GetFirstDeviceOfTypeFromPlatform(cl_platform_id platform, cl_device_type typeToFind)
{
	for each (cl_device_id eachDevice in _platforms[platform])
	{
		cl_device_type deviceType;
		clGetDeviceInfo(eachDevice, CL_DEVICE_TYPE, sizeof(cl_device_id), &deviceType, nullptr);

		if (deviceType == typeToFind)
		{
			return eachDevice;
		}
	}

	return 0;
}

/**
	Get the OpenCL platforms and devices available.
*/
void GetAvailablePlatforms()
{
	/* Get the number of available platforms. */
	_numPlatforms = 0;
	clGetPlatformIDs(0, nullptr, &_numPlatforms);

	/* Get the IDs of available platforms. */
	_platformIds = std::vector<cl_platform_id>(_numPlatforms);
	clGetPlatformIDs(_numPlatforms, _platformIds.data(), nullptr);

	/* Get the devices for each platform. */
	_platforms = std::map<cl_platform_id, std::vector<cl_device_id>>();
	for each (cl_platform_id eachPlatform in _platformIds)
	{
		cl_uint numDevices = 0;
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices);

		_platforms[eachPlatform] = std::vector<cl_device_id>(numDevices);
		clGetDeviceIDs(eachPlatform, CL_DEVICE_TYPE_ALL, numDevices, _platforms[eachPlatform].data(), nullptr);
	}

	/* Print out platforms and devices. */
	std::cout << "Detected the following platforms and devices:\n";
	for each (std::pair<cl_platform_id, std::vector<cl_device_id>> eachPlatform in _platforms)
	{
		std::cout << "\t" << eachPlatform.first << "\n";

		for each (cl_device_id eachDevice in eachPlatform.second)
		{
			cl_device_type deviceType;
			clGetDeviceInfo(eachDevice, CL_DEVICE_TYPE, sizeof(cl_device_id), &deviceType, nullptr);

			std::string deviceTypeString;
			switch (deviceType)
			{
			case CL_DEVICE_TYPE_CPU:
				deviceTypeString = "CPU";
				break;
			case CL_DEVICE_TYPE_GPU:
				deviceTypeString = "GPU";
				break;
			default:
				deviceTypeString = "OTHER";
				break;
			}

			std::cout << "\t\t" << eachDevice << " (" << deviceTypeString.data() << ")\n";
		}
	}
	std::cout << "\n";
}

/**
	Generates pixels to use.
*/
void GeneratePixels()
{
	/* Initialize fields. */
	_startPixels = std::vector<cl_float4>();
	_resultPixels = std::vector<cl_float4>();

	/* Seed the random number generator. */
	srand((unsigned int)time(nullptr));

	/* Generate pixels. */
	std::cout << "Generating pixels...\n\n";
	for (int i = 0; i < NUM_PIXELS; i++)
	{
		_startPixels.push_back(Pixel::MakeRandomPixel());
	}
}
