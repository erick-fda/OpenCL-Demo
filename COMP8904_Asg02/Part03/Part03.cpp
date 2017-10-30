/*===================================================================================*//**
	Part 03
	
	Example of calculating average colour for a collection of pixels using OpenCL to 
	run on the GPU only.

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
void ExecuteSerially();
void GetAvailablePlatforms();
cl_platform_id GetFirstPlatformWithDeviceOfType(cl_device_type typeToFind);
cl_device_id GetFirstDeviceOfTypeFromPlatform(cl_platform_id platform, cl_device_type typeToCheck);
bool GetCpuAndGpu();
bool SetUpCL(cl_platform_id platform, cl_device_id device);
bool ReadKernelFile();
bool ExecuteKernel();
bool CheckKernelResults();
void CleanUpCl();

/*========================================================================================
	Fields
========================================================================================*/
const int NUM_PIXELS = 1000000;
std::vector<cl_float4> _startPixels;
std::vector<cl_float4> _resultPixels;
cl_float4* _startPixelHostBuffer;
cl_float4* _resultPixelHostBuffer;

int _timeTaken = 0;

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

	/* Execute on host. */
	ExecuteSerially();

	/* Find CL devices. */
	GetAvailablePlatforms();

	if (!GetCpuAndGpu())
	{
		std::cin.ignore();
		return 1;
	}

	/* Execute on GPU. */
	if (!SetUpCL(_gpuPlatform, _gpuDevice))
	{
		std::cin.ignore();
		return 1;
	}

	std::cout << "Executing using OpenCL on GPU. Timer start.\n\n";
	_timeTaken = clock();
	if (!ExecuteKernel())
	{
		std::cin.ignore();
		return 1;
	}
	_timeTaken = (clock() - _timeTaken) / (double)CLOCKS_PER_SEC * 1000;
	std::cout << "Finished executing using OpenCL on GPU. Took " << _timeTaken << " ms.\n\n";

	if (!CheckKernelResults())
	{
		std::cin.ignore();
		return 1;
	}

	std::cout << "OpenCL example finished successfully!\n\n";

	std::cin.ignore();

	CleanUpCl();

	return 0;
}

/**
	Clean up OpenCL memory objects.
*/
void CleanUpCl()
{
	/* Free OpenCL memory objects. */
	clReleaseMemObject(_clStartPixels);
	clReleaseMemObject(_clResultPixels);
	clReleaseProgram(_program);
	clReleaseKernel(_kernel);
	clReleaseCommandQueue(_commandQueue);
	clReleaseContext(_context);
}

/**
	Check the results of executing the kernel.
*/
bool CheckKernelResults()
{
	cl_int result = 0;

	if (_resultPixelHostBuffer == nullptr)
	{
		std::cout << "Host buffer is null.\n\n";
		return false;
	}

	result = clEnqueueReadBuffer(
		_commandQueue, _clResultPixels,
		CL_TRUE, 0,
		_bufferSize, _resultPixelHostBuffer,
		0, NULL,
		NULL
	);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to read output buffer.\n\n";
		return false;
	}

	std::cout << "Finished reading output buffer successfully.\n\n";

	/* Print out a sample from the results. */
	std::cout << "Sample initial pixel: \n" <<
		"\tX: " << _startPixels[0].x << "\n"
		"\tY: " << _startPixels[0].y << "\n"
		"\tZ: " << _startPixels[0].z << "\n"
		"\tW: " << _startPixels[0].w << "\n\n";
	std::cout << "Corresponding result pixel: \n" <<
		"\tX: " << _resultPixelHostBuffer[0].x << "\n"
		"\tY: " << _resultPixelHostBuffer[0].y << "\n"
		"\tZ: " << _resultPixelHostBuffer[0].z << "\n"
		"\tW: " << _resultPixelHostBuffer[0].w << "\n\n";

	return true;
}

/**
	Executes the kernel.
*/
bool ExecuteKernel()
{
	cl_int result = 0;

	/* Execute the kernel. */
	size_t localSize = 64;
	size_t globalSize = ceil(NUM_PIXELS / (float)localSize)*localSize;
	result = clEnqueueNDRangeKernel(
		_commandQueue, _kernel,
		1, NULL,
		&globalSize, &localSize,
		0, NULL,
		NULL
	);

	clFinish(_commandQueue);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to execute kernel.\n\n";
		return false;
	}

	std::cout << "Finished executing kernel successfully.\n\n";
	return true;
}

/**
	Create the OpenCL context.
*/
bool SetUpCL(cl_platform_id platform, cl_device_id device)
{
	cl_int result = 0;

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
		return false;
	}
	else if (result != CL_SUCCESS)
	{
		std::cout << "Failed to build program.\n\n";
		return false;
	}

	/* Create the kernel. */
	_kernel = clCreateKernel(_program, "halveBrightness", &result);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create kernel.\n\n";
		return false;
	}

	/* Create input and output buffers. */
	_clStartPixels = clCreateBuffer(
		_context, CL_MEM_READ_ONLY,
		_bufferSize, NULL,
		&result
	);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create input buffer.\n\n";
		return false;
	}

	_clResultPixels = clCreateBuffer(
		_context, CL_MEM_WRITE_ONLY,
		_bufferSize, NULL,
		&result
	);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to create output buffer.\n\n";
		return false;
	}

	/* Write data to the input buffer. */
	result = clEnqueueWriteBuffer(
		_commandQueue, _clStartPixels,
		CL_TRUE, 0,
		_bufferSize, _startPixelHostBuffer,
		0, NULL,
		NULL
	);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to write to the input buffer.\n\n";
		return false;
	}

	/* Set kernel arguments. */
	result = clSetKernelArg(_kernel, 0, sizeof(cl_mem), &_clStartPixels);
	result |= clSetKernelArg(_kernel, 1, sizeof(cl_mem), &_clResultPixels);

	if (result != CL_SUCCESS)
	{
		std::cout << "Failed to set kernel arguments.\n\n";
		return false;
	}

	std::cout << "Finished setting up CL program successfully.\n\n";
	return true;
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
	Halves the brightness of the pixels serially.
*/
void ExecuteSerially()
{
	std::cout << "Executing serially. Timer start.\n\n";
	_timeTaken = clock();
	Pixel::HalveBrightness(_startPixels, _resultPixels);
	_timeTaken = (clock() - _timeTaken) / (double)CLOCKS_PER_SEC * 1000;
	std::cout << "Finished executing serially. Took " << _timeTaken << " ms.\n\n";
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

	_startPixelHostBuffer = (cl_float4*)malloc(_bufferSize);
	_resultPixelHostBuffer = (cl_float4*)malloc(_bufferSize);
	_startPixelHostBuffer = _startPixels.data();
	_resultPixelHostBuffer = new cl_float4[NUM_PIXELS];
}
