//==============================================================================

//==============================================================================

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <string>

//==============================================================================
class PositionTime : public Component, public Timer
{
public:
	PositionTime(const AudioTransportSource& transportSourceToUse)
		: transportSource(transportSourceToUse)
	{
		addAndMakeVisible(&playtimeLabel);
		playtimeLabel.setFont(Font(20, Font::bold));
		playtimeLabel.setJustificationType(Justification::centred);
		playtimeLabel.setText("00:00:000", dontSendNotification);
		playtimeLabel.setInterceptsMouseClicks(false, false);
		startTimer(130);
	}

	void paint(Graphics& g) override
	{
	}

	void resized() override
	{
		playtimeLabel.setBounds(getLocalBounds());
	}

	void timerCallback() override
	{
		float time;
		if (reverseTime)
		{
			time = transportSource.getLengthInSeconds() - transportSource.getCurrentPosition();
		}
		else
		{
			time = transportSource.getCurrentPosition();
		}
		RelativeTime position(time);
		auto minutes{ ((int)position.inMinutes()) % 60 };
		auto seconds{ ((int)position.inSeconds()) % 60 };
		auto m{ (int)((((int)position.inMilliseconds()) % 1000)*0.01) };

		auto positionString{ String::formatted("%02d:%02d:%01d", minutes, seconds, m) };
		if (reverseTime)
		{
			positionString = "-" + positionString;
		}
		playtimeLabel.setText(positionString, dontSendNotification);
	}

	void mouseDown(const MouseEvent& e) override
	{
		//DBG("mouse test");
		reverseTime = !reverseTime;
	}
private:

	Label playtimeLabel;
	String timeToDisplay;
	bool reverseTime{ false };

	const AudioTransportSource& transportSource;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionTime)
};

//==============================================================================
class MarsAudioProcessorEditor : public AudioProcessorEditor, private ChangeListener
{
public:
	MarsAudioProcessorEditor(MarsAudioProcessor&);
	~MarsAudioProcessorEditor();

	//==============================================================================
	void paint(Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster* source) override;

	void playButtonClicked();
	void pauseButtonClicked();
	void stopButtonClicked();

private:
	MarsAudioProcessor& processor;

	std::unique_ptr<AudioThumbnailComp> thumbnail;
	TextButton playPauseButton;
	TextButton pauseButton;
	TextButton stopButton;
	PositionTime positionTime;
	Slider volumeSlider;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarsAudioProcessorEditor)
};
