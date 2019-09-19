#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class AudioThumbnailComp : public Component, public ChangeListener,
	public ChangeBroadcaster, public FileDragAndDropTarget, public Timer
{
public:
	AudioThumbnailComp(AudioFormatManager& formatManager,
		AudioTransportSource& transport,
		AudioThumbnailCache& thumbCache,

		const File& existingFile = File());
	~AudioThumbnailComp();

	void paint(Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster*) override;
	bool isInterestedInFileDrag(const StringArray& files) override { return true; }
	void filesDropped(const StringArray& files, int x, int y) override;
	void timerCallback() override;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& e) override;

	void setFile(const File& file);
	File getLastDroppedFile() const noexcept;
	void updateCursorPosition();
	float timeToX() const;
	float xToTime(const float x) const;

private:
	AudioTransportSource& transportSource;

	AudioThumbnail thumbnail;
	File lastFileDropped;

	DrawableRectangle currentPositionMarker;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioThumbnailComp)
};
