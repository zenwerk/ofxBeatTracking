#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
// ヘッダをインクルードすればbeatDetect使用可能
#include "beatDetect.h"

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

    ofTrueTypeFont  fbook;
	void audioReceived(float* input, int bufferSize, int nChannels);

    // beatDetectオブジェクトのインスタンスを宣言する
    // 初期化はコンストラクタで完了するので
    // testAppのsetup()ですることは無い
    beatDetect bd;
};

#endif
