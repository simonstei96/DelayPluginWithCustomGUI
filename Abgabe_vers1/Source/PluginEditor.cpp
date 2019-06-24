/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleDelayAudioProcessorEditor::SimpleDelayAudioProcessorEditor (SimpleDelayAudioProcessor& p, float time, float feedb, float drywet)
    : AudioProcessorEditor (&p), processor (p)
{
    
	//Fenstergroeﬂe festlegen
    setSize (200, 200);

	//Wertebereich von Verzoegerungszeit und Feedback im Controlpane festlegen
	pane.setMinMax(100, 1000, 0, 95);
	//Aktuelle Werte uebergeben
	pane.setValue(time, feedb);

	//Wertebereich von Slider festelgen
	mix.setRange(0, 100, 1);
	//Aktuellen Wert setzen
	mix.setValue(round(drywet));
	//Textbox, welche den aktuellen Sliderwert anzeigt, festlegen (Groeﬂe und Position)
	mix.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxLeft, false, 30, 20);
	//Standardschriftfarbe geht im Hintergrundbild unter, daher auf durchsetzungsfaehigere Farbe gesetzt
	mix.setColour(Slider::ColourIds::textBoxTextColourId, Colours::black);

	//Editor als Listener von der ControlPane hinzufuegen
	pane.addChangeListener(this);

	//Slider an Wert im ValueTree binden
	mixAttachment = new AudioProcessorValueTreeState::SliderAttachment(processor.getValueTreeState(), "mixID", mix);

	//Slider und Pane auf Oberflaeche anzeigen
	addAndMakeVisible(mix);
	addAndMakeVisible(pane);
	
}

SimpleDelayAudioProcessorEditor::~SimpleDelayAudioProcessorEditor()
{
	//Sollte der Editor zerstoert werden, so auch diesen Listener aus ControlPane entfernen
	pane.removeChangeListener(this);
}

//==============================================================================
void SimpleDelayAudioProcessorEditor::paint (Graphics& g)
{
    //Kompletes Graphics-Objekt mit einer Farbe fuellen
	g.fillAll(Colours::cornflowerblue);

	//BackgroundImg auf der Oberflaeche zeichnen
	Image img = ImageCache::getFromMemory(BinaryData::bg_new_jpg, BinaryData::bg_new_jpgSize);
	g.drawImageAt(img, 0, 0);
	

  
}

void SimpleDelayAudioProcessorEditor::resized()
{
    //Position(Bounds) von Kindelementen setzen

	//ControlPane groﬂe berechnen
	int paneX = 10;
	int paneY = 10;
	int paneWidth = getLocalBounds().getWidth() - (2.0 * paneX);
	int paneHeight = (getLocalBounds().getHeight() *2/ 3) - paneY;

	Rectangle<int> paneArea(paneX, paneY, paneWidth, paneHeight);
	//Position und Groﬂe der ControlPane setzen
	pane.setBounds(paneArea);
	
	//Position und Groeﬂe des Sliders setzen
	mix.setBounds(paneX, 150, paneWidth, 30);
	
}

//Callback der ControlPane
void SimpleDelayAudioProcessorEditor::changeListenerCallback(ChangeBroadcaster * cb) {

	//ueberpruefen, ob es auch der richtige Broadcast ist
	if (cb == &pane) {
		//Veraenderter DelaytimeWert im ValueTree aktualisieren
		Value time = processor.getValueTreeState().getParameterAsValue("timeID");
		time = pane.getXValue();

		//Veraenderter FeedbackWert im ValueTree aktualisieren
		Value feedback = processor.getValueTreeState().getParameterAsValue("feedbackID");
		feedback = pane.getYValue();
	
	}
}

//Sollte auf das MainWindows geklickt werden, so verlieren alle anderen Elemente(Kinder) ihren Focus
//TextBoxen der Controlpane wurden sonst noch im Eingabemodus sein
void SimpleDelayAudioProcessorEditor::mouseDown(const MouseEvent& event) {
	setWantsKeyboardFocus(true);
}