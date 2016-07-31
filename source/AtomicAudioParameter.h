#pragma once
#include <atomic>
#include "../JuceLibraryCode/JuceHeader.h"


class AtomicAudioParameter : public AudioProcessorParameter
{
public:
	AtomicAudioParameter(String name, String label, NormalisableRange<float> range, float defaultValue);

	void setNewValue(float newValue);
	float getValueAndMarkRead();
	bool hasNewValue() const { return state.load().valueChangedSinceLastRead == 1? true : false; }

	const NormalisableRange<float>& getRange() const { return range; }
	String getLabel() const override { return label; }
	float getDefault() const { return defaultValue; }

private:
	float getValue() const override;
	float getDefaultValue() const override;
	void setValue(float newValueNormalized) override;
	String getName(int maximumStringLength) const override;
	float getValueForText(const String& text) const override;

	String name;
	String label;
	NormalisableRange<float> range;
	float defaultValue;
	struct AtomicAudioParameterState
	{
		float value;
		int valueChangedSinceLastRead;
	};
	std::atomic<AtomicAudioParameterState> state;
};
