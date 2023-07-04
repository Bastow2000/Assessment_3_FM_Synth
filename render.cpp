#include <Bela.h>

#include <libraries/Gui/Gui.h>

#include <libraries/GuiController/GuiController.h>

#include <libraries/math_neon/math_neon.h>

#include <libraries/Scope/Scope.h>

#include <cmath>

#include <vector>

#include <sstream>

#include <iostream>

#include "Debouncer.h"

#include "ADSR.h"

#include "Filter.h"

#include "Ramp.h"

// Max 31 sliders 

// Used for generating wavetype
#include "WavetableGenerate.h" 

// Used for reading wavetable
#include "WavetableSetup.h"	

// Constants that define the program behaviour
const unsigned int kWavetableSize = 256;

//Number of partials for additive synthesis
const unsigned int kNumOscillators = 8;

//Number of modulators for additive fm synthesis
const unsigned int kNumModulators = 4;

//Number of modulators for additive fm synthesis
const unsigned int kNumFeedback = 4;


// Declaring global variables in the main file
float phase = 0;

float phaseIncrement = 0;

// Digital Io 
const int kButtonPin = 1;

const int kLedPin = 0;

const int kLedPin2 = 2;

const int kLedPin3 = 3;

const int kLedPin4 = 4;

const int kLedPin5 = 5;

// Declaring an array so That ledPins can be called more efficiently 
std::vector<int> kLedPins = {kLedPin2, kLedPin3, kLedPin4, kLedPin5};

// To check if button is pressed
int gIsPlaying = 1;

// Debounce class taken from week 7 ADSR lecture
Debouncer gDebouncer;

// ADSR class taken from week 7 ADSR lecture 
ADSR gAmplitudeADSR, gFilterADSR;

// ADSR class taken from week 7 ADSR lecture 
Filter gFilter;

// Browser-based GUI to setup Gui
Gui * gGui;

// Browser-based GUI to control Gui
GuiController * gGuiController;

// Browser-based oscilloscope
Scope gScope;

// Carrier oscillators for additive fm synth
std::vector <Wavetable*> gOscillators;

// Modulator oscillators for additive fm synth
std::vector <std::vector <Wavetable*>> gModulators(4);

// Feedback vectors with the size of the modulators initialised at 0.375
std::vector <std::vector <float>> gFeedback(4, std::vector <float> (gModulators.size(), 0.375f));

// Feedback vectors with the size of the modulators initialised at 0.375
std::vector <std::vector <float>> gModFeedback(4, std::vector <float> (gModulators.size(), 0.375f));

// Amplitudes for the additive fm synth
std::vector <float> gAmplitudes;

