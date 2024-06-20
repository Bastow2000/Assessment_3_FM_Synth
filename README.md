# Assessment_3_FM_Synth
QMUL University Assessment AP 3

For my undergraduate project, I developed an additive synthesiser in C++ using the JUCE framework. At that time, my understanding of audio programming was limited, having issues with clicks and pops.
This practical assessment attempts to expand on prior attempts, going further by- constructing a fm multi-feedback bandlimited additive synthesiser. YouTube: https://www.youtube.com/watch?v=yr4RxDwaD60

<img width="561" alt="Screenshot 2024-06-20 at 10 36 34" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/6fa9f2f1-09b8-4ea6-8c59-4bb008475024">

<img width="585" alt="Screenshot 2024-06-20 at 10 36 53" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/052d3c46-4f98-4165-b014-618143004807">

All the modulators are assigned a specific wave form before generating.
<img width="563" alt="Screenshot 2024-06-20 at 10 37 59" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/25529da7-23fe-40bc-b2ae-da20f5b9cb16">



Polynomial bandlimited step is then applied for reducing aliasing, I have used the technique provided by Martin Finke


<img width="469" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/716c4695-ff63-444a-802a-a0d066777ccc">

We now have the main building blocks for my synthesiser, in the image below you can see what the wave form looks like with different amounts of modulators.

<img width="667" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/bd713751-7a86-4a95-b4e3-3bccb4727c9a">

<img width="579" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/5bef8db4-356e-46c6-95a2-511ff8e8291d">

<img width="688" alt="image" src="https://github.com/Bastow2000/Assessment_3_FM_Synth/assets/77554338/d2dad7bd-afc6-408c-9c02-2165bcc50f25">



