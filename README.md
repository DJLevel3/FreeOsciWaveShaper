#FreeOsciWaveShaper
A free, open source waveshaper VST for making oscilloscope music on a budget!


**WARNING: DO NOT uncomment line 123 in FOWS.cpp, it will render the file selector button correctly but clicking it causes an access violation and crashes both the VST and the host.**
----------
At the moment the only way to get a waveshaper function in is by manually entering the values in the float array func in FOWS.h.  
The format of the array is {input, left, right} where all values are between -1 and 1 and outputs are interpolated linearly between points.
