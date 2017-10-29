==========================================================================================
    COMP 8551 Assignment 02
    
    Erick Fernandez de Arteaga
    John Janzen

    Version 0.1.0
    
==========================================================================================

------------------------------------------------------------------------------------------
    TABLE OF CONTENTS
    
    ## Overview
    ## Convenience Builds
    ## Timings
    
------------------------------------------------------------------------------------------

==========================================================================================
    ## Overview
==========================================================================================
    This project demonstrates an example of using OpenCL to optimize an operation on a 
    collection of one million float4 values. In this example, the collection of float4s is 
    meant to represent RGBA values for pixels. The operation we perform is to halve the 
    value of each element of each float4 to simulate decreasing screen brightness.

    The project located at 
        ./COMP8904_Asg02/Part01/ demonstrates performing the operation serially using C++.
        
    The project located at 
        ./COMP8904_Asg02/Part02/ demonstrates performing the operation using OpenCL on 
        either the CPU.
        
    The project located at 
        ./COMP8904_Asg02/Part03/ demonstrates performing the operation using OpenCL on 
        either the GPU.
        
    The project located at 
        ./COMP8904_Asg02/Part04/ demonstrates performing the operation using OpenCL on 
        both the CPU and GPU.

    Note that the current Visual Studio project only adds the necessary includes and 
    dependencies for running OpenCL on Intel devices on x64 machines. If building and 
    running on a machine with OpenCL for AMD or Nvidia installed, additional include 
    directories may need to be added.

==========================================================================================
    ## Convenience Builds
==========================================================================================
    Builds for each of the projects and the resources to run them have been provided at
        ./Convenience Builds/. Each project can be run as is.
         
==========================================================================================
    ## Timings
==========================================================================================
    On our test machine, the timings for the different examples are typically around the 
    following amounts:

        Serial:                     1300 ms - 1700 ms
        OpenCL on CPU:              10 ms - 20 ms
        OpenCL on GPU:              5 ms - 7 ms
        OpenCL on both:             -
    
    This shows a clear benefit in using OpenCL. This is because of the serial execution 
    of the kernel on the OpenCL device. In some cases, depending on the type and amount 
    of data, the overhead of copying data to the OpenCL device can outweigh the benefit 
    gained by executing serially, but in our case, using OpenCL provides a marked 
    improvement whether the kernel is run on the CPU or GPU, with the GPU being faster.
