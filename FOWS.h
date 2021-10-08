#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kGainIn = 1,
  kFade = 2,
  kInverted,
  kSwapped,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class FOWS final : public Plugin
{
private:
  void ReadFunction(char* name);
  int FindPlace(double input);
  double TransformLeft(double input);
  double TransformRight(double input);
  double ProcessSample(double input, int channel, bool swap);
  double ProcessSample(double input, int channel, bool swap, double fade);
  double ProcessSample(double input1, double input2, int channel, bool swap);
  double ProcessSample(double input1, double input2, int channel, bool swap, double fade);
public:
  FOWS(const InstanceInfo& info);
  float func[100][3] = { {-1.0f,  1.0f,  0.0f},
                         {-0.5f,  0.0f,  1.0f},
                         { 0.0f, -1.0f,  0.0f},
                         { 0.5f,  0.0f, -1.0f},
                         { 1.0f,  1.0f,  0.0f} };
  int funcLen;
  WDL_String currentFunc;
  const char* defaultFunc = "diamond";

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
