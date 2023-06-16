Certainly! Here's an example of generating an ADSR (Attack, Decay, Sustain, Release) envelope in Python:

```python
import numpy as np
import matplotlib.pyplot as plt

# Parameters
sample_rate = 44100  # Sample rate in Hz
duration = 5  # Duration of the envelope in seconds

# Time values
num_samples = int(duration * sample_rate)
time = np.arange(num_samples) / sample_rate

# ADSR parameters
attack_time = 0.5  # Attack time in seconds
decay_time = 0.3  # Decay time in seconds
sustain_level = 0.6  # Sustain level (0 to 1)
release_time = 1.0  # Release time in seconds

# Generate the ADSR envelope
envelope = np.zeros(num_samples)
attack_samples = int(attack_time * sample_rate)
decay_samples = int(decay_time * sample_rate)
release_samples = int(release_time * sample_rate)

# Attack phase
envelope[:attack_samples] = np.linspace(0, 1, num=attack_samples)

# Decay phase
decay_slope = (1 - sustain_level) / decay_samples
envelope[attack_samples:attack_samples + decay_samples] = np.linspace(1, sustain_level, num=decay_samples) - decay_slope * np.arange(decay_samples)

# Sustain phase
envelope[attack_samples + decay_samples:-release_samples] = sustain_level

# Release phase
release_slope = sustain_level / release_samples
envelope[-release_samples:] = sustain_level - release_slope * np.arange(release_samples)

# Normalize the envelope
envelope /= np.max(envelope)

# Plot the envelope
plt.plot(time, envelope)
plt.xlabel('Time (s)')
plt.ylabel('Amplitude')
plt.title('ADSR Envelope')
plt.show()
```

In this example, we specify the sample rate and duration of the envelope. We define the ADSR parameters: attack time, decay time, sustain level, and release time.

We create an array to store the envelope values and initialize it with zeros. We calculate the number of samples for each phase based on the sample rate and duration.

We then calculate the envelope values for each phase. The attack phase increases linearly from 0 to 1. The decay phase decreases linearly from 1 to the sustain level. The sustain phase maintains a constant value equal to the sustain level. The release phase decreases linearly from the sustain level to 0.

After generating the envelope, we normalize it to ensure the maximum value is 1. Finally, we plot the envelope using Matplotlib.

You can modify the ADSR parameters to create different envelope shapes or experiment with adding modulation to create more dynamic and expressive sounds.