#pragma once
#include "PluginProcessor.h"
#include "MainDisplay.h"
#include "Util.h"


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
	void paintOverChildren(Graphics& g) override;
	void resized() override;
	void sliderValueChanged(Slider* slider) override;
	void buttonClicked(Button* button) override;

private:
	void drawBordersAndLabels(Graphics& g);

	HrtfBiAuralAudioProcessor& processor_;
	ScopedPointer<MainDisplay> mainDisplay_;
	Colour bgColor_;
	Colour fgColor_;
	Slider elevationSlider_;
	Slider crossoverSlider_;
	Slider amountSlider_;
	Slider gainSlider_;
	TextButton bypassButton_;
	int topSectionY_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessorEditor)
};
