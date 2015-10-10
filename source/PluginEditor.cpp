#include "PluginEditor.h"
#include "AudioParameter.h"


HrtfBiAuralAudioProcessorEditor::HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& p)
	: AudioProcessorEditor(&p)
	, processor_(p)
	, mainDisplay_(new MainDisplay(this))
	, bgColor_(51, 51, 51)
	, fgColor_(73, 166, 201)
	, topSectionY_(300)
{
	setSize(350, 400);
	addAndMakeVisible(mainDisplay_);

	// map knobs to parameters
	knobToParam_.insert
		({
			{ &crossoverKnob_, p.getCrossoverFrequencyParameter() },
			{ &wetKnob_, p.getWetParameter() },
			{ &gainKnob_, p.getGainParameter() }
		});

	for (auto& pair : knobToParam_)
	{
		auto& knob = pair.first;
		const auto& param = pair.second;

		const auto interval = 0.1;
		knob->setRange(param->minValue(), param->maxValue(), interval);
		knob->setValue(param->defaultValue());
		knob->setTextValueSuffix(param->getLabel());

		knob->setSliderStyle(Slider::Rotary);
		knob->setColour(Slider::rotarySliderFillColourId, fgColor_);
		knob->setColour(Slider::textBoxBackgroundColourId, Colours::black);
		knob->setColour(Slider::textBoxTextColourId, Colours::white);
		knob->setColour(Slider::textBoxOutlineColourId, Colours::black);
		knob->setRotaryParameters(10 / 8.f * Pi, 22 / 8.f * Pi, true);
		knob->setTextBoxStyle(Slider::TextBoxBelow, true, 70, 15);
		knob->addListener(this);
		addAndMakeVisible(knob);
	}

	// TODO: map elevation slider to audio parameter
	elevationSlider_.setSliderStyle(Slider::LinearVertical);
	elevationSlider_.setRange(-90, 90, 0.1);
	elevationSlider_.setValue(0.);
	elevationSlider_.setColour(Slider::thumbColourId, fgColor_);
	elevationSlider_.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	elevationSlider_.setColour(Slider::textBoxTextColourId, Colours::white);
	elevationSlider_.setColour(Slider::textBoxOutlineColourId, Colours::black);
	elevationSlider_.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
	elevationSlider_.addListener(mainDisplay_);
	addAndMakeVisible(elevationSlider_);

	// TODO: map bypass to audio parameter
	bypassButton_.setName("Bypass");
	bypassButton_.setButtonText("BYPASS");
	bypassButton_.setColour(TextButton::buttonColourId, Colours::black);
	bypassButton_.setColour(TextButton::buttonOnColourId, fgColor_);
	bypassButton_.setColour(TextButton::textColourOffId, fgColor_);
	bypassButton_.setColour(TextButton::textColourOnId, Colours::black);
	bypassButton_.setClickingTogglesState(true);
	bypassButton_.addListener(this);
	addAndMakeVisible(bypassButton_);

	// if hrir could not be loaded, disable any clicking on the component
	if (!p.isHRIRLoaded())
		setInterceptsMouseClicks(false, false);
}

HrtfBiAuralAudioProcessorEditor::~HrtfBiAuralAudioProcessorEditor()
{
}

void HrtfBiAuralAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(bgColor_);
	g.fillAll();
	drawBordersAndLabels(g);
}

void HrtfBiAuralAudioProcessorEditor::paintOverChildren(Graphics& g)
{
	if (!processor_.isHRIRLoaded())
	{
		FillType fill;
		fill.setColour(Colours::black);
		fill.setOpacity(0.7f);
		g.setFillType(fill);
		g.fillRect(getBounds());
		g.setColour(Colours::whitesmoke);
		g.setFont(40);
		g.drawFittedText("HRIR not loaded\nPlugin disabled", getBounds(), Justification::centred, 2);
	}
}

void HrtfBiAuralAudioProcessorEditor::drawBordersAndLabels(Graphics& g)
{
	g.setColour(fgColor_);
	Rectangle<int> topSection(5, 5, getWidth() - 10, topSectionY_ - 10);
	Rectangle<int> bottomSection(5, topSectionY_, getWidth() - 10, getHeight() - topSectionY_ - 5);
	g.drawRoundedRectangle(topSection.toFloat(), 3.f, 2.f);
	g.drawRoundedRectangle(bottomSection.toFloat(), 3.f, 2.f);
	g.setColour(Colours::white);
	g.setFont(11.0f);
	g.drawFittedText(String("ELEVATION (deg)"), elevationSlider_.getX(), elevationSlider_.getY() - 22, elevationSlider_.getWidth(), 22, Justification::centredTop, 2);
	g.drawFittedText(String("CROSS FREQ"), crossoverKnob_.getX(), crossoverKnob_.getY() - 12, crossoverKnob_.getWidth(), 12, Justification::centred, 1);
	g.drawFittedText(String("WET"), wetKnob_.getX(), wetKnob_.getY() - 12, wetKnob_.getWidth(), 12, Justification::centred, 1);
	g.drawFittedText(String("GAIN"), gainKnob_.getX(), gainKnob_.getY() - 12, gainKnob_.getWidth(), 12, Justification::centred, 1);

	Rectangle<int> labelSection(getWidth() - 90, topSectionY_ + 5, 80, 50);
	g.setColour(fgColor_);
	g.drawRoundedRectangle(labelSection.toFloat(), 3.f, 1.f);
	g.setFont(14.f);
	g.setColour(Colours::white);
	g.drawFittedText("BinAural VST\nby TWoz", labelSection, Justification::centred, 2);
}

void HrtfBiAuralAudioProcessorEditor::resized()
{
	mainDisplay_->setBounds(20, 20, 260, 260);
	elevationSlider_.setBounds(280, 30, 50, 260);
	crossoverKnob_.setBounds(10, topSectionY_ + 15, 75, 75);
	wetKnob_.setBounds(90, topSectionY_ + 15, 75, 75);
	gainKnob_.setBounds(170, topSectionY_ + 15, 75, 75);
	bypassButton_.setBounds(getWidth() - 90, getBottom() - 40, 80, 30);
}

void HrtfBiAuralAudioProcessorEditor::sliderDragStarted(Slider* slider)
{
	knobToParam_.at(slider)->beginChangeGesture();
}

void HrtfBiAuralAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	auto& changedParam = knobToParam_.at(slider);
	changedParam->setValueAndNotifyHost(static_cast<float>(slider->getValue()));
	processor_.onAudioParameterChanged(changedParam);
}

void HrtfBiAuralAudioProcessorEditor::sliderDragEnded(Slider* slider)
{
	knobToParam_.at(slider)->endChangeGesture();
}

void HrtfBiAuralAudioProcessorEditor::buttonClicked(Button* button)
{
	auto buttonName = button->getName();
	if (buttonName == bypassButton_.getName())
		processor_.toggleBypass(button->getToggleState());

	repaint();
}
