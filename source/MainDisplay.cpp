#include "MainDisplay.h"


MainDisplay::MainDisplay(HrtfBiAuralAudioProcessorEditor* editor)
	:
	editor_(editor),
	sourcePos_({1.0, 0.0, 0.0})
{
	sourceImage_ = ImageFileFormat::loadFrom(source_icon_png, source_icon_png_size);
	headImage_ = ImageFileFormat::loadFrom(head_top_png, head_top_png_size);
}

MainDisplay::~MainDisplay()
{
}

void MainDisplay::paint(Graphics& g)
{
	g.fillAll(Colours::transparentBlack);

	g.setOpacity(1.0f);
	g.setColour(Colours::black);
	auto w = getWidth();
	auto h = getHeight();
	g.fillEllipse(0, 0, w, h);

	if (sourcePos_.elevation < 0)
		drawSource(g);
	g.setOpacity(1.f);
	g.drawImageWithin(headImage_, 0, 0, w, h, RectanglePlacement::centred | RectanglePlacement::doNotResize);
	drawGridLines(g);
	if (sourcePos_.elevation >= 0)
		drawSource(g);
}

void MainDisplay::mouseDrag(const MouseEvent& event)
{
	auto pos = event.getPosition();
	auto x = pos.x - getWidth() * 0.5f;
	auto y = pos.y - getHeight() * 0.5f;
	sourcePos_.azimuth = std::atan2(x, -y);
	updateHRTF();
}

void MainDisplay::sliderValueChanged(Slider* slider)
{
	sourcePos_.elevation = deg2rad(slider->getValue());
	updateHRTF();
}

void MainDisplay::drawGridLines(Graphics& g)
{
	g.setColour(Colours::white);
	g.setOpacity(0.5f);
	auto w = getWidth();
	auto h = getHeight();
	auto lineHorizontal = Line<float>(0, h * 0.5f, w, h * 0.5f);
	auto lineVertical = Line<float>(w * 0.5f, 0, w * 0.5f, h);
	float dashes[] = {3, 2};
	g.drawDashedLine(lineHorizontal, dashes, 2);
	g.drawDashedLine(lineVertical, dashes, 2);
}

void MainDisplay::drawSource(Graphics& g)
{
	auto w = getWidth();
	auto h = getHeight();
	auto radius = w * 0.5f - 30;
	auto scale = sourcePos_.elevation / 90.f;
	auto x = radius * std::sin(sourcePos_.azimuth) * std::cos(sourcePos_.elevation);
	auto y = radius * std::cos(sourcePos_.azimuth) * std::cos(sourcePos_.elevation);
	x = w * 0.5f + x;
	y = h * 0.5f - y;
	auto color = editor_->fgColor_;
	if (sourcePos_.elevation < 0)
	{
		color = color.darker();
		g.setOpacity(0.8f);
	}
	ColourGradient grad(color, x, y, Colours::transparentBlack, x + w * 0.25f, y + h * 0.25f, true);
	g.setGradientFill(grad);
	g.fillEllipse(0, 0, w, h);
	auto scaleFactor = 0.75 * (std::sin(sourcePos_.elevation) * 0.25 + 1);
	auto sw = sourceImage_.getWidth();
	auto sh = sourceImage_.getHeight();
	g.drawImageWithin(sourceImage_,
		x - sw * 0.5f * scaleFactor,
		y - sh * 0.5f * scaleFactor,
		sw * scaleFactor,
		sh * scaleFactor,
		RectanglePlacement::centred,
		true);
}

void MainDisplay::updateHRTF()
{
	auto p = sphericalToInteraural(sourcePos_);
	editor_->processor_.updateHRTF(rad2deg(p.azimuth), rad2deg(p.elevation));
	repaint();
}
