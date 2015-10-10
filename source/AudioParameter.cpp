#include "AudioParameter.h"
#include "Util.h"


AudioParameter::AudioParameter(String name, String label, float minValue, float maxValue, float defaultValue)
	: name_(name)
	, label_(label)
	, minValue_(minValue)
	, maxValue_(maxValue)
	, defaultValue_(defaultValue)
{
	valueNormalized_ = mapToRange(defaultValue, minValue, maxValue, 0.f, 1.f);
}

float AudioParameter::getValue() const
{
	return valueNormalized_;
}

void AudioParameter::setValue(float newValueNormalized)
{
	valueNormalized_.store(newValueNormalized);
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

void AudioParameter::setValueAndNotifyHost(float newValue)
{
	setValueNotifyingHost(mapToRange(newValue, minValue_, maxValue_, 0.f, 1.f));
}

float AudioParameter::value() const
{
	return mapToRange(valueNormalized_.load(), 0.f, 1.f, minValue_, maxValue_);
}

float AudioParameter::minValue() const
{
	return minValue_;
}

float AudioParameter::maxValue() const
{
	return maxValue_;
}

float AudioParameter::defaultValue() const
{
	return defaultValue_;
}
