# FreeOsciWaveShaper
### *A free waveshaper VST for making oscilloscope music on a budget!*

----------

**What is oscilloscope music?**  
Oscilloscope music is, in a nutshell, music designed for visualization. Using an oscilloscope, usually an old analog one, you plot the left channel of the audio on the X axis and the right channel on the Y axis. By differing the waveforms, you can get different shapes with different sounds. And with the €34/$39.25 [OsciStudio](https://oscilloscopemusic.com/oscistudio.php) software, which is the only audio software I have paid for, you can bring 3D animations from Blender onto the scope.  
*For an intuitive, fun explanation about oscilloscope music, I highly recommend [SmarterEveryDay's video](https://www.youtube.com/watch?v=4gibcRfp4zA) on the topic.*

**What is FOWS?**  
FOWS is a VST3 waveshaper that uses a set of functions to turn one waveform into another without destroying what it sounds like. Internally, the stereo input is mixed down into a mono signal and fed into two functions that plot the signal into a point on one channel. If this is done for both channels with different functions, you get [visuals](https://youtu.be/YyP5Ri4NXtg "Demonstration of an early-prototype algorithm")!

**Where did FOWS begin?**  
I'm an amateur at making music, and I'm not nearly invested enough into it to spend a ton of money, so it's very frustrating to find so many expensive VST plugins and effects and almost no free ones that do what I want. Eventually, I decided to use my programming skills and start writing my own VSTs when I couldn't find what I wanted. This is my first VST, and I hope it won't be the last.

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

Note: The free DAW [lmms.io](LMMS) doesn't currently support VST3, so you'll have to use it in [https://kushview.net/element/](Element)
