#pragma once

#include <cmath>

#include <vector>

class Wavetable {
public:

	// Default Constructor
	Wavetable() {}	
	
	// Default Destructor
	~Wavetable() {wavetable_.clear();}	
	
	// Additional Constructor with arguments
	Wavetable(float sampleRate, std::vector<float>& wavetable , float phase ); 		
	
	// Sets up important variables used in the classes functions
	void setup(float sampleRate, std::vector<float>& wavetable, float phase); 		
	
	// Sets the frequency for the <Wavetable*> thats called
	void setFrequency(float f);
	
	// Gets the frequency for the <Wavetable*> thats called
	float getFrequency();
	
	// Takes wavetable from the Constructor reads through the buffer
	float process();
	
	// Takes wavetable from the Constructor and applies linearInterpolation
	float linearInterpolation() ;

private:
	// Buffer holding the wavetable
	std::vector<float> wavetable_;	
	
	// 1 divided by the audio sample rate	
	float inverseSampleRate_;	
	
	float frequency_;
	
	float readPointer_;	
	
	float phaseIncrement_;
	
	float phase_;
};