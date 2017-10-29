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
    
------------------------------------------------------------------------------------------

==========================================================================================
    ## Overview
==========================================================================================
    This project demonstrates an example of using OpenCL to optimize an operation on 
    float4 values. In this example, a collection of float4s is meant to represent 
    RGBA values for pixels. The operation we perform is to halve the value of each 
    element of each float4 to simulate decreasing screen brightness.

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

==========================================================================================
    ## Convenience Builds
==========================================================================================
    Builds for each of the projects and the resources to run them have been provided at
        ./Convenience Builds/. Each project can be run as is.
         