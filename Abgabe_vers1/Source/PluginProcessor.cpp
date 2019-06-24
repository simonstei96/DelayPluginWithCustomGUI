/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessor::SimpleDelayAudioProcessor(): 
	AudioProcessor (BusesProperties()
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                       .withOutput ("Output", AudioChannelSet::stereo(), true))
	
{
	//Initialiserung der Datenstruktur
	//Die Datenstruktur macht es moeglich, mit Hilfe einer DAW die Regler/Werte automatisiert zu veraendern
	parameters = new AudioProcessorValueTreeState(*this, nullptr);
	//3 Parameter f¸r Zeitverzoegerung, Feedback und Mixlautstaerke erstellen und hinzufuegen
	//Die ID-Bezeichner sind wichtig fuer den spaeteren zugriff auf die Werte
	//Mit der NormalisableRange wird angegeben in welchem Bereich sich die jeweiligen Werte befinden duerfen
	//createAndAddParamter(ID, Anzeigename in DAW, nicht relevant, Wertebereich, Startwert, unrelevant, unrelevant)
	parameters->createAndAddParameter("timeID", "Delay Time", "timeLabel", NormalisableRange<float>(100.0f, 1000.0f,1.0f), 100.0f, nullptr, nullptr);
	parameters->createAndAddParameter("feedbackID", "Feedback", "feedbackLabel", NormalisableRange<float>(0.0f, 95.0f, 1.0f), 0.0f, nullptr, nullptr);
	parameters->createAndAddParameter("mixID", "Mix", "mixLabel", NormalisableRange<float>(0.0f, 100.0f, 1.0f), 50.0f, nullptr, nullptr);
	//State muss damit initialisert werden nachdem Parameter hinzugefuegt wurden
	parameters->state = ValueTree("ParameterValues");
}

SimpleDelayAudioProcessor::~SimpleDelayAudioProcessor()
{
	
}

//==============================================================================
const String SimpleDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String SimpleDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SimpleDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //Initialisierung des Delayobjekts mit Anzahl der Kan‰le und Samplerate der DAW
	d.init(getTotalNumInputChannels(), sampleRate);
}

void SimpleDelayAudioProcessor::releaseResources()
{
   
	//Alte Werte im Delaybuffer loeschen 
	d.stop();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif
/*
param 1: buffer enthaelt Inputsamples, welche gerade durch das PLugin wandern, ist gleichzeitig auch Outputbuffer
param 2: EingangsMIDIsignal, da das Plugin aber nur 2 EIn- und Ausgaenge besitz ist dies nicht von relevanz
*/
void SimpleDelayAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

	//Groﬂe bzw. Anzahl der Samples pro Kanal im Eingangsbuffer bestimmen
	int inputNumSamples = buffer.getNumSamples();

	//Erstellen eines Audiobuffers zur speicherung der aktuellen Vergangenen Signale
	AudioBuffer<float> delayedSignal;
	//Initialisierung, so dass er die gleiche Groeﬂe wie Eingangsbuffer hat
	delayedSignal.setSize(totalNumInputChannels, inputNumSamples, false, true);
	
    //Bearbeitung der eingehenden Samples 
	//Die aeussere Schleife kann entweder pro Kanal oder pro Sample iterieren
	//Fuer dieses Plugin bietet sich pro Kanal an fuer jedes Sample die Delayoperationen zu rechenaufwendig werden.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //Pointer zum schreiben/lesen von den Eingangsbuffer bekommen
		float* inputData = buffer.getWritePointer (channel);

		//Die Samples aus dem Eingangsbuffer in den Kreisspeicher des Delayobjekts schreiben
		d.setBufferData(channel, inputNumSamples, inputData);
		
		//Zeitverzoegerungswert besorgen
		float delayTimeVal = *parameters->getRawParameterValue("timeID");
		
		//Die vergangenen Samples im Kreisspeicher des Delayobjekts in den delaySignal-Buffer schreiben
		d.getBufferData(channel, inputNumSamples,delayTimeVal, delayedSignal);
		//Dieser Buffer enthaelt nun die Samples von vor 100ms oder 1000ms (oder jenachdem welcher bereich angegeben wird)
		
		//Feedbackwert besorgen
		float feedbackVal = *parameters->getRawParameterValue("feedbackID");
		//Die vergangenen Samples aus delayedSignal-Buffer in den Kreispseicher hinzufuegen ,
		//an der selben Position wie die aktuellen Eingangssignale gespeichert wurden
		d.addFeedbackData(channel, inputNumSamples, feedbackVal/100.0f, delayedSignal.getReadPointer(channel));

		//Der Lautstaerkewert der verzoegerten Samples holen
		float mixVal = *parameters->getRawParameterValue("mixID");
		//Alle Samples an die angegebene prozentualle Lautstaerke anpassen
		delayedSignal.applyGain(mixVal/100.0f);
		
		//Zu dem Eingangbuffer die verzoegerten Samples aus dem delaySignal-Buffer hinzufuegen
		buffer.addFrom(channel, 0, delayedSignal, channel, 0, inputNumSamples);
		//Ein direkter Aufrufe das dieser Buffer der Output ist muss nicht passieren
    }
	//Nach dem Bearbeiten dieses Sampleblocks die Schreibeposition verschieben
	d.updateBufferPos(inputNumSamples);
}

//==============================================================================
bool SimpleDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SimpleDelayAudioProcessor::createEditor()
{
	//Der AudioProcessor arbeitet auch wenn die GUI nicht geoeffnet ist
	//Zur richtigen Anzeige der Werte muessen die aktuellen Werte an die GUI beim Erstellen uebergeben werden

	//Lesen der aktuellen Werte
	float delay =  *parameters->getRawParameterValue("timeID");
	float feedback = *parameters->getRawParameterValue("feedbackID");
	float mix = *parameters->getRawParameterValue("mixID");
	//GUI erstellen
    return new SimpleDelayAudioProcessorEditor (*this, delay, feedback, mix);
}

//==============================================================================
void SimpleDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    //Persistentes speichern von Werten mit Hilfe einer Konvertierung zu XML

	ValueTree data = parameters->copyState();
	std::unique_ptr<XmlElement> xml(data.createXml());
	copyXmlToBinary(*xml, destData);
}

void SimpleDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
   //Laden der gespeicherten Werte

	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(parameters->state.getType()))
			parameters->replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleDelayAudioProcessor();
}

AudioProcessorValueTreeState& SimpleDelayAudioProcessor::getValueTreeState() {
	//Referenz zu diesem Datenstruktur/ValueTree zurueckliefern
	return *parameters;
}
