#ifndef _LIVE_POEM
#define _LIVE_POEM

#include "ofMain.h"
#include "fft.h"

#include <list>

//class LivePoet;

#define FFT_BINS 512
#define FFT_SUBBANDS 32
#define ENERGY_HISTORY 43

using namespace std;

class beatDetect
{
protected:
    // fft & beat detection
	float fftSmoothed[FFT_BINS]; // 平均化されたfft値を格納する配列
	float fftSubbands[FFT_SUBBANDS]; // 各サブバンドの生の値
	float averageEnergy[FFT_SUBBANDS]; // サブバンドの平均エネルギ
    float fftVariance[FFT_SUBBANDS]; // 各サブバンドのエネルギの分散を格納する
    float beatValueArray[FFT_SUBBANDS]; // 各サブバンドのビートディテクトに使われる定数Cを格納する
	float energyHistory[FFT_SUBBANDS][ENERGY_HISTORY]; //32のサブバンドに42のエネルギヒストリ
	float *in_fft;
	float beatValue;
	int historyPos;
    
	
	bool fftInit;
	ofSoundPlayer soundtrack;
	
public:
	beatDetect();
	virtual ~beatDetect() {}

    // FFT functions
	void initFFT();
    void updateFFT();
    //void updateFFT(float* in_fft, int infft_size);
    void drawSmoothedFFT();
    void drawSubbands();
    //void drawBeats();
    void audioReceived(float* input, int bufferSize);
    
    // detect beats
    void enableBeatDetect() {bDetectBeat = true;}
    void disableBeatDetect() {bDetectBeat = false;}
    bool isBeat(int subband);
    bool isKick();
    bool isSnare();
    bool isHat();
    bool isBeatRange(int low, int high, int threshold);
    void setBeatValue(float bv) {beatValue = bv;}

    // fftオブジェクト用
    float *magnitude, *phase, *power, *audio_input;
    float *magnitude_average, *magnitude_average_snapshot; 

	bool bDetectBeat;

    // fft object
    fft myfft;
    
};

#endif
