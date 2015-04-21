#include "PluginEditor.h"


HrtfBiAuralAudioProcessorEditor::HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& p)
	:
	AudioProcessorEditor(&p),
	processor(p),
	mainDisplay(new MainDisplay(this)),
	bgColor(51, 51, 51),
	fgColor(73, 166, 201),
	nextHrtf("next", 0.f, fgColor),
	prevHrtf("prev", 0.5f, fgColor)
{
	setSize(350, 420);

	elevationSlider.setSliderStyle(Slider::LinearVertical);
	elevationSlider.setRange(-90, 90, 0.1);
	elevationSlider.setValue(0);
	elevationSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
	elevationSlider.addListener(this);
	elevationSlider.setName("Elevation");
	elevationSlider.addListener(mainDisplay);
	addAndMakeVisible(elevationSlider);

	crossoverSlider.setSliderStyle(Slider::LinearHorizontal);
	crossoverSlider.setTextBoxStyle(Slider::TextBoxRight, true, 50, 20);
	crossoverSlider.setRange(20, 500, 1);
	crossoverSlider.setValue(p.crossover.f0);
	crossoverSlider.addListener(this);
	crossoverSlider.setName("Crossover");
	addAndMakeVisible(crossoverSlider);

	bypassButton.setName("Bypass");
	bypassButton.setImages(
		false,
		true,
		true,
		ImageFileFormat::loadFrom(bypass_up_png, bypass_up_png_size),
		1.0f,
		Colours::transparentWhite,
		Image::null,
		1.0f,
		Colours::transparentWhite,
		ImageFileFormat::loadFrom(bypass_down_png, bypass_down_png_size),
		1.0f,
		Colours::transparentWhite);
	bypassButton.setClickingTogglesState(true);
	bypassButton.addListener(this);
	addAndMakeVisible(bypassButton);

	prevHrtf.addListener(this);
	addAndMakeVisible(nextHrtf);
	nextHrtf.addListener(this);
	addAndMakeVisible(prevHrtf);

	addAndMakeVisible(mainDisplay);
}

HrtfBiAuralAudioProcessorEditor::~HrtfBiAuralAudioProcessorEditor()
{
}

void HrtfBiAuralAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(bgColor);
	g.fillAll();
	drawBordersAndLabels(g);
}

void HrtfBiAuralAudioProcessorEditor::drawBordersAndLabels(Graphics& g)
{
	g.setColour(fgColor);
	Rectangle<int> topSection(5, 5, getWidth() - 10, 50);
	Rectangle<int> middleSection(5, 55, getWidth() - 10, getHeight() - 120);
	Rectangle<int> bottomSection(5, getHeight() - 65, getWidth() - 10, 60);
	g.drawRoundedRectangle(topSection.toFloat(), 3.f, 2.f);
	g.drawRoundedRectangle(middleSection.toFloat(), 3.f, 2.f);
	g.drawRoundedRectangle(bottomSection.toFloat(), 3.f, 2.f);
	g.setColour(Colours::white);
	g.setFont(20.f);
	g.drawFittedText(processor.hrtfContainer.getCurrentSubjectInfo().name, topSection, Justification::centred, 1);
	g.setFont(11.0f);
	g.drawFittedText("HRTF Selection", topSection, Justification::topLeft, 1);
	g.drawFittedText("Stereo controls", middleSection, Justification::topLeft, 1);
	g.drawFittedText(String("ELEVATION"), elevationSlider.getX(), elevationSlider.getY() - 10, elevationSlider.getWidth(), 12, Justification::centredTop, 1);
	g.drawFittedText(String("CROSSSOVER FREQUENCY"), crossoverSlider.getX(), crossoverSlider.getY(), crossoverSlider.getWidth(), 12, Justification::left, 1);
}

void HrtfBiAuralAudioProcessorEditor::resized()
{
	prevHrtf.setBounds(100, 25, 40, 20);
	nextHrtf.setBounds(220, 25, 40, 20);
	mainDisplay->setBounds(20, 70, 260, 260);
	elevationSlider.setBounds(280, 80, 50, 260);
	crossoverSlider.setBounds(10, getBottom() - 50, 250, 50);
	bypassButton.setBounds(getWidth() - 90, getBottom() - 40, 90, 30);
}

void HrtfBiAuralAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == crossoverSlider.getName())
	{
		processor.crossover.set(processor.crossover.fs, slider->getValue());
	}
}

void HrtfBiAuralAudioProcessorEditor::buttonClicked(Button* button)
{
	auto buttonName = button->getName();
	if (buttonName == bypassButton.getName())
		processor.bypassed = button->getToggleState();
	else if (buttonName == nextHrtf.getName())
		processor.hrtfContainer.setCurrentSubject(processor.hrtfContainer.getCurrentSubjectIndex() + 1);
	else if (buttonName == prevHrtf.getName())
		processor.hrtfContainer.setCurrentSubject(processor.hrtfContainer.getCurrentSubjectIndex() - 1);

	processor.reset();
	repaint();
}