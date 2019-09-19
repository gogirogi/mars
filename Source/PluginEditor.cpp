#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MarsAudioProcessorEditor::MarsAudioProcessorEditor(MarsAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p), positionTime(p.transportSource)
{
	thumbnail = std::make_unique<AudioThumbnailComp>(processor.formatManager,
		processor.transportSource, processor.thumbnailCache, processor.currentlyLoadedFile);
	addAndMakeVisible(thumbnail.get());
	thumbnail->addChangeListener(this);

	addAndMakeVisible(&playPauseButton);
	playPauseButton.setButtonText("Play");
	playPauseButton.onClick = [this] { playButtonClicked(); };

	addAndMakeVisible(&pauseButton);
	pauseButton.setButtonText("Pause");
	pauseButton.onClick = [this] { pauseButtonClicked(); };

	addAndMakeVisible(&stopButton);
	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };

	addAndMakeVisible(&positionTime);

	addAndMakeVisible(&volumeSlider);
	volumeSlider.setSliderStyle(Slider::LinearBar);

	processor.transportSource.addChangeListener(this);

	setResizeLimits(450, 150, 1200, 300);

	//getConstrainer()->setFixedAspectRatio(2.5);
	setSize(600, 150);
}

MarsAudioProcessorEditor::~MarsAudioProcessorEditor()
{
	thumbnail->removeChangeListener(this);
	processor.transportSource.removeChangeListener(this);
}

//==============================================================================
void MarsAudioProcessorEditor::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MarsAudioProcessorEditor::resized()
{
	auto a = getLocalBounds().reduced(4);

	auto aTransport = a.removeFromBottom(32).reduced(2);
	playPauseButton.setBounds(aTransport.removeFromLeft(60));
	pauseButton.setBounds(aTransport.removeFromLeft(60));
	stopButton.setBounds(aTransport.removeFromLeft(60));
	volumeSlider.setBounds(aTransport.removeFromRight(150));
	positionTime.setBounds(aTransport);

	thumbnail->setBounds(a);
}

void MarsAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster * source)
{
	if (source == thumbnail.get())
	{
		processor.loadFileIntoTransport(thumbnail->getLastDroppedFile());
		thumbnail->setFile(thumbnail->getLastDroppedFile());
	}
	if (source == &processor.transportSource && processor.transportSource.hasStreamFinished())
	{
		processor.transportSource.setPosition(0);
	}
}

void MarsAudioProcessorEditor::playButtonClicked()
{
	if (processor.transportSource.isPlaying())
	{
		processor.transportSource.setPosition(0);
	}
	else
	{
		processor.transportSource.setPosition(0);
		processor.transportSource.start();
	}
}

void MarsAudioProcessorEditor::pauseButtonClicked()
{
	if (processor.transportSource.getCurrentPosition() > 0.0)
	{
		if (processor.transportSource.isPlaying())
		{
			processor.transportSource.stop();
			pauseButton.setButtonText("Resume");
		}
		else
		{
			processor.transportSource.start();
			pauseButton.setButtonText("Pause");
		}
	}
}

void MarsAudioProcessorEditor::stopButtonClicked()
{
	processor.transportSource.stop();
	processor.transportSource.setPosition(0);
}