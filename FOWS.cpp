#include "FOWS.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <fstream>

void FOWS::ReadFunction(char* name) {
  float ref = 0;
  char* input = (char*)&ref;
  std::ifstream file(name, std::ios::binary);
  int i = 0;
  for (i = 0; i < 100 && !file.eof(); i ++) {
    for (int j = 0; j < 3; j++) {
      file.read(input, 4);
      func[i][j] = ref;
    }
  }
  funcLen = i;
  for (int k = i; k < 100; k++) {
    for (int j = 0; j < 3; j++) {
      func[k][j] = 0.f;
    }
  }
  file.close();
}

int FOWS::FindPlace(double input) {
  int low = 0, high = funcLen, diff = high - low;
  while (diff > 1) {
    if (input >= func[low + (diff >> 1)][0]) low = low + (diff >> 1);
    else high = low + (diff >> 1);
    diff = high - low;
  }
  return low;
}

double FOWS::TransformLeft(double input) {
  int index1 = FindPlace(input), index2 = index1 + 1;
  double output = input - func[index1][0];
  output /= (double)func[index2][0] - func[index1][0];
  output *= (double)func[index2][1] - func[index1][1];
  output += func[index1][1];
  return output;
}

double FOWS::TransformRight(double input) {
  int index1 = FindPlace(input), index2 = index1 + 1;
  double output = input - func[index1][0];
  output /= (double)func[index2][0] - func[index1][0];
  output *= (double)func[index2][2] - func[index1][2];
  output += func[index1][2];
  return output;
}

double FOWS::ProcessSample(double input, int channel, bool swap) {
  double store = input;
  if (!channel) {
    store = swap ? TransformLeft(store) : TransformRight(store);
  }
  else {
    store = (!swap) ? TransformLeft(store) : TransformRight(store);
  }
  return store;
}
double FOWS::ProcessSample(double input, int channel, bool swap, double fade) {
  double store = ProcessSample(input, channel, swap);
  store = (fade * store) + (input * (1 - store));
  return store;
}

FOWS::FOWS(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  currentFunc.Set("Diamond");

  GetParam(kGain)->InitDouble("Output Gain", 0., 0., 100.0, 0.01, "%");
  GetParam(kFade)->InitDouble("Fade", 100.0, 0., 100.0, 0.01, "%");
  GetParam(kSwapped)->InitBool("Swap L/R", true, "", IParam::kFlagsNone, "", "Swap", "Norm");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);

    const IVStyle style {
      true, // Show label
      true, // Show value
      {
        DEFAULT_BGCOLOR, // Background
        DEFAULT_FGCOLOR, // Foreground
        DEFAULT_PRCOLOR, // Pressed
        COLOR_BLACK, // Frame
        DEFAULT_HLCOLOR, // Highlight
        DEFAULT_SHCOLOR, // Shadow
        COLOR_BLACK, // Extra 1
        DEFAULT_X2COLOR, // Extra 2
        DEFAULT_X3COLOR  // Extra 3
      }, // Colors
      IText(12.f, EAlign::Center) // Label text
    };

    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "FreeOsciWaveShaper", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(50), kGain));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(-50), kFade));
    pGraphics->AttachControl(new IVSlideSwitchControl(b.GetCentredInside(100).SubRectHorizontal(2, 1).GetVShifted(100), kSwapped, "Swap L/R",
      style.WithValueText(IText(12.f, DEFAULT_TEXT_FGCOLOR, "Roboto-Regular", EAlign::Center)), true, EDirection::Vertical));

    auto promptFunction = [&](IControl* pCaller) {
      WDL_String fileName;
      WDL_String path;
      pGraphics->PromptForFile(fileName, path, EFileAction::Open, "wsf");

      if (fileName.GetLength()) {
        ReadFunction(fileName.Get());
      }
    };

    //pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(100).SubRectVertical(4, 1).GetVShifted(200), SplashClickActionFunc, "Choose Function...", style))->SetAnimationEndActionFunction(promptFunction);
  };
#endif
}

#if IPLUG_DSP
void FOWS::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 100.;
  const double fade = GetParam(kFade)->Value() / 100.;
  const bool swap = GetParam(kSwapped)->Bool();
  const int nChans = NOutChansConnected();
  const int nInputs = NInChansConnected();
  bool idle = false;

  for (int s = 0; s < nFrames; s++) {
    if (nInputs == 1 && nChans == 2 && nFrames > 0) {
      for (int c = 0; c < 2; c++) {
        outputs[c][s] = gain * ProcessSample(inputs[0][s], c, swap, fade);
      }
    }
    else if (nInputs == 2 && nChans == 2 && nFrames > 0) {
      for (int c = 0; c < 2; c++) {
        outputs[c][s] = gain * ProcessSample((inputs[0][s] + inputs[1][s])/2, c, swap, fade);
      }
    }
  }
}
#endif