bool setup(BelaContext *context, void *userData)
{
	// Initialise the filter
	gFilter.setSampleRate(context->audioSampleRate);

	// Initialise the ADSR objects
	gFilterADSR.setSampleRate(context->audioSampleRate);
		
	gAmplitudeADSR.setSampleRate(context->audioSampleRate);
	
	// Initialise the debouncer with 50ms interval
	gDebouncer.setup(context->audioSampleRate, .05);
	
	// Pointer to GuiController 
	gGuiController = new GuiController();

	// Pointer to Gui
	gGui = new Gui();
	
	// Declares a vector called wavetable
	std::vector<float> wavetable;
 
	//Resizes wavetable with the value 256
	wavetable.resize(kWavetableSize);
	
	// Creates a pointer to the GenerateWavetable class, initialising variables to be used in that class
	GenerateWavetable* generation = new GenerateWavetable(context->audioSampleRate, wavetable, phase);
	
	// Uses the earlier defined wavetable and assigns a wave type (Sine,Triangle,Square,Saw)
    // using the prompt_Harmonics function: prompt_Harmonics(waveNumber) 1-4
	wavetable =  generation->prompt_Harmonics(1);

	// Populates gOscillators with the earlier defined wavetable, repeats this for all additive partials
	for(unsigned int n = 0; n < kNumOscillators; n++) 
	{
		Wavetable* oscillators = new Wavetable(context->audioSampleRate, wavetable, phase); 
		gOscillators.push_back(oscillators); 
	}

	// Creates a loop that assigns <Wavetable*> to each modulator  
	// assigns the wavetype to the wavetable initialised inside <Wavetable*>
	for(int n = 0; n < kNumModulators; n++)
	{
    	unsigned int waveNumber = n < 1 ? n+1 : (n < 2 ? n+2 : (n < 3 ? n+3 : n+4));
    	gModulators[n] = generation->prompt_Modulator(gModulators[n], waveNumber);
	}
	
	// Clears myGenerateWavetable pointer
	delete generation;
	
	wavetable.clear();
	
	// Resize gAmplitudes
	gAmplitudes.resize(kNumOscillators+kNumModulators+kNumFeedback);
	
	// Set up the GUI
	gGui->setup(context->projectName);
	
	gGuiController->setup(gGui, "Wavetable Controller");	
	
	// Arguments: name, default value, minimum, maximum, increment
	
	// Controls frequency for carrier oscillators
	gGuiController->addSlider("MIDI note", 60, 36, 96, 1);
	
	// Controls overall Amplitude
	gGuiController->addSlider("Amplitude (dB)", -20, -40, 0, 0);

	// Adds level slider attachment for carrier oscillators
	for(unsigned int n = 0; n < kNumOscillators; n++) 
	{
    	std::string name = "Harmonic " + std::to_string(n + 1); 
    	gGuiController->addSlider(name, -40 , -60, 0, 0);
	}
	
	// Adds frequency slider attachment for modulator oscillators
	for(int n = 0; n < kNumModulators; n++) 
	{
    	gGuiController->addSlider("FrequencyMod " + std::to_string(n+1), 48 - 2*n, 16, 76, 1);
    }
    
    // Adds slider to switch between number of modulators
    gGuiController -> addSlider("ModulatorSwitch ", 1, 1, 4, 1);
    
    // Applies Modulation Depth
	gGuiController -> addSlider("ModulatorDepth ", 0.2, 0.07, 1, 0.01);
	
	// Applies extra Amplitude for modulators
    gGuiController -> addSlider("extraAmplitude ", 0.2, 0.1, 1, 0.01);
    
    //Adds slider to switch between feedbackAlgorithms
    gGuiController -> addSlider("FeedbackAlgorithm ", 0, 0, 4, 1);
    
    // Adds attack Amplitude taken from week 7 lecture
    gGuiController->addSlider("Amplitude Attack time", 0.01, 0.001, 0.1, 0);
    
    // Adds decay Amplitude taken from week 7 lecture
	gGuiController->addSlider("Amplitude Decay time", 0.05, 0.01, 0.3, 0);
	
	// Adds sustain Amplitude taken from week 7 lecture
	gGuiController->addSlider("Amplitude Sustain level", 0.7, 0, 1, 0);
	
	// Adds release Amplitude taken from week 7 lecture
	gGuiController->addSlider("Amplitude Release time", 0.2, 0.001, 2, 0);
	
	// Adds Filter base frequency taken from week 7 lecture
	gGuiController->addSlider("Filter base frequency", 200, 50, 1000, 0);
	
	// Adds filter Q taken from week 7 lecture
	gGuiController->addSlider("Filter Q", 4, 0.5, 10, 0);
    
    // Sets up digital Io
    // Start note, Stop note
    pinMode(context, 0, kButtonPin, INPUT); 
    
    // Note Duration on || off
	pinMode(context, 0, kLedPin, OUTPUT);
	
	// Modulator 1 on || off
	pinMode(context, 0, kLedPin2, OUTPUT);
	
	// Modulator 2 on || off
	pinMode(context, 0, kLedPin3, OUTPUT);
	
	// Modulator 3 on || off
	pinMode(context, 0, kLedPin4, OUTPUT);
	
	// Modulator 4 on || off
	pinMode(context, 0, kLedPin5, OUTPUT);
	
    
	//Sets up Scope
	gScope.setup(1, context->audioSampleRate);
	
	return true;
}

