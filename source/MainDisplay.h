#pragma once
#include <cmath>
#include "../JuceLibraryCode/JuceHeader.h"
#include "Util.h"
#include "images/head_top.h"
#include "images/source_icon.h"


class HrtfBiAuralAudioProcessorEditor;

class MainDisplay :
	public Component,
	public Slider::Listener
{
public:
	MainDisplay(HrtfBiAuralAudioProcessorEditor* editor);
	~MainDisplay();

	void paint(Graphics& g) override;
	void mouseDrag(const MouseEvent& event) override;
	void sliderValueChanged(Slider* slider) override;

private:
	void drawGridLines(Graphics& g);
	void drawSource(Graphics& g);
	void updateHRTF();

	Image headImage_;
	Image sourceImage_;
	Point3DoublePolar<float> sourcePos_;
	HrtfBiAuralAudioProcessorEditor* editor_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainDisplay)
};
