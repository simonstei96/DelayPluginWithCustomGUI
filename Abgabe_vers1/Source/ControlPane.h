/*
  ==============================================================================

    ControlPane.h
    Created: 13 Jan 2019 7:04:43pm
    Author:  Simon

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/*
Eigene Komponente, die es ermoeglicht mit Hilfe eines Trackballs (Punkt) auf einer rechteckigen Flaeche
zwei Werte anhand von x und y Achse auf einen gewissen Werte in einem Wertebereich abzubilden.
Die Komponente enthaelt eine rechteckigen Bereich, in diesem Bereich kann mit der Maus ein Trackball bewegt werden, in zwei 2 TextBoxen werden
der Achsenkoordinate entsprechend der passende Wert angezeigt.
*/
class ControlPane    : public Component, public Component::MouseListener, public ChangeBroadcaster, public TextEditor::Listener
{
public:
    ControlPane();
    ~ControlPane();

	//Zeichnen auf der Oberflaeche
    void paint (Graphics&) override;
    void resized() override;

	//Wertebereichlimits für xValue und yValue setzen
	void setMinMax(float xMin, float xMax, float yMin, float yMax);
	//Getter fuer yValue
	float getXValue();
	//Getter fuer yValue
	float getYValue();
	//Setter fuer xValue und yValue
	void setValue(float x, float y);

	//ObserverPattern Methoden
	//Listener zur Listenerlist hinzufuegen
	void addChangeListener(ChangeListener* listener);
	//Listener aus Listenerlist entfernen
	void removeChangeListener(ChangeListener* listener);


private:
	//Trackball Mittelpunkt
	Point<int> position;
	//Bereich in dem sich der Mittelpunkt des Trackballs befinden darf
	Rectangle<int> limitArea;
	//Zustandshilfvariable fuer das erste Zeichnen
	bool firstDraw;
	//Breite und Hoehe des Trackballs
	int sizeTrackball;
	//Wertelimits fuer xValue/yValue, [0] := MIN , [1] := MAX
	float xValueMinMax[2],yValueMinMax[2];
	//Werte
	float xValue, yValue;

	//TextBoxen die den aktuellen Werte anzeigen 
	TextEditor te_val1, te_val2;
	
	//Mittelpunkt des Trackballs setzen
	void setPosition(int xP, int yP);
	//Berechnen von Achsenposition zu richtigem Wert
	void convertAxisToValue();
	//Berechnen von Wert zu Achsenposition abhaengig von welcher wert sich geaendert hat
	void convertValueToAxis(bool x , bool y);
	//Richtige Werte in den Textboxen anzeigen
	void displayValues();
	
	//Mouse Listener fuer ControlPane
	void mouseDrag(const MouseEvent &e) override;
	void mouseDown(const MouseEvent &e) override;

	//Listener fuer TextEditorKeyboardInput
	void textEditorReturnKeyPressed(TextEditor& te) override;
	void textEditorFocusLost(TextEditor& te) override;
	
	//ListenerList, enthaelt alle angemeldeten Listener
	ListenerList<ChangeListener> listenerList;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControlPane)
};
