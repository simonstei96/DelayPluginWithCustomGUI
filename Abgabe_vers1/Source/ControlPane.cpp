/*
  ==============================================================================

    ControlPane.cpp
    Created: 13 Jan 2019 7:04:43pm
    Author:  Simon

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "ControlPane.h"

//==============================================================================
ControlPane::ControlPane()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

	//ListenerList leeren
	listenerList.clear();

	//Initialisieren von Werten
	firstDraw = true;
	//Breite des Trackballs festlegen
	sizeTrackball = 20;


	//Schriftgroesse beider Textboxen einstellen
	Font f;
	f.setHeight(12);
	te_val1.setFont(f);
	te_val2.setFont(f);


	//Textausrichtung innerhalb von TextEditor Objekt auf rechts festlegen
	te_val1.setJustification(Justification::centredRight);
	te_val2.setJustification(Justification::centredRight);

	//Eingaberestriktion, nur Zahlen oder je nach Wertebereich auch Vorzeichen
	te_val1.setInputRestrictions(10, "1234567890-");
	te_val2.setInputRestrictions(10, "1234567890-");

	//TextEditor Listener
	te_val1.addListener(this);
	te_val2.addListener(this);

	//Textboxen sichtbar machen
	addAndMakeVisible(te_val1);
	addAndMakeVisible(te_val2);
}

ControlPane::~ControlPane()
{
	
}

void ControlPane::paint (Graphics& g)
{

	//Radius des Trackballs bekommen
	int offset = sizeTrackball / 2;

	//Flaeche fefstlegen
	Rectangle<int> area(getLocalBounds());
	//Hoehe reduzieren um Platz für Textboxen zu erzeugen
	area.setHeight(area.getHeight() - 20);
	//Rand reduzieren damit Trackball ueberstehen darf
	area.reduce(offset, offset);
	
	
	//Erster Aufruf, Steuerpunkt links unten zeichnen, limitArea festlegen, Veraenderungen anzeigen
	if (firstDraw) {
		
		limitArea = area;
		convertValueToAxis(true, true);
		setPosition(position.getX(), position.getY());
		displayValues();
		firstDraw = false;
	}


	//ControlPad zeichnen
	setAlpha(0.8); //Transparenz
	g.setColour(Colours::white); //Farbe fuer alle naechsten Zeichnungen festlegen
	g.fillRect(area);	//Ein gefuelltes Rechteck zeichnen 


	//Trackball
	//Flaeche des Punkts festlegen von Mittelpunkt ausgehend
	Rectangle<float> pointArea(position.getX()-offset, position.getY()-offset, sizeTrackball, sizeTrackball);
	//farbe des Trackballs
	g.setColour(Colours::cornflowerblue);
	//Zeichnen der Trackballs
	g.fillEllipse(pointArea);

}

void ControlPane::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

	//TextEditor Objekte Positionen/Groesse festlegen und setzen
	Rectangle<int> locBounds = getLocalBounds();
	te_val1.setBounds(sizeTrackball/2, locBounds.getHeight()-20, 50, 20);
	te_val2.setBounds((locBounds.getWidth()-(sizeTrackball/2)) - 50, locBounds.getHeight() - 20, 50, 20);
	
}

//Mouse Listener
void ControlPane::mouseDrag(const MouseEvent &e) {
	//Neue Position des Trackballs setzen
	setPosition(e.getPosition().getX(), e.getPosition().getY());
}

void ControlPane::mouseDown(const MouseEvent &e) {
	//Neue Position des Trackballs setzen sollte sich die Klickposition auf der Rechteckflaeche befinden
	if (limitArea.contains(e.getPosition())) {
		setPosition(e.getPosition().getX(), e.getPosition().getY());
	}
}


void ControlPane::setPosition(int xP, int yP) {

	//Limits von ControlPad bekommen
	int xLimit = limitArea.getX();
	int yLimit = limitArea.getY();
	int wLimit = xLimit + limitArea.getWidth() -1 ;
	int hLimit = yLimit + limitArea.getHeight()- 1;

	//Ueberpruefen ob Trackball sich in dem festgelegten Limit befindet
	Point<int> pNew;
	//X Koordiante ueberpruefen
	if (xP >= xLimit) { //Fall 1: X Coord nicht links vom Anfang der Box

		if (xP < wLimit) { //Fall 2: X Coord nicht rechts vom Ende der Box
			pNew.setX(xP);
			
		}
		else {	//Fall 3: X Coord rechts vom Ende der Box
			
			pNew.setX(wLimit);
		}
	}
	else {	//Fall 4: X Coord links vom Ende der Box
		pNew.setX(xLimit);
	}
	//Y Koordinate ueberpruefen
	if (yP >= yLimit) { //Fall 1: Y Coord nicht ueber dem Anfang der Box

		if (yP < hLimit) { //Fall 2: Y Coord nicht unter dem Ende der Box

			pNew.setY(yP);
		}
		else {	//Fall 3: Y Coord unter dem Ende der Box
			pNew.setY(hLimit);
		}
	}
	else {	//Fall 4: Y Coord ueber dem Ende der Box
		pNew.setY(yLimit);
	}

	//Falls Position sich veraendert hat
	if (pNew != position) {
		position = pNew;	//Neuer Punkt setzen
		repaint();	//GUI Component neu zeichnen
		convertAxisToValue(); //Werte berechnen/aktualisieren
		displayValues();	//Werte auf GUI anzeigen
	}

}


void ControlPane::convertAxisToValue()
{
	//y-Achsenwert auf richtigen Wert ableiten
	float max = limitArea.getHeight()-1;
	float value = max -( position.getY() - (sizeTrackball/2));
	float diff = yValueMinMax[1] - yValueMinMax[0];
	yValue= (diff*(value/max))+yValueMinMax[0];
	
	//x-Achsenwert auf richtigen Wert ableiten
	max = limitArea.getWidth()-1;
	value = position.getX() - (sizeTrackball / 2);
	diff = xValueMinMax[1] - xValueMinMax[0];
	xValue = (diff*(value / max)) + xValueMinMax[0];

	//Nur ganze Zahlen
	xValue =round(xValue);
	yValue = round(yValue);
	
	//Listener benachrichtigen
	listenerList.call([this](ChangeListener& cl) { 
		cl.changeListenerCallback(this); 
	});

	
}
//Paramter geben an welcher Wert sich veraendert hat
void ControlPane::convertValueToAxis( bool x, bool y) {

	//Richtigen y-Wert auf y-Achsenposition ableiten
	if (y==true) {
		float height = limitArea.getHeight()-1;
		float diff = yValueMinMax[1] - yValueMinMax[0];
		position.setY((sizeTrackball/2)+height-(height*((yValue-yValueMinMax[0])/diff)));
	}
	//Richtigen x-Wert auf x-Achsenposition ableiten
	if (x==true) {
		float width = limitArea.getWidth()-1;
		float diff = xValueMinMax[1] - xValueMinMax[0];
		position.setX((sizeTrackball/2)+ width*((xValue - xValueMinMax[0]) / diff));
		
	}

	//Listener benachrichtigen
	listenerList.call([this](ChangeListener& cl) {
		cl.changeListenerCallback(this);
	});
	//Oberflaeche neu zeichnen
	repaint();

}

void ControlPane::displayValues()
{
	//TextBox Text setzen
	te_val1.setText((String)xValue,false);
	te_val2.setText((String)yValue, false);
	//Keyboard Fokus auf die unsichtbare Komponente(Container: ControlPane) setzen.
	//Andernfalls sitz Cursor noch in Textbox und dies ist kein schoenes UI-Verhalten
	setWantsKeyboardFocus(true);
}

void ControlPane::setMinMax(float xMin, float xMax, float yMin, float yMax) {
	//Limits setzen
	xValueMinMax[0] = xMin;
	xValueMinMax[1] = xMax;
	yValueMinMax[0] = yMin;
	yValueMinMax[1] = yMax;
}

void ControlPane::setValue(float x, float y) {
	//Richtige Werte setzen, falls in korrektem Wertebereich
	if (x >= xValueMinMax[0] && x <= xValueMinMax[1] &&
		y >= yValueMinMax[0] && y <= yValueMinMax[1]) {
		
		xValue = x;
		yValue= y;
		
	}
		
}

float ControlPane::getXValue() {
	//Richtigen x Werte zurueckliefern
	return xValue;
}

float ControlPane::getYValue() {
	//Richtigen y Werte zurueckliefern
	return yValue;
}

void ControlPane::addChangeListener(ChangeListener* listener) {
	//Listener zur Liste hinzufuegen
	listenerList.add(listener);
}
void ControlPane::removeChangeListener(ChangeListener* listener) {
	//Listener von Liste entfernen
	listenerList.remove(listener);
}

void ControlPane::textEditorReturnKeyPressed(TextEditor& te) {
	//Aufruf von textEditorFocusLost wenn in TextBox Enter gedrueckt wurde
	te.focusLost(FocusChangeType::focusChangedDirectly);
}

void ControlPane::textEditorFocusLost(TextEditor& te) {
	//Werte der Textbox besorgen
	float value = te.getText().getFloatValue();
	//xValue oder yValue nur aender wenn der Wertebereich eingehalten wird
	if (&te == &te_val1) {
		if (value >= xValueMinMax[0] && value <= xValueMinMax[1]) {
			xValue = value;
			//Wert auf Achsenposition berechnen
			convertValueToAxis(true, false);
		}
	}
	else {
		if (&te == &te_val2) {
			if (value >= yValueMinMax[0] && value <= yValueMinMax[1]) {
				yValue = value;
				//Wert auf Achsenposition berechnen
				convertValueToAxis(false, true);
			}
		}
	}
	//Keyboard Fokus auf die unsichtbare Komponente(Container: ControlPane) setzen.
	//Andernfalls sitz Cursor noch in Textbox und dies ist kein schoenes UI-Verhalten
	this->grabKeyboardFocus();
	//Geaenderte Werte anzeigen
	displayValues();
}