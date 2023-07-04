#pragma once

class ExponentialSegment {

public:
	// Constructor
	ExponentialSegment();
	
	// Constructor specifying a sample rate
	ExponentialSegment(float sampleRate);
	
	// Set the sample rate, used for all calculations
	void setSampleRate(float rate);
	
	// Jump to a value
	void setValue(float value);
	
	// Ramp to a value over a period of time, with a given percent overshoot
	void rampTo(float value, float time, float overshootRatio = 1.001);
	
	// Generate and return the next ramp output
	float process();
	
	// Return whether the ramp is finished
	bool finished();
	
	// Destructor
	~ExponentialSegment();

private:
	// State variables, not accessible to the outside world
	double sampleRate_;
	double currentValue_;
	double targetValue_;
	double asymptoteValue_;
	double expValue_;
	double multiplier_;
};