void render(BelaContext *context, void *userData)
{
	
	
	// Arguments: name, default value, minimum, maximum, increment
	
	// ("MIDI note", 60, 36, 96, 1) First Slider
	float midiNote = gGuiController->getSliderValue(0);	
	
	// ("Amplitude (dB)", -20, -40, 0, 0) Second Slider
	float amplitudeDB = gGuiController->getSliderValue(1);	
	
	// ("ModulatorSwitch ", 1, 0, 4, 1) Fiftheenth Slider
	unsigned int modulatorLimitNumber = gGuiController -> getSliderValue(6 + kNumOscillators);
	
	// ("ModDepth ", 0.2, 0.07, 1, 0.01) Sixteenth Slider
	float modDepth = gGuiController -> getSliderValue(7 + kNumOscillators);
	
	// ("ExtraAmp ", 0.2, 0.1, 1, 0.01) Seventeenth Slider
	float extraAmp = gGuiController -> getSliderValue(8 + kNumOscillators);
	
	// ("FeedbackAlgorithm ", 0, 0, 4, 1) Eighteenth Slider
	unsigned int feedbackAlgorithm = gGuiController -> getSliderValue(9 + kNumOscillators);
	
	// ("Amplitude Attack time", 0.01, 0.001, 0.1, 0) Nineteenth Slider
	float ampAttack = gGuiController -> getSliderValue(10 + kNumOscillators);
	
	// ("Amplitude Decay time", 0.05, 0.01, 0.3, 0) Twentieth Slider
	float ampDecay = gGuiController -> getSliderValue(11 + kNumOscillators);
	
	// ("Amplitude Sustain level", 0.7, 0, 1, 0) Twenty First Slider
	float ampRelease = gGuiController -> getSliderValue(12 + kNumOscillators);
	
	// ("Amplitude Release time", 0.2, 0.001, 2, 0) Twenty Second Slider
	float ampSustain = gGuiController -> getSliderValue(13 + kNumOscillators);
	
	// ("Filter base frequency", 200, 50, 1000, 0) Twenty Third Slider
	float baseFrequencyFilter = gGuiController -> getSliderValue(14 + kNumOscillators);
	
	// ("Filter Q", 4, 0.5, 10, 0) Twenty Fourth Slider
	float filterQ = gGuiController -> getSliderValue(15 + kNumOscillators);
	 
	// Variable for setting frequency slider 
	float frequency = 440.0 * powf(2.0, (midiNote - 69.0) / 12.0);
	
	// Variable for global amplitude
	float amplitude = powf(10.0, amplitudeDB / 20); 

	const float nyquistRate = context->audioSampleRate / 2.0;

	// Sets the partials frequency and amplitude sliders
	for(unsigned int s = 0; s < gOscillators.size(); s++) 
	{
    	gOscillators[s]->setFrequency((frequency * modDepth)*(s + 1));
    	
    	// ("Harmonic", -40 , -60, 0, 0) Third - Tenth Slider
    	gAmplitudes[s] = (frequency >= nyquistRate || gGuiController->getSliderValue(2 + s) <= -60) ? 0 : powf(10.0, gGuiController->getSliderValue(2 + s) / 20);
	}
	
	// Sets the modulators frequency sliders
	for(int n = 0; n < kNumModulators; n++) 
	{
		// ("FrequencyMod ", 48, 16, 76, 1) Eleventh - Fourteenth Slider
    	float midiNoteMod = gGuiController->getSliderValue(2 + kNumOscillators + n);
    	
        float frequencyMod = 440.0 * powf(2.0, (midiNoteMod - 69.0) / 12.0);
        
        for(unsigned int s = 0; s < gModulators[n].size(); s++) 
        {
        	float modFrequency = (frequencyMod * modDepth)*(s + 1);
            gModulators[n][s]->setFrequency(modFrequency);
           
        }
    }
	
	
	for (unsigned int n = 0; n < context -> audioFrames; n++) 
	{
		// Sets adsr components from week 7 lecture
		
		// Sets filter components to the analog inputs
		
		// Attack
		float input1 = analogRead(context, n/2, 0);
		
		// Decay
		float input2 = analogRead(context, n/2, 1);
		
		// Sustain
		float input3 = analogRead(context, n/2, 2);
		
		// Release
		float input4 = analogRead(context, n/2, 3);
		
		// Input1
		float attack = map(input1, 0, 3.3/4.096, 0.001, 0.1);
		
		// Input2
		float decay = map(input2, 0, 3.3/4.096, 0.01, 0.3);
		
		// Input3
		float sustain = map(input3, 0, 3.3/4.096, 0, 1);
		
		// Input4
		float release = map(input4, 0, 3.3/4.096, 0.001, 2);
		
		gAmplitudeADSR.setAttackTime(ampAttack);
		
		gAmplitudeADSR.setDecayTime(ampDecay);
		
		gAmplitudeADSR.setSustainLevel(ampSustain);
		
		gAmplitudeADSR.setReleaseTime(ampRelease);
		
		gFilterADSR.setAttackTime(attack);
		
		gFilterADSR.setDecayTime(decay);
		
		gFilterADSR.setSustainLevel(sustain);
		
		gFilterADSR.setReleaseTime(release);
		
		gFilter.setQ(filterQ);
				
		// Sets up Io button
		int status = digitalRead(context, n, kButtonPin);

		float gLastButtonStatus = 0;
		
		// Uses debouncer class from week 7 lecture
		
		// If button is pressed play sound, if not don't
		if(gDebouncer.process(status))
		{
			gIsPlaying = 1;
			
		} 
		else 
		{
			gIsPlaying = 0;
		}
		gLastButtonStatus = status;
		
		if(gDebouncer.fallingEdge()) 
		{
			gAmplitudeADSR.trigger();
			
    		gFilterADSR.trigger();
    	}    	
    	if(gDebouncer.risingEdge()) 
    	{
    		gAmplitudeADSR.trigger();
    		
    		gFilterADSR.release();
    	}
		
		// If audio is playing led 1 on else off
		if (gIsPlaying == 1) 
		{
    		digitalWriteOnce(context, n, kLedPin, HIGH);
		} 
		else 
		{
			digitalWriteOnce(context, n, kLedPin, LOW);
		}
		
		if( gIsPlaying == 1)
		{
		
			// Initalises Variables for later use
			float out = 0;
		
    		float currentFeedback = 1;
    	
    		float modulatorOutput = 0;
    	
			float carrierOutput = 0;
		
    		for (unsigned int mlt = 0; mlt < modulatorLimitNumber; mlt++) 
    		{
    			for (unsigned int mSize = 0; mSize < gModulators[mlt].size(); mSize++) 
    			{	
    				float amplitudeADSR = gAmplitudeADSR.process();
        			modulatorOutput += ((gModulators[mlt][mSize] -> process() * gModFeedback[mlt][mSize])* amplitudeADSR) * extraAmp;
        		
        		
        			// If modulator[i] is playing led [i] is on else off
        			for (int i = 0; i < kNumModulators; i++) 
        			{
    					if (gModulators[mlt][mSize] == gModulators[i][mSize]) 
    					{
        					digitalWriteOnce(context, n, kLedPins[i], HIGH);
						} 
						else 
						{
        					digitalWriteOnce(context, n, kLedPins[i], LOW);
						 }
					}

					// Creates a feedback loop around the modulators
        			gModFeedback[0][mSize] = gModulators[mlt][mSize]->process();
        		
        			gModFeedback[1][mSize] = gModFeedback[0][mSize];
        		
        			gModFeedback[2][mSize] = gModFeedback[1][mSize];
        		
        			gModFeedback[3][mSize] = gModFeedback[2][mSize];
        		
        			for (unsigned int cSize = 0; cSize < gOscillators.size(); cSize++) 
        			{
        				carrierOutput += gOscillators[cSize] -> process() * amplitudeADSR;
        			
        				out += ((gAmplitudes[cSize] * carrierOutput) * modulatorOutput) * (currentFeedback);
        
        				switch (feedbackAlgorithm) 
        				{
        				case 0:
        				// No feedback
        					currentFeedback = 1;
        				
        					break;
        				case 1: 
        				// Algorithm 1
            				gFeedback[0][mSize] = modulatorOutput ;
            			
            				gFeedback[1][mSize] = gFeedback[0][mSize];
            				
            				gFeedback[2][mSize] = gFeedback[1][mSize];
            			
            				gFeedback[3][mSize] = gFeedback[2][mSize];
            			
            				currentFeedback = tanf_neon(gFeedback[3][mSize]) ;
            				break;
        				case 2: 
        				// Algorithm 2
            				gFeedback[0][mSize] = modulatorOutput;
            			
            				gFeedback[1][mSize] = gFeedback[0][mSize] - modulatorOutput;
            			
            				gFeedback[2][mSize] = gFeedback[1][mSize] + gFeedback[0][mSize];
            			
            				gFeedback[3][mSize] = gFeedback[2][mSize] - modulatorOutput;
            			
            				currentFeedback = tanf_neon(gFeedback[3][mSize] + modulatorOutput);
            			
            				break;
        				case 3: 
        				// Algorithm 3
            				gFeedback[0][mSize] = modulatorOutput + gModFeedback[1][mSize];
            			
            				gFeedback[1][mSize] = gFeedback[0][mSize] -modulatorOutput;
            			
            				gFeedback[2][mSize] = gFeedback[1][mSize];
            			
            				gFeedback[3][mSize] = gFeedback[2][mSize];
            			
            				currentFeedback = tanf_neon(gFeedback[mlt][mSize]);
            			
            				break;
        				case 4:
        				// Algorithm 4
            				gFeedback[0][mSize] = modulatorOutput + gFeedback[3][mSize];
            			
            				gFeedback[1][mSize] = gFeedback[0][mSize] -modulatorOutput;
            			
            				gFeedback[2][mSize] = gFeedback[1][mSize];
            			
            				gFeedback[3][mSize] = gFeedback[2][mSize];
            			
            				currentFeedback = tanf_neon(gFeedback[mlt][mSize]);
            			
            				break;
        				}

        			}
    			}
    		}
    	
			float filterControl = gFilterADSR.process();
			
        	gFilter.setFrequency(baseFrequencyFilter * filterControl);

    		// Scale global amplitude
			out *= amplitude;
			
			out +=  gFilter.process(out);
		
			for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) 
			{
				// Write the sample to every audio output channel
    			audioWrite(context, n, channel, out);
    			
    			gScope.log(out,filterControl);
    		}
		}
		else 
		{
			float out = 0; 
			
			for(unsigned int channel = 0; channel < context->audioOutChannels; channel++) 
			{
				// Write the sample to every audio output channel
    			audioWrite(context, n, channel, out);
    			
    			gScope.log(out);
    		}
    	}
	}
}

void cleanup(BelaContext *context, void *userData)
{
	delete gGuiController;
	
	delete gGui;
	
	// Delete the memory allocated for the Wavetable objects
	for (unsigned int s = 0; s < gOscillators.size()-1; s++)
	{
		delete gOscillators[s];
	}
	
	// Clear the vector to release any remaining memory
	gOscillators.clear();
	
	gModulators.clear();
	
	gFeedback.clear();
	
	gModFeedback.clear();
	
	gAmplitudes.clear();
}