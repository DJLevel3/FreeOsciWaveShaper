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

If you want to try the plugin, download the repository, open FOWS.sln in Visual Studio 2019, and compile. Everything should work, at least as much as it's programmed to. However, it is still a prototype and not everyting that I want to have is implemented yet.