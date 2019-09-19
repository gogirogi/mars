#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioThumbnailComp.h"

//==============================================================================
AudioThumbnailComp::AudioThumbnailComp(AudioFormatManager& formatManager,
	AudioTransportSource& transport, AudioThumbnailCache& thumbCache,
	const File& existingFile)
	: transportSource(transport), thumbnail(512, formatManager, thumbCache)
{
	thumbnail.addChangeListener(this);

	addAndMakeVisible(&currentPositionMarker);
	currentPositionMarker.setFill(Colours::white.withAlpha(0.85f));

	setFile(existingFile);
}

AudioThumbnailComp::~AudioThumbnailComp()
{
	thumbnail.removeChangeListener(this);
}

void AudioThumbnailComp::paint(Graphics& g)
{
	g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).darker());
	g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId).brighter());

	if (thumbnail.getTotalLength() > 0.0)
	{
		Rectangle<int> thumbArea(getLocalBounds());
		thumbnail.drawChannels(g, thumbArea.reduced(2),
			0.0, thumbnail.getTotalLength(), 1.0f);
	}
	else
	{
		g.setFont(14.0f);
		g.drawFittedText("Drop a file here", getLocalBounds(), Justification::centred, 2);
	}
}

void AudioThumbnailComp::resized()
{
}

void AudioThumbnailComp::changeListenerCallback(ChangeBroadcaster *)
{
	repaint();
}

void AudioThumbnailComp::filesDropped(const StringArray & files, int x, int y)
{
	lastFileDropped = File(files[0]);
	sendChangeMessage();
}

void AudioThumbnailComp::timerCallback()
{
	updateCursorPosition();
}

void AudioThumbnailComp::mouseDown(const MouseEvent & e)
{
	mouseDrag(e);
}

void AudioThumbnailComp::mouseDrag(const MouseEvent & e)
{
	transportSource.setPosition(jmax(0.0f, xToTime(e.x)));
}

void AudioThumbnailComp::mouseUp(const MouseEvent & e)
{
	transportSource.start();
}

void AudioThumbnailComp::setFile(const File & file)
{
	if (file.existsAsFile())
	{
		thumbnail.setSource(new FileInputSource(file));
		if (transportSource.isPlaying())
		{
			transportSource.setPosition(0.0);

			transportSource.start();
		}
		startTimerHz(40);
	}
}

File AudioThumbnailComp::getLastDroppedFile() const noexcept
{
	return lastFileDropped;
}

void AudioThumbnailComp::updateCursorPosition()
{
	if (thumbnail.getNumChannels() != 0)
	{
		currentPositionMarker.setVisible(true);
		currentPositionMarker.setRectangle(Rectangle<float>(timeToX() - 1.0f, 0, 2.0f, getHeight()));
	}
}

float AudioThumbnailComp::timeToX() const
{
	auto x{ transportSource.getCurrentPosition() / transportSource.getLengthInSeconds()*getWidth() };
	return x;
}

float AudioThumbnailComp::xToTime(const float x) const
{
	auto xt{ x / getWidth()*transportSource.getLengthInSeconds() };
	return xt;
}