http://basicsynth.com/index.php?page=basic

Signal Generator
The most straightforward method of sound generation in software is to evaluate a periodic function for each sample time. A periodic function is any function that repeats at a constant interval, called the period. Consider the circle in the figure below. Starting at the 3:00 position and then sweeping around the circle counter-clockwise, we make a complete cycle in 2π radians and then the movement repeats. Thus the period is 2π radians. If we plot the points on the circumference over time we produce the waveform as shown below.



For audio signals, the period is the time it takes for the waveform to repeat and is thus the inverse of the frequency. In other words, a frequency of 100Hz repeats every 1/100 second. We need to generate an amplitude value for every sample time, thus the number of samples in one period is equal to the time of the period divided by the time of one sample. Since the time of one sample is the inverse of the sample rate, and the period is the inverse of the frequency, the number of samples is also the sample rate divided by the frequency: ((1/f) / (1/fs)) = (fs/f). Since our period is also equal to 2π radians, the phase increment for one sample time (φ) is 2π divided by the number of samples in one period:

 

where the frequency of the signal is f and the sample rate fs. The amplitude for any given sample is the y value of the phase at that point in time multiplied by the radius of the circle. In other words, the amplitude is the sine of the phase angle and we can also derive the phase increment from the sine function.

Signal Generation Equation

The value sn  is the nth sample, An the peak amplitude (volume) at sample n, and θn the phase at sample n. To calculate θn for any sample n, we can multiply the phase increment for one sample time (φ) by the sample number. To calculate φ we need to determine the radians for one sample time at a given frequency. As there are 2π radians per cycle, we multiply the frequency by 2π to get the radians per second. The phase increment for one sample time is then the radians per second multiplied by the time for one sample. Substiting for θn in the original equation yields:

Signal Generation Equation

We can implement this as a program loop.

totalSamples = duration * sampleRate;
for (n = 0; n < totalSamples; n++)
    sample[n] = sin((twoPI/sampleRate) * frequency * n);
Since  2π/fs is constant through the loop, we can calculate it once. We can also replace the multiplication of the phase with a repeated addition.  

phaseIncr = (twoPI/sampleRate) * frequency;
phase = 0;
totalSamples = duration * sampleRate;
for (n = 0; n < totalSamples; n++) {
    sample[n] = sin(phase);
    phase += phaseIncr;
    if (phase >= twoPI)
        phase -= twoPI;
}
 

We can replace the sin function with any periodic function that returns an amplitude for a given phase angle. Thus, this small piece of code can be used to produce a very wide range of sounds. Functionally, it is the software equivalent of an oscillator, the basic building block of almost all synthesizers.