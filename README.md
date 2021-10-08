# FreeOsciWaveShaper
A free, open source waveshaper VST for making oscilloscope music on a budget!

----------

If you want to try the plugin, just move the VST from vst3/ to your VST3 folder, open it in your DAW of choice, and select a waveshaper shape (.shp) file from test/ to begin!

----------

SHP Specifications (not final):
- Formatted as ASCII text files
- SHP files contain up to 100 groups of three decimal numbers (up to 300 individual numbers) between -1 and 1, separated by spaces or newlines.
- In each group, the first number represents the input sample, the second represents the X coordinate, and the third represents the Y coordinate.
- Samples that fall between 2 groups will be linearly interpolated between the coordinates.
- Inputs MUST be in ascending order as a binary search is used to find the groups above and below a sample, and a binary search expects a sorted list.
- Inputs outside the bounds of the list are not guaranteed to work, so make sure your first and last values are -1 and 1 respectively!  

Possible future additions:
- Per-point interpolation modes
- True instant jumps between points
- More samples

Note: The free DAW [LMMS](lmms.io) doesn't currently support VST3, so you'll have to use it in [Element](https://kushview.net/element/)
