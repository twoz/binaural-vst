#include "AtomicAudioParameter.h"
#include "Util.h"


AtomicAudioParameter::AtomicAudioParameter(String name, String label, float minValue, float maxValue, float defaultValue)
	: name_(name)
	, label_(label)
	, minValue_(minValue)
	, maxValue_(maxValue)
	, defaultValue_(defaultValue)
{
	valueNormalized_ = mapToRange(defaultValue, minValue, maxValue, 0.f, 1.f);
}

float AtomicAudioParameter::getValue() const
{
	return valueNormalized_;
}

void AtomicAudioParameter::setValue(float newValueNormalized)
{
	valueNormalized_.store(newValueNormalized);
}

float AtomicAudioParameter::getDefaultValue() const
{
	return mapToRange(defaultValue_, minValue_, maxValue_, 0.f, 1.f);
}

String AtomicAudioParameter::getName(int maximumStringLength) const
{
	return name_.dropLastCharacters(name_.length() - maximumStringLength);
}

String AtomicAudioParameter::getLabel() const
{
	return label_;
}

float AtomicAudioParameter::getValueForText(const String& text) const
{
	return mapToRange(text.getFloatValue(), minValue_, maxValue_, 0.f, 1.f);
}

void AtomicAudioParameter::setValueAndNotifyHost(float newValue)
{
	setValueNotifyingHost(mapToRange(newValue, minValue_, maxValue_, 0.f, 1.f));
}

float AtomicAudioParameter::value() const
{
	return mapToRange(valueNormalized_.load(), 0.f, 1.f, minValue_, maxValue_);
}

float AtomicAudioParameter::minValue() const
{
	return minValue_;
}

float AtomicAudioParameter::maxValue() const
{
	return maxValue_;
}

float AtomicAudioParameter::defaultValue() const
{
	return defaultValue_;
}
