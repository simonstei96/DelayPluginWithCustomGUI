/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "ControlPane.h"

//==============================================================================
/**
*/
class SimpleDelayAudioProcessorEditor  : public AudioProcessorEditor, public ChangeListener, public Component::MouseListener
{
public:
    SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor&, float time, float feedb, float drywet);
    ~SimpleDelayAudioProcessorEditor();

    //==============================================================================
	//Zeichnen auf der Oberflaeche
    void paint (Graphics&) override;
    void resized() override;

	//ControllPane Callback falls sich die Werte veraendert haben
	void changeListenerCallback(ChangeBroadcaster* cb) override;

	//Mouse Listener
	void mouseDown(const MouseEvent& event) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleDelayAudioProcessor& processor;

	//Slider fuer Lautstaerke von dem verzoegerten Signal
	Slider mix; 
	//Verbindungsglied zwischen Slider und einem Parameter eines AudioProcessorValueTreeState
	ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

	//ControlPane Objekt zur Steuerung von Verzoegerungszeit und Feedback
	ControlPane pane;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleDelayAudioProcessorEditor)
};
