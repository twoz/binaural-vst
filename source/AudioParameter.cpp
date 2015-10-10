#include "AudioParameter.h"
#include "Util.h"


AudioParameter::AudioParameter(String name, String label, float minFreqHz, float maxFreqHz, float defaultFreqHz)
	: name_(name)
	, label_(label)
	, value_(defaultFreqHz)
	, minValue_(minFreqHz)
	, maxValue_(maxFreqHz)
	, defaultValue_(defaultFreqHz)
{
}

float AudioParameter::getValue() const
{
	return mapToRange(value_.load(), minValue_, maxValue_, 0.f, 1.f);
}

void AudioParameter::setValue(float newValue)
{
	value_.store(mapToRange(newValue, 0.f, 1.0f, minValue_, maxValue_));
}

float AudioParameter::getDefaultValue() const
{
	return mapToRange(defaultValue_, minValue_, maxValue_, 0.f, 1.f);
}

String AudioParameter::getName(int maximumStringLength) const
{
	return name_.dropLastCharacters(name_.length() - maximumStringLength);
}

String AudioParameter::getLabel() const
{
	return label_;
}

float AudioParameter::getValueForText(const String& text) const
{
	return mapToRange(text.getFloatValue(), minValue_, maxValue_, 0.f, 1.f);
}

void AudioParameter::setValueTo(float newFreqHz)
{
	value_.store(newFreqHz);
}

float AudioParameter::value() const
{
	return value_.load();
}

float AudioParameter::minValue() const
{
	return minValue_;
}

float AudioParameter::maxValue() const
{
	return maxValue_;
}
