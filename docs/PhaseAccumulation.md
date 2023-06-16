Certainly! Here's a simple example of phase accumulation in Python code:

```python
import numpy as np
import matplotlib.pyplot as plt

# Parameters
sample_rate = 44100  # Sample rate in Hz
frequency = 440  # Frequency of the oscillator in Hz
duration = 1  # Duration of the generated waveform in seconds

# Calculate the phase increment per sample
phase_increment = 2 * np.pi * frequency / sample_rate

# Initialize phase and time arrays
num_samples = int(duration * sample_rate)
phase = np.zeros(num_samples)
time = np.arange(num_samples) / sample_rate

# Perform phase accumulation
for i in range(1, num_samples):
    phase[i] = phase[i - 1] + phase_increment

# Generate the waveform (sine wave) based on the accumulated phase
waveform = np.sin(phase)

# Plot the waveform
plt.plot(time, waveform)
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('Phase Accumulation Example - Sine Wave')
plt.show()
```

In this example, we specify the sample rate, frequency, and duration of the waveform. We calculate the phase increment per sample based on the desired frequency. Then, we initialize arrays to store the phase values and time values. By iterating through each sample, we perform phase accumulation by adding the phase increment to the previous phase value.

Finally, we generate a sine wave by taking the sine of the accumulated phase values. The resulting waveform is plotted using Matplotlib.

This code demonstrates the basic concept of phase accumulation, where the phase of an oscillator is incremented over time to generate a periodic waveform. You can modify the parameters, try different waveforms, or experiment with phase modulation to create more complex sounds.