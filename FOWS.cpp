#include "FOWS.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include <fstream>
#include <cstdlib>

void FOWS::ReadFunction(char* name) {
  float ref = 0;
  std::ifstream file(name);
  int i = 0;
  for (i = 0; i < 100 && !file.eof(); i ++) {
    for (int j = 0; j < 3; j++) {
      file >> ref;
      func[i][j] = ref;
      ref = 0;
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
  int index1 = FindPlace(input);
  int index2 = index1 + 1;
  double output = input - func[index1][0];
  output /= (double)func[index2][0] - func[index1][0];
  output *= (double)func[index2][1] - func[index1][1];
  output += func[index1][1];
  return output;
}

double FOWS::TransformRight(double input) {
  int index1 = FindPlace(input);
  int index2 = index1 + 1;
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
  store = (fade * store) + ((1 - fade) * input);
  return store;
}
double FOWS::ProcessSample(double input1, double input2, int channel, bool swap) {
  return ProcessSample((input1 + input2) / 2, channel, swap);
}
double FOWS::ProcessSample(double input1, double input2, int channel, bool swap, double fade) {
  double store = ProcessSample((input1 + input2)/2, channel, swap);
  store = (fade * store) + ((1 - fade) * ((channel == 0) ? input1 : input2));
  return store;
}

FOWS::FOWS(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  for (int i = 1; i < 100 && func[i][0] > func[i - 1][0]; i++) funcLen = i;
  currentFunc.Set(defaultFunc);

  GetParam(kGain)->InitDouble("Output Gain", 100., 0., 100.0, 0.01, "%");
  GetParam(kGainIn)->InitDouble("Input Gain", 100., 0., 1000.0, 0.1, "%");
  GetParam(kFade)->InitDouble("Fade", 100.0, 0., 100.0, 0.01, "%");
  GetParam(kInverted)->InitBool("Flip Input", true, "", IParam::kFlagsNone, "", "Invert", "Norm");
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
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(100), kGain));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(0), kFade));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100).GetHShifted(-100), kGainIn));
    pGraphics->AttachControl(new IVSlideSwitchControl(b.GetCentredInside(100).SubRectHorizontal(2, 0).GetVShifted(100), kInverted, "Invert",
      style.WithValueText(IText(12.f, DEFAULT_TEXT_FGCOLOR, "Roboto-Regular", EAlign::Center)), true, EDirection::Vertical));
    pGraphics->AttachControl(new IVSlideSwitchControl(b.GetCentredInside(100).SubRectHorizontal(2, 1).GetVShifted(100), kSwapped, "Swap L/R",
      style.WithValueText(IText(12.f, DEFAULT_TEXT_FGCOLOR, "Roboto-Regular", EAlign::Center)), true, EDirection::Vertical));

    auto promptFunction = [this,pGraphics](IControl* pCaller) {
      WDL_String fileName;
      WDL_String path;
      pGraphics->PromptForFile(fileName, path, EFileAction::Open, "shp");

      if (fileName.GetLength()) {
        ReadFunction(fileName.Get());
      }
    };

    pGraphics->AttachControl(new IVButtonControl(b.GetCentredInside(100).SubRectVertical(4, 1).GetVShifted(200), SplashClickActionFunc, "Choose Function...", style))->SetAnimationEndActionFunction(promptFunction);
  };
#endif
}

#if IPLUG_DSP
void FOWS::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGainIn)->Value() / 100.;
  const double fade = GetParam(kFade)->Value() / 100.;
  const double gainOut = GetParam(kGain)->Value() / 100;
  const bool swap = GetParam(kSwapped)->Bool();
  const bool invert = GetParam(kInverted)->Bool();
  const int nChans = NOutChansConnected();
  const int nInputs = NInChansConnected();

  for (int s = 0; s < nFrames; s++) {
    if (nInputs == 1 && nChans == 2 && nFrames > 0) {
      for (int c = 0; c < 2; c++) {
        outputs[c][s] = gainOut * ProcessSample(inputs[0][s] * (invert ? gain : (-1 * gain)), c, swap, fade);
      }
    }
    else if (nInputs == 2 && nChans == 2 && nFrames > 0) {
      for (int c = 0; c < 2; c++) {
        outputs[c][s] = gainOut* ProcessSample(inputs[0][s] * (invert ? gain : (-1 * gain)), inputs[1][s] * (invert ? gain : (-1 * gain)), c, swap, fade);
      }
    }
  }
}
#endif
