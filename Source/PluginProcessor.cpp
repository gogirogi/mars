#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MarsAudioProcessor::MarsAudioProcessor()
	: AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)),
	thumbnailCache(1)
{
	formatManager.registerBasicFormats();
}

MarsAudioProcessor::~MarsAudioProcessor()
{
	transportSource.setSource(nullptr);
}

//==============================================================================
const String MarsAudioProcessor::getName() const { return JucePlugin_Name; }
bool MarsAudioProcessor::acceptsMidi() const { return false; }
bool MarsAudioProcessor::producesMidi() const { return false; }
bool MarsAudioProcessor::isMidiEffect() const { return false; }
double MarsAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int MarsAudioProcessor::getNumPrograms() { return 1; }
int MarsAudioProcessor::getCurrentProgram() { return 0; }
void MarsAudioProcessor::setCurrentProgram(int index) {}
const String MarsAudioProcessor::getProgramName(int index) { return {}; }
void MarsAudioProcessor::changeProgramName(int index, const String& newName) {}

//==============================================================================
void MarsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	transportSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void MarsAudioProcessor::releaseResources()
{
	transportSource.releaseResources();
}

bool MarsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
		return false;
	return true;
}

void MarsAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;

	for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	transportSource.setGain(0.1);

	transportSource.getNextAudioBlock(AudioSourceChannelInfo(buffer));
}

//==============================================================================
bool MarsAudioProcessor::hasEditor() const { return true; }
AudioProcessorEditor* MarsAudioProcessor::createEditor()
{
	return new MarsAudioProcessorEditor(*this);
}

//==============================================================================
void MarsAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	XmlElement xml("MARS-settings");
	xml.setAttribute("audiofile", currentlyLoadedFile.getFullPathName());
	copyXmlToBinary(xml, destData);
}

void MarsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState != nullptr)
	{
		if (xmlState->hasTagName("MARS-settings"))
		{
			currentlyLoadedFile = File::createFileWithoutCheckingPath(xmlState->getStringAttribute("audiofile"));
			if (currentlyLoadedFile.existsAsFile())
			{
				loadFileIntoTransport(currentlyLoadedFile);
			}
		}
	}
}

void MarsAudioProcessor::loadFileIntoTransport(const File & audioFile)
{
	// unload the previous file source and delete it..
	//transportSource.stop();
	transportSource.setSource(nullptr);
	currentAudioFileSource = nullptr;

	AudioFormatReader* reader = formatManager.createReaderFor(audioFile);
	currentlyLoadedFile = audioFile;

	if (reader != nullptr)
	{
		currentAudioFileSource = std::make_unique<AudioFormatReaderSource>(reader, true);

		// ..and plug it into our transport source
		transportSource.setSource(
			currentAudioFileSource.get(),
			0,                   // tells it to buffer this many samples ahead
			nullptr,        // this is the background thread to use for reading-ahead
			reader->sampleRate);     // allows for sample rate correction
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new MarsAudioProcessor();
}