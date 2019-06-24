#include "../JuceLibraryCode/JuceHeader.h"

/*
Klasse stellt einen kreisförmigen Audiobuffer sowie Funktionen zum speichern und lesen von vergangenen AudioSamples bereit
*/
class Delay {
public:
	//Speichern von neuen Samples in dem Audiobuffer
	void setBufferData(int channel, int numSamples, const float* source);
	//Lesen von vergangenen Audiosamples in ein Audiobuffer Objekt
	void getBufferData(int channel, int numSamples, float delayTime, AudioBuffer<float>& destination);
	//Hinzufuegen von vergangenen Samples zum aktuellen Audiobuffer
	void addFeedbackData(int channel, int numSamples, float volume, const float* source);
	//Schreibeposition um die Anzahl der gelesenen Samples verschieben
	void updateBufferPos(int numSamples);
	//Wird das spielen von Samples angehalten, so wird der Buffer gecleared
	void stop();
	//Initialisierung des Speichers, setzen von Anfangswerten sowie Samplerate
	void init(int numChannels, int samplerate);
	Delay();
	Delay(int numChannels, int samplerate);
	~Delay();
private:
	//Delaybuffer, kein echter Kreis aber mit Hilfe von Modulooperator moeglich
	AudioBuffer<float> circularBuffer;
	//Aktuelle Position im Buffer zum Speichern von Samples
	int bufferPos;
	//Samplerate
	int sampleRate;
};