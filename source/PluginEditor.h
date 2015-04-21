#pragma once

#include "PluginProcessor.h"
#include "MainDisplay.h"
#include "Helpers.h"
#include "images/bypass_down.h"
#include "images/bypass_up.h"


class HrtfBiAuralAudioProcessorEditor :
	public AudioProcessorEditor,
	public Slider::Listener,
	public Button::Listener
{
	friend class MainDisplay;
public:
	HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& processor);
	~HrtfBiAuralAudioProcessorEditor();

	void paint(Graphics& g) override;
	void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* button) override;

private:
	void drawBordersAndLabels(Graphics& g);

	HrtfBiAuralAudioProcessor& processor;
	Colour bgColor;
	Colour fgColor;
	ScopedPointer<MainDisplay> mainDisplay;
	ArrowButton nextHrtf;
	ArrowButton prevHrtf;
	Slider elevationSlider;
	Slider crossoverSlider;
	ImageButton bypassButton;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessorEditor)
};