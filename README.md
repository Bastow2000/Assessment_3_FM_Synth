# Assessment_3_FM_Synth
QMUL University Assessment AP 3

For my undergraduate project, I developed an additive synthesiser in C++ using the JUCE framework (Bastow Synth V1). At that time, my understanding of audio programming was limited, having issues with clicks and pops.
This practical assessment attempts to expand on prior attempts, going further by- constructing a fm multi-feedback bandlimited additive synthesiser. YouTube: https://www.youtube.com/watch?v=yr4RxDwaD60

<img width="964" alt="Screenshot 2023-07-05 at 09 09 03" src="https://github.com/Bastow2000/BastowSynth.V2/assets/77554338/a1973ddc-3ec9-450a-93a5-262a775d83ab">

<img width="585" alt="Screenshot 2024-06-20 at 10 36 53" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/052d3c46-4f98-4165-b014-618143004807">

All the modulators are assigned a specific wave form before generating.
<img width="563" alt="Screenshot 2024-06-20 at 10 37 59" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/25529da7-23fe-40bc-b2ae-da20f5b9cb16">



Polynomial bandlimited step is then applied for reducing aliasing, I have used the technique provided by Martin Finke


<img width="469" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/716c4695-ff63-444a-802a-a0d066777ccc">

We now have the main building blocks for my synthesiser, in the image below you can see what the wave form looks like with different amounts of modulators.

<img width="667" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/bd713751-7a86-4a95-b4e3-3bccb4727c9a">

The next is the feedback algorithms.
All modulators have their own feedback loops.
These modulators are then added together, which is then looped multiple times before being added to the carrier.

Attempts were made to do feedback with the carrier; due to the multiple partials there was only noise, I decided to pass on this.
I had to experiment a lot here, due to certain algorithms causing instant distortion and crackling.


<img width="579" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/5bef8db4-356e-46c6-95a2-511ff8e8291d">

From the very start I wanted to have some breadboard interaction.

<img width="688" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/d2dad7bd-afc6-408c-9c02-2165bcc50f25">

This is an image when the 4th modulator is turned on and the note is on (yellow led).

<img width="553" alt="Screenshot 2024-06-20 at 10 44 09" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/590f72de-4690-4511-aa0f-07f1e034d001">

<img width="365" alt="Screenshot 2024-06-20 at 10 45 28" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/36c8dbaf-11e3-454e-9ae5-2ef94deff853">



