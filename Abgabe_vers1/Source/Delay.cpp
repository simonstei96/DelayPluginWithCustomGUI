#include "Delay.h"

/*
param 1: Kanal, aus dem gelesen werden soll
param 2: Anzahl der zu speichernden Samples pro Channel
param 3: pointer zur Samplequelle
*/
void Delay::setBufferData(int channel, int numSamples, const float * source)
{
	//DelayBuffer Groesse abfragen
	int delayBufferSize = circularBuffer.getNumSamples();

	//Ueberpruefen, ob das Ende der Buffers ueberschritten wird bei speichern
	if (delayBufferSize > bufferPos + numSamples) { //Nein
		//Alle Samples können gespeichert werden ohne den Speicherbereich zu verlassen
		circularBuffer.copyFrom(channel, bufferPos, source, numSamples);
	}
	else {//Ja
		//Anzahl der Samples von der Schreibeposition bis zum Ende des Buffers
		int samplesTillEnd = delayBufferSize - bufferPos;
		//Rest der Samples, welche noch gespeichert werden muessen
		int restSamples = numSamples - samplesTillEnd;
		//Speichern der Samples im Delaybuffer
		//...bis Ende
		circularBuffer.copyFrom(channel, bufferPos, source, samplesTillEnd);
		//...Rest der Samples von vorne beginnen
		circularBuffer.copyFrom(channel, 0, source, restSamples);
	}
	
}

/*
param 1: Kanal, in den speichert wird
param 2: Zeitverzoegerungspunkt ( zB: 500 ms on der Vergangenheit)
param 3: Zielspeicherort der gelesenen Samples
*/
void Delay::getBufferData(int channel, int numSamples, float delayTime ,AudioBuffer<float>& destination) {
	//Berechnen, wie viele Samples der Zeitverzoegerung entspricht anhand der Samplerate
	int numSamplesGoBack = sampleRate*(delayTime / 1000);
	//Anzahl der Samples eines Kanals des Delaybuffers
	int delayBufferSize = circularBuffer.getNumSamples();
	
	//Berechnen der Lesestartposition im Buffer rueckgehend von der aktuellen Schreibposition
	//Da beim abziehen von der Leseposition von der Schreibposition der Bereich im Buffer verlassen werden koennte,
	//wird dem mit Hilfe von Modulooperation vorgewirkt (Einhaltung des Kreispeichers)
	int bufferReadPos = (int)((delayBufferSize + (bufferPos - numSamplesGoBack ))% delayBufferSize);

	//Ueberpruefen, ob beim Kopieren der Werte das Ende ueberschritten wird
	if (delayBufferSize > bufferReadPos + numSamples) {//Nein
		//Kopieren der vergangenen Samples aus dem Delaybuffer in den uebergebenen Zielbuffer
		destination.copyFrom(channel,0, circularBuffer.getReadPointer(channel)+bufferReadPos, numSamples);
	}
	else {//Ja
		//Anzahl der Samples von Leseposition bis Ende des Delaybuffers
		int samplesTillEnd = delayBufferSize - bufferReadPos;
		//Rest der Samples wo noch gelesen werden muessen
		int restSamples = numSamples - samplesTillEnd;

		//Kopieren der Samples von Delaybuffer in den Zielspeicher
		//..von Leseposition im Delaybuffer bis Ende des Delaybuffers, 
		//..in Zielbuffer von Position 0 bis Anzahl der gelesenen Samples (samplesTillEnd)
		destination.copyFrom(channel, 0, circularBuffer.getReadPointer(channel) + bufferReadPos, samplesTillEnd);
		//..von Anfang des Delaybuffers bis Ende des Delaybuffers,
		//..im Zielbuffer von samplesTillEnd bis die restlichen Samples auch im kopiert sind
		destination.copyFrom(channel, samplesTillEnd, circularBuffer.getReadPointer(channel) , restSamples);
	}
}

/*
param 1: Kanal, aus und in den gelesen/gespeichert wird
param 2: Anzahl der Samples 
param 3: Feedbacklautstaerke
param 4: Quelle des Feedback
*/
void Delay::addFeedbackData(int channel, int numSamples, float volume, const float* source) {
	//DelayBuffer Groesse abfragen
	int delayBufferSize = circularBuffer.getNumSamples();

	////Ueberpruefen, ob das Ende der Buffers ueberschritten wird beim hinzufuegen zu den bestehenden Samples
	if (delayBufferSize > bufferPos + numSamples) {//Nein
		//Die vergangenen Samples zu dem Delaybuffer hinzufuegen ( nicht ersetzen sondern addieren)
		circularBuffer.addFrom(channel, bufferPos, source, numSamples, volume);
	}
	else {//Ja
		//Anzahl der Samples von der Schreibeposition bis zum Ende des Buffers
		int samplesTillEnd = delayBufferSize - bufferPos;
		//Rest der Samples, welche noch gespeichert werden muessen
		int restSamples = numSamples - samplesTillEnd;

		//Hinzufuegen/addieren der Delaysamples (aus source) in den Delaybuffer
		//..von Schreibposition bis Ende
		circularBuffer.addFrom(channel, bufferPos, source, samplesTillEnd, volume);
		//..von Anfang bis die restlichen Samples aus source drinnen sind
		circularBuffer.addFrom(channel, 0, source + samplesTillEnd, restSamples, volume);
	}
}

//param 1: Anzahl der gespeicherten Samples
void Delay::updateBufferPos(int numSamples)
{
	//Schreibeposition um die Anzahl der geschriebenen Samples verschieben
	//Position darf nicht außerhalb des Buffers liegen (Modulo)
	bufferPos = (bufferPos + numSamples) % circularBuffer.getNumSamples();
}

void Delay::stop()
{
	//Entfernt alle Samples aus dem Delaybuffer (vorbeugung von Artifakten)
	circularBuffer.clear();
}

/*
param 1: Anzahl der Kanäle die der Delaybuffer/Kreisbuffer benoetigt
param 2: aktuelle Samplerate
*/
void Delay::init(int numChannels, int samplerate)
{
	//Anzahl der Samples pro Kanal, hier umfasst er pro Kanal Samples fuer 2 Sekunden
	int size = samplerate * 2;
	//Initialisierung des Buffers mit Anzahl der Kanäle und Groeße, alte Werte entfernen
	circularBuffer.setSize(numChannels, size, false, true);
	//Samplerate speichern, wichtig fuer das Lesen von Samples
	sampleRate = samplerate;
	//Die erste Schreibposition auf 0 setzen (Anfang)
	bufferPos = 0;
}

Delay::Delay()
{
}

Delay::Delay(int numChannels, int samplerate) {
	
	int size = samplerate * 2;
	circularBuffer.setSize(numChannels, size, false, true);
	sampleRate = samplerate;
}

Delay::~Delay() {

}