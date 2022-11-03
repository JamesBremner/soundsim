# SoundSim

Finite Difference Time Domain Simulation of Sound Propogation.

![image](https://user-images.githubusercontent.com/2046227/199831661-0dcc6917-b89f-452a-8ba3-a0dd3fb5cbc5.png)

The propagation of sound waves is governed by the following system of first order partial differential
equations:

![image](https://user-images.githubusercontent.com/2046227/196517720-22b888f3-21b4-4761-bb5d-7346c2e97728.png)

which can be converted to the following update formula for pressure and velocity

![image](https://user-images.githubusercontent.com/2046227/196518566-7f98a14e-20f6-4ee6-972e-c9dea9083192.png)


![image](https://user-images.githubusercontent.com/2046227/196518987-9fb5ef3e-dcad-49c4-9082-6f6186d7769e.png)


Understanding the Finite-Difference Time-Domain Method, John B. Schneider, www.eecs.wsu.edu/~schneidj/ufdtd, 2010.

The simulation is based on the above work, but re-implemented in modern C++ and offering a GUI.
