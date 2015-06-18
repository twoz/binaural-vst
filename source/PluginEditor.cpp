#include "PluginEditor.h"


HrtfBiAuralAudioProcessorEditor::HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& p)
	:
	AudioProcessorEditor(&p),
	processor_(p),
	mainDisplay_(new MainDisplay(this)),
	bgColor_(51, 51, 51),
	fgColor_(73, 166, 201),
	topSectionY_(300)
{
	setSize(350, 400);

	// sliders -------------------------------------------------------------------
	elevationSlider_.setSliderStyle(Slider::LinearVertical);
	elevationSlider_.setRange(-90, 90, 0.1);
	elevationSlider_.setValue(0.);
	elevationSlider_.setColour(Slider::thumbColourId, fgColor_);
	elevationSlider_.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	elevationSlider_.setColour(Slider::textBoxTextColourId, Colours::white);
	elevationSlider_.setColour(Slider::textBoxOutlineColourId, Colours::black);
	elevationSlider_.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
	elevationSlider_.addListener(mainDisplay_);
	elevationSlider_.setName("Elevation");
	addAndMakeVisible(elevationSlider_);

	crossoverSlider_.setSliderStyle(Slider::Rotary);
	crossoverSlider_.setColour(Slider::rotarySliderFillColourId, fgColor_);
	crossoverSlider_.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	crossoverSlider_.setColour(Slider::textBoxTextColourId, Colours::white);
	crossoverSlider_.setColour(Slider::textBoxOutlineColourId, Colours::black);
	crossoverSlider_.setRotaryParameters(10 / 8. * Pi, 22 / 8. * Pi, true);
	crossoverSlider_.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 15);
	crossoverSlider_.setRange(20, 500, 1);
	crossoverSlider_.setValue(p.crossover_.f0);
	crossoverSlider_.addListener(this);
	crossoverSlider_.setName("Crossover");
	addAndMakeVisible(crossoverSlider_);

	wetMixSlider_.setSliderStyle(Slider::Rotary);
	wetMixSlider_.setColour(Slider::rotarySliderFillColourId, fgColor_);
	wetMixSlider_.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	wetMixSlider_.setColour(Slider::textBoxTextColourId, Colours::white);
	wetMixSlider_.setColour(Slider::textBoxOutlineColourId, Colours::black);
	wetMixSlider_.setRotaryParameters(10 / 8. * Pi, 22 / 8. * Pi, true);
	wetMixSlider_.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 15);
	wetMixSlider_.setRange(0, 100, 1);
	wetMixSlider_.setValue(100.);
	wetMixSlider_.addListener(this);
	wetMixSlider_.setName("Mix");
	addAndMakeVisible(wetMixSlider_);

	gainSlider_.setSliderStyle(Slider::Rotary);
	gainSlider_.setColour(Slider::rotarySliderFillColourId, fgColor_);
	gainSlider_.setColour(Slider::textBoxBackgroundColourId, Colours::black);
	gainSlider_.setColour(Slider::textBoxTextColourId, Colours::white);
	gainSlider_.setColour(Slider::textBoxOutlineColourId, Colours::black);
	gainSlider_.setRotaryParameters(10 / 8. * Pi, 22 / 8. * Pi, true);
	gainSlider_.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 15);
	gainSlider_.setRange(-12., 12., 0.1);
	gainSlider_.setValue(0.);
	gainSlider_.addListener(this);
	gainSlider_.setName("Gain");
	addAndMakeVisible(gainSlider_);
	// ---------------------------------------------------------------------------

	bypassButton_.setName("Bypass");
	bypassButton_.setButtonText("BYPASS");
	bypassButton_.setColour(TextButton::buttonColourId, Colours::black);
	bypassButton_.setColour(TextButton::buttonOnColourId, fgColor_);
	bypassButton_.setColour(TextButton::textColourOffId, fgColor_);
	bypassButton_.setColour(TextButton::textColourOnId, Colours::black);
	bypassButton_.setClickingTogglesState(true);
	bypassButton_.addListener(this);
	addAndMakeVisible(bypassButton_);

	addAndMakeVisible(mainDisplay_);
	
	// if hrir could not be loaded, disable any clicking on the component
	if (!p.hrirLoaded_)
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
	if (!processor_.hrirLoaded_)
	{
		FillType fill;
		fill.setColour(Colours::black);
		fill.setOpacity(0.7);
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
	g.setFont(20.f);
	g.setFont(11.0f);
	g.drawFittedText(String("ELEVATION (deg)"), elevationSlider_.getX(), elevationSlider_.getY() - 22, elevationSlider_.getWidth(), 22, Justification::centredTop, 2);
	g.drawFittedText(String("CROSS FREQ (Hz)"), crossoverSlider_.getX(), crossoverSlider_.getY() - 12, crossoverSlider_.getWidth(), 12, Justification::centred, 1);
	g.drawFittedText(String("WET MIX (%)"), wetMixSlider_.getX(), wetMixSlider_.getY() - 12, wetMixSlider_.getWidth(), 12, Justification::centred, 1);
	g.drawFittedText(String("GAIN (dB)"), gainSlider_.getX(), gainSlider_.getY() - 12, gainSlider_.getWidth(), 12, Justification::centred, 1);
}

void HrtfBiAuralAudioProcessorEditor::resized()
{
	mainDisplay_->setBounds(20, 20, 260, 260);
	elevationSlider_.setBounds(280, 30, 50, 260);
	crossoverSlider_.setBounds(10, topSectionY_ + 15, 75, 75);
	wetMixSlider_.setBounds(90, topSectionY_ + 15, 75, 75);
	gainSlider_.setBounds(170, topSectionY_ + 15, 75, 75);
	bypassButton_.setBounds(getWidth() - 90, getBottom() - 40, 80, 30);
}

void HrtfBiAuralAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	auto sliderName = slider->getName();
	if (sliderName == crossoverSlider_.getName())
		processor_.crossover_.set(processor_.crossover_.fs, slider->getValue());
	else if (sliderName == wetMixSlider_.getName())
		processor_.panAmount_ = wetMixSlider_.getValue() / 100;
	else if (sliderName == gainSlider_.getName())
		processor_.gain_ = Decibels::decibelsToGain(gainSlider_.getValue());
}

void HrtfBiAuralAudioProcessorEditor::buttonClicked(Button* button)
{
	auto buttonName = button->getName();
	if (buttonName == bypassButton_.getName())
	{
		processor_.toggleBypass(button->getToggleState());
		setEnabled(false);
	}

	repaint();
}
