#include "beatDetect.h"
#include "fft.h"
// FFT BIN = 256
// サブバンド数 = FFT_SUBBANDS  32 -> subbands
// ヒストリ数 = ENERGY_HISTORY  43 -> energyHistory


int fft_size=512;
int buffer_size = 1024;

beatDetect::beatDetect()
{
    //int fft_size=512;
    //int buffer_size = 1024;
    // 配列数は512 これを初期化
    for(int i = 0; i < fft_size; i++)
        fftSmoothed[i] = 0;

    // 各周波数サブバンド32個には43個のエネルギーヒストリが含まれている
    // もろもろの配列を初期化
    for(int i = 0; i < FFT_SUBBANDS; i++)
    {
        for(int l = 0; l < ENERGY_HISTORY; l++){
            energyHistory[i][l] = 0;
        }
        fftSubbands[i] = 0;
        averageEnergy[i] = 0;
        fftVariance[i] = 0;
        beatValueArray[i] = 0;
    }

    // fftオブジェクト用の配列を宣言
	audio_input = new float[buffer_size];
	magnitude = new float[fft_size];
	phase = new float[fft_size];
	power = new float[fft_size];
	magnitude_average = new float[fft_size]; //FFTの配列の平均
	magnitude_average_snapshot = new float[fft_size];

	for (int i = 0; i < fft_size; i++) {
		magnitude[i] = 0;
		phase[i] = 0;
		power[i] = 0;
		magnitude_average_snapshot[i] = 0;
		magnitude_average[i] = 0;
	}

    historyPos = 0; 
    fftInit = true;
    beatValue = 1.08;
    enableBeatDetect();
    printf("beatDetect setup OK!! \n");
}


//void beatDetect::updateFFT(float* in_fft, int infft_size)
void beatDetect::updateFFT()
{
    if(fftInit) //初期化済んでいれば真
    {
        //fft = in_fft;
        // audioReceivedで取得したmagnitude配列をfftへ渡す
        in_fft = magnitude;
        for (int i = 0; i < fft_size; i++)
        {
            // take the max, either the smoothed or the incoming:
            // 0.9倍したものよりも大きければ値を更新する
            if (fftSmoothed[i] < in_fft[i]) 
                fftSmoothed[i] = in_fft[i];
            // let the smoothed value sink to zero:
            // 取得した周波数の振幅値に0.9倍にして、徐々に0に減らしていく
            // (古い振幅値の影響をなくす)
            fftSmoothed[i] *= 0.90f;
        }

        if(bDetectBeat) //初期化が済んでいれば真
        {
            // サブバンドの計算
            // このソースではサブバンド数は32 -> 32回loop
            for(int i = 0; i < FFT_SUBBANDS; i++)
            {
                fftSubbands[i] = 0;
                // 256 / 32 = 8回のループ
                for(int b = 0; b < fft_size/FFT_SUBBANDS; b++) {
                    //                 [i * 8 + 1~8] -> 8刻みで,0から7,8->15,16->23という感じで和をとる
                    fftSubbands[i] +=  in_fft[i*(fft_size/FFT_SUBBANDS)+b];
                }
                // サブバンド値の和をとった後, 加算した回数分割って平均値にする
                fftSubbands[i] = fftSubbands[i]*(float)FFT_SUBBANDS/(float)fft_size;
                
                // サブバンドの分散値の計算
                for(int b=0; b < fft_size/FFT_SUBBANDS; b++)
                    fftVariance[i] += pow(in_fft[i*(fft_size/FFT_SUBBANDS)+b] - fftSubbands[i], 2);
                fftVariance[i] = fftVariance[i]*(float)FFT_SUBBANDS/(float)fft_size;
                
                //定数Cを分散値によって動的に変更する
                beatValueArray[i] = (-0.0025714*fftVariance[i])+1.35;
            }

            // エネルギ平均の計算   32回loop
            for(int i = 0; i < FFT_SUBBANDS; i++) {
                averageEnergy[i] = 0;
                // 42回loop -> ヒストリ数だけ
                for(int h = 0; h < ENERGY_HISTORY; h++) {
                    // 全体のエネルギーの平均 += 各サブバンドのエネルギヒストリ
                    averageEnergy[i] += energyHistory[i][h];
                }
                // 総和をヒストリ数で割って加重平均にする
                averageEnergy[i] /= ENERGY_HISTORY;
            }

            // put new values into energy history
            // 32回loop
            for(int i = 0; i < FFT_SUBBANDS; i++) {
                // 算出したサブバンドをエネルギヒストリのhistoryPosの位置に追加
                energyHistory[i][historyPos] = fftSubbands[i];
            }
            // historyPos = (現在のpos+1)を43で割った余り
            // 配列の添字がループすることによって擬似的な循環リストを表現している
            historyPos = (historyPos+1) % ENERGY_HISTORY; // forward pointer and rotate if necessary
        }
    }
}

// 音声入力を受け取ったとき, FFT解析を行いmagnitude配列を更新
void beatDetect::audioReceived(float* input, int bufferSize){
	// 入力された音声シグナルを audio_input にコピー
	memcpy(audio_input, input, sizeof(float) * bufferSize);
	
	float avg_power = 0.0f;
	
	// アドオンのFFTクラスを使ってスペクトルを解析、 magnitude が欲しかったFFT振幅の値になる。
	myfft.powerSpectrum(0, (int)fft_size, audio_input, buffer_size, 
						magnitude, phase, power, &avg_power);
	
	// 結果の値の大小が激しかったので平方根をとった
	for (int i = 0; i < fft_size; i++) {
		magnitude[i] = powf(magnitude[i], 0.5);
	}

	// FFT振幅の平均値を計算
	for (int i = 0; i < fft_size; i++) {
		float x = 0.085;
		magnitude_average[i] = (magnitude[i] * x) + (magnitude_average[i] * (1 - x));
	}
}

// ビートそのものがあったか
bool beatDetect::isBeat(int subband)
{
    return fftSubbands[subband] > averageEnergy[subband]*beatValueArray[subband];
}

bool beatDetect::isKick()
{
    return isBeat(0);
}

bool beatDetect::isSnare()
{
    int low = 1;
    int hi = FFT_SUBBANDS/3;
    int thresh = (hi-low)/3;
    return isBeatRange(low, hi, thresh);
}

bool beatDetect::isHat()
{
    int low = FFT_SUBBANDS/2;
    int hi = FFT_SUBBANDS-1;
    int thresh = (hi-low)/3;
    return isBeatRange(low, hi, thresh);
}

// 検出に用いるサブバンド幅を指定するタイプ
bool beatDetect::isBeatRange(int low, int high, int threshold)
{
    int num = 0;
    for(int i = low; i < high+1; i++) 
        if(isBeat(i)) 
            num++;
    return num > threshold;
}
