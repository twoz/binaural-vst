#include "PluginEditor.h"

GridPanel::GridPanel()
{
	auto thisDir = File::getSpecialLocation(File::currentExecutableFile).getParentDirectory().getFullPathName();
	headIcon = ImageFileFormat::loadFrom(File(thisDir + "/head.png"));
}

GridPanel::~GridPanel()
{
}

void GridPanel::paint(Graphics& g)
{
	g.fillAll(Colours::white);
	g.setColour(Colours::black);

	// 
	g.drawLine(0, 0, getWidth(), 0);
	g.drawLine(0, 0, 0, getHeight());
	g.drawLine(0, getHeight(), getWidth(), getHeight());

	int r = 15;
	// draw grid
	Rectangle<float> gridRect(r, r, getWidth() - 2 * r, getHeight() - 2 * r);
	g.drawEllipse(gridRect, 2);
	g.drawLine(gridRect.getX(), gridRect.getCentreY(), gridRect.getRight(), gridRect.getCentreY(), 1);
	g.drawLine(gridRect.getCentreX(), gridRect.getY(), gridRect.getCentreX(), gridRect.getBottom(), 1);

	// draw source
	r = 25;
	Rectangle<int> rect(gridRect.getCentreX() + gridRect.getWidth() * 0.5 * sin(sourceAzimuth) - r * 0.5, gridRect.getCentreY() - gridRect.getHeight() * 0.5 * cos(sourceAzimuth) - r * 0.5, r, r);
	g.setColour(Colours::grey);
	g.fillEllipse(rect.toFloat());
	g.setColour(Colours::black);
	g.drawFittedText(String("S"), rect, Justification::centred, 1);

	// draw "head"
	r = 50;
	g.drawImage(headIcon, gridRect.getCentreX() - r / 2, gridRect.getCentreY() - r / 2, r, r, 0, 0, 100, 100);
}


HrtfBiAuralAudioProcessorEditor::HrtfBiAuralAudioProcessorEditor(HrtfBiAuralAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p)
{

	setSize(320, 400);

	elevationSlider.setSliderStyle(Slider::RotaryVerticalDrag);
	elevationSlider.setRotaryParameters(PI, PI * 2, true);
	elevationSlider.setRange(-90, 90, 0.1);
	elevationSlider.setValue(0);
	elevationSlider.setTextBoxStyle(Slider::TextBoxBelow, true, 50, 20);
	elevationSlider.addListener(this);
	elevationSlider.setName("Elevation");
	addAndMakeVisible(elevationSlider);

	crossoverSlider.setSliderStyle(Slider::LinearHorizontal);
	crossoverSlider.setTextBoxStyle(Slider::TextBoxRight, true, 50, 20);
	crossoverSlider.setRange(20, 2000, 1);
	crossoverSlider.setValue(p.crossover.f0);
	crossoverSlider.addListener(this);
	crossoverSlider.setName("Crossover");
	addAndMakeVisible(crossoverSlider);

	grid.addMouseListener(this, false);
	addAndMakeVisible(grid);

	auto& subjectsList = p.hrtfContainer.getSubjects();
	for (auto& subject : subjectsList)
		hrtfList.addItem(subject.toShortString(), subject.id);
	hrtfList.addListener(this);
	hrtfList.setSelectedId(21);
	addAndMakeVisible(hrtfList);
}

HrtfBiAuralAudioProcessorEditor::~HrtfBiAuralAudioProcessorEditor()
{
}

//==============================================================================
void HrtfBiAuralAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(Colours::white);
	g.setColour(Colours::black);
	g.setFont(15.0f);

	g.drawRect(grid.getX(), grid.getY(), getWidth() - 20, grid.getHeight());

	g.setFont(15.0f);
	g.drawFittedText("Current Subject", hrtfList.getX(), hrtfList.getY() - 20, hrtfList.getWidth(), 20, Justification::centred, 1);
	g.setFont(13.0f);
	g.drawFittedText(processor.hrtfContainer.getCurrentSubject().toLongString(), hrtfList.getX(), hrtfList.getBottom() + 10, hrtfList.getWidth(), 30, Justification::left, 1);
	g.drawFittedText(String("Elevation [deg]"), elevationSlider.getX(), elevationSlider.getY() - 20, elevationSlider.getWidth(), 10, Justification::centredTop, 1);
	g.drawFittedText(String("Crossover cutoff [Hz] "), crossoverSlider.getX(), crossoverSlider.getY() - 10, crossoverSlider.getWidth(), 10, Justification::left, 1);
}

void HrtfBiAuralAudioProcessorEditor::resized()
{
	int x0 = 10;
	hrtfList.setBounds(10, 20, getWidth() - 20, 30);
	grid.setBounds(x0, 100, 200, 200);
	elevationSlider.setBounds(grid.getRight(), grid.getY() + grid.getHeight() / 2 - 40, 100, 100);
	crossoverSlider.setBounds(x0, grid.getBottom() + 20, getWidth() - 2 * x0, 50);
}

void HrtfBiAuralAudioProcessorEditor::mouseDrag(const MouseEvent &event)
{
	auto pos = event.getEventRelativeTo(&grid).position;
	if (grid.contains(Point<int>(pos.x, pos.y)))
	{
		auto x = pos.x - grid.centre().x;
		auto y = grid.centre().y - pos.y;
		azimuth = std::atan2(x, y);
		grid.sourceAzimuth = azimuth;
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

void HrtfBiAuralAudioProcessorEditor::comboBoxChanged(ComboBox* comboBox)
{
	processor.hrtfContainer.setCurrentSubject(comboBox->getSelectedId());
	updateHrir();
	repaint();
}

void HrtfBiAuralAudioProcessorEditor::updateHrir()
{
	auto x = cos(elevation) * sin(azimuth);
	auto y = cos(elevation) * cos(azimuth);
	auto z = sin(elevation);

	auto azmHeadPolar = rad2deg(asin(x));
	auto elvHeadPolar = rad2deg(atan2(z, y));
	if (y < 0 && z < 0)
		elvHeadPolar += 360;

	processor.updateHrir(azmHeadPolar, elvHeadPolar);
}
