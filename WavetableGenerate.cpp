#include "WavetableGenerate.h"

// Constructor taking arguments for sample rate, wavetable data and phase
GenerateWavetable::GenerateWavetable(float sampleRate, std::vector<float>& wavetable, float phase)
{
	setup(sampleRate, wavetable, phase);
} 

// Setting up wavetable data, phase and sampleRate to be used in functions & their calls
void GenerateWavetable::setup(float sampleRate, std::vector<float>& wavetable, float phase)
{
	// It's faster to multiply than to divide on most platforms, so we save the inverse
	// of the sample rate for use in the phase calculation later
	inverseSampleRate_ = 1.0f / sampleRate;
	
	// Assign parameters 
	sampleRate_ = sampleRate;
	
	wavetable_ = wavetable;
	
	phase_ = phase;
	
	// Calculates the phase increment for a waveform table of a given size and sample rate
	phaseIncrement_ = (wavetable_.size() * inverseSampleRate_); 
	
}

// Calculates poly_blep (Martin Finke,(no date))
float GenerateWavetable::poly_blep(float t /*phase*/, float dt /*phaseHalfIncrement*/) 
{
	if (t < dt) 
	{
    	t /= dt;
    	return t + t - t * t - 1.0f;
    } 
	else if (t > 1.0f - dt) 
	{
		t = (t - 1.0f) / dt;
		return t * t + t + t + 1.0f;
	} 
	else 
	{
	    return 0.0f;
	}
}

float GenerateWavetable::prompt_WaveType(unsigned int waveNumber, float n)
{
	// (Allnor, R., 2009)
	// DC(n) = alpha * DC(n-1) + (1 - alpha) * x(n)
	
	// Alpha is put into the equation to smoothen the jittering from the dcCorrection
	float alpha = 0.99f;
	
	float dcCorrection = 0.0f;
	
	// Calculate the DC Correction apply smoothener (alpha) to reduce jittering 
	for(unsigned int s = 0; s < wavetable_.size(); s++) 
	{
    	dcCorrection = alpha * dcCorrection + (1.0f - alpha) * wavetable_[s];
	}

	// Subtract the DC Correction from each sample in wavetable_
	for (unsigned int s = 0; s < wavetable_.size(); s++) 
	{
	    wavetable_[s] -= dcCorrection;
	}
	
	// Set values for poly_blep
	float dt = phaseIncrement_ / wavetable_.size();
	
    float t = phase_;
    
    // Initalise value for further use
    float value = 0.0f;
    
    // Use argument waveNumber to change the type of the wave 
    switch (waveNumber)
    {
        case 1:
        //Sine
            value = (sinf(2.0f * M_PI * (float)n / (float)wavetable_.size() + phase_));
            break;
        case 2:	
        //Triangle
            value = (2.0f * fabs(-1.0f + 2.0f * (float)n / (float)(wavetable_.size() - 1))-0.5f)+ phase_;
            break;
        case 3: 
        //Square
            value = (sinf(2.0f * M_PI * (float)n / (float)wavetable_.size() + phase_));
            value = value >= 0.0f ? 1.0f : -1.0f;
            break;
        case 4: 
        //Sawtooth
            value = (-1.0f + 2.0f * (float)n / (float)(wavetable_.size() - 1)+ phase_);
            break;
    }
    // Return the wave type and Subtract poly_blep
    return  value -= poly_blep(t, dt);
}

// Uses the wavetable_ defined from the constuctor to assign a wave type to wavetable_ then adds the 
// wavetable_ to <Wavetable*> and sends the <Wavetable*> back to render to be used 
std::vector<Wavetable*> GenerateWavetable::prompt_Modulator(std::vector<Wavetable*> gOscillators, unsigned int waveNumber)
{
	for(unsigned int n = 0; n < wavetable_.size(); n++) 
    {
        wavetable_[n]= prompt_WaveType(waveNumber, n);
    }
    Wavetable* oscillators = new Wavetable(sampleRate_, wavetable_, phase_); 
    
	gOscillators.push_back(oscillators); 

	// Delete memory without -1 there is a bus error
    for (unsigned int i = 0; i < gOscillators.size()-1; i++)
    {
        delete gOscillators[i];
    }
	
    return gOscillators;
}

// Uses the wavetable_ defined from the constuctor to assign a wave type to wavetable_ then returns value
std::vector<float> GenerateWavetable::prompt_Harmonics(unsigned int waveNumber)
{
	  for(unsigned int n = 0; n < wavetable_.size(); n++) 
    {
        wavetable_[n]= prompt_WaveType(waveNumber, n);
    }
    return wavetable_;
}




