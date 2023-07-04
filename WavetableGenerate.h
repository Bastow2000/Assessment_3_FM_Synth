#pragma once

#include <Bela.h>

#include <vector>

#include <cmath>

#include <libraries/GuiController/GuiController.h>

#include <libraries/Gui/Gui.h>

#include "WavetableSetup.h"

class GenerateWavetable {
public:

	// Default Constructor
	GenerateWavetable(){}
	
	// Default Destructor
	~GenerateWavetable(){wavetable_.clear();}
	
	// Additional Constructor with arguments
	GenerateWavetable(float sampleRate, std::vector<float>& wavetable, float phase); 

	// Sets up important variables used in the classes functions
	void setup(float sampleRate, std::vector<float>& wavetable, float phase); 		

	// When applies makes a wave semi bandlimited 
	float poly_blep(float t, float dt);

	// Creates the wave type and sets it to the wavetable
	float prompt_WaveType(unsigned int waveNumber, float n);
	
	// Used to create modulators 
	std::vector<Wavetable*> prompt_Modulator(std::vector<Wavetable*> gOscillators, unsigned int waveNumber);
	
	// Used to create carriers 
	std::vector<float> prompt_Harmonics(unsigned int waveNumber);

private:
	// Buffer holding the wavetable
	std::vector<float> wavetable_;

	// 1 divided by the audio sample rate
	float inverseSampleRate_;
	
	float sampleRate_;
	
	float phaseIncrement_;
	
	float phase_;
};

