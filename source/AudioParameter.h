#pragma once
#include <atomic>
#include "../JuceLibraryCode/JuceHeader.h"


class AudioParameter :
	public AudioProcessorParameter
{
public:
	AudioParameter(String name, String label, float minValue, float maxValue, float defaultValue);

public: // AudioProcessorParameter implementation
	float getValue() const override;
	void setValue(float newValueNormalized) override;
	float getDefaultValue() const override;
	String getName(int maximumStringLength) const override;
	String getLabel() const override;
	float getValueForText(const String& text) const override;

public:
	void setValueAndNotifyHost(float newValue);
	float value() const;

	float minValue() const;
	float maxValue() const;
	float defaultValue() const;

private:
	String name_;
	String label_;

	std::atomic<float> valueNormalized_;
	float minValue_;
	float maxValue_;
	float defaultValue_;
};
