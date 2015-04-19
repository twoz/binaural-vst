#include "PluginEditor.h"

HrtfBiAuralAudioProcessorEditor::HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& p)
	:
	AudioProcessorEditor(&p),
	processor(p),
	bgColor(51, 51, 51),
	fgColor(73, 166, 201),
	topViewX(15.f),
	topViewY(60.f),
	sideViewX(240.f),
	sideViewY(250.f),
	azimuth(0.),
	elevation(0.),
	nextHrtf("next", 0.f, fgColor),
	prevHrtf("prev", 0.5f, fgColor)
{
	setSize(350, 420);

	topViewImage = ImageFileFormat::loadFrom(head_top_png, head_top_png_size);
	sideViewImage = ImageFileFormat::loadFrom(head_side_png, head_side_png_size);
	sourceImage = ImageFileFormat::loadFrom(source_icon_png, source_icon_png_size);

	elevationSlider.setSliderStyle(Slider::LinearVertical);
	elevationSlider.setRange(-90, 90, 0.1);
	elevationSlider.setValue(0);
	elevationSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
	elevationSlider.addListener(this);
	elevationSlider.setName("Elevation");
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
}

HrtfBiAuralAudioProcessorEditor::~HrtfBiAuralAudioProcessorEditor()
{
}

void HrtfBiAuralAudioProcessorEditor::paint(Graphics& g)
{
	g.setColour(bgColor);
	g.fillAll();
	drawDisplays(g);
	drawGridLines(g);
	drawSources(g);
	drawBordersAndLabels(g);
}

void HrtfBiAuralAudioProcessorEditor::drawGridLines(Graphics& g)
{
	auto w = topViewImage.getWidth();
	auto h = topViewImage.getHeight();
	g.setColour(Colours::white);
	g.setOpacity(0.5f);
	auto lineHorizontal = Line<float>(topViewX, topViewY + h * 0.5f, topViewX + w, topViewY + h * 0.5f);
	auto lineVertical = Line<float>(topViewX + w * 0.5f, topViewY, topViewX + w * 0.5f, topViewY + h);
	float dashes [] = { 3, 2 };
	g.drawDashedLine(lineHorizontal, dashes, 2);
	g.drawDashedLine(lineVertical, dashes, 2);
	w = sideViewImage.getWidth();
	h = sideViewImage.getHeight();
	lineHorizontal = Line<float>(sideViewX, sideViewY + h * 0.5f, sideViewX + w, sideViewY + h * 0.5f);
	lineVertical = Line<float>(sideViewX + w * 0.5f, sideViewY, sideViewX + w * 0.5f, sideViewY + h);
	g.drawDashedLine(lineHorizontal, dashes, 2);
	g.drawDashedLine(lineVertical, dashes, 2);
}

void HrtfBiAuralAudioProcessorEditor::drawDisplays(Graphics& g)
{
	g.drawImageAt(topViewImage, topViewX, topViewY);
	g.drawImageAt(sideViewImage, sideViewX, sideViewY);
	g.setColour(bgColor);
	g.drawEllipse(topViewX, topViewY, topViewImage.getWidth(), topViewImage.getHeight(), 2.0f);
	g.drawEllipse(sideViewX, sideViewY, sideViewImage.getWidth(), sideViewImage.getHeight(), 2.0f);
}

void HrtfBiAuralAudioProcessorEditor::drawSources(Graphics& g)
{
	auto w = topViewImage.getWidth();
	auto h = topViewImage.getHeight();
	auto radius = w * 0.5f - 30;
	auto x = radius * std::sin(azimuth);
	auto y = radius * std::cos(azimuth);
	auto sourceX = topViewX + w * 0.5f + x;
	auto sourceY = topViewY + h * 0.5f - y;
	ColourGradient grad(fgColor, sourceX, sourceY, Colours::transparentBlack, topViewX + w * 0.5f, topViewY + h * 0.5f, true);
	g.setGradientFill(grad);
	g.fillEllipse(topViewX, topViewY, w, h);
	g.drawImageAt(sourceImage, sourceX - sourceImage.getWidth() * 0.5f, sourceY - sourceImage.getHeight() * 0.5f);
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
	elevationSlider.setBounds(280, 80, 50, 160);
	crossoverSlider.setBounds(10, getBottom() - 50, 250, 50);
	bypassButton.setBounds(getWidth() - 90, getBottom() - 40, 90, 30);
}

void HrtfBiAuralAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
	auto pos = event.getPosition();
	if (pos.x > topViewX && pos.x < topViewX + topViewImage.getWidth() &&
		pos.y > topViewY && pos.y < topViewY + topViewImage.getHeight())
	{
		auto x = pos.x - topViewX - topViewImage.getWidth() * 0.5f;
		auto y = pos.y - topViewY - topViewImage.getHeight() * 0.5f;
		azimuth = std::atan2(x, -y);
		updateHrir();
		repaint();
	}
}

void HrtfBiAuralAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	if (slider->getName() == crossoverSlider.getName())
	{
		processor.crossover.set(processor.crossover.fs, slider->getValue());
	}
	else
	{
		elevation = deg2rad(slider->getValue());
		updateHrir();
		repaint();
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

void HrtfBiAuralAudioProcessorEditor::updateHrir()
{
	auto x = cos(elevation) * sin(azimuth);
	auto y = cos(elevation) * cos(azimuth);
	auto z = sin(elevation);

	auto point = cartesianToInteraural(Point3Cartesian<double>(x, y, z));

	processor.updateHRTF(rad2deg(point.azimuth), rad2deg(point.elevation));
}