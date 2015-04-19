#pragma once

#include <cmath>
#include "PluginProcessor.h"
#include "Helpers.h"
#include "images/head_top.h"
#include "images/head_side.h"
#include "images/source_icon.h"
#include "images/bypass_down.h"
#include "images/bypass_up.h"


class HrtfBiAuralAudioProcessorEditor : public AudioProcessorEditor,
	private Slider::Listener,
	private Button::Listener
{
public:
	HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor&);
	~HrtfBiAuralAudioProcessorEditor();

	void paint(Graphics&) override;
	void resized() override;
	void mouseDrag(const MouseEvent&) override;
	void sliderValueChanged(Slider*) override;
	void buttonClicked(Button* button) override;

private:
	void drawGridLines(Graphics& g);
	void drawDisplays(Graphics& g);
	void drawSources(Graphics& g);
	void drawBordersAndLabels(Graphics& g);

	void updateHrir();

	HrtfBiAuralAudioProcessor& processor;
	Colour bgColor;
	Colour fgColor;
	ArrowButton nextHrtf;
	ArrowButton prevHrtf;
	Slider elevationSlider;
	Slider crossoverSlider;
	ImageButton bypassButton;
	Image sourceImage;
	Image topViewImage;
	Image sideViewImage;

	float topViewX;
	float topViewY;
	float sideViewX;
	float sideViewY;
	double azimuth;
	double elevation;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessorEditor)
};