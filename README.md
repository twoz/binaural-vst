# HRTF-Vst
An experimental mono-to-stereo VST plugin that positiones sound in a 3D space using Head-Related Transfer Functions from the [CIPIC database](http://interface.cipic.ucdavis.edu/sound/hrtf.html).

It allows you to switch between HRTF from 45 different subjects.

It uses [CGAL](https://www.cgal.org/) for triangulation that speeds up the interpolation process and [JUCE C++](http://www.juce.com/) framework for everything else.

How-to:

Copy the "build" folder to your VST directory (note that "hrir" folder containing Head-Related Impulse Responses of all subjects is almost 100mb)
and move libgmp-10.dll and libmpfr-4.dll to your host's exe directory (This is necessary since for now i use VC++ compiler which isn't supported by gmp)
