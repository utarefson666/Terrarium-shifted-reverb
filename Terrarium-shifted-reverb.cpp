#include "daisy_petal.h"
#include "daisysp.h"
#include "terrarium.h"
#include <math.h>
#include <string>

using namespace daisy;
using namespace daisysp;
using namespace terrarium;

// Declare a local daisy_petal for hardware access
DaisyPetal hw;

Parameter vtime, vfreq, vsend, trans, wet, freqParam;
bool      bypass;
ReverbSc  verb;
//PitchShifter ps;

static ATone      flt;

daisysp::PitchShifter DSY_SDRAM_BSS ps;

//daisysp::ATone DSY_SDRAM_BSS flt;

//daisysp::ReverbSc DSY_SDRAM_BSS verb;

Led led1;

// This runs at a fixed rate, to prepare audio samples
void callback(float *in, float *out, size_t size)
{
    float dryl, dryr, wetl, wetr, sendl, sendr, shiftedl, shiftedr;
    float hfreq, outputl, outputr;


  


    hw.ProcessDigitalControls();
    
    verb.SetLpFreq(vfreq.Process());
    vsend.Process(); // Process Send to use later

    

    hfreq = freqParam.Process();
    flt.SetFreq(hfreq);    


    //bypass = hw.switches[DaisyPetal::SW_5].Pressed();
    if(hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
     {
        bypass = !bypass;
        led1.Set(bypass ? 0.0f : 1.0f);
     }

    if(hw.switches[Terrarium::FOOTSWITCH_2].Pressed()) // secondary feedback footswitch
	   { verb.SetFeedback(0.999f);
       }
	else
    {
	    verb.SetFeedback(vtime.Process());
       }

     led1.Update();





    float transpose = floor((trans.Process() * 100.0f) / 7.0f);
    
    if (hw.switches[Terrarium::SWITCH_1].Pressed()) {
        ps.SetTransposition(12.0f + transpose);
    } else {
        ps.SetTransposition(transpose);
    }

    



    for(size_t i = 0; i < size; i += 2)
    {
        dryl  = in[i];
        dryr  = in[i + 1];
        sendl = dryl * vsend.Value();
        sendr = dryr * vsend.Value();

        verb.Process(sendl, sendr, &wetl, &wetr);

        outputl = flt.Process(wetl);
        outputr = flt.Process(wetr);

        shiftedl = ps.Process(outputl);
        shiftedr = ps.Process(outputr);
        
        if(bypass)
        {
            out[i]     = in[i];     // left
            out[i + 1] = in[i + 1]; // right
        }
        else
        {
            out[i]     = dryl + outputl + shiftedl * wet.Process();
            out[i + 1] = dryr + outputr + shiftedr * wet.Process();
        }
    }
}

int main(void)
{
    float samplerate;

    hw.Init();
    samplerate = hw.AudioSampleRate();
    
    led1.Init(hw.seed.GetPin(Terrarium::LED_1),false);

    bypass = true;

    vtime.Init(hw.knob[Terrarium::KNOB_1], 0.6f, 0.999f, Parameter::LOGARITHMIC);
    vfreq.Init(hw.knob[Terrarium::KNOB_2], 500.0f, 20000.0f, Parameter::LOGARITHMIC);
    vsend.Init(hw.knob[Terrarium::KNOB_3], 0.0f, 1.0f, Parameter::LINEAR);
    verb.Init(samplerate);


    freqParam.Init(hw.knob[Terrarium::KNOB_6], 80.0f, 20000.0f, Parameter::LOGARITHMIC);
    flt.Init(samplerate);





    wet.Init(hw.knob[Terrarium::KNOB_4], 0.01f, 0.999f, Parameter::LOGARITHMIC);
    trans.Init(hw.knob[Terrarium::KNOB_5], 0.0001f, 0.91f, Parameter::LINEAR);
    ps.Init(samplerate);


    hw.StartAdc();
    hw.StartAudio(callback);
    while(1)
    {
        // Do Stuff InfInitely Here
        System::Delay(10);
        //hw.ClearLeds();
        //hw.SetFootswitchLed(hw.FOOTSWITCH_LED_1, bypass ? 0.0f : 1.0f);
        //hw.UpdateLeds();
    }
}