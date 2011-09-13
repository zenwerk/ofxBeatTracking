#include "testApp.h"
#include "beatDetect.h"

int testApp_buffer_size = 1024;
int testApp_fft_size = 512;


bool detect=true;
bool drumVisible = true;
bool snareVisible = true;
bool hihatVisible = true;

void testApp::setup(){
	// フレームレートを60Hzにセット
	ofSetFrameRate(60);
	
	// オーディオインプットを指定 この値は固定すること
    // Output->0 Input->mono  sampleRate->44100  bufSize->1024
	ofSoundStreamSetup(0, 1, this, 44100, testApp_buffer_size, 4);

    fbook.loadFont("frabk.ttf", 32, true, false);
	ofBackground(20,20,20);
    printf("testApp setup() OK!\n");
}

void testApp::audioReceived(float* input, int bufferSize, int nChannels) {
    bd.audioReceived(input, bufferSize);
}


void testApp::keyPressed(int key){
	
    // 真偽値を反転させて画面表示をコントロール
    if(key == 'd'){
        drumVisible = !drumVisible;
    }
    if(key == 's'){
        snareVisible = !snareVisible;
    }
    if(key == 'h'){
       hihatVisible = !hihatVisible;
    }
}

void testApp::update() {
    // BeatDetectオブジェクトをupdateする
    // この時点でビートが検出されたかどうかの計算を行なっている
    // 計算の結果はbd.isBeat()などの関数から得られる
    bd.updateFFT();
}

void testApp::draw()
{
    char str[32];
	// 背景黒
	//ofBackground(0, 0, 0);
	
    // 取得したFFTの値は
    // bd.magnitude[i] のようにすればアクセス可能
    // 以下では取得したFFTをbd.magnitude[i]の形式でアクセスしてFFTを描画する

    // 生のFFTの値を描画 (1列目)
    ofSetColor(155,155,75);
    for (int i = 1; i < (int)testApp_fft_size/2; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(155,155,75);
		ofLine(10+(i*3),150,  10+(i*3),150-bd.magnitude[i]*10.0f);
        //printf("%f \n", magnitude_average[i]);
	}
    // 画面に入りきらないので2列目
    for (int i = (int)testApp_fft_size/2,b = 1; i<testApp_fft_size ; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(155,155,75);
		ofLine(10+(b*3),300,  10+(b*3),300-bd.magnitude[i]*10.0f);
        b++;
        //printf("%f \n", magnitude_average[i]);
	}


    // 平均化されたFFTの値を描画 (1列目)
    ofSetColor(134,113,89);
	for (int i = 1; i < (int)testApp_fft_size/2; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(134,113,89);
		ofLine(10+(i*3),500,  10+(i*3),500-bd.magnitude_average[i]*10.0f);
	}
    // 画面に入り切らないので2列
    for (int i = (int)testApp_fft_size/2,b = 1; i<testApp_fft_size ; i++){
        if(i % 16 == 0)
            ofSetColor(200,0,0);
        else
            ofSetColor(134,113,89);
		ofLine(10+(b*3),650,  10+(b*3),650-bd.magnitude_average[i]*10.0f);
        b++;
	}

    //ofSetColor(200,200,200);
	//for (int i = 1; i < (int)testApp_fft_size/2; i++){
	//	//ofLine(200+(i*4),200,200+(i*4),400-magnitude[i]*10.0f);
    //    //ofLine(10+(i*5.3),400,  10+(i*5.3),400-magnitude[i]*10.0f);
    //    sprintf(str, "%.0f", magnitude[i]);
    //    fbook.drawString(str,5+(i*7), 270);
    //    fbook.drawString(str,10+(i*5.3),250-magnitude[i]*10.0f);
    //}

    // ドラム音の検出
    if(drumVisible){
        if(bd.isBeatRange(0,2,2)){
            ofSetColor(255,0,0);
            fbook.drawString("DRUM!!",100,735);
        }else{
            ofSetColor(0,255,0);
            ofRect(100,700,200,50);
        }
    }

    // スネア音の検出
    if(snareVisible){
        if(bd.isBeatRange(12,18,4)){
            ofSetColor(255,0,0);
            fbook.drawString("SNARE!!", 350, 735);
        }else{
            ofSetColor(0,255,0);
            ofRect(350,700,200,50);
        }
    }

    // ハイハット音の検出
    if(hihatVisible){
        if(bd.isBeatRange(27,31,3)){
            ofSetColor(255,0,0);
            fbook.drawString("HiHat!!", 600, 735);
        }else{
            ofSetColor(0,255,0);
            ofRect(600,700,200,50);
        }
    }

}

void testApp::keyReleased(int key){

}

void testApp::mouseMoved(int x, int y){

}

void testApp::mouseDragged(int x, int y, int button){

}

void testApp::mousePressed(int x, int y, int button){

}

void testApp::mouseReleased(int x, int y, int button){

}

void testApp::windowResized(int w, int h){

}
