#pragma once
#include <unordered_map>

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

public: // Component implementation
	void paint(Graphics& g) override;
	void paintOverChildren(Graphics& g) override;
	void resized() override;

public: // Slider::Listener implementation
	void sliderDragStarted(Slider* slider) override;
	void sliderValueChanged(Slider* slider) override;
	void sliderDragEnded(Slider* slider) override;

public: // Button::Listener implementation
	void buttonClicked(Button* button) override;

private:
	void drawBordersAndLabels(Graphics& g);

private:
	HrtfBiAuralAudioProcessor& processor_;
	ScopedPointer<MainDisplay> mainDisplay_;
	Colour bgColor_;
	Colour fgColor_;
	Slider elevationSlider_;
	Slider crossoverKnob_;
	Slider wetKnob_;
	Slider gainKnob_;
	TextButton bypassButton_;
	int topSectionY_;

	std::unordered_map<Slider*, AudioParameter*> knobToParam_;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HrtfBiAuralAudioProcessorEditor)
};
