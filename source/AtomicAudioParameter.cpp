#include "AtomicAudioParameter.h"


AtomicAudioParameter::AtomicAudioParameter(String name, String label, NormalisableRange<float> range, float defaultValue)
	: name(name)
	, label(label)
	, range(range)
	, defaultValue(defaultValue)
	, state({defaultValue, true})
{
	jassert(state.is_lock_free());
}

void AtomicAudioParameter::setNewValue(float newValue)
{
	setValueNotifyingHost(range.convertTo0to1(newValue));
}

float AtomicAudioParameter::getValueAndMarkRead()
{
	const auto currentVal = state.load().value;
	state.store({currentVal, 0});
	return currentVal;
}

float AtomicAudioParameter::getDefaultValue() const
{
	return defaultValue;
}

float AtomicAudioParameter::getValue() const
{
	return range.convertTo0to1(state.load().value);
}

void AtomicAudioParameter::setValue(float newValueNormalized)
{
	state.store({ range.convertFrom0to1(newValueNormalized), 1 });
}

String AtomicAudioParameter::getName(int maximumStringLength) const
{
	return name.dropLastCharacters(name.length() - maximumStringLength);
}

float AtomicAudioParameter::getValueForText(const String& text) const
{
	return range.convertTo0to1(text.getFloatValue());
}
