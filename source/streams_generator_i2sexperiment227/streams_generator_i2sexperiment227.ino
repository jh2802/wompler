/**
 * @file streams-generator-i2s.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/examples-stream/streams-generator-i2s/README.md 
 * @copyright GPLv3
 */

#include "AiEsp32RotaryEncoder.h"
#include <stack>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <Update.h>


//FOR WOMPLER XM TESTS
#define RX2 14
//end wompler xm test

bool preview=true;
#define SMPL 1936552044
const int16_t pb_power [24]  = {-48,-46,-44,-42,-39,-36,-32,-28,-24,-19,-13,-7,8,17,27,38,50,65,80,98,118,140,165,194};
// 'wompler1', 123x22px
const uint8_t oscmode_to_partialmode[6]={0,1,1,2,3,4};
const unsigned char epd_bitmap_wompler1 []  = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0c, 0x00, 0x60, 0x7f, 0xf0, 0x20, 0x01, 0x03, 0xff, 0x83, 0x80, 0x07, 0xfe, 0x0f, 0xfe, 0x00, 
	0x0c, 0x00, 0x60, 0x7f, 0xf0, 0x30, 0x03, 0x03, 0xff, 0x83, 0x80, 0x07, 0xfe, 0x0f, 0xfe, 0x00, 
	0x0e, 0x00, 0x60, 0xe0, 0x38, 0x38, 0x07, 0x00, 0x01, 0x83, 0x80, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x0e, 0x10, 0xe0, 0xe0, 0x38, 0x3c, 0x0f, 0x00, 0x01, 0x83, 0x80, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x0e, 0x38, 0xe0, 0xe0, 0x38, 0x3e, 0x1f, 0x00, 0x01, 0x83, 0x80, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x0e, 0x7c, 0xe0, 0xe0, 0x38, 0x3f, 0x1f, 0x80, 0x01, 0x83, 0x80, 0x00, 0x00, 0x00, 0x06, 0x00, 
	0x0e, 0xfe, 0xe0, 0xe0, 0x38, 0x3b, 0xbb, 0x80, 0x03, 0x83, 0x80, 0x07, 0xfc, 0x0f, 0xfe, 0x00, 
	0x0f, 0xef, 0xe0, 0xe0, 0x38, 0x3b, 0xf3, 0x83, 0xff, 0x83, 0x80, 0x07, 0xfc, 0x0f, 0xfe, 0x00, 
	0x0f, 0xc7, 0xe0, 0xe0, 0x38, 0x39, 0xe3, 0x83, 0xff, 0x03, 0x80, 0x07, 0x00, 0x0e, 0x70, 0x00, 
	0x0f, 0x83, 0xe0, 0xe0, 0x38, 0x30, 0xc3, 0x83, 0x80, 0x03, 0x80, 0x07, 0x00, 0x0e, 0x30, 0x00, 
	0x0f, 0x01, 0xc0, 0xe0, 0x38, 0x30, 0x03, 0x83, 0x80, 0x03, 0x80, 0x07, 0x00, 0x0e, 0x38, 0x00, 
	0x06, 0x00, 0xc0, 0xf0, 0x78, 0x70, 0x03, 0x83, 0x80, 0x03, 0x80, 0x07, 0x00, 0x0e, 0x1c, 0x00, 
	0x04, 0x00, 0x40, 0x7f, 0xf0, 0x70, 0x03, 0x83, 0x80, 0x03, 0xff, 0x87, 0xfe, 0x0e, 0x1c, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xe0, 0x70, 0x03, 0x83, 0x80, 0x03, 0xff, 0x87, 0xfe, 0x0e, 0x0e, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wompler2', 123x22px
const unsigned char epd_bitmap_wompler2 []  = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x3f, 0x01, 0xf9, 0xff, 0xfc, 0xf8, 0x07, 0xcf, 0xff, 0xef, 0xe0, 0x1f, 0xff, 0xbf, 0xff, 0x80, 
	0x21, 0x01, 0x09, 0x00, 0x04, 0x8c, 0x0c, 0x48, 0x00, 0x28, 0x20, 0x10, 0x00, 0xa0, 0x00, 0x80, 
	0x2d, 0x81, 0x6b, 0x7f, 0xf6, 0xa6, 0x19, 0x4b, 0xff, 0xab, 0xa0, 0x17, 0xfe, 0xaf, 0xfe, 0x80, 
	0x2c, 0xff, 0x6a, 0x7f, 0xf2, 0xb3, 0x33, 0x4b, 0xff, 0xab, 0xa0, 0x17, 0xfe, 0xaf, 0xfe, 0x80, 
	0x2e, 0xc6, 0x6a, 0xe0, 0x3a, 0xb9, 0xe7, 0x48, 0x01, 0xab, 0xa0, 0x10, 0x00, 0xa0, 0x06, 0x80, 
	0x2e, 0x92, 0xea, 0xef, 0xba, 0xbc, 0xcf, 0x6f, 0xfd, 0xab, 0xa0, 0x1f, 0xff, 0xbf, 0xf6, 0x80, 
	0x2e, 0x38, 0xea, 0xe8, 0xba, 0xbe, 0x5f, 0x20, 0x0d, 0xab, 0xa0, 0x1f, 0xff, 0x3f, 0xf6, 0x80, 
	0x2e, 0x7c, 0xea, 0xea, 0xba, 0xbf, 0x1f, 0xaf, 0xf9, 0xab, 0xa0, 0x10, 0x01, 0x20, 0x06, 0x80, 
	0x2e, 0xfe, 0xea, 0xea, 0xba, 0xbb, 0xbb, 0xa8, 0x03, 0xab, 0xa0, 0x17, 0xfd, 0x2f, 0xfe, 0x80, 
	0x2f, 0xef, 0xea, 0xea, 0xba, 0xbb, 0xf3, 0xab, 0xff, 0xab, 0xa0, 0x17, 0xfd, 0x2f, 0xfe, 0x80, 
	0x2f, 0xc7, 0xea, 0xe8, 0xba, 0xb9, 0xe3, 0xab, 0xff, 0x2b, 0xa0, 0x17, 0x01, 0x2e, 0x70, 0x80, 
	0x2f, 0x93, 0xea, 0xed, 0xbb, 0xb0, 0xcb, 0xab, 0x80, 0x6b, 0xa0, 0x17, 0x7f, 0x2e, 0x33, 0x80, 
	0x2f, 0x39, 0xca, 0xe7, 0x3b, 0x36, 0x1b, 0xab, 0xbf, 0xcb, 0xbf, 0xf7, 0x7f, 0xae, 0xb9, 0x00, 
	0x26, 0x6c, 0xda, 0xf0, 0x7b, 0x77, 0xfb, 0xab, 0xa0, 0x0b, 0x80, 0x37, 0x00, 0xae, 0x9d, 0x80, 
	0x34, 0xc6, 0x52, 0x7f, 0xf3, 0x74, 0x0b, 0xab, 0xa0, 0x0b, 0xff, 0xb7, 0xfe, 0xae, 0xdc, 0x80, 
	0x11, 0x83, 0x13, 0x3f, 0xe7, 0x74, 0x0b, 0xab, 0xa0, 0x0b, 0xff, 0xb7, 0xfe, 0xae, 0xce, 0x80, 
	0x1f, 0x01, 0xf1, 0x80, 0x0d, 0x04, 0x08, 0x28, 0x20, 0x08, 0x00, 0x30, 0x00, 0xa0, 0xe0, 0x80, 
	0x00, 0x00, 0x00, 0xff, 0xf9, 0xfc, 0x0f, 0xef, 0xe0, 0x0f, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0x80, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'wompler3', 123x22px
const unsigned char epd_bitmap_wompler3 []  = {
	0xff, 0xc7, 0xff, 0xff, 0xff, 0xfe, 0x1f, 0xff, 0xff, 0xff, 0xf8, 0x7f, 0xff, 0xff, 0xff, 0xe0, 
	0x80, 0x44, 0x00, 0x00, 0x00, 0x03, 0x30, 0x00, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x00, 0x20, 
	0xbf, 0x65, 0xf9, 0xff, 0xfc, 0xf9, 0xe7, 0xcf, 0xff, 0xef, 0xe8, 0x5f, 0xff, 0xbf, 0xff, 0xa0, 
	0xa1, 0x3d, 0x09, 0x00, 0x04, 0x8c, 0xcc, 0x48, 0x00, 0x28, 0x28, 0x50, 0x00, 0xa0, 0x00, 0xa0, 
	0xad, 0x81, 0x6b, 0x7f, 0xf6, 0xa6, 0x19, 0x4b, 0xff, 0xab, 0xa8, 0x57, 0xfe, 0xaf, 0xfe, 0xa0, 
	0xac, 0xff, 0x6a, 0x7f, 0xf2, 0xb3, 0x33, 0x4b, 0xff, 0xab, 0xa8, 0x57, 0xfe, 0xaf, 0xfe, 0xa0, 
	0xae, 0xc6, 0x6a, 0xe0, 0x3a, 0xb9, 0xe7, 0x48, 0x01, 0xab, 0xa8, 0x50, 0x00, 0xa0, 0x06, 0xa0, 
	0xae, 0x92, 0xea, 0xef, 0xba, 0xbc, 0xcf, 0x6f, 0xfd, 0xab, 0xa8, 0x5f, 0xff, 0xbf, 0xf6, 0xa0, 
	0xae, 0x38, 0xea, 0xe8, 0xba, 0xbe, 0x5f, 0x20, 0x0d, 0xab, 0xa8, 0x5f, 0xff, 0x3f, 0xf6, 0xa0, 
	0xae, 0x7c, 0xea, 0xea, 0xba, 0xbf, 0x1f, 0xaf, 0xf9, 0xab, 0xa8, 0x50, 0x01, 0x20, 0x06, 0xa0, 
	0xae, 0xfe, 0xea, 0xea, 0xba, 0xbb, 0xbb, 0xa8, 0x03, 0xab, 0xa8, 0x57, 0xfd, 0x2f, 0xfe, 0xa0, 
	0xaf, 0xef, 0xea, 0xea, 0xba, 0xbb, 0xf3, 0xab, 0xff, 0xab, 0xa8, 0x57, 0xfd, 0x2f, 0xfe, 0xa0, 
	0xaf, 0xc7, 0xea, 0xe8, 0xba, 0xb9, 0xe3, 0xab, 0xff, 0x2b, 0xaf, 0xd7, 0x01, 0x2e, 0x70, 0xa0, 
	0xaf, 0x93, 0xea, 0xed, 0xbb, 0xb0, 0xcb, 0xab, 0x80, 0x6b, 0xa0, 0x17, 0x7f, 0x2e, 0x33, 0xa0, 
	0xaf, 0x39, 0xca, 0xe7, 0x3b, 0x36, 0x1b, 0xab, 0xbf, 0xcb, 0xbf, 0xf7, 0x7f, 0xae, 0xb9, 0x20, 
	0xa6, 0x6c, 0xda, 0xf0, 0x7b, 0x77, 0xfb, 0xab, 0xa0, 0x0b, 0x80, 0x37, 0x00, 0xae, 0x9d, 0xa0, 
	0xb4, 0xc6, 0x52, 0x7f, 0xf3, 0x74, 0x0b, 0xab, 0xaf, 0xeb, 0xff, 0xb7, 0xfe, 0xae, 0xdc, 0xa0, 
	0x91, 0x93, 0x13, 0x3f, 0xe7, 0x75, 0xeb, 0xab, 0xa8, 0x2b, 0xff, 0xb7, 0xfe, 0xae, 0xce, 0xa0, 
	0xdf, 0x39, 0xf1, 0x80, 0x0d, 0x05, 0x28, 0x28, 0x28, 0x28, 0x00, 0x30, 0x00, 0xa0, 0xe0, 0xa0, 
	0x40, 0x6c, 0x04, 0xff, 0xf9, 0xfd, 0x2f, 0xef, 0xe8, 0x2f, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0xa0, 
	0x7f, 0xc7, 0xfe, 0x00, 0x00, 0x01, 0x20, 0x00, 0x08, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 
	0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0
};

// Array of all bitmaps for convenience. (Total bytes used to store images in  = 1104)





#define VOICE_QT 8
//#define LFO_QT 2 * VOICE QT
#define RIFF 1380533830
#define WAVE 1463899717
#define FACT 1717658484
#define DATA_ID 1684108385
const char *my_file;
#define FMTT 1718449184

#define FIRMWARE_VERSION "Build 31026"
//int long_press;
uint8_t voice2pitch[VOICE_QT];
bool enable_click=true;
int update_prog=0;
uint8_t pb_range=0;
bool dual_init=false;
String filename;
uint8_t edit_dest;
bool is_replace=false;
#include "do1.h"
bool sustain=false;
uint8_t sample_length=172;
uint8_t sample_threshold=0;
uint8_t sample_pos=0;
int samp_level_average_cache=0;
int8_t samp_level_average=0;
bool sd_init=false;
float boost_factor=1.0;
uint8_t dir_pos=0;
uint8_t edit_wavesample_2=0;
int edit_wavesample_buffer=0;
bool keystate[128];
const char *test_dir = "/";
const char *prev_dir = "/";
const char *file_save_path ="/test";
const char *test_dir_root = "/";
char sample_name[9]="USERSAMP";
int16_t next_dir_pos=0;
int16_t prev_dir_pos=0;
uint8_t fm_type=0;
File root;
File dir_file;
File dir_file_cache;
File prev_dir_file[6];
int8_t file_layer=0;
extern bool getKeystate(byte key){
    return keystate[key];
}

int8_t old_sample_pos;


#define ROTARY_ENCODER_A_PIN 25
#define ROTARY_ENCODER_B_PIN 27
#define ROTARY_ENCODER_BUTTON_PIN 4
#define ROTARY_ENCODER_STEPS 4
#define ROTARY_ENCODER_VCC_PIN -1 
#define SAMP_LEN 512

#define STEAL_FADE_TIME 4096
#define LOWPASS 0
#define HIGHPASS 1

#define WSB_HEADER 826430295
#define WSB2_HEADER 843207511
#define WSB3_HEADER 859984727
int8_t wavesample_combine=0;
bool sample_ready=false;
bool ld_default=false;
bool oscy=false;
char key2voice[128];
int8_t zoom=0;
bool queue_test=false;
uint8_t midi_channel=1;
uint8_t gp_reg8b=0;
bool nprog=0;
uint8_t gp_reg8a=0; //used for splashscreen animation and rectify mode
int8_t user_wavesamples = 0;
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}


void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}
SPIClass sd_spi(HSPI);

#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCLK 18


#define FM_SAMPLE 0
#define FM_BANK 1
#define FM_DIRWAV 2
#define FM_DIRBNK 3
//char voice2key[max_voices]={255,255,255,255,255,255}; 
//byte queuePitch[max_voices]={0,0,0,0,0,0};
//byte queueVelo[max_voices]={0,0,0,0,0,0};
char voice2key[VOICE_QT]={255,255,255,255,255,255,255,255}; 
byte queuePitch[VOICE_QT]={0,0,0,0,0,0};
byte queueVelo[VOICE_QT]={0,0,0,0,0,0};
#define WAVESAMPLE_AMOUNT 100
#define PROGRAM_AMOUNT 64
#define VOICE_HF VOICE_QT>>1
bool voices_init=false;
byte last_velo=0;
#define DEFAULT 0
#define PER_WAVESAMPLE 1
#define POLY2 2
byte poly_mode=DEFAULT;
#define ROUND_ROBIN 2
#include <MIDI.h>
#include "logtable.h"
//#include "sqlw.h"

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

extern void setQueuedNote(byte voice,byte pitch,byte velo) {
queuePitch[voice]=pitch;
queueVelo[voice]=velo;

}



#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#include "AudioTools.h"
#include "C:\Users\JJ Hardt\Documents\arduino-1.8.19\portable\sketchbook\libraries\audio-tools\src\AudioTools\MusicalNotes.h"
int8_t samp_level_buffer[128]={};
bool sampling_level=false;
//ChannelConverter<int32_t> converter(16,32);
AudioInfo from(30000,2,24);
AudioInfo to(30000,2,16);
MemoryOutput samp_mem_out{(uint8_t*)samp_memory,88064};
MemoryOutput samp_level{(uint8_t*)samp_level_buffer,128};
ConverterFillLeftAndRight<int16_t> sampfix(LeftIsEmpty,2);
volatile int16_t lfo[VOICE_QT];
volatile uint8_t random_counter[VOICE_QT];
volatile int16_t saw[VOICE_QT];
volatile int16_t tri;
//int totalInterruptCounter;

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, DIN_MIDI);
const uint16_t sample_rate=30000;
const uint8_t channels = 2;
char mode=0;    
int men[VOICE_QT];

char voice2ws[VOICE_QT];             
AudioInfo sampy(sample_rate,1,8);
byte program=0;

//AUDIO PROCESS FILTER CONSTANTS
#define FILTER_D_LP 0
#define FILTER_D_HP 1
#define FILTER_D_BP 2
#define FILTER_D_BR 3
#define FILTER_D_PKG 4

//DISPLAY CONTEXT CONSTANTS

//MAIN MENU
#define PROG 0
#define SAMPLE 2
#define EDIT 1
#define SETTINGS 3

// EDIT MENU
#define EDIT_SAMP 4
#define AMP_MENU 5
#define MOD_MENU 6
#define MORE 7

#define LOOP_MENU 8
#define FILTER 9
#define KEYSET 10
#define MORE_SAMPEDIT 11
//AMP EG MENU
#define AMP_EG_ATK 12
#define AMP_EG_DEC 13
#define AMP_EG_SUS 14
#define AMP_EG_REL 15

//keyset menu
#define KEYSET_TOPKEY 216
#define KEYSET_CTUNE 217
#define KEYSET_KEYT 218
#define KEYSET_FTUNE 219
#define KEYSET_PORTATIME 220

//SAMPLE menu
#define SAMPLE_LENGTH 20
#define SAMPLE_THRESHOLD 21
#define SAMPLE_NEWPROG 22
#define SAMPLE_LOAD 23
#define SAMPLE_START 24
#define SAMPLE_EDITOR 25
#define ZOOM 26
#define SEEK 27
#define TOOLS 28
//#define PROCESS_WARN 29

#define PROCESS 29

#define BOOST 30
#define COMBINE 31
#define RECTIFY 32
#define FADE 33
#define INVERT 34
#define NORMALIZE 35
#define REVERSE_D 36
#define XFLOOP 37
#define FILTER_D 38
#define OCTUP 39

#define BOOST2 40
#define COMBINE2 41
#define RECTIFY2 42
#define FADE2 43

#define FILTER_D_CTF 44
#define FILTER_D_RES 45
#define FILTER_D_WIDTH 46
#define FILTER_D_TYPE 47
#define FILTER_D_PROCESS 48

#define SAVE_AS_WAV 49
#define REPLACE 50
#define DUPLICATE 51
//#define ERASE 52
#define BACK_TO_SE 52

#define SAVE_WAV_NAME 54
#define SAVE_WAV_DIR 55
#define SAVE_WAV_CONFRIM 56
#define SAVE_WAV_BACK 57

#define SAVE_WAV_CHAR1 58
#define SAVE_WAV_CHAR8 65
#define WAV_NAME_CONFIRM 66

#define PROG_NUMBER 67
#define PROG_POLYMODE 68
#define PROG_OSCMODE 69
#define PROG_INITWS 70
#define PROG_DETUNE 71
#define PROG_NEWPROG 72

#define LOOP_LOOPST 73
#define LOOP_LOOPEN 74

#define LOOP_VIEW 75
#define LOOP_VIEW_ZOOM 76
#define LOOP_VIEW_SEEK 77


#define LOOP_SAMP 80
#define LOOP_SAMP_START 81
#define LOOP_SAMP_END 82

#define FILTER_TYPE 83
#define FILTER_CF 84
#define FILTER_RES 85
#define FILTER_EG 86

//AMP EG MENU
#define FILTER_EG_ATK 87
#define FILTER_EG_DEC 88
#define FILTER_EG_SUS 89
#define FILTER_EG_REL 90

#define MOD_DEST 91
#define MOD_MOD_ENV 92
#define MOD_VELOCITY 93
#define MOD_LFO 94
#define MOD_CCA 95
#define MOD_CCB 96
#define MOD_KEYFOLLOW 97
#define MOD_CCB 98

#define LOAD_SAMPLE_WAV 100
#define BROWSE_DIR 101






//MOD EG MENU
#define MOD_EG_ATK 416
#define MOD_EG_DEC 417
#define MOD_EG_SUS 418
#define MOD_EG_REL 419

#define SETT_CLEARBNK 120
#define SETT_LOADBNK 121
#define SETT_SAVEBNK 122
#define SETT_MIDIRC 123
#define SETT_PBRANGE 124
#define SETT_SOFTWARE 125

#define SAVE_BNK_NAME 126
#define SAVE_BNK_DIR 127
#define SAVE_BNK_CONFRIM 128
#define SAVE_BNK_BACK 129

#define SAVE_BNK_CHAR1 130
#define SAVE_BNK_CHAR8 137
#define BNK_NAME_CONFIRM 138

#define SW_UPDATE 139
#define SW_BACK 140

#define LOAD_SEL_BANK 600
#define BROWSE_DIR_BNK 601

#define DIALOG_D2Z_OK 500
#define DIALOG_D2Z_BACK 501
#define DIALOG_CZ_OK 502
#define DIALOG_CZ_BACK 503

#define DIALOG_SZ_OK 504



#define DIALOG_CB_OK 506
#define DIALOG_CB_BACK 507


#define DIALOG_CS_OK 509
#define DIALOG_CS_BACK 510

#define FM_SELECT 299
#define FM_PREVIEW 300
#define FM_LOAD 301
#define FM_DESELECT 302
#define FM_USE 303
#define DIR_SELECT 304
#define DIR_OPEN 305
#define DIR_DESELECT 306
#define DIR_CONFIRM 307

#define EDIT_VOL 710
#define EDIT_PAN 711
#define AMP_EG 712

//mod menu
#define LFO_MENU 713
#define MOD_EG 714
#define MOD_EG_DEPTH 715
#define MODMATRIX 716

//lfo menu
#define MORE_LFODEPTH 717
#define MORE_LFORATE 718
#define MORE_LFOSHAPE 719

//loop menu
#define LOOP 720
#define MORE_LOOPMODE 721

// new MORE menu
#define MORE2_DUPLICATE2ZONE 722
#define MORE2_CLEARZONE 723
#define MORE_COPYSET 724
#define MORE2_SWAPZONE 725
#define MORE2_VASSIGN 726
#define MORE_OSCB 727
#define MORE2_INITO2INDEX 728
#define MORE2_IOFFSET 729
#define MORE2_CCA 730
#define MORE2_CCB 731

#define WARNING_DUPLICATE2ZONE 900
#define WARNING_CLEARZONE 901
#define WARNING_CLEARBANK 902
#define WARNING_LOADBANK 903
#define WARNING_CS 904
#define ERROR_NOSD 800
#define ERROR_NOSDB 801
#define ERROR_NOSW 802
#define ERROR_PREVMEM 803

#define SAVE_SCRN 700

uint16_t hovercontext=SAMPLE;
uint16_t hovercontext_cache=SAVE_WAV_CHAR1;
uint16_t displaycontext=EDIT;
byte mw_now=0;
byte wavesample_init_o2index[WAVESAMPLE_AMOUNT];
#define PARTIAL_QT 10
AudioEffectStream *p_fx_stream[VOICE_QT];
//ADSRGainXP *p_amp_env[VOICE_QT];
//ADSRFilter *partials[VOICE_QT];
//VolumeStreamX *p_pan[VOICE_QT];
//ChannelFormatConverterStreamT<int16_t> *p_pan[VOICE_QT];
ADSRxp *p_mod_env[VOICE_QT+1];

WomprisSynthVoice<int16_t> *partials[VOICE_QT];
GeneratorFromArray<int16_t> *dualosc[VOICE_QT];
//GeneratorMixer<int8_t> *p_mixers[VOICE_QT];
GeneratedSoundStream<int16_t> *pstreams[VOICE_QT];

//GeneratedSoundStream<int8_t> *pstreams[PARTIAL_QT];
int8_t lfo_rate[VOICE_QT];
int16_t lfo_depth[VOICE_QT];
//GeneratorFromArray<int8_t> partA (samp_memory,1,true,0),GeneratorFromArray <int8_t> (samp_memory,1,true,0);    

InputMixer<int16_t> voice_mixer;

//FilteredStream<int16_t,int16_t> filtered(voice_mixer,1);
//GeneratorFromArray<int8_t> wavesamp(samp_memory,1,true,0);
//GeneratedSoundStream<int8_t> ws18b(wavesamp);        
I2SStream adc;
CsvOutput<int32_t> csvStream(Serial, channels);
StreamCopy copier1(csvStream, adc); // copy i2sStream to csvStream
FormatConverterStream converter(adc); 
TaskHandle_t Task1;
byte current_voice=0;
char next_voice=0;
//int16_t m1_detune=441;
int pitchBend=0;
byte last_key;
bool sampling=false;

//AnalogConfig ana_in(RX_MODE);
//NumberFormatConverterStream convo(ws18b);
//ConverterScaler<int16_t> scaler(0.03, -26427, 32700,1 );
//ConverterScaler<int16_t> scaler(2.5, -32768, 32768,2 );
//FilteredStream<int16_t, float> samp_filter(scaler, 1);  
             // subclass of SoundGenerator with max amplitude of 32000
//GeneratedSoundStream<int16_t> sound(convo);             // Stream generated from sine wave
I2SStream out; 
//AudioStream sampling_out;
//ChannelReducerT<int8_t> samplingfixb(1, 2);
//I2SConfig mi2s;

NumberFormatConverterStreamT< int16_t, int8_t > bitcrush(converter);
ChannelFormatConverterStreamT< int8_t > final_samp(bitcrush);
byte lfo_shape[VOICE_QT]={0,2,0,1,2,1}; //0:saw,1:tri,2:square
//byte poly=0;
//ConverterStream<int16_t,int16_t> sampling = new ConverterStream <int16_t,int16_t> (adc,scaler);
//StreamCopy copier(out, voice_mixer,512);    
//CsvOutput<int16_t> csvStream(Serial);
StreamCopy copier(out,voice_mixer,512); 
//StreamCopy scopier(sampling_out,voice_mixer,512); 
StreamCopyT<int8_t> sample_copy(samp_mem_out,final_samp,512); 
StreamCopyT<int8_t> level_copy(samp_level,final_samp,128); 
//float pan;
//int last_enc_value;
extern MusicalNotes midinotes;
bool laststate;
byte edit_wavesample=0;
uint8_t waveform_seek=0;
//int m=0;
bool hovermode=true;
uint16_t menuoffset=0;
char wavesample_topkey[WAVESAMPLE_AMOUNT];
int wavesample_start[WAVESAMPLE_AMOUNT];
int wavesample_end[WAVESAMPLE_AMOUNT];
int wavesample_loopst[WAVESAMPLE_AMOUNT];
int wavesample_loopen[WAVESAMPLE_AMOUNT];
uint8_t wavesample_loopmode[WAVESAMPLE_AMOUNT];
int8_t wavesample_wsb[WAVESAMPLE_AMOUNT];
uint8_t wavesample_amp_atk[WAVESAMPLE_AMOUNT];
uint8_t wavesample_amp_sus[WAVESAMPLE_AMOUNT];
uint8_t wavesample_amp_dec[WAVESAMPLE_AMOUNT];
uint8_t wavesample_amp_rel[WAVESAMPLE_AMOUNT];
uint8_t wavesample_filter_atk[WAVESAMPLE_AMOUNT];
uint8_t wavesample_filter_sus[WAVESAMPLE_AMOUNT];
uint8_t wavesample_filter_dec[WAVESAMPLE_AMOUNT];
uint8_t wavesample_filter_rel[WAVESAMPLE_AMOUNT];
uint8_t wavesample_mod_atk[WAVESAMPLE_AMOUNT];
uint8_t wavesample_mod_sus[WAVESAMPLE_AMOUNT];
uint8_t wavesample_mod_dec[WAVESAMPLE_AMOUNT];
uint8_t wavesample_mod_rel[WAVESAMPLE_AMOUNT];
byte wavesample_filter_reso[WAVESAMPLE_AMOUNT];
uint8_t *wavesample_amp_env[4]={wavesample_amp_atk,wavesample_amp_dec,wavesample_amp_sus,wavesample_amp_rel};
uint8_t *wavesample_filter_env[4]={wavesample_filter_atk,wavesample_filter_dec,wavesample_filter_sus,wavesample_filter_rel};
uint8_t *wavesample_mod_env[4]={wavesample_mod_atk,wavesample_mod_dec,wavesample_mod_sus,wavesample_mod_rel};
//char wavesample_pbmode[16];
char wavesample_pan[WAVESAMPLE_AMOUNT];
char wavesample_vassign[WAVESAMPLE_AMOUNT];
int8_t wavesample_ctune[WAVESAMPLE_AMOUNT];
int16_t wavesample_portatime[WAVESAMPLE_AMOUNT];
int8_t wavesample_lfoshape[WAVESAMPLE_AMOUNT];
int8_t wavesample_lforate[WAVESAMPLE_AMOUNT];
int8_t wavesample_lfodepth[WAVESAMPLE_AMOUNT];
int8_t wavesample_cc_a[WAVESAMPLE_AMOUNT];
int8_t wavesample_cc_b[WAVESAMPLE_AMOUNT];
int8_t wavesample_cca_data[WAVESAMPLE_AMOUNT];
int8_t wavesample_ccb_data[WAVESAMPLE_AMOUNT];
int8_t wavesample_menv_depth[WAVESAMPLE_AMOUNT];
int8_t wavesample_init_offset[WAVESAMPLE_AMOUNT];
int8_t wavesample_sqr_ctune[WAVESAMPLE_AMOUNT];
//byte wavesample_fcoffset[WAVESAMPLE_AMOUNT];
byte p_velo[VOICE_QT];
int samp_num;
/*
int16_t mod_env_stream[VOICE_QT];
int16_t fc_modstream[VOICE_QT];
int16_t fr_modstream[VOICE_QT];
int16_t oap_modstream[VOICE_QT];
int16_t amp_modstream[VOICE_QT];
int16_t lfr_modstream[VOICE_QT];
int16_t lfd_modstream[VOICE_QT];
int16_t oscb_modstream[VOICE_QT];
int16_t obd_modstream[VOICE_QT];
int16_t fes_modstream[VOICE_QT];
int16_t aes_modstream[VOICE_QT];
int16_t pan_modstream[VOICE_QT];
int16_t so_modstream[VOICE_QT];
char fc_modstream_cables[VOICE_QT];
char fr_modstream_cables[VOICE_QT];
char oap_modstream_cables[VOICE_QT];
char amp_modstream_cables[VOICE_QT];
char lfr_modstream_cables[VOICE_QT];
char lfd_modstream_cables[VOICE_QT];
char oscb_modstream_cables[VOICE_QT];
char obd_modstream_cables[VOICE_QT];
char fes_modstream_cables[VOICE_QT];
char aes_modstream_cables[VOICE_QT];
char pan_modstream_cables[VOICE_QT];
char so_modstream_cables[VOICE_QT];
//destination bitmasks 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//                     P O N M L K J I H G F E D C B A
// A - Filter Cutoff
// B - Filter Resonance
// C - Osc A Pitch
// D - Amplitude
// E - LFO Rate
// F - LFO Depth
// G - Sample Offset
// H - Pan
// I - Amp Env Scale
// J - Filter Env Scale
// K - Osc B Pitch
// L - Mix/FM Depth

#define FILTER_CUTOFF_USED 1
#define FILTER_RESO_USED 2
#define OSC_A_PITCH_USED 4
#define AMP_USED 8
#define LFO_RATE_USED 16
#define LFO_DEPTH_USED 32
#define SAMP_OFFSET_USED 64
#define PAN_USED 128
#define AMP_ENV_SCALE_USED 256
#define FILT_ENV_SCALE_USED 512
#define OSC_B_DETUNE_USED 512
#define OSC_B_INDEX_USED 2048
#define FILTER_CUTOFF 1
#define FILTER_RESO 2
#define OSC_A_PITCH 3
#define AMP 4
#define LFO_RATE 5
#define LFO_DEPTH 6
#define SAMP_OFFSET 7
#define PAN 8
#define AMP_ENV_SCALE 9
#define FILT_ENV_SCALE 10
#define OSC_B_DETUNE 11
#define OSC_B_INDEX 12
char wavesample_menv_dest[16]={3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
*/
//file explorer
void listDir(fs::FS &fs, const char * dirname, uint8_t levels,bool load,int8_t direction=1){
  enable_click=false;
    Serial.printf("Listing directory: %s\n", dirname);
  int8_t files_on_screen=0;
  int16_t files_in_folder=0;
   
    if(!root){
        Serial.println("Failed to open directory");
        sd_init=false;
        hovercontext=EDIT;
          enable_click=true;
        no_sd_error2();
        
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }
    uint8_t current_dir_pos=0;
    int8_t i=0;
      String str="null";






String str1=root.path();
int8_t file_offset=0;
if ((str1)!="/"){
  file_offset=1;
}
int16_t directory_length=0;
root.seekDir(0);
while (str!=""){
  str=root.getNextFileName();directory_length++;}
  if (file_offset==0){directory_length-=1;}
//Serial.println(directory_length);
     if ((next_dir_pos!=0) && (file_offset==1)){
      root.seekDir(next_dir_pos-file_offset); str=root.getNextFileName();} else if (file_offset==0) {root.seekDir(next_dir_pos); str=root.getNextFileName();}
      
   // dir_file = root.openNextFile();





Serial.println(str1);

int8_t path_len=str1.length();
    while((files_on_screen<5)){
       
      if ((current_dir_pos<5)){
        display.setTextColor(WHITE);
i = current_dir_pos;
byte hpos=2;

 byte vpos=((i)*10)+17;
 display.setCursor(hpos,vpos);

      
           str.remove(0,path_len);
            str=str.substring(0, 17);
            
       
           if (i==0){
          
           if ((next_dir_pos==0) && ((str1)!="/")){
               str="../";
               root.seekDir(next_dir_pos);
             } else if (str==""){
      dir_pos=0;
    next_dir_pos=0;
  if (str1=="/"){ root.seekDir(next_dir_pos);}
   // str=root.getNextFileName();}
     rotaryEncoder.setEncoderValue(0);
     if (((str1)!="/")){str="../";
               root.seekDir(next_dir_pos);} else {
       str=root.getNextFileName();
       str.remove(0,path_len);
            str=str.substring(0, 17);}
            // display.print(str);}
    }

     display.drawRect(hpos-2,vpos-2,str.length()*7,11,WHITE);
      if (load==true){
   
   if (str=="../"){
       if (direction==1){ displaycontext=SAMPLE_LOAD;hovercontext=FM_SELECT;fm_type=FM_SAMPLE;}
       if (direction==2){ displaycontext=SAVE_WAV_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRWAV;}
            if (direction==3){ displaycontext=SETT_LOADBNK;hovercontext=FM_SELECT;fm_type=FM_BANK;}
            if (direction==4){ displaycontext=SAVE_BNK_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRBNK;}
  dir_pos=0;
    next_dir_pos=0;
    //dir_file_cache=fs.open(dir_file.path());
     if (file_layer>0){
     file_layer--;} else {file_layer=5;};
 test_dir=prev_dir_file[file_layer].path();
   root = fs.open(test_dir);
  
  //prev_dir="/";
hovermode=false;
      rotaryEncoder.setEncoderValue(0);
   } else {
    root.seekDir(next_dir_pos-file_offset);

    
    
    dir_file=root.openNextFile();
 if(dir_file.isDirectory()){

    if (direction==1){ displaycontext=SAMPLE_LOAD;hovercontext=FM_SELECT;fm_type=FM_SAMPLE;}
      if (direction==2){ displaycontext=SAVE_WAV_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRWAV;}
            if (direction==3){ displaycontext=SETT_LOADBNK;hovercontext=FM_SELECT;fm_type=FM_BANK;}
            if (direction==4){ displaycontext=SAVE_BNK_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRBNK;}
    dir_pos=0;
    next_dir_pos=0;
    dir_file_cache=fs.open(dir_file.path());

   prev_dir_file[file_layer]=fs.open(root.path());
 test_dir= dir_file_cache.path();
   root = fs.open(test_dir);
   if (file_layer<5){
  file_layer++;} else {file_layer=0;}
hovermode=false;
      rotaryEncoder.setEncoderValue(0);
     
   return;
 } else if (direction==1){loadWAV(SD,dir_file.path());
  return;
 } else if (direction==3){loadBank(SD,dir_file.path());return;}
 if (direction==2){ displaycontext=SAVE_WAV_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRWAV;}
    if (direction==4){ displaycontext=SAVE_BNK_DIR;hovercontext=FM_SELECT;fm_type=FM_DIRBNK;}
 


 }}
 //my_dir_file=dir_file.path();
 }

 display.print(str);
      files_on_screen++;
        }
        
        current_dir_pos++;
       // dir_file = root.openNextFile();
     str=root.getNextFileName();
         }
    
   if (displaycontext!=FM_SELECT){rotaryEncoder.setBoundaries(0,directory_length-1, true);}
   enable_click=true;
}




//MINI AUDIO PROCESS FILTER
  //	10.5 to 12.5 us, mostly 10.5 us (was 14us)

 uint8_t  ucutoff;
  uint8_t q=200;
  uint8_t f=0;
   uint8_t f2=0;
  uint8_t width=1;
  int8_t filter_d_type=0;
  unsigned int fb=0;
  unsigned int fb2;
  int buf0=0, buf1=0,buf2=0,buf3=0;



int ifxmul(int a, uint8_t b) { return ((a * b) >> FX_SHIFT); }

  // multiply two fixed point numbers (returns fixed point)
 long fxmul(long a, int b) { return ((a * b) >> FX_SHIFT); }
  unsigned int ucfxmul(uint8_t a, uint8_t b)
	{
    return (((unsigned int)a * b) >> FX_SHIFT);
  }

 void setCutoffFreq(uint8_t cutoff)
	{
    ucutoff = cutoff;
	f=constrain(ucutoff,0,255);
    fb = q + ucfxmul(q, SHIFTED_1 - cutoff);
  }
  
  void setCenterFreq(int cutoff)
	{
		
    f = constrain(cutoff+width,0,255);
	f2 = constrain(cutoff-width,0,255);
    fb = q + ucfxmul(q, SHIFTED_1 - f);
	fb2 = q + ucfxmul(q, SHIFTED_1 - f2);
  }

   int nextLP(int in){
	in=in<<7;
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
    // setPin13Low();
    return buf1>>8;
  }
  
  int nextHP(int in)
	{
    // setPin13High();
	in=in<<8;
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
    // setPin13Low();
    return (in-buf1)>>9;
  }
  
    int nextBP(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (buf3)>>1;
  }

 int nextBS(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (in-buf3)>>1;
  }
  
    int nextPKG(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (buf3+in)>>2;
  }






#define MOD_ENV 1
#define VELOCITY 2
#define LFO 3
#define CCA 4
#define MODWHEEL 5
#define KEYFOLLOW 6
#define CCB 7
char wavesample_cutoff_mod[WAVESAMPLE_AMOUNT];
char wavesample_reso_mod[WAVESAMPLE_AMOUNT];
char wavesample_o2index_mod[WAVESAMPLE_AMOUNT];
char wavesample_o2detune_mod[WAVESAMPLE_AMOUNT];
char wavesample_amp_mod[WAVESAMPLE_AMOUNT];
char wavesample_oap_mod[WAVESAMPLE_AMOUNT];
char wavesample_lfr_mod[WAVESAMPLE_AMOUNT];
char wavesample_lfd_mod[WAVESAMPLE_AMOUNT];
char wavesample_so_mod[WAVESAMPLE_AMOUNT];
char wavesample_pan_mod[WAVESAMPLE_AMOUNT];
int8_t wavesample_detune[WAVESAMPLE_AMOUNT];
char* wavesample_mod_assigns[10]={wavesample_cutoff_mod,wavesample_reso_mod,wavesample_oap_mod,wavesample_o2detune_mod,wavesample_amp_mod,wavesample_lfr_mod,wavesample_lfd_mod,wavesample_so_mod,wavesample_o2index_mod,wavesample_pan_mod};
int8_t wavesample_filter_offset[WAVESAMPLE_AMOUNT];
char wavesample_filter_type[WAVESAMPLE_AMOUNT];
uint8_t user_programs=0;
byte program_init_ws[PROGRAM_AMOUNT];
byte program_polymode[PROGRAM_AMOUNT];
char wavesample_vol[WAVESAMPLE_AMOUNT];
const char *env_strings[4]={"ATK","DEC","SUS","REL"};

char bankname[9]="Default ";
bool drawing=false;
int8_t ws_combine=0;
//stack<int8_t> *mono_stack[VOICE_QT];
uint8_t voice_counter;
uint8_t voice_order[128];
byte osc2_detune=0;
int16_t master_tune=445;
#define NORMAL 0
#define CHORUS_MODE 1
#define DUAL_MODE 2
#define FM_MODE 3
byte osc_mode=NORMAL;
byte prev_osc_mode=NORMAL;
byte program_osc_mode[PROGRAM_AMOUNT];
byte program_oscbdetune[PROGRAM_AMOUNT];
byte osc_blend;
//byte lfo_rate6b;
byte max_voices=VOICE_QT;
#define MAX_EDIT_OPTIONS 11
//DISPLAY STRINGS
void longPress(){
if (displaycontext==SAVE_WAV_DIR){
hovercontext=SAVE_AS_WAV;
onButtonClick();
}
if (displaycontext==SAVE_BNK_DIR){
hovercontext=SETT_SAVEBNK;
onButtonClick();
}


}
File firmware;
void progressCallBack(size_t currSize, size_t totalSize) {
      Serial.printf("CALLBACK:  Update process at %d of %d bytes...\n", currSize, totalSize);
      float updateprog=((float)currSize/(float)totalSize)*100.00;
      update_prog=(int)updateprog;
      refreshDisplay();
}
  //int16_t temp_dc;
void onButtonClick()
{
  //Serial.println("test");
  int8_t temp=0;
  if (enable_click==true){
   rotaryEncoder.setAcceleration(0);//TEST SHIT CAREFUL REMOVING
  if (((hovercontext!=SAMPLE) or (hovercontext!=SAMPLE_START)) && (displaycontext==SAMPLE || displaycontext==REPLACE)){
    copier.begin(out,voice_mixer);
    //sample_copy.end();
    sampling=false;
    sampling_level=false;
    //initVoices();
    }
  /*newVoices();
  osc_mode=1;
  //poly_mode=PER_WAVESAMPLE;
  max_voices=5;
  initVoices();*/
  if ((displaycontext==ERROR_NOSD)||(displaycontext==ERROR_NOSDB)){init_sd();}
 /* if (hovermode==true){
    temp_dc=displaycontext;
displaycontext=hovercontext;
hovercontext=temp_dc;
} else {
  displaycontext=temp_dc;
} */

if (hovermode==true && hovercontext<3){displaycontext=hovercontext;} 
     int16_t temp_dc=displaycontext;
displaycontext=hovercontext;
hovercontext=temp_dc;
//if (hovercontext>3){rotaryEncoder.setEncoderValue(hovercontext-menuoffset);}

  
display.clearDisplay();
if (displaycontext==FM_PREVIEW){


  
  preview=true;

} else {preview=false;}
if ((displaycontext==FM_LOAD) || (displaycontext==FM_PREVIEW)) {
  enable_click=false;
if (fm_type==FM_SAMPLE){displaycontext=LOAD_SAMPLE_WAV;}
if (fm_type==FM_BANK){displaycontext=LOAD_SEL_BANK;}
if (fm_type==FM_DIRWAV){displaycontext=BROWSE_DIR;}
if (fm_type==FM_DIRBNK){displaycontext=BROWSE_DIR_BNK;}
}
if (displaycontext==FM_USE){

if (fm_type==FM_DIRWAV){  listDir(SD,test_dir,0,true,2);displaycontext=SAVE_AS_WAV;enable_click=true;}
if (fm_type==FM_DIRBNK){  listDir(SD,test_dir,0,true,4);displaycontext=SETT_SAVEBNK;enable_click=true;}
}
switch (displaycontext){
 case SAMPLE:
 case REPLACE:
 {
// out.end();

//if (displaycontext==SAMPLE){is_replace=false;}
if ((displaycontext==REPLACE) && (is_replace!=true)){
  is_replace=true;
   old_sample_pos=sample_pos;
  }
if (displaycontext==REPLACE) {
uint8_t testu=(uint8_t)(ceil((float)(wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample] )/ 512.00));
//if (sample_length>testu){
sample_length=testu;
rotaryEncoder.setBoundaries(0,8, true); 
      //rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
     
      sample_pos=(uint8_t)ceil((float)wavesample_start[edit_wavesample] / 512.00);
      menuoffset=16; //4+8=12
      hovermode=true;
        samp_level.begin();
      csvStream.begin();

 

      bitcrush.begin();
      final_samp.begin(2,1);
      //sample_copy.begin();
      level_copy.begin();
     samp_num=0;
     //sampling=true;
sampling_level=true;
sample_ready=false;
}  else {
  if (is_replace==true){

 is_replace=false;
 sample_pos=old_sample_pos;
 Serial.println("switch from replace to sample");
/*
 if (user_wavesamples>0){
  int last_samp_pos=0;

  for (int i=0;i<user_wavesamples+1;i++){
if (wavesample_end[i]>last_samp_pos){
  last_samp_pos=wavesample_end[i];
}

  }
  sample_pos=(uint8_t)(ceil((float)(last_samp_pos)/ 512.00));}*/
 // sample_pos=(uint8_t)ceil((float)wavesample_start[edit_wavesample] / 512.00);
   
    }
for (int8_t i;i<VOICE_QT;i++){
  pstreams[i]->end();
  
  }
  //newVoices(6);
 Serial.println(ESP.getFreeHeap());
 copier.end();

//hovercontext=SAMPLE_LENGTH;

      
      
     // copier.begin(out,converter);
      //samp_mem_out.reset();
     // samp_mem_out.begin();
     
      samp_level.begin();
      

 

      bitcrush.begin();
      final_samp.begin(2,1);
      //sample_copy.begin();
      level_copy.begin();
     samp_num=0;
     //sampling=true;
sampling_level=true;
sample_ready=false;
//*/
rotaryEncoder.setBoundaries(0,8, true); 
      rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
      menuoffset=16; //4+8=12
      if (sample_length>172-sample_pos){
sample_length=172-sample_pos;}
     
hovermode=true;

    
  
 
//displaycontext=SAMPLE;
 }}
break; 
case SW_UPDATE:
 hovermode=false;
 firmware =  SD.open("/firmware.bin");
  if (firmware) {
      Serial.println(F("found!"));
      Serial.println(F("Try to update!"));
 
      Update.onProgress(progressCallBack);
 
      Update.begin(firmware.size(), U_FLASH);
      Update.writeStream(firmware);
      if (Update.end()){
          Serial.println(F("Update finished!"));
      }else{
          Serial.println(F("Update error!"));
          Serial.println(Update.getError());
      }
 
      firmware.close();
 
      if (SD.rename("/firmware.bin", "/firmware.bak")){
          Serial.println(F("Firmware rename succesfully!"));
      }else{
          Serial.println(F("Firmware rename error!"));
      }
      delay(2000);
 
      ESP.restart();
  }else{
    displaycontext=ERROR_NOSW;
    hovermode=false;
    hovercontext=ERROR_NOSW;
      Serial.println(F("not found!"));
  }
break;
case EDIT_VOL:

hovermode=false;
hovercontext=AMP_MENU;
  rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_vol[edit_wavesample]);
break;

case EDIT_PAN:

hovermode=false;
hovercontext=AMP_MENU;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_pan[edit_wavesample]);
break;
case EDIT_SAMP:
hovermode=false;
hovercontext=EDIT;
temp=user_wavesamples-1;
if (temp<0){temp=0;};
rotaryEncoder.setBoundaries(0,temp, true); 
      rotaryEncoder.setEncoderValue(edit_wavesample);
break;
case AMP_EG:
hovermode=true;
//hovercontext=AMP_EG_ATK;
rotaryEncoder.setBoundaries(0,7, true); 
      //rotaryEncoder.setEncoderValue(4);
       menuoffset=8; //4+8=12
      rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
     
break;
case AMP_EG_ATK:
hovermode=false;
hovercontext=AMP_EG;
rotaryEncoder.setBoundaries(1,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_amp_atk[edit_wavesample]);
break;
case AMP_EG_DEC:
hovermode=false;
hovercontext=AMP_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_amp_dec[edit_wavesample]);
break;
case AMP_EG_SUS:
hovermode=false;
hovercontext=AMP_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_amp_sus[edit_wavesample]);
break;
case AMP_EG_REL:
hovermode=false;
hovercontext=AMP_EG;
rotaryEncoder.setBoundaries(1,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_amp_rel[edit_wavesample]);
break;
case FILTER:
hovermode=true;
//hovercontext=FILTER_TYPE;
rotaryEncoder.setBoundaries(0,7, true); 
    //  rotaryEncoder.setEncoderValue(4);
      menuoffset=FILTER_TYPE-4; 
       rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
break;
case FILTER_TYPE:
hovermode=false;
hovercontext=FILTER;
rotaryEncoder.setBoundaries(0,1, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_type[edit_wavesample]);
      menuoffset=FILTER_TYPE-4; 
break;
case FILTER_CF:
hovermode=false;
hovercontext=FILTER;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_offset[edit_wavesample]);
      menuoffset=FILTER_TYPE-4; 
break;
case FILTER_RES:
hovermode=false;
hovercontext=FILTER;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_reso[edit_wavesample]);
      menuoffset=FILTER_TYPE-4; 
break;
case FILTER_EG:
hovermode=true;
//hovercontext=FILTER_EG_ATK;
rotaryEncoder.setBoundaries(0,7, true); 
     // rotaryEncoder.setEncoderValue(4);
      menuoffset=FILTER_EG_ATK-4; //4+8=12
       rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
break;
case FILTER_EG_ATK:
hovermode=false;
hovercontext=FILTER_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_atk[edit_wavesample]);
break;
case FILTER_EG_DEC:
hovermode=false;
hovercontext=FILTER_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_dec[edit_wavesample]);
break;
case FILTER_EG_SUS:
hovermode=false;
hovercontext=FILTER_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_sus[edit_wavesample]>>6);
break;
case FILTER_EG_REL:
hovermode=false;
hovercontext=FILTER_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_filter_rel[edit_wavesample]);
break;
case KEYSET:
hovermode=true;
hovercontext=KEYSET_TOPKEY;
rotaryEncoder.setBoundaries(0,8, true); 
      rotaryEncoder.setEncoderValue(4);
      menuoffset=KEYSET_TOPKEY-4; //4+8=12
break;
case KEYSET_TOPKEY:
hovermode=false;
hovercontext=KEYSET;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_topkey[edit_wavesample]);
break;
case KEYSET_CTUNE:
hovermode=false;
hovercontext=KEYSET;
rotaryEncoder.setBoundaries(-128,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_ctune[edit_wavesample]);
break;
case KEYSET_KEYT:
hovermode=false;
hovercontext=KEYSET;

rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_sqr_ctune[edit_wavesample]);
break;
case KEYSET_FTUNE:
hovermode=false;
hovercontext=KEYSET;
rotaryEncoder.setBoundaries(-128,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_detune[edit_wavesample]);
break;
case KEYSET_PORTATIME:
hovermode=false;
hovercontext=KEYSET;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(wavesample_portatime[edit_wavesample]>>8);
break;
case SAMPLE_LENGTH:
hovermode=false;

if (is_replace==false){
  hovercontext=SAMPLE;
rotaryEncoder.setBoundaries(1,172-sample_pos, true); } else {
rotaryEncoder.setBoundaries(1,sample_length, true); 
hovercontext=REPLACE;
 rotaryEncoder.setEncoderValue(sample_length);
}
     
      sampling_level=true;
break;
case SAMPLE_THRESHOLD:
hovermode=false;

if (is_replace==false){hovercontext=SAMPLE;} else{hovercontext=REPLACE;}
rotaryEncoder.setBoundaries(0,40, true); 
      rotaryEncoder.setEncoderValue(sample_threshold);
      sampling_level=true;
break;
case SAMPLE_START:
if (is_replace==true){  
  
  //old_sample_pos=sample_pos;
sample_pos=(uint8_t)ceil((float)wavesample_start[edit_wavesample] / 512.00);}
gp_reg8a=0;
gp_reg8b=0;
hovermode=false;
hovercontext=SAMPLE;
if (172-sample_pos>0){

rotaryEncoder.setBoundaries(2,2, true); 
      rotaryEncoder.setEncoderValue(hovercontext);
       sampling_level=true;
      sample_ready=true;} 
break;
case ZOOM:
hovermode=false;
hovercontext=SAMPLE_EDITOR;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(zoom);
     
break;
case SEEK:
hovermode=false;
hovercontext=SAMPLE_EDITOR;
rotaryEncoder.setBoundaries(0,127, true); 
      rotaryEncoder.setEncoderValue(waveform_seek);
     
break;
case SAMPLE_EDITOR:
case BACK_TO_SE:
case MORE_SAMPEDIT:
case DUPLICATE:{
if (displaycontext==DUPLICATE){
  if (user_wavesamples<126){
  int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
  memmove (&samp_memory[sample_pos * 512], &samp_memory[wavesample_start[edit_wavesample]], length );
 
     wavesample_start[user_wavesamples]=(sample_pos * 512);
      wavesample_loopst[user_wavesamples]=wavesample_start[user_wavesamples];
      wavesample_end[user_wavesamples]=wavesample_start[user_wavesamples]+length;
     wavesample_loopen[user_wavesamples]=wavesample_end[user_wavesamples];
  sample_pos+=ceil((float)length / 512.00);
  edit_wavesample=user_wavesamples;
  user_wavesamples++;}
}
displaycontext=SAMPLE_EDITOR;
hovermode=true;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
    rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
  }
break;
case PROCESS:
gp_reg8b=0;
gp_reg8a=0;
hovermode=true;
 menuoffset=BOOST-4;
  rotaryEncoder.setBoundaries(0,14, true); 
break;

case AMP_MENU:
hovermode=true;
 menuoffset=EDIT_VOL-4;
  rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
  rotaryEncoder.setBoundaries(0,6, true); 
break;
case MOD_MENU:
hovermode=true;
 menuoffset=LFO_MENU-4;
  rotaryEncoder.setBoundaries(0,7, true); 
break;
case LOOP_MENU:
hovermode=true;
 menuoffset=LOOP-4;
 rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
  rotaryEncoder.setBoundaries(0,5, true); 
break;
case LFO_MENU:
hovermode=true;
 menuoffset=MORE_LFODEPTH-4;
 rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
  rotaryEncoder.setBoundaries(0,6, true); 
break;
case MORE:
case DIALOG_D2Z_BACK:
case DIALOG_CZ_BACK:
case DIALOG_CS_BACK:
hovermode=true;
displaycontext=MORE;
hovercontext=MORE2_DUPLICATE2ZONE;
menuoffset=MORE2_DUPLICATE2ZONE-4;

  rotaryEncoder.setBoundaries(0,13, true); 
  rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
break;
case MORE_LFODEPTH:
hovermode=false;
hovercontext=LFO_MENU;
 menuoffset=MORE_LFODEPTH-4;
  rotaryEncoder.setBoundaries(0,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_lfodepth[edit_wavesample]);
break;
case MORE_LFORATE:
hovermode=false;
hovercontext=LFO_MENU;
 menuoffset=MORE_LFODEPTH-4;
  rotaryEncoder.setBoundaries(0,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_lforate[edit_wavesample]);
break;
case MORE_LFOSHAPE:
hovermode=false;
hovercontext=LFO_MENU;
 menuoffset=MORE_LFODEPTH-4;
  rotaryEncoder.setBoundaries(0,3, true); 
   rotaryEncoder.setEncoderValue(wavesample_lfoshape[edit_wavesample]);
break;
case MORE_LOOPMODE:
hovermode=false;
hovercontext=LOOP_MENU;
 menuoffset=LOOP-4;
  rotaryEncoder.setBoundaries(0,4, true); 
   rotaryEncoder.setEncoderValue(wavesample_loopmode[edit_wavesample]);
break;
case MORE_OSCB:
hovermode=false;
hovercontext=MORE;
menuoffset=MORE2_DUPLICATE2ZONE-4;
  rotaryEncoder.setBoundaries(0,user_wavesamples-1, true); 
   rotaryEncoder.setEncoderValue(wavesample_wsb[edit_wavesample]);
break;


/*
case MORE2_DUPLICATE2ZONE:
hovermode=false;
hovercontext=WARNING_DUPLICATE2ZONE;
 
  rotaryEncoder.setBoundaries(0,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_wsb[edit_wavesample]);
break;*/

case MORE2_CCA:
hovermode=false;
hovercontext=MORE;
 
  rotaryEncoder.setBoundaries(-1,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_cc_a[edit_wavesample]);
break;
case MORE2_CCB:
hovermode=false;
hovercontext=MORE;
 
  rotaryEncoder.setBoundaries(-1,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_cc_b[edit_wavesample]);
break;
case MORE2_IOFFSET:
hovermode=false;
hovercontext=MORE;
 
  rotaryEncoder.setBoundaries(0,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_init_offset[edit_wavesample]);
break;
case MORE2_SWAPZONE:
hovermode=false;
hovercontext=DIALOG_SZ_OK;
 
  rotaryEncoder.setBoundaries(0,user_wavesamples-1, true); 
   rotaryEncoder.setEncoderValue(0);
break;
case MORE_COPYSET:
hovermode=false;
hovercontext=WARNING_CS;
 
  rotaryEncoder.setBoundaries(0,user_wavesamples-1, true); 
   rotaryEncoder.setEncoderValue(0);
break;
case MORE2_VASSIGN:
hovermode=false;
hovercontext=MORE;

  rotaryEncoder.setBoundaries(0,7, true); 
  
   rotaryEncoder.setEncoderValue(wavesample_vassign[edit_wavesample]);
break;
case MORE2_INITO2INDEX:
hovermode=false;
hovercontext=MORE;
 
  rotaryEncoder.setBoundaries(0,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_init_o2index[edit_wavesample]);
break;
case MOD_EG_DEPTH:
hovermode=false;
hovercontext=MOD_MENU;
 
  rotaryEncoder.setBoundaries(-127,127, true); 
   rotaryEncoder.setEncoderValue(wavesample_menv_depth[edit_wavesample]);
break;
case MOD_EG:
hovermode=true;
hovercontext=MOD_EG_ATK;
rotaryEncoder.setBoundaries(0,7, true); 
      rotaryEncoder.setEncoderValue(4);
      menuoffset=MOD_EG_ATK-4; //4+8=12
break;
case MOD_EG_ATK:
hovermode=false;
hovercontext=MOD_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_mod_atk[edit_wavesample]);
break;
case MOD_EG_DEC:
hovermode=false;
hovercontext=MOD_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_mod_dec[edit_wavesample]);
break;
case MOD_EG_SUS:
hovermode=false;
hovercontext=MOD_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_mod_sus[edit_wavesample]);
break;
case MOD_EG_REL:
hovermode=false;
hovercontext=MOD_EG;
rotaryEncoder.setBoundaries(0,255, true); 
      rotaryEncoder.setEncoderValue(wavesample_mod_rel[edit_wavesample]);
break;
case WARNING_DUPLICATE2ZONE:
case MORE2_DUPLICATE2ZONE:
displaycontext=WARNING_DUPLICATE2ZONE;
hovermode=true;
hovercontext=DIALOG_D2Z_OK;
menuoffset=DIALOG_D2Z_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
break;
case WARNING_CLEARZONE:
case MORE2_CLEARZONE:
displaycontext=WARNING_CLEARZONE;
hovermode=true;
hovercontext=DIALOG_CZ_OK;
menuoffset=DIALOG_CZ_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
break;
case WARNING_CS:

displaycontext=WARNING_CS;
hovermode=true;
hovercontext=DIALOG_CS_OK;
menuoffset=DIALOG_CS_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
break;
case DIALOG_D2Z_OK:

wavesample_topkey[user_wavesamples] = wavesample_topkey[edit_wavesample];
wavesample_start[user_wavesamples] = wavesample_start[edit_wavesample];
wavesample_end[user_wavesamples] = wavesample_end[edit_wavesample];
wavesample_loopst[user_wavesamples] = wavesample_loopst[edit_wavesample];
wavesample_loopen[user_wavesamples] = wavesample_loopen[edit_wavesample];
wavesample_loopmode[user_wavesamples] = wavesample_loopmode[edit_wavesample];
wavesample_vol[user_wavesamples] = wavesample_vol[edit_wavesample];
wavesample_pan[user_wavesamples] = wavesample_pan[edit_wavesample];
wavesample_amp_atk[user_wavesamples] = wavesample_amp_atk[edit_wavesample];
wavesample_amp_dec[user_wavesamples] = wavesample_amp_dec[edit_wavesample];
wavesample_amp_sus[user_wavesamples] = wavesample_amp_sus[edit_wavesample];
wavesample_amp_rel[user_wavesamples] = wavesample_amp_rel[edit_wavesample];
wavesample_mod_atk[user_wavesamples] = wavesample_mod_atk[edit_wavesample];
wavesample_mod_dec[user_wavesamples] = wavesample_mod_dec[edit_wavesample];
wavesample_mod_sus[user_wavesamples] = wavesample_mod_sus[edit_wavesample];
wavesample_mod_rel[user_wavesamples] = wavesample_mod_rel[edit_wavesample];
wavesample_filter_atk[user_wavesamples] = wavesample_filter_atk[edit_wavesample];
wavesample_filter_dec[user_wavesamples] = wavesample_filter_dec[edit_wavesample];
wavesample_filter_sus[user_wavesamples] = wavesample_filter_sus[edit_wavesample];
wavesample_filter_rel[user_wavesamples] = wavesample_filter_rel[edit_wavesample];
wavesample_filter_type[user_wavesamples] = wavesample_filter_type[edit_wavesample];
wavesample_filter_offset[user_wavesamples] = wavesample_filter_offset[edit_wavesample];
wavesample_filter_reso[user_wavesamples] = wavesample_filter_reso[edit_wavesample];
wavesample_wsb[user_wavesamples] = wavesample_wsb[edit_wavesample];
wavesample_init_o2index[user_wavesamples] = wavesample_init_o2index[edit_wavesample];
wavesample_init_offset[user_wavesamples] = wavesample_init_offset[edit_wavesample];
wavesample_vassign[user_wavesamples] = wavesample_vassign[edit_wavesample];
wavesample_ctune[user_wavesamples] = wavesample_ctune[edit_wavesample];
wavesample_detune[user_wavesamples] = wavesample_detune[edit_wavesample];
wavesample_lfoshape[user_wavesamples] = wavesample_lfoshape[edit_wavesample];
wavesample_lfodepth[user_wavesamples] = wavesample_lfodepth[edit_wavesample];
wavesample_menv_depth[user_wavesamples] = wavesample_menv_depth[edit_wavesample];
wavesample_lforate[user_wavesamples] = wavesample_lforate[edit_wavesample];
wavesample_cc_a[user_wavesamples] = wavesample_cc_a[edit_wavesample];
wavesample_cca_data[user_wavesamples] = wavesample_cca_data[edit_wavesample];
wavesample_cc_b[user_wavesamples] = wavesample_cc_b[edit_wavesample];
wavesample_ccb_data[user_wavesamples] = wavesample_ccb_data[edit_wavesample];
wavesample_cutoff_mod[user_wavesamples] = wavesample_cutoff_mod[edit_wavesample];
wavesample_reso_mod[user_wavesamples] = wavesample_reso_mod[edit_wavesample];
wavesample_o2index_mod[user_wavesamples] = wavesample_o2index_mod[edit_wavesample];
wavesample_o2detune_mod[user_wavesamples] = wavesample_o2detune_mod[edit_wavesample];
wavesample_amp_mod[user_wavesamples] = wavesample_amp_mod[edit_wavesample];
wavesample_oap_mod[user_wavesamples] = wavesample_oap_mod[edit_wavesample];
wavesample_lfr_mod[user_wavesamples] = wavesample_lfr_mod[edit_wavesample];
wavesample_lfd_mod[user_wavesamples] = wavesample_lfd_mod[edit_wavesample];
wavesample_so_mod[user_wavesamples] = wavesample_so_mod[edit_wavesample];
wavesample_pan_mod[user_wavesamples] = wavesample_pan_mod[edit_wavesample];
wavesample_portatime[user_wavesamples] = wavesample_portatime[edit_wavesample];
wavesample_sqr_ctune[user_wavesamples] = wavesample_sqr_ctune[edit_wavesample];
user_wavesamples++;
hovermode=true;
displaycontext=MORE;
hovercontext=MORE2_DUPLICATE2ZONE;
menuoffset=MORE2_DUPLICATE2ZONE-4;
  rotaryEncoder.setBoundaries(0,13, true); 
break;
case DIALOG_SZ_OK:

edit_wavesample_buffer=wavesample_topkey[edit_wavesample_2];
wavesample_topkey[edit_wavesample_2] = wavesample_topkey[edit_wavesample];
wavesample_topkey[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_start[edit_wavesample_2];
wavesample_start[edit_wavesample_2] = wavesample_start[edit_wavesample];
wavesample_start[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_end[edit_wavesample_2];
wavesample_end[edit_wavesample_2] = wavesample_end[edit_wavesample];
wavesample_end[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_loopst[edit_wavesample_2];
wavesample_loopst[edit_wavesample_2] = wavesample_loopst[edit_wavesample];
wavesample_loopst[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_loopen[edit_wavesample_2];
wavesample_loopen[edit_wavesample_2] = wavesample_loopen[edit_wavesample];
wavesample_loopen[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_loopmode[edit_wavesample_2];
wavesample_loopmode[edit_wavesample_2] = wavesample_loopmode[edit_wavesample];
wavesample_loopmode[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_vol[edit_wavesample_2];
wavesample_vol[edit_wavesample_2] = wavesample_vol[edit_wavesample];
wavesample_vol[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_pan[edit_wavesample_2];
wavesample_pan[edit_wavesample_2] = wavesample_pan[edit_wavesample];
wavesample_pan[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_amp_atk[edit_wavesample_2];
wavesample_amp_atk[edit_wavesample_2] = wavesample_amp_atk[edit_wavesample];
wavesample_amp_atk[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_amp_dec[edit_wavesample_2];
wavesample_amp_dec[edit_wavesample_2] = wavesample_amp_dec[edit_wavesample];
wavesample_amp_dec[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_amp_sus[edit_wavesample_2];
wavesample_amp_sus[edit_wavesample_2] = wavesample_amp_sus[edit_wavesample];
wavesample_amp_sus[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_amp_rel[edit_wavesample_2];
wavesample_amp_rel[edit_wavesample_2] = wavesample_amp_rel[edit_wavesample];
wavesample_amp_rel[edit_wavesample] = edit_wavesample_buffer;


edit_wavesample_buffer = wavesample_mod_atk[edit_wavesample_2];
wavesample_mod_atk[edit_wavesample_2] = wavesample_mod_atk[edit_wavesample];
wavesample_mod_atk[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_mod_dec[edit_wavesample_2];
wavesample_mod_dec[edit_wavesample_2] = wavesample_mod_dec[edit_wavesample];
wavesample_mod_dec[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_mod_sus[edit_wavesample_2];
wavesample_mod_sus[edit_wavesample_2] = wavesample_mod_sus[edit_wavesample];
wavesample_mod_sus[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_mod_rel[edit_wavesample_2];
wavesample_mod_rel[edit_wavesample_2] = wavesample_mod_rel[edit_wavesample];
wavesample_mod_rel[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_atk[edit_wavesample_2];
wavesample_filter_atk[edit_wavesample_2] = wavesample_filter_atk[edit_wavesample];
wavesample_filter_atk[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_dec[edit_wavesample_2];
wavesample_filter_dec[edit_wavesample_2] = wavesample_filter_dec[edit_wavesample];
wavesample_filter_dec[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_sus[edit_wavesample_2];
wavesample_filter_sus[edit_wavesample_2] = wavesample_filter_sus[edit_wavesample];
wavesample_filter_sus[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_rel[edit_wavesample_2];
wavesample_filter_rel[edit_wavesample_2] = wavesample_filter_rel[edit_wavesample];
wavesample_filter_rel[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_type[edit_wavesample_2];
wavesample_filter_type[edit_wavesample_2] = wavesample_filter_type[edit_wavesample];
wavesample_filter_type[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_offset[edit_wavesample_2];
wavesample_filter_offset[edit_wavesample_2] = wavesample_filter_offset[edit_wavesample];
wavesample_filter_offset[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_filter_reso[edit_wavesample_2];
wavesample_filter_reso[edit_wavesample_2] = wavesample_filter_reso[edit_wavesample];
wavesample_filter_reso[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_wsb[edit_wavesample_2];
wavesample_wsb[edit_wavesample_2] = wavesample_wsb[edit_wavesample];
wavesample_wsb[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_init_o2index[edit_wavesample_2];
wavesample_init_o2index[edit_wavesample_2] = wavesample_init_o2index[edit_wavesample];
wavesample_init_o2index[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_vassign[edit_wavesample_2];
wavesample_vassign[edit_wavesample_2] = wavesample_vassign[edit_wavesample];
wavesample_vassign[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_ctune[edit_wavesample_2];
wavesample_ctune[edit_wavesample_2] = wavesample_ctune[edit_wavesample];
wavesample_ctune[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_detune[edit_wavesample_2];
wavesample_detune[edit_wavesample_2] = wavesample_detune[edit_wavesample];
wavesample_detune[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_lfoshape[edit_wavesample_2];
wavesample_lfoshape[edit_wavesample_2] = wavesample_lfoshape[edit_wavesample];
wavesample_lfoshape[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_lfodepth[edit_wavesample_2];
wavesample_lfodepth[edit_wavesample_2] = wavesample_lfodepth[edit_wavesample];
wavesample_lfodepth[edit_wavesample] = edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_lforate[edit_wavesample_2];
wavesample_lforate[edit_wavesample_2] = wavesample_lforate[edit_wavesample];
 wavesample_lforate[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_menv_depth[edit_wavesample_2];
wavesample_menv_depth[edit_wavesample_2] = wavesample_menv_depth[edit_wavesample];
 wavesample_menv_depth[edit_wavesample]=edit_wavesample_buffer;

 
edit_wavesample_buffer = wavesample_init_offset[edit_wavesample_2];
wavesample_init_offset[edit_wavesample_2] = wavesample_init_offset[edit_wavesample];
wavesample_init_offset[edit_wavesample]=edit_wavesample_buffer;

 
edit_wavesample_buffer = wavesample_cc_a[edit_wavesample_2];
wavesample_cc_a[edit_wavesample_2] = wavesample_cc_a[edit_wavesample];
wavesample_cc_a[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_cca_data[edit_wavesample_2];
wavesample_cca_data[edit_wavesample_2] = wavesample_cca_data[edit_wavesample];
wavesample_cca_data[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_cc_b[edit_wavesample_2];
wavesample_cc_b[edit_wavesample_2] = wavesample_cc_b[edit_wavesample];
wavesample_cc_b[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_ccb_data[edit_wavesample_2];
wavesample_ccb_data[edit_wavesample_2] = wavesample_ccb_data[edit_wavesample];
wavesample_ccb_data[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_cutoff_mod[edit_wavesample_2];
wavesample_cutoff_mod[edit_wavesample_2] = wavesample_cutoff_mod[edit_wavesample];
wavesample_cutoff_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_reso_mod[edit_wavesample_2];
wavesample_reso_mod[edit_wavesample_2] = wavesample_reso_mod[edit_wavesample];
wavesample_reso_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_o2index_mod[edit_wavesample_2];
wavesample_o2index_mod[edit_wavesample_2] = wavesample_o2index_mod[edit_wavesample];
wavesample_o2index_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_o2detune_mod[edit_wavesample_2];
wavesample_o2detune_mod[edit_wavesample_2] = wavesample_o2detune_mod[edit_wavesample];
 wavesample_o2detune_mod[edit_wavesample]=edit_wavesample_buffer;

 
edit_wavesample_buffer = wavesample_amp_mod[edit_wavesample_2];
wavesample_amp_mod[edit_wavesample_2] = wavesample_amp_mod[edit_wavesample];
wavesample_amp_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_oap_mod[edit_wavesample_2];
wavesample_oap_mod[edit_wavesample_2] = wavesample_oap_mod[edit_wavesample];
wavesample_oap_mod[edit_wavesample]=edit_wavesample_buffer;


edit_wavesample_buffer = wavesample_lfr_mod[edit_wavesample_2];
wavesample_lfr_mod[edit_wavesample_2] = wavesample_lfr_mod[edit_wavesample];
wavesample_lfr_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_lfd_mod[edit_wavesample_2];
 wavesample_lfd_mod[edit_wavesample_2] =  wavesample_lfd_mod[edit_wavesample];
 wavesample_lfd_mod[edit_wavesample]=edit_wavesample_buffer;


edit_wavesample_buffer = wavesample_so_mod[edit_wavesample_2];
 wavesample_so_mod[edit_wavesample_2] =  wavesample_so_mod[edit_wavesample];
 wavesample_so_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_pan_mod[edit_wavesample_2];
 wavesample_pan_mod[edit_wavesample_2] =  wavesample_pan_mod[edit_wavesample];
wavesample_pan_mod[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_portatime[edit_wavesample_2];
wavesample_portatime[edit_wavesample_2] =  wavesample_portatime[edit_wavesample];
wavesample_portatime[edit_wavesample]=edit_wavesample_buffer;

edit_wavesample_buffer = wavesample_sqr_ctune[edit_wavesample_2];
wavesample_sqr_ctune[edit_wavesample_2] =  wavesample_sqr_ctune[edit_wavesample];
wavesample_sqr_ctune[edit_wavesample]=edit_wavesample_buffer;


hovermode=true;
displaycontext=MORE;
hovercontext=MORE2_DUPLICATE2ZONE;
menuoffset=MORE2_DUPLICATE2ZONE-4;
  rotaryEncoder.setBoundaries(0,9, true); 
break;
case DIALOG_CS_OK:
wavesample_vol[edit_wavesample_2] = wavesample_vol[edit_wavesample];
wavesample_pan[edit_wavesample_2] = wavesample_pan[edit_wavesample];
wavesample_amp_atk[edit_wavesample_2] = wavesample_amp_atk[edit_wavesample];
wavesample_amp_dec[edit_wavesample_2] = wavesample_amp_dec[edit_wavesample];
wavesample_amp_sus[edit_wavesample_2] = wavesample_amp_sus[edit_wavesample];
wavesample_amp_rel[edit_wavesample_2] = wavesample_amp_rel[edit_wavesample];
wavesample_mod_atk[edit_wavesample_2] = wavesample_mod_atk[edit_wavesample];
wavesample_mod_dec[edit_wavesample_2] = wavesample_mod_dec[edit_wavesample];
wavesample_mod_sus[edit_wavesample_2] = wavesample_mod_sus[edit_wavesample];
wavesample_mod_rel[edit_wavesample_2] = wavesample_mod_rel[edit_wavesample];
wavesample_filter_atk[edit_wavesample_2] = wavesample_filter_atk[edit_wavesample];
wavesample_filter_dec[edit_wavesample_2] = wavesample_filter_dec[edit_wavesample];
wavesample_filter_sus[edit_wavesample_2] = wavesample_filter_sus[edit_wavesample];
wavesample_filter_rel[edit_wavesample_2] = wavesample_filter_rel[edit_wavesample];
wavesample_filter_type[edit_wavesample_2] = wavesample_filter_type[edit_wavesample];
wavesample_filter_offset[edit_wavesample_2] = wavesample_filter_offset[edit_wavesample];
wavesample_filter_reso[edit_wavesample_2] = wavesample_filter_reso[edit_wavesample];
wavesample_wsb[edit_wavesample_2] = wavesample_wsb[edit_wavesample];
wavesample_init_o2index[edit_wavesample_2] = wavesample_init_o2index[edit_wavesample];
wavesample_vassign[edit_wavesample_2] = wavesample_vassign[edit_wavesample];
//wavesample_ctune[edit_wavesample_2] = wavesample_ctune[edit_wavesample];
//wavesample_detune[edit_wavesample_2] = wavesample_detune[edit_wavesample];
wavesample_lfoshape[edit_wavesample_2] = wavesample_lfoshape[edit_wavesample];
wavesample_lfodepth[edit_wavesample_2] = wavesample_lfodepth[edit_wavesample];
wavesample_lforate[edit_wavesample_2] = wavesample_lforate[edit_wavesample];
wavesample_menv_depth[edit_wavesample_2] = wavesample_menv_depth[edit_wavesample];
wavesample_init_offset[edit_wavesample_2] = wavesample_init_offset[edit_wavesample];
wavesample_cc_a[edit_wavesample_2] = wavesample_cc_a[edit_wavesample];
wavesample_cca_data[edit_wavesample_2] = wavesample_cca_data[edit_wavesample];
wavesample_cc_b[edit_wavesample_2] = wavesample_cc_b[edit_wavesample];
wavesample_ccb_data[edit_wavesample_2] = wavesample_ccb_data[edit_wavesample];
wavesample_cutoff_mod[edit_wavesample_2] = wavesample_cutoff_mod[edit_wavesample];
wavesample_reso_mod[edit_wavesample_2] = wavesample_reso_mod[edit_wavesample];
wavesample_o2index_mod[edit_wavesample_2] = wavesample_o2index_mod[edit_wavesample];
wavesample_o2detune_mod[edit_wavesample_2] = wavesample_o2detune_mod[edit_wavesample];
wavesample_amp_mod[edit_wavesample_2] = wavesample_amp_mod[edit_wavesample];
wavesample_oap_mod[edit_wavesample_2] = wavesample_oap_mod[edit_wavesample];
wavesample_lfr_mod[edit_wavesample_2] = wavesample_lfr_mod[edit_wavesample];
 wavesample_lfd_mod[edit_wavesample_2] =  wavesample_lfd_mod[edit_wavesample];
 wavesample_so_mod[edit_wavesample_2] =  wavesample_so_mod[edit_wavesample];
 wavesample_pan_mod[edit_wavesample_2] =  wavesample_pan_mod[edit_wavesample];
wavesample_portatime[edit_wavesample_2] =  wavesample_portatime[edit_wavesample];
wavesample_sqr_ctune[edit_wavesample_2] =  wavesample_sqr_ctune[edit_wavesample];



hovermode=true;
displaycontext=MORE;
hovercontext=MORE;
menuoffset=MORE2_DUPLICATE2ZONE-4;
  rotaryEncoder.setBoundaries(0,9, true); 
break;
case DIALOG_CZ_OK:
wavesample_topkey[edit_wavesample]=127;

wavesample_loopst[edit_wavesample]=wavesample_start[edit_wavesample];
wavesample_loopen[edit_wavesample]=wavesample_end[edit_wavesample];
wavesample_loopmode[edit_wavesample]=0;
wavesample_vol[edit_wavesample]=127;
wavesample_pan[edit_wavesample]=64;
wavesample_amp_atk[edit_wavesample]=255;
wavesample_amp_dec[edit_wavesample]=255;
wavesample_amp_sus[edit_wavesample]=255;
wavesample_amp_rel[edit_wavesample]=255;
wavesample_filter_atk[edit_wavesample]=255;
wavesample_filter_dec[edit_wavesample]=255;
wavesample_filter_sus[edit_wavesample]=255;
wavesample_filter_rel[edit_wavesample]=255;
wavesample_mod_atk[edit_wavesample]=255;
wavesample_mod_dec[edit_wavesample]=255;
wavesample_mod_sus[edit_wavesample]=255;
wavesample_mod_rel[edit_wavesample]=255;
wavesample_filter_type[edit_wavesample]=0;
wavesample_filter_offset[edit_wavesample]=64;
wavesample_filter_reso[edit_wavesample]=0;
wavesample_wsb[edit_wavesample]=0;
wavesample_init_o2index[edit_wavesample]=64;
wavesample_vassign[edit_wavesample]=0;
wavesample_ctune[edit_wavesample]=0;
wavesample_detune[edit_wavesample]=0;
wavesample_lfoshape[edit_wavesample]=1;
wavesample_lfodepth[edit_wavesample]=0;
wavesample_lforate[edit_wavesample]=64;
wavesample_cc_a[edit_wavesample]=0;
wavesample_cca_data[edit_wavesample]=0;
wavesample_cc_b[edit_wavesample]=0;
wavesample_ccb_data[edit_wavesample]=0;
wavesample_cutoff_mod[edit_wavesample]=0;
wavesample_reso_mod[edit_wavesample]=0;
wavesample_o2index_mod[edit_wavesample]=0;
wavesample_o2detune_mod[edit_wavesample]=0;
wavesample_amp_mod[edit_wavesample]=0;
wavesample_oap_mod[edit_wavesample]=LFO;
wavesample_lfr_mod[edit_wavesample]=0;
wavesample_lfd_mod[edit_wavesample]=MODWHEEL;
wavesample_so_mod[edit_wavesample]=0;
wavesample_pan_mod[edit_wavesample]=0;
wavesample_portatime[edit_wavesample]=0;
wavesample_menv_depth[edit_wavesample]=64;
wavesample_init_offset[edit_wavesample]=0;
wavesample_sqr_ctune[edit_wavesample]=0;
hovermode=true;
displaycontext=MORE;
hovercontext=MORE2_DUPLICATE2ZONE;
menuoffset=MORE2_DUPLICATE2ZONE-4;
  rotaryEncoder.setBoundaries(0,9, true); 
break;
case INVERT:
//Invert Sample
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  samp_memory[i] *= -1;
}
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
break;

case REVERSE_D:
{
//reverse Sample
int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample]-(length/2);i++){
  int8_t cache=samp_memory[i];
  samp_memory[i]=samp_memory[wavesample_end[edit_wavesample]-i];
  samp_memory[wavesample_end[edit_wavesample]-i]=cache;
}
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case NORMALIZE:
{
//find peak
int8_t peak=0;
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  if (abs(samp_memory[i]) > peak){
    peak=abs(samp_memory[i]);
  }}
Serial.println(peak);
//find boostfactor to normalize
float normalize_boost=127.00/(float)peak;
//boost/attenuate
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  float sample_calc=(float)samp_memory[i] * normalize_boost;

 samp_memory[i]=(int8_t)sample_calc;
 
}
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case BOOST:
{
  hovermode=false;
 hovercontext=BOOST2; 
 rotaryEncoder.setBoundaries(0,50, true); 
 rotaryEncoder.setEncoderValue(boost_factor*10);
}
break;
case BOOST2:
{


for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  //boost/attenuate
  float sample_calc=(float)samp_memory[i] * boost_factor;
  //clip
    if (sample_calc>127){sample_calc=127;}
  if (sample_calc<-127){sample_calc=-127;}
 samp_memory[i]=(int8_t)sample_calc;
 
}

hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case RECTIFY:
{
  hovermode=false;
 hovercontext=RECTIFY2; 
 rotaryEncoder.setBoundaries(0,1, true); 
 rotaryEncoder.setEncoderValue(gp_reg8a);
}
break;
case RECTIFY2:
//Invert Sample
if (gp_reg8a==0){
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  samp_memory[i] = abs(samp_memory[i] );
}} else {
for (int i=wavesample_start[edit_wavesample];i<wavesample_end[edit_wavesample];i++){
  if (samp_memory[i]<0){
  samp_memory[i] = 0;}
}

}


hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
break;
case FADE:
{
  hovermode=false;
 hovercontext=FADE2; 
 rotaryEncoder.setBoundaries(0,1, true); 
 rotaryEncoder.setEncoderValue(gp_reg8b);
}
break;
case FADE2:
{
  
  int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
//fade
if (gp_reg8b==0){
for (int i=0;i<length;i++){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (samp_memory[i+wavesample_start[edit_wavesample]]*i)/length;
}} else {
for (int i=0;i<length;i++){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (samp_memory[i+wavesample_start[edit_wavesample]]*(length-i))/length;
}

}


hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case COMBINE:
{
  hovermode=false;
 hovercontext=COMBINE2; 
 rotaryEncoder.setBoundaries(0,127, true); 
 rotaryEncoder.setEncoderValue(ws_combine);
}
break;
case COMBINE2:
{
  
  int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
    int wsblength=wavesample_end[ws_combine]-wavesample_start[ws_combine];
//fade

for (int i=0;i<length;i++){
  
  samp_memory[i+wavesample_start[edit_wavesample]] =  ((samp_memory[i+wavesample_start[edit_wavesample]])>>1)+((samp_memory[(i%wsblength)+wavesample_start[ws_combine]])>>1);
}


hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case XFLOOP:
{
 int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
    int hlen=length/2;

for (int i=0;i<hlen;i++){
  
  samp_memory[wavesample_start[edit_wavesample]+hlen+i] =  (((samp_memory[i+wavesample_start[edit_wavesample]])*i)+((samp_memory[i+wavesample_start[edit_wavesample]+hlen])*(hlen-i)))/hlen;
}

  wavesample_loopst[edit_wavesample]=wavesample_start[edit_wavesample]+hlen;

wavesample_loopmode[edit_wavesample]=1;
//sample_pos=ceil((float)wavesample_end[edit_wavesample] / 512.00);
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
  /*old algortithm (pre Build 31026)
  int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
    int hlen=length/2;
//fade

for (int i=0;i<hlen;i++){
  
  samp_memory[i+wavesample_start[edit_wavesample]] =  (((samp_memory[i+wavesample_start[edit_wavesample]])*i)+((samp_memory[i+wavesample_start[edit_wavesample]+hlen])*(hlen-i)))/hlen;
}

wavesample_end[edit_wavesample]=wavesample_start[edit_wavesample]+hlen;

wavesample_loopmode[edit_wavesample]=1;
//sample_pos=ceil((float)wavesample_end[edit_wavesample] / 512.00);
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
  */
  }
break;
case OCTUP:
{
  
  int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
 int hlen=length/2;
 //sample_pos is pointer to next free sample memory location
int copy_pos=(sample_pos+1)*512;
Serial.println(copy_pos);
if (copy_pos+hlen>88064){

  //out of memory error
  errorPreviewMemory();
  return;
  
  }
for (int i=0;i<hlen;i++){

  samp_memory[copy_pos+i] = samp_memory[(i*2)+wavesample_start[edit_wavesample]];
}

int max_blocks= (hlen/128)+1;
  for (int i=0;i<max_blocks;i++){
   for (int j=0;j<128;j++){
  samp_memory[(i*256)+j+wavesample_start[edit_wavesample]] = samp_memory[copy_pos+(i*128)+j];
    samp_memory[128+(i*256)+j+wavesample_start[edit_wavesample]] = samp_memory[copy_pos+(i*128)+j];
   
  }
}


 /*old algorithm pre 031026
  for (int i=0;i<hlen;i++){
  
  samp_memory[i+wavesample_start[edit_wavesample]] = samp_memory[(i*2)+wavesample_start[edit_wavesample]];
}
//copy cache
   int cache_pos=wavesample_start[edit_wavesample]+hlen;
   for (int i=0;i<hlen;i++){
samp_memory[cache_pos+i]=samp_memory[wavesample_start[edit_wavesample]+i];
}
//apply rudimentary time stretch algorithm
int max_blocks=length/512;
for (uint8_t blocks_written=0;blocks_written<max_blocks;blocks_written++){
int write_block_pos=wavesample_start[edit_wavesample]+(blocks_written * 512);
int read_block_pos=cache_pos+(blocks_written * 512);
for (int i=0;i<512;i++){
int writepos=write_block_pos+i;
int readpos=read_block_pos+(i % 512);
if ((writepos<wavesample_end[edit_wavesample]) and (readpos<wavesample_end[edit_wavesample])){
samp_memory[writepos]=samp_memory[readpos];};
}
}

*/
hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); 
  
  }
break;
case FILTER_D:
{
   menuoffset=FILTER_D_CTF-4;
  hovermode=true;
 hovercontext=FILTER_D_CTF; 
 rotaryEncoder.setBoundaries(0,9, true); 
 rotaryEncoder.setEncoderValue(hovercontext);
}
break;
case FILTER_D_CTF:
{
  hovermode=false;
 hovercontext=FILTER_D; 
 rotaryEncoder.setBoundaries(0,255, true); 
 rotaryEncoder.setEncoderValue(ucutoff);
}
break;
case FILTER_D_RES:
{
  hovermode=false;
 hovercontext=FILTER_D; 
 rotaryEncoder.setBoundaries(0,255, true); 
 rotaryEncoder.setEncoderValue(q);
}
break;
case FILTER_D_WIDTH:
{
  hovermode=false;
 hovercontext=FILTER_D; 
 rotaryEncoder.setBoundaries(0,127, true); 
 rotaryEncoder.setEncoderValue(width);
}
break;
case FILTER_D_TYPE:
{
  hovermode=false;
 hovercontext=FILTER_D; 
 rotaryEncoder.setBoundaries(0,4, true); 
 rotaryEncoder.setEncoderValue(filter_d_type);
}
break;
case FILTER_D_PROCESS:
{
 
 int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
//fade
buf0=0;
buf1=0;
buf2=0;
buf3=0;
for (int i=0;i<length;i++){
  if (filter_d_type==FILTER_D_LP){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (int8_t)nextLP(samp_memory[i+wavesample_start[edit_wavesample]]);}
  if (filter_d_type==FILTER_D_HP){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (int8_t)nextHP(samp_memory[i+wavesample_start[edit_wavesample]]);}
  if (filter_d_type==FILTER_D_BP){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (int8_t)nextBP(samp_memory[i+wavesample_start[edit_wavesample]]);}
   if (filter_d_type==FILTER_D_BR){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (int8_t)nextBS(samp_memory[i+wavesample_start[edit_wavesample]]);}
   if (filter_d_type==FILTER_D_PKG){
  samp_memory[i+wavesample_start[edit_wavesample]] =  (int8_t)nextPKG(samp_memory[i+wavesample_start[edit_wavesample]]);}
}

hovermode=true;
displaycontext=SAMPLE_EDITOR;
hovercontext=PROCESS;
 menuoffset=ZOOM-4;
  rotaryEncoder.setBoundaries(0,7, true); }
break;
case SETTINGS:
case SAVE_BNK_BACK:
case SW_BACK:
hovermode=true;
displaycontext=SETTINGS;
hovercontext=SETT_CLEARBNK;
 menuoffset=SETT_CLEARBNK-4;
rotaryEncoder.setBoundaries(0,9, true); 
      rotaryEncoder.setEncoderValue(4);
      break;
case SETT_SOFTWARE:
hovermode=true;

hovercontext=SW_BACK;
 menuoffset=SW_UPDATE-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(5);
      break;
case SETT_CLEARBNK:
case WARNING_CLEARBANK:
displaycontext=WARNING_CLEARBANK;
hovermode=true;
hovercontext=DIALOG_CB_OK;
menuoffset=DIALOG_CB_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
      break;

case DIALOG_CB_OK:
ESP.restart();
break;
case SETT_LOADBNK:

if (sd_init==true){
  root = SD.open(test_dir);
//hovercontext=LOAD_SEL_BANK;
hovercontext=FM_SELECT;
fm_type=FM_BANK;
hovermode=false;
      rotaryEncoder.setEncoderValue(next_dir_pos);} else {no_sd_error();}
    break;
case SETT_MIDIRC:

hovermode=false;
//displaycontext=SETTINGS;
hovercontext=SETTINGS;
 menuoffset=SETT_CLEARBNK-4;
rotaryEncoder.setBoundaries(1,16, true); 
      rotaryEncoder.setEncoderValue(midi_channel);
      break;
case SETT_PBRANGE:

hovermode=false;
//displaycontext=SETTINGS;
hovercontext=SETTINGS;
 menuoffset=SETT_CLEARBNK-4;
rotaryEncoder.setBoundaries(0,11, true); 
      rotaryEncoder.setEncoderValue(pb_range);
      break;
case SAMPLE_NEWPROG:

nprog=nprog ^ 1;
sampling_level=true;
if (is_replace==false){
displaycontext=SAMPLE;} else {
  displaycontext=REPLACE;
  }
hovercontext=SAMPLE_NEWPROG;
//rotaryEncoder.setBoundaries(0,8, true); 
      rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
      menuoffset=16; //4+8=12
hovermode=true;
break;
case SAMPLE_LOAD:
if (172-sample_pos>0){
if (sd_init==true){
  root = SD.open(test_dir);
hovercontext=FM_SELECT;
fm_type=FM_SAMPLE;
hovermode=false;
      rotaryEncoder.setEncoderValue(next_dir_pos);} else {
        no_sd_error();
      }} else {
        hovercontext=SAMPLE;
        hovermode=false;
        displaycontext=SAMPLE_START;

      }
      

break;
case FM_SELECT:
drawing=false;
menuoffset=FM_LOAD-4;
hovermode=true;
hovercontext=FM_LOAD;
if (fm_type==FM_SAMPLE){menuoffset=FM_PREVIEW-4;hovercontext=FM_PREVIEW;}

if (fm_type!=FM_BANK){rotaryEncoder.setBoundaries(0,6, true);} else {
  rotaryEncoder.setBoundaries(0,5, true);
  }



rotaryEncoder.setEncoderValue(4);
break;
case FM_DESELECT:
if (fm_type==FM_SAMPLE){displaycontext=SAMPLE_LOAD;
fm_type=FM_SAMPLE;}
if (fm_type==FM_BANK){displaycontext=SETT_LOADBNK;
fm_type=FM_BANK;}
if (fm_type==FM_DIRWAV){displaycontext=SAVE_WAV_DIR;}
if (fm_type==FM_DIRBNK){displaycontext=SAVE_BNK_DIR;}
hovercontext=FM_SELECT;
hovermode=false;
      rotaryEncoder.setEncoderValue(0);
break;
case SAVE_WAV_DIR:


if (sd_init==true){
  root = SD.open(test_dir);
hovercontext=FM_SELECT;
fm_type=FM_DIRWAV;
hovermode=false;
      rotaryEncoder.setEncoderValue(next_dir_pos);}else {
        no_sd_error();
      }
      

break;
case SAVE_BNK_DIR:


if (sd_init==true){
  root = SD.open(test_dir);
hovercontext=FM_SELECT;
fm_type=FM_DIRBNK;
hovermode=false;
      rotaryEncoder.setEncoderValue(next_dir_pos);
}else {
        no_sd_error();
      }

break;
case SAVE_WAV_CONFRIM:
if (sd_init==true){
filename=root.path();
filename.remove(0,1);
filename="/"+filename+"/"+sample_name+".WAV";
Serial.println(filename);
file_save_path=filename.c_str();
saveWAV(SD,file_save_path);}else {
        no_sd_error();
      }
break;
case BROWSE_DIR:
hovermode=false;
listDir(SD,test_dir,0,true,2);
//loadWAV(SD,my_file);


break;
case SAVE_BNK_CONFRIM:
if (sd_init==true){
filename=root.path();
filename.remove(0,1);
filename="/"+filename+"/"+bankname+".WSB";
Serial.println(filename);
file_save_path=filename.c_str();
writeBank(SD,file_save_path);}else {
        no_sd_error();
      }
break;
case BROWSE_DIR_BNK:
hovermode=false;
listDir(SD,test_dir,0,true,4);
//loadWAV(SD,my_file);


break;

case LOAD_SAMPLE_WAV:
if (sd_init==true){
hovermode=false;
listDir(SD,test_dir,0,true);}else {
        no_sd_error();
      }
//loadWAV(SD,my_file);


break;
case LOAD_SEL_BANK:
if (sd_init==true){
hovermode=false;
listDir(SD,test_dir,0,true,3);}else {
        no_sd_error();
      }
//loadWAV(SD,my_file);


break;
case PROG:
case PROG_NEWPROG:
if (displaycontext==PROG_NEWPROG){
  if (user_programs<62){
   prev_osc_mode=osc_mode;
          osc_mode=0;
  poly_mode=0;
  user_programs++;
  program=user_programs;}
}
displaycontext=PROG;
rotaryEncoder.setBoundaries(0,9, true); 
 rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
hovermode=true;
menuoffset=PROG_NUMBER-4; //4+12=16

   break; 

case TOOLS:
case SAVE_WAV_BACK:
displaycontext=TOOLS;
hovermode=true;
menuoffset=45; //4+8=12
hovercontext=SAVE_AS_WAV;
rotaryEncoder.setBoundaries(0,7, true); 
      rotaryEncoder.setEncoderValue(4);
    break;
case SAVE_AS_WAV:
case WAV_NAME_CONFIRM:
displaycontext=SAVE_AS_WAV;
hovermode=true;
menuoffset=50; //4+8=12
hovercontext=SAVE_WAV_NAME;
rotaryEncoder.setBoundaries(0,7, true); 
      rotaryEncoder.setEncoderValue(4);
    break;

case SAVE_WAV_NAME:
hovermode=true;
menuoffset=54; //4+8=12
hovercontext=hovercontext_cache;
rotaryEncoder.setBoundaries(0,12, true); 
      rotaryEncoder.setEncoderValue(hovercontext_cache-54);
    break;
case SETT_SAVEBNK:
case BNK_NAME_CONFIRM:
displaycontext=SETT_SAVEBNK;
hovermode=true;
menuoffset=SAVE_BNK_NAME-4; //4+8=12
hovercontext=SAVE_BNK_NAME;
rotaryEncoder.setBoundaries(0,7, true); 
      //rotaryEncoder.setEncoderValue(4);
        rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
    break;

case SAVE_BNK_NAME:
hovermode=true;
menuoffset=SAVE_BNK_CHAR1-4; //4+8=12
//hovercontext=SAVE_BNK_NAME;
rotaryEncoder.setBoundaries(0,12, true); 
  rotaryEncoder.setEncoderValue(hovercontext-menuoffset);
      //rotaryEncoder.setEncoderValue(hovercontext_cache-125);
    break;

case PROG_NUMBER:
hovermode=false;
hovercontext=PROG;
rotaryEncoder.setBoundaries(0,user_programs, true); 
      rotaryEncoder.setEncoderValue(program);
osc_mode=program_osc_mode[program];
poly_mode=program_polymode[program];
   /*
 for (char i;i<128;i++){key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};*/
break;
case PROG_POLYMODE:
hovermode=false;
hovercontext=PROG;
rotaryEncoder.setBoundaries(0,2, true); 
      rotaryEncoder.setEncoderValue(program_polymode[program]);
 for (char i;i<128;i++){handleNoteOff(midi_channel,i, 0);key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};
poly_mode=program_polymode[program];

break;
case PROG_OSCMODE:
hovermode=false;
hovercontext=PROG;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(program_osc_mode[program]);
      prev_osc_mode=osc_mode;
osc_mode=program_osc_mode[program];
//poly_mode=program_polymode[program];
break;
case PROG_INITWS:
hovermode=false;
hovercontext=PROG;
temp=user_wavesamples-1;
if (temp<0){temp=0;};
rotaryEncoder.setBoundaries(0,temp, true); 
      rotaryEncoder.setEncoderValue(program_init_ws[program]);
break;
case PROG_DETUNE:
hovermode=false;
hovercontext=PROG;
rotaryEncoder.setBoundaries(-127,127, true); 
      rotaryEncoder.setEncoderValue(program_oscbdetune[program]);
break;
case LOOP:
hovermode=false;
 
  rotaryEncoder.setBoundaries(0,7, true); 
  rotaryEncoder.setEncoderValue(4);
break;
case LOOP_VIEW_SEEK:
hovermode=false;
hovercontext=LOOP_VIEW;
  rotaryEncoder.setBoundaries(0,128, true); 
      rotaryEncoder.setEncoderValue(waveform_seek);
break;
case LOOP_VIEW_ZOOM:
hovermode=false;
hovercontext=LOOP_VIEW;
  rotaryEncoder.setBoundaries(0,7, true); 
      rotaryEncoder.setEncoderValue(zoom);
break;
case LOOP_LOOPST:
{
hovermode=false;
hovercontext=LOOP;
int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
  rotaryEncoder.setBoundaries(0,length, true); 
      rotaryEncoder.setEncoderValue(wavesample_loopst[edit_wavesample]-wavesample_start[edit_wavesample]);
      rotaryEncoder.setAcceleration(1000);
      }
break;
case LOOP_LOOPEN:
{
hovermode=false;
hovercontext=LOOP;
int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
  rotaryEncoder.setBoundaries(1,length, true); 
      rotaryEncoder.setEncoderValue(wavesample_loopen[edit_wavesample]-wavesample_start[edit_wavesample]);
      rotaryEncoder.setAcceleration(1000);
      }
break;
case LOOP_SAMP_START:
hovermode=false;
hovercontext=LOOP;
//int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
  rotaryEncoder.setBoundaries(0,88064, true); 
  rotaryEncoder.setAcceleration(1000);
      rotaryEncoder.setEncoderValue(wavesample_start[edit_wavesample]);
break;
case LOOP_SAMP_END:
hovermode=false;
hovercontext=LOOP;
rotaryEncoder.setAcceleration(1000);
//int length=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
  rotaryEncoder.setBoundaries(0,88064, true); 
      rotaryEncoder.setEncoderValue(wavesample_end[edit_wavesample]);
break;
case MOD_DEST:
rotaryEncoder.setBoundaries(0,9, true); 
 rotaryEncoder.setEncoderValue(edit_dest);
hovermode=false;
hovercontext=MODMATRIX;
break;
default:
drawing=false;
if (user_wavesamples>0){
rotaryEncoder.setBoundaries(0,MAX_EDIT_OPTIONS, true); } else {rotaryEncoder.setBoundaries(0,3, true);}
 rotaryEncoder.setEncoderValue(hovercontext);
hovermode=true;
menuoffset=0; //4+12=16
break;

}

if ((displaycontext>=SAVE_WAV_CHAR1) and (displaycontext<=SAVE_WAV_CHAR8)){
hovermode=false;

hovercontext_cache=displaycontext;

hovercontext=SAVE_WAV_NAME;

rotaryEncoder.setBoundaries(48,122, true); 
rotaryEncoder.setEncoderValue((uint8_t)sample_name[displaycontext-58]);
}

if ((displaycontext>=SAVE_BNK_CHAR1) and (displaycontext<=SAVE_BNK_CHAR8)){
hovermode=false;

hovercontext_cache=displaycontext-1;

hovercontext=SAVE_BNK_NAME;

rotaryEncoder.setBoundaries(48,122, true); 
rotaryEncoder.setEncoderValue((uint8_t)bankname[displaycontext-130]);
}

if ((displaycontext==MODMATRIX) or ((displaycontext>=MOD_MOD_ENV) and (displaycontext<=MOD_CCB))){
rotaryEncoder.setBoundaries(0,11, true); 
 rotaryEncoder.setEncoderValue(displaycontext);
hovermode=true;
menuoffset=MOD_DEST-4; //4+12=16


if (((displaycontext>=MOD_MOD_ENV) and (displaycontext<=MOD_CCB))){

if (*(wavesample_mod_assigns[edit_dest]+edit_wavesample)==0){
*(wavesample_mod_assigns[edit_dest]+edit_wavesample)=(displaycontext-MOD_MOD_ENV)+1;} else {

  *(wavesample_mod_assigns[edit_dest]+edit_wavesample)=0;
}
  /*
char *slot=(*(*(wavesample_mod_assigns + edit_dest) + edit_wavesample);

if ((*(*(wavesample_mod_assigns + edit_dest) + edit_wavesample))==0){
(*(*(wavesample_mod_assigns + edit_dest) + edit_wavesample))=MOD_MOD_ENV-displaycontext;} else {

  (*(*(wavesample_mod_assigns + edit_dest) + edit_wavesample))=0
}

*/
for (int8_t j=0;j<max_voices;j++){
partials[j]->setLFM(0);
partials[j]->setOffset(0);
 if (osc_mode>0){ dualosc[j]->setOffset(0); dualosc[j]->setLFM(0);}
   
   }

}






}

if (drawing==false){
refreshDisplay();}
}
//if (hovercontext>3 && hovermode==true){rotaryEncoder.setEncoderValue(displaycontext-menuoffset);} else if (hovermode==true) {rotaryEncoder.setEncoderValue(displaycontext);}
  //Serial.println(next_dir_pos);
}
void rotary_loop()
{
  if ((sampling==false) && (enable_click==true)){
	//dont print anything unless value changed
	if (rotaryEncoder.encoderChanged())
	{
		//Serial.print("Value: ");
    //int rotvalue=(int)rotaryEncoder.readEncoder();
    uint16_t rotval=rotaryEncoder.readEncoder();
    if (rotval>3){rotval+=menuoffset;}
		if (hovermode==true){
    hovercontext=rotval;}
    if (displaycontext==EDIT_VOL){
     
     
      wavesample_vol[edit_wavesample]=(char)rotaryEncoder.readEncoder();
    }
    if (displaycontext==EDIT_PAN){
       
     
      wavesample_pan[edit_wavesample]=(char)rotaryEncoder.readEncoder();
    }
     if (displaycontext==EDIT_SAMP){
       
     
      edit_wavesample=(byte)rotaryEncoder.readEncoder();
    }
    if (displaycontext==AMP_EG_ATK){
       
     
      wavesample_amp_atk[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==AMP_EG_DEC){
       
     
      wavesample_amp_dec[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==AMP_EG_SUS){
       
     
      wavesample_amp_sus[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==AMP_EG_REL){
       
     
      wavesample_amp_rel[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }

    if (displaycontext==FILTER_CF){
       
     
      wavesample_filter_offset[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
     if (displaycontext==FILTER_RES){
       
     
      wavesample_filter_reso[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
     if (displaycontext==FILTER_TYPE){
       
     
      wavesample_filter_type[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
if (displaycontext==FILTER_EG_ATK){
       
     
      wavesample_filter_atk[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==FILTER_EG_DEC){
       
     
      wavesample_filter_dec[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==FILTER_EG_SUS){
       
     
      wavesample_filter_sus[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==FILTER_EG_REL){
       
     
      wavesample_filter_rel[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }

if (displaycontext==MOD_EG_ATK){
       
     
      wavesample_mod_atk[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==MOD_EG_DEC){
       
     
      wavesample_mod_dec[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==MOD_EG_SUS){
       
     
      wavesample_mod_sus[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==MOD_EG_REL){
       
     
      wavesample_mod_rel[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
    
if (displaycontext==MORE2_DUPLICATE2ZONE){
       
     
      edit_wavesample_2=rotaryEncoder.readEncoder();
    }

    if ((displaycontext==MORE2_SWAPZONE) or (displaycontext==MORE_COPYSET)){
       
     
      edit_wavesample_2=rotaryEncoder.readEncoder();
    }
    if ((displaycontext==MORE2_VASSIGN)){
       
     
      wavesample_vassign[edit_wavesample]=rotaryEncoder.readEncoder();
    }
     if ((displaycontext==MORE2_CCA)){
       
     
      wavesample_cc_a[edit_wavesample]=rotaryEncoder.readEncoder();
    }
      if ((displaycontext==MORE2_CCB)){
       
     
      wavesample_cc_b[edit_wavesample]=rotaryEncoder.readEncoder();
    }
      if ((displaycontext==MORE2_IOFFSET)){
       
     
      wavesample_init_offset[edit_wavesample]=rotaryEncoder.readEncoder();
    }
    
       if ((displaycontext==MORE2_INITO2INDEX)){
      wavesample_init_o2index[edit_wavesample]=rotaryEncoder.readEncoder();
    }


if (displaycontext==LOOP_LOOPST){
       
     
      wavesample_loopst[edit_wavesample]=wavesample_start[edit_wavesample]+rotaryEncoder.readEncoder();
       if (wavesample_loopen[edit_wavesample]==wavesample_loopst[edit_wavesample]){wavesample_loopst[edit_wavesample]=wavesample_loopen[edit_wavesample]-1;}
    }

    if (displaycontext==LOOP_LOOPEN){
       
     
      wavesample_loopen[edit_wavesample]= wavesample_start[edit_wavesample]+rotaryEncoder.readEncoder();
      if (wavesample_loopen[edit_wavesample]==wavesample_loopst[edit_wavesample]){wavesample_loopen[edit_wavesample]=wavesample_loopst[edit_wavesample]+1;}
    }

     if (displaycontext==LOOP_SAMP_END){
       
     
      wavesample_end[edit_wavesample]=rotaryEncoder.readEncoder();
    }

     if (displaycontext==LOOP_SAMP_START){
       
     
      wavesample_start[edit_wavesample]=rotaryEncoder.readEncoder();
    }


    if (displaycontext==KEYSET_TOPKEY){
       
     
      wavesample_topkey[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }

     if (displaycontext==KEYSET_CTUNE){
       
     
      wavesample_ctune[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }

    
     if (displaycontext==KEYSET_KEYT){
       
     
      wavesample_sqr_ctune[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
       if (displaycontext==KEYSET_FTUNE){
       
     
      wavesample_detune[edit_wavesample]=(byte)rotaryEncoder.readEncoder();
    }
     if (displaycontext==KEYSET_PORTATIME){
       
     
      wavesample_portatime[edit_wavesample]=rotaryEncoder.readEncoder()<<8;
    }
     if (displaycontext==SAMPLE_LENGTH){
       
     if (is_replace!=true){
      sample_length=rotaryEncoder.readEncoder();}
    }
    if (displaycontext==SAMPLE_THRESHOLD){
       
     
      sample_threshold=rotaryEncoder.readEncoder();
    }

 if ((displaycontext==ZOOM) or (displaycontext==LOOP_VIEW_ZOOM) ){
       
     
      zoom=rotaryEncoder.readEncoder();
    }

     if ( (displaycontext==SEEK) or (displaycontext==LOOP_VIEW_SEEK)){
       
     
      waveform_seek=rotaryEncoder.readEncoder();
    }
 if (displaycontext==BOOST){
       
     
      boost_factor=rotaryEncoder.readEncoder()/10.0;
    }

     if (displaycontext==FADE){
       
     
      gp_reg8b=rotaryEncoder.readEncoder();
    }
    if (displaycontext==RECTIFY){
       
     
      gp_reg8a=rotaryEncoder.readEncoder();
    }
 if (displaycontext==COMBINE){
       
     
      ws_combine=rotaryEncoder.readEncoder();
    }
     if (displaycontext==FILTER_D_CTF){
       if (filter_d_type<FILTER_D_BP){
      setCutoffFreq(rotaryEncoder.readEncoder());} else {

         setCenterFreq(rotaryEncoder.readEncoder());
      }
    }

    if (displaycontext==FILTER_D_RES){
      q=rotaryEncoder.readEncoder();
      setCutoffFreq(f);
    }

     if (displaycontext==FILTER_D_WIDTH){
      width=rotaryEncoder.readEncoder();
    
    }
   if (displaycontext==FILTER_D_TYPE){
      filter_d_type=rotaryEncoder.readEncoder();
    
    }

       if (((displaycontext==SAMPLE_LOAD) or (displaycontext==SAVE_WAV_DIR)) or ( (displaycontext==SETT_LOADBNK))){
      if (sd_init==true){
        int rotposs=rotaryEncoder.readEncoder();
        if (rotposs<=9998){
        next_dir_pos=rotaryEncoder.readEncoder();} else {rotaryEncoder.setEncoderValue(0);
        }
        
        }else {
        no_sd_error();
      }
    
    }
    if (displaycontext==SAVE_BNK_DIR){

     if (sd_init==true){ next_dir_pos=rotaryEncoder.readEncoder();}else {
        no_sd_error();
      }
    }
if (displaycontext==PROG_NUMBER){
      handleProgramChange(midi_channel,rotaryEncoder.readEncoder());

    }
  if (displaycontext==PROG_POLYMODE){
      program_polymode[program]=rotaryEncoder.readEncoder();


  for (char i;i<128;i++){handleNoteOff(midi_channel,i, 0);};
  
   for (char i;i<128;i++){key2voice[i]=255;};
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};
poly_mode=program_polymode[program];
    }
  if (displaycontext==PROG_OSCMODE){
      program_osc_mode[program]=rotaryEncoder.readEncoder();

      for (char i;i<128;i++){handleNoteOff(midi_channel,i, 0);key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};
         prev_osc_mode=osc_mode;
osc_mode=program_osc_mode[program];
      for (int i=0;i<max_voices;i++){
  partials[i]->setMode(oscmode_to_partialmode[osc_mode]);
}
    }
       if (displaycontext==PROG_INITWS){
      program_init_ws[program]=rotaryEncoder.readEncoder();
    }
          if (displaycontext==PROG_DETUNE){
      program_oscbdetune[program]=rotaryEncoder.readEncoder();
    
    }

          if (displaycontext==MORE_LFODEPTH){
      wavesample_lfodepth[edit_wavesample]=rotaryEncoder.readEncoder();
    
    }

  if (displaycontext==MORE_LFORATE){
      wavesample_lforate[edit_wavesample]=rotaryEncoder.readEncoder();
    
    }

      if (displaycontext==MORE_LFOSHAPE){
      wavesample_lfoshape[edit_wavesample]=rotaryEncoder.readEncoder();
    
    
    }

      if (displaycontext==MORE_LOOPMODE){
      wavesample_loopmode[edit_wavesample]=rotaryEncoder.readEncoder();
    
    }

     if (displaycontext==MORE_OSCB){
      wavesample_wsb[edit_wavesample]=rotaryEncoder.readEncoder();
    
    }

       if (displaycontext==MOD_EG_DEPTH){
      wavesample_menv_depth[edit_wavesample]=rotaryEncoder.readEncoder();
    
    }

      if (displaycontext==MOD_DEST){
      edit_dest=rotaryEncoder.readEncoder();
    
    }


     if (displaycontext==SETT_MIDIRC){
      midi_channel=rotaryEncoder.readEncoder();
    
    }

       if (displaycontext==SETT_PBRANGE){
      pb_range=rotaryEncoder.readEncoder();
    
    }



    if (displaycontext==LOOP){
      int my_rot_val=rotaryEncoder.readEncoder();
      if (my_rot_val<4){hovermode=true;} else {
        hovermode=false;
      } 
      switch (my_rot_val){
        case 4:
        hovercontext=LOOP_LOOPST;
        break;
        case 5:
        hovercontext=LOOP_LOOPEN;
        break;
        case 6:
        hovercontext=LOOP_VIEW;
        break;
        case 7:
        hovercontext=LOOP_SAMP;
        break;
      }
     
    
    }

 if (displaycontext==LOOP_VIEW){
  int my_rot_val=rotaryEncoder.readEncoder();
      if (my_rot_val<4){hovermode=true;} else {
        hovermode=false;
      } 
      switch (my_rot_val){
        case 4:
        hovercontext=LOOP_VIEW_ZOOM;
        break;
        case 5:
        hovercontext=LOOP_VIEW_SEEK;
        break;
        case 6:
        hovercontext=LOOP;
        break;
        case 7:
        hovercontext=LOOP_SAMP;
        break;
      }
     
    
    }

    if (displaycontext==LOOP_SAMP){
      int my_rot_val=rotaryEncoder.readEncoder();
      if (my_rot_val<4){hovermode=true;} else {
        hovermode=false;
      } 
      switch (my_rot_val){
        case 4:
        hovercontext=LOOP_SAMP_START;
        break;
        case 5:
        hovercontext=LOOP_SAMP_END;
        break;
        case 6:
        hovercontext=LOOP_VIEW;
        break;
        case 7:
        hovercontext=LOOP;
        break;
      }
     
    
    }

if ((displaycontext>=SAVE_WAV_CHAR1) and (displaycontext<=SAVE_WAV_CHAR8)){
  if (rotaryEncoder.readEncoder()==58){rotaryEncoder.setEncoderValue(65);}
    if (rotaryEncoder.readEncoder()==64){rotaryEncoder.setEncoderValue(57);}
      if (rotaryEncoder.readEncoder()==91){rotaryEncoder.setEncoderValue(95);}
      if (rotaryEncoder.readEncoder()==94){rotaryEncoder.setEncoderValue(90);}
sample_name[displaycontext-58]=rotaryEncoder.readEncoder();
}

if ((displaycontext>=SAVE_BNK_CHAR1) and (displaycontext<=SAVE_BNK_CHAR8)){
  if (rotaryEncoder.readEncoder()==58){rotaryEncoder.setEncoderValue(65);}
    if (rotaryEncoder.readEncoder()==64){rotaryEncoder.setEncoderValue(57);}
      if (rotaryEncoder.readEncoder()==91){rotaryEncoder.setEncoderValue(95);}
      if (rotaryEncoder.readEncoder()==94){rotaryEncoder.setEncoderValue(90);}
bankname[displaycontext-130]=rotaryEncoder.readEncoder();
}

     if (drawing==false){

       if (((displaycontext!=SAMPLE) and ((displaycontext<SAMPLE_LENGTH) or (displaycontext>SAMPLE_START))) or (displaycontext==SAMPLE_LOAD)){
refreshDisplay();}}

     	Serial.println(rotaryEncoder.readEncoder());
       }
   
  }
  
  }

 


void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}
//const char str_prog[] = "PROG";
//const char str_edit[] = "EDIT";
//const char str_samp[] = "SAMP";
//const char str_sett[] = "SETT";
const char taskbar_strings[4][5]={ "PROG","EDIT", "SAMP", "SETT" };
const char *fm_strings[5]={"PLAY " ,"OPEN ", "BACK ", "USE ","    "};
const char *edit_strings[7]={ "AMP", "MOD", "MORE ", "LOOP", "FILTER","KEY SET","EDITOR"};
const char *amp_strings[4]={ "VOL", "PAN", "AMP EG",};
const char modmenu_strings[4][10]={ "LFO ","MOD EG", "EG DEPTH", "MODMATRIX" };
const char loopmenu_strings[2][10]={ "LOOPRANGE","LOOPMODE"};
const char lfomenu_strings[3][10]={ "LFO DEPTH","LFO RATE","LFO SHAPE"};
const char *more_strings[10]={ "CLONE", "INIT", "COPY", "SWAP", "VNUM","O2","O2INDEX","OFFSET","CCA","CCB"};
const char *keyset_strings[5]={ "TOPKEY", "COARSE TUNE","SQR TUNE","FINE TUNE","PORTATIME"};
//const char *more2_strings[10]={ "DUPLICATE", "SWAP","CLEAR","VASSIGN","O2INDEX","MOD EG","DEPTH","CCA","CCB","OFS"};
const char *sample_strings[5]={ "LENGTH", "THRSHLD","NEWPROG","LOAD","START"};
const char *editor_strings[4]={ "ZOOM", "SEEK","TOOLS","PROCESS"};
const char *proc_strings[10]={ "BOOST ", "CMBINE ","RECTIFY","FADE","INVERT","NRMLIZE","REVERSE","XFLOOP","FILTER","OCT-UP"};
const char *filter_d_strings[5]={ "CUTOFF", "RESON.","WIDTH","TYPE","PROCESS"};
const char *filter_strings[5]={ "TYPE","CUTOFF", "RESON.","ENV-GEN"};
const char *tools_strings[5]={ "SAVE WAV", "REPLACE","DUPLICATE","BACK","BACK"};
const char *save_strings[5]={ "NAME", "DIR","SAVE","BACK"};
const char *sett_strings[6]={ "CLEAR BANK", "LOAD BANK","SAVE BANK","MIDI CHAN.","BEND RANGE","SOFTWARE"};

const char *prog_strings[7]={ "PROGRAM", "POLYMODE","OSCMODE","INIT WS","CHORUS D","NEW"};
const char *polymode_strings[4]={ "STEAL", "PER SAMP.","WAIT"};
const char *sw_strings[3]={ "UPDATE", "BACK"};
const char *loop_strings[3]={ "START", "END"};
const char *oscmode_strings[6]={ "NORMAL", "CHORUS","DUAL","FM","SQR","SQR2"};
const char *filter_type_strings[5]={ "LP", "HP","BP","BR","PKG"};
const char *lfo_shape_strings[4]={ "SAW", "TRI","SQR","S&H"};
const char *dialog_strings[2]={ "OK", "BACK"};
const char *loopmode_strings[5]={ "SHOT", "LOOP","RSHT","RLOP","PIPO"};
const char *rect_strings[2]={" F"," H"};
const char *fade_strings[2]={" I"," O"};
const char *nprog_strings[2]={" N"," Y"};

const char *mod_strings[8]={"MOD ENV","VELOCITY","LFO","MIDI CC A","MODWHEEL","KEYFOLLOW","CCB"};
const char *mod_dest_strings[10]={" CUTOFF"," RESO","PITCH (A)","PITCH (B)","AMP","LFO RATE","LFO DEPTH","OFFSET","O2 INDEX","PAN"};
const char edit_strings_width[8]{21,21,28,27,39,45,27,42};
const char more_strings_width[10]{35,28,28,28,28,14,49,42,21,21};
const char keyset_strings_width[5]{48,78,49,54,54};
const char sample_strings_width[5]{36,42,36,24,30};
const char proc_strings_width[10]{36,42,42,24,36,42,42,36,36,36};
const char filter_d_strings_width[5]{36,36,30,24,42};
const char tools_strings_width[5]{56,49,63,35,28};
const char save_strings_width[5]{24,18,24,24};
const char prog_strings_width[7]{42,48,42,42,48,18};
const char mod_strings_width[8]{42,48,18,54,48,54,18};
const char more2_strings_width[10]{63,28,35,49,49,42,35,18,18,18};
const char sett_strings_width[6]{70,63,63,70,70,49};
const char sw_strings_width[2]{43,24};
const char amp_strings_width[3]{28,28,42};
const char ampmenu_strings_width[3]{18,18,36};
//replace with generic gen env later
void genAmpEnv(){
byte attackhpos=((256-wavesample_amp_atk[edit_wavesample])>>3)+1;
  byte sustainpos=(24-((wavesample_amp_sus[edit_wavesample]*3)>>5))+16;
  display.drawLine(1,40,attackhpos,16,WHITE); // set text
    //dec line
    byte decayhpos=((256-wavesample_amp_dec[edit_wavesample])>>3)+attackhpos;
  display.drawLine(attackhpos,16,decayhpos,sustainpos,WHITE);
   //sus line
  display.drawFastHLine(decayhpos,sustainpos,32,WHITE);
      //dec line
    byte releasehpos=((256-wavesample_amp_rel[edit_wavesample])>>3)+decayhpos+32;
  display.drawLine(decayhpos+32,sustainpos,releasehpos,40,WHITE);

}

void genFilterEnv(){
byte attackhpos=((256-wavesample_filter_atk[edit_wavesample])>>3)+1;
  byte sustainpos=(24-((wavesample_filter_sus[edit_wavesample]*3)>>5))+16;
  display.drawLine(1,40,attackhpos,16,WHITE); // set text
    //dec line
    byte decayhpos=((256-wavesample_filter_dec[edit_wavesample])>>3)+attackhpos;
  display.drawLine(attackhpos,16,decayhpos,sustainpos,WHITE);
   //sus line
  display.drawFastHLine(decayhpos,sustainpos,32,WHITE);
      //dec line
    byte releasehpos=((256-wavesample_filter_rel[edit_wavesample])>>3)+decayhpos+32;
  display.drawLine(decayhpos+32,sustainpos,releasehpos,40,WHITE);

}
void genModEnv(){
byte attackhpos=((256-wavesample_mod_atk[edit_wavesample])>>3)+1;
  byte sustainpos=(24-((wavesample_mod_sus[edit_wavesample]*3)>>5))+16;
  display.drawLine(1,40,attackhpos,16,WHITE); // set text
    //dec line
    byte decayhpos=((256-wavesample_mod_dec[edit_wavesample])>>3)+attackhpos;
  display.drawLine(attackhpos,16,decayhpos,sustainpos,WHITE);
   //sus line
  display.drawFastHLine(decayhpos,sustainpos,32,WHITE);
      //dec line
    byte releasehpos=((256-wavesample_mod_rel[edit_wavesample])>>3)+decayhpos+32;
  display.drawLine(decayhpos+32,sustainpos,releasehpos,40,WHITE);

}
void genWaveform(){
  
  int my_loopst=0;
  int my_loopen=0;
 for (int i=0;i<128;i++){
    
     int divisor=(wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample])/128;
    int true_samp_pos=wavesample_start[edit_wavesample]+(waveform_seek * (divisor))+ (i * (divisor>>zoom));
if ((true_samp_pos>=wavesample_loopst[edit_wavesample]) and (my_loopst==0)){
my_loopst=i;
}

if ((true_samp_pos>=wavesample_loopen[edit_wavesample]-divisor) and (my_loopen==0)){
my_loopen=i;
}
if (true_samp_pos>88064){true_samp_pos=88064;};
samp_level_buffer[i]=(int8_t)samp_memory[true_samp_pos];
//Serial.println(samp_level_buffer[i]);
}
    
for (int i=0;i<128;i++){
 if (samp_level_buffer[i]<wavesample_end[edit_wavesample]){
  display.setTextColor(WHITE);    // set text color
  //display.setCursor(i, (samp_level_buffer[i]+128)>>2);  
  if((i==my_loopst) or ((i==my_loopen)&&(my_loopen!=0))){
    display.drawFastVLine(i,22,22,WHITE);
  }
  if (samp_level_buffer[i]>0){
  display.drawFastVLine(i,31-((samp_level_buffer[i])/10),((samp_level_buffer[i])/10),WHITE); }   else{
display.drawFastVLine(i,31,((samp_level_buffer[i]*-1)/10),WHITE);
  }
  }
  //Serial.println(String(samp_level_buffer[i]));
  }  


}

void no_sd_error(){

  displaycontext=ERROR_NOSD;
hovermode=false;
//hovercontext=DIALOG_D2Z_OK;
//menuoffset=DIALOG_D2Z_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
}


void errorPreviewMemory(){

  displaycontext=ERROR_PREVMEM;
hovermode=false;
//hovercontext=DIALOG_D2Z_OK;
//menuoffset=DIALOG_D2Z_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
enable_click=true;
      rotaryEncoder.setEncoderValue(4);
}

void no_sd_error2(){

  displaycontext=ERROR_NOSDB;
hovermode=false;
//hovercontext=DIALOG_D2Z_OK;
//menuoffset=DIALOG_D2Z_OK-4;
rotaryEncoder.setBoundaries(0,5, true); 
      rotaryEncoder.setEncoderValue(4);
}

void refreshDisplay(){
  
  if (displaycontext!=SAVE_SCRN){
  drawing=true;
  
  display.clearDisplay();
    display.setTextSize(1);
  display.setTextColor(WHITE);
 for (byte i=0;i<4;i++){
  byte soffset=0;
  if (fm_type>=FM_BANK &&  ((hovercontext<=FM_USE) && (hovercontext>=FM_PREVIEW)) ){
    soffset=1;
    }
   display.setCursor(4+(i*30),1);
 if ((displaycontext==i) or ((displaycontext-300)==i)){
display.fillRect(((i-soffset)*30)+1,0,28,10,WHITE);
  display.setTextColor(BLACK);
 } else {
  display.setTextColor(WHITE);

 }

 if ((hovercontext==i) or ((hovercontext-300)==i)){
   display.drawRect(((i-soffset)*30)+1,0,28,10,WHITE);

 }


//if (fm_type)

 if ((hovercontext<=FM_USE) && (hovercontext>=FM_PREVIEW)) {
  // if ((fm_type<FM_DIRWAV) && (i>1)) {i=5}
   display.print(*(fm_strings+i+(soffset)));
    if ((fm_type==FM_SAMPLE) && (i==2)) {i=5;}
    if ((fm_type==FM_BANK) && (i==1)) {i=5;}
   } else {display.print(*(taskbar_strings+i));}
 
 }
 
 const int epd_bitmap_allArray_LEN = 3;
const unsigned char* epd_bitmap_allArray[4] = {
	epd_bitmap_wompler1,
	epd_bitmap_wompler2,
	epd_bitmap_wompler3,
  epd_bitmap_wompler2
};

 if (displaycontext==EDIT or displaycontext==EDIT_SAMP){

   if (user_wavesamples>0){
 display.setTextColor(WHITE);
if (displaycontext==EDIT_SAMP){  display.fillRect(0,16,43,8,WHITE); display.setTextColor(BLACK);}
if (hovercontext==EDIT_SAMP){ display.drawRect(0,16,43,8,WHITE);}
display.setCursor(2,16);
display.print("SAMPLE:");
display.setCursor(44,16);
 display.setTextColor(WHITE);
display.print(String((int8_t)edit_wavesample));


   for (byte i=0;i<7;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%4)*10)+25;

 if (i<4){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+5){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)edit_strings_width[i],11,WHITE);}
    if (displaycontext==i+5){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)edit_strings_width[i],11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(edit_strings+i));

 }
 } else {
  display.drawBitmap(0, 16, epd_bitmap_allArray[gp_reg8a>>6], 123,22, WHITE);
  display.setCursor(2,40);
 display.print("Add Samples to Begin");
   display.setCursor(2,55);
 display.print(FIRMWARE_VERSION);
 }
 }

 
if ((displaycontext==MODMATRIX) or ((displaycontext>=MOD_DEST) and (displaycontext<=MOD_CCB))){
 display.setTextColor(WHITE);
if (displaycontext==MOD_DEST){  display.fillRect(0,16,43,8,WHITE); display.setTextColor(BLACK);}
if (hovercontext==MOD_DEST){ display.drawRect(0,16,43,8,WHITE);}
display.setCursor(2,16);
display.print("DEST:");
display.setCursor(44,16);
 display.setTextColor(WHITE);
display.print(*(mod_dest_strings+edit_dest));


   for (byte i=0;i<7;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%4)*10)+25;

 if (i<4){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+(MOD_MOD_ENV)){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)mod_strings_width[i],11,WHITE);}
    if (*(wavesample_mod_assigns[edit_dest]+edit_wavesample)==i+1){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)mod_strings_width[i],11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(mod_strings+i));

 }
 
}

 if ((displaycontext==AMP_EG) or ((displaycontext > 11) and (displaycontext<16))){
genAmpEnv();
  
  for(byte i;i<4;i++){
        byte xpos=1+(i*36);
    if (hovercontext==i+12){ display.drawFastHLine(xpos,51,18,WHITE);}
if (displaycontext==i+12){display.fillRect(xpos,42,18,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,42);
  display.print(*(env_strings+i));
   display.setCursor(xpos,52);
     display.setTextColor(WHITE);
   byte env_percent;
   
   env_percent=(*(*(wavesample_amp_env + i) + edit_wavesample));
   display.print(env_percent);
  }

 }

 
 if (((displaycontext >= FILTER_EG) and (displaycontext<=FILTER_EG_REL))){
genFilterEnv();
  
  for(byte i;i<4;i++){
        byte xpos=1+(i*36);
    if (hovercontext==i+FILTER_EG_ATK){ display.drawFastHLine(xpos,51,18,WHITE);}
if (displaycontext==i+FILTER_EG_ATK){display.fillRect(xpos,42,18,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,42);
  display.print(*(env_strings+i));
   display.setCursor(xpos,52);
     display.setTextColor(WHITE);
   byte env_percent;
   
   env_percent=(*(*(wavesample_filter_env + i) + edit_wavesample)); 
   display.print(env_percent);
  }

 }

 
 if ((displaycontext==MOD_EG) or ((displaycontext >= MOD_EG_ATK) and (displaycontext<=MOD_EG_REL))){
genModEnv();
  
  for(byte i;i<4;i++){
        byte xpos=1+(i*36);
    if (hovercontext==i+MOD_EG_ATK){ display.drawFastHLine(xpos,51,18,WHITE);}
if (displaycontext==i+MOD_EG_ATK){display.fillRect(xpos,42,18,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,42);
  display.print(*(env_strings+i));
   display.setCursor(xpos,52);
     display.setTextColor(WHITE);
   byte env_percent;
   
   env_percent=(*(*(wavesample_mod_env + i) + edit_wavesample)); 
   display.print(env_percent);
  }

 }

  if ((displaycontext==AMP_MENU) or ((displaycontext >=EDIT_VOL) and (displaycontext<=EDIT_PAN))){

  
  for(byte i=0;i<3;i++){
     byte vpos=((i%4)*10)+25;
     if (i==1){  display.setCursor(28,vpos);display.print(String((int8_t)wavesample_pan[edit_wavesample]));};

   if (i==0){  display.setCursor(28,vpos);display.print(String((int8_t)wavesample_vol[edit_wavesample]));


 };
        byte xpos=2;
    if (hovercontext==i+EDIT_VOL){ display.drawFastHLine(xpos,vpos+8,(int16_t)ampmenu_strings_width[i],WHITE);}
if (displaycontext==i+EDIT_VOL){display.fillRect(xpos,vpos,(int16_t)ampmenu_strings_width[i],11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,vpos);
  display.print(*(amp_strings+i));
   display.setCursor(xpos,vpos);
     display.setTextColor(WHITE);
  
  }

 }

   if ((displaycontext==MOD_MENU) or ((displaycontext ==MOD_EG_DEPTH))){

  
  for(byte i=0;i<4;i++){
     byte vpos=((i%4)*10)+18;
     if (i==2){  display.setCursor(60,vpos);display.print(String((int8_t)wavesample_menv_depth[edit_wavesample]));};

 
        byte xpos=2;
    if (hovercontext==i+LFO_MENU){ display.drawFastHLine(xpos,vpos+7,54,WHITE);}
if (displaycontext==i+LFO_MENU){display.fillRect(xpos,vpos,54,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,vpos);
  display.print(*(modmenu_strings+i));
   display.setCursor(xpos,vpos);
     display.setTextColor(WHITE);
  
  }

 }

if ((displaycontext==LOOP_MENU) or ((displaycontext ==MORE_LOOPMODE))){

  
  for(byte i=0;i<2;i++){
     byte vpos=((i%4)*10)+25;
      if (i==1){  display.setCursor(60,vpos);display.print((*(loopmode_strings+wavesample_loopmode[edit_wavesample])));}

 
        byte xpos=2;
    if (hovercontext==i+LOOP){ display.drawFastHLine(xpos,vpos+7,60,WHITE);}
if (displaycontext==i+LOOP){display.fillRect(xpos,vpos,60,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,vpos);
  display.print(*(loopmenu_strings+i));
   display.setCursor(xpos,vpos);
     display.setTextColor(WHITE);
  
  }

 }

 if ((displaycontext==LFO_MENU) or ((displaycontext >= MORE_LFODEPTH) and (displaycontext<=MORE_LFOSHAPE))){

  
  for(byte i=0;i<3;i++){
     byte vpos=((i%4)*10)+25;
       if (i==0){  display.setCursor(60,vpos);display.print(wavesample_lfodepth[edit_wavesample]);};
 if (i==1){  display.setCursor(60,vpos);display.print(wavesample_lforate[edit_wavesample]);};

   if (i==2){  display.setCursor(60,vpos);display.print((*(lfo_shape_strings+wavesample_lfoshape[edit_wavesample])));}

 
        byte xpos=2;
    if (hovercontext==i+MORE_LFODEPTH){ display.drawFastHLine(xpos-2,vpos+7,54,WHITE);}
if (displaycontext==i+MORE_LFODEPTH){display.fillRect(xpos-2,vpos,54,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,vpos);
  display.print(*(lfomenu_strings+i));
   display.setCursor(xpos,vpos);
     display.setTextColor(WHITE);
  
  }

 }

 if ((displaycontext==KEYSET) or ((displaycontext >= KEYSET_TOPKEY) and (displaycontext<=KEYSET_PORTATIME))) {
for(byte i=0;i<5;i++){
  display.setTextColor(WHITE);
byte hpos=2;
 byte vpos=(i*10)+16;
    
    switch (i+KEYSET_TOPKEY){

case KEYSET_TOPKEY:
   display.setCursor(keyset_strings_width[i],vpos);display.print(String((int8_t)wavesample_topkey[edit_wavesample]));
break;
case KEYSET_CTUNE:
  display.setCursor(keyset_strings_width[i],vpos);display.print(String((int8_t)wavesample_ctune[edit_wavesample]));
break;
case KEYSET_KEYT:
  display.setCursor(keyset_strings_width[i],vpos);display.print(String((int8_t)wavesample_sqr_ctune[edit_wavesample]));
break;
case KEYSET_FTUNE:
   display.setCursor(keyset_strings_width[i],vpos);display.print(String((int8_t)wavesample_detune[edit_wavesample]));
break;
case KEYSET_PORTATIME:
   display.setCursor(keyset_strings_width[i],vpos);display.print(String((float)((float)wavesample_portatime[edit_wavesample])/(float)sample_rate)+"sec");
break;
      }

  if (hovercontext==i+KEYSET_TOPKEY){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)keyset_strings_width[i],11,WHITE);}
    if (displaycontext==i+KEYSET_TOPKEY){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)keyset_strings_width[i],11,WHITE);
   display.setTextColor(BLACK);
   }
 display.setCursor(hpos,vpos);
 display.print(*(keyset_strings+i));


 }}

 if ((displaycontext==SAMPLE) or ((displaycontext > 19) and (displaycontext<SAMPLE_LOAD)) or (displaycontext==REPLACE)) {
    display.setTextColor(WHITE);
display.setCursor(2,16);
display.print("REMAINING:");
display.setCursor(60,16);
 display.setTextColor(WHITE);
 float dorf=88064.00-(sample_pos*512.00);
 //Serial.println(sample_pos);
display.print(String((float)(dorf)/(float)15000)+"sec");


for(byte i=0;i<5;i++){
  display.setTextColor(WHITE);
byte hpos=2;
 byte vpos=(i*10)+25;
    
if (i>2){
 hpos=56;
  vpos=((i-2)*10)+25;
    

}

    switch (i+20){
case SAMPLE_LENGTH:
  display.setCursor(sample_strings_width[i],vpos);display.print(String((float)((float)((float)sample_length * 512)/(float)15000))+"sec");
break;
case SAMPLE_THRESHOLD:
   display.setCursor(sample_strings_width[i],vpos);display.print(String((int8_t)sample_threshold));
break;
case SAMPLE_NEWPROG:
   display.setCursor(sample_strings_width[i],vpos);display.print((*(nprog_strings+nprog)));;
break;

      }

  if (hovercontext==i+20){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)sample_strings_width[i],11,WHITE);}
    if (displaycontext==i+20){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)sample_strings_width[i],11,WHITE);
   display.setTextColor(BLACK);
   }


 
  display.setCursor(hpos,vpos);
 display.print(*(sample_strings+i));}
display.setCursor(2,55);
display.drawFastHLine(2,55,(samp_level_average ),WHITE);
  display.setCursor(2+(sample_threshold * 3),57);
  display.setTextColor(WHITE);
  display.print("^");
 }

if (displaycontext == SAMPLE_LOAD) {
  
listDir(SD,test_dir,0,false);

}

if (displaycontext == FM_SELECT) {
  
listDir(SD,test_dir,0,false);

}

if (displaycontext == SAVE_WAV_DIR) {
listDir(SD,test_dir,0,false);

}

if (displaycontext == SETT_LOADBNK) {
listDir(SD,test_dir,0,false);

}

if (displaycontext == SAVE_BNK_DIR) {
listDir(SD,test_dir,0,false);

}

if (displaycontext==SAMPLE_START){
  display.setCursor(21,23);

if (172-sample_pos>0){

if ((sample_threshold>0) && (sampling==false)){display.print("WAITING..");} else {
display.print("SAMPLING..");

}


} else {

display.print("OUT OF MEMORY");
}

}

if (displaycontext==SW_UPDATE){
  display.setCursor(21,23);
display.print("UPDATING..");
display.drawFastHLine(10,36,update_prog,WHITE);
  display.setCursor(21,40);
  display.print(update_prog);
}

if (displaycontext==ERROR_NOSW){
  display.setCursor(0,23);
display.print("ERROR: no firmware.bin in root of SD");
hovercontext=SETTINGS;
hovermode=false;
}

if (displaycontext==ERROR_PREVMEM){
  display.setCursor(0,23);
display.print("ERROR: Out of memory");
hovercontext=FM_SELECT;
hovermode=false;
}

if ((displaycontext > SAMPLE_START) and (displaycontext<TOOLS)){
  genWaveform();
 
  for(byte i=0;i<4;i++){
        byte xpos=1+(i*36);
        byte yoffset=0;
        if (i==3){yoffset=10; xpos=73;};
    if (hovercontext==i+ZOOM){ display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
if (displaycontext==i+ZOOM){display.fillRect(xpos,42+yoffset,18,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,44+yoffset);
  display.print(*(editor_strings+i));
   display.setCursor(xpos,54);
     display.setTextColor(WHITE);
   
   switch (i){

     case 0:
   display.print(zoom);
   break;
     case 1:
   display.print(waveform_seek);
   break;
   }
  }

 }


 if (((displaycontext>=PROCESS) and (displaycontext<=FADE))){
 display.setTextColor(WHITE);

   for (byte i=0;i<10;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%5)*10)+17;
  if (i==0){  display.setCursor(2+proc_strings_width[i],vpos);display.print(String(boost_factor));};
 if (i==1){  display.setCursor(2+proc_strings_width[i],vpos);display.print(String((int8_t)wavesample_combine));};

   if (i==2){  display.setCursor(2+proc_strings_width[i],vpos);display.print((*(rect_strings+gp_reg8a)));}
 if (i==3){  display.setCursor(2+proc_strings_width[i],vpos);display.print((*(fade_strings+gp_reg8b)));}

 
 if (i<5){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+BOOST){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)proc_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+BOOST){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)proc_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(proc_strings+i));

 }
 
 }


if ((displaycontext==MORE) or ((displaycontext>=MORE_COPYSET) and (displaycontext<=MORE2_CCB))){
 display.setTextColor(WHITE);

   for (byte i=0;i<10;i++){
      display.setTextColor(WHITE);
      
byte hpos=60;
 if (i<5){
   hpos=2;
 
   } 
 byte vpos=((i%5)*10)+17;
 if (i==2 && displaycontext==MORE_COPYSET) {  display.setCursor(hpos+42,vpos);display.print(edit_wavesample_2);};
  if (i==3 && displaycontext==MORE2_SWAPZONE) {  display.setCursor(hpos+42,vpos);display.print(edit_wavesample_2);};
 if (i==4){  display.setCursor(hpos+42,vpos);display.print((int8_t)wavesample_vassign[edit_wavesample]);};

   if (i==5){  display.setCursor(hpos+24,vpos);display.print((int8_t)wavesample_wsb[edit_wavesample]);}
 if (i==6){  display.setCursor(hpos+42,vpos);display.print(wavesample_init_o2index[edit_wavesample]);}
if (i==7){  display.setCursor(hpos+42,vpos);display.print(wavesample_init_offset[edit_wavesample]);};
 if (i==8){  display.setCursor(hpos+24,vpos);display.print(wavesample_cc_a[edit_wavesample]);};
  if (i==9){  display.setCursor(hpos+24,vpos);display.print(wavesample_cc_b[edit_wavesample]);};

    display.setCursor(hpos,vpos);
  if (hovercontext==i+MORE2_DUPLICATE2ZONE){
    
   display.drawRect(hpos-2,vpos-2,more_strings_width[i],11,WHITE);}
    if (displaycontext==i+MORE2_DUPLICATE2ZONE){
    
   display.fillRect(hpos-2,vpos-2,more_strings_width[i],11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(more_strings+i));

 }
 
 }


/*

 if ((displaycontext==MORE) or ((displaycontext>=MORE2_SWAPZONE) and (displaycontext<=MORE2_CCB))){
 display.setTextColor(WHITE);

   for (byte i=0;i<8;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%5)*10)+17;
  if (i==1){  display.setCursor(2+more_strings_width[i],vpos);display.print(wavesample_lfodepth[edit_wavesample]);};
 if (i==2){  display.setCursor(2+more_strings_width[i],vpos);display.print(wavesample_lforate[edit_wavesample]);};

   if (i==3){  display.setCursor(2+more_strings_width[i],vpos);display.print((*(lfo_shape_strings+wavesample_lfoshape[edit_wavesample])));}
 if (i==4){  display.setCursor(2+more_strings_width[i],vpos);display.print((*(loopmode_strings+wavesample_loopmode[edit_wavesample])));}
if (i==5){  display.setCursor(62+more_strings_width[i],vpos);display.print(wavesample_wsb[edit_wavesample]);};
 
 if (i<5){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+MODMATRIX){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)more_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+MODMATRIX){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)more_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(more_strings+i));

 }
 
 }*/

/*
bool is_more2=false;
 if ((displaycontext==MORE_MORE2) or ((displaycontext>=MORE2_DUPLICATE2ZONE) and (displaycontext<=MORE2_INITO2INDEX))){ is_more2=true;}
 if (displaycontext==MORE2_CCB || displaycontext==MORE2_CCA || displaycontext==MORE2_IOFFSET){is_more2=true;}
 if ((is_more2==true)){
 display.setTextColor(WHITE);

   for (byte i=0;i<10;i++){
      display.setTextColor(WHITE);
byte hpos=66;
 byte vpos=((i%5)*10)+17;


   if (i==3){  display.setCursor(2+more2_strings_width[i],vpos);display.print((int8_t)wavesample_vassign[edit_wavesample]);}
 if (i==4){  display.setCursor(2+more2_strings_width[i],vpos);display.print(wavesample_init_o2index[edit_wavesample]);}
 

 
 if (i<5){
   hpos=2;
 
   } 
   if (i==6){  display.setCursor(hpos+more2_strings_width[i],vpos);display.print(wavesample_menv_depth[edit_wavesample]);}
    if (i==7){  display.setCursor(hpos+more2_strings_width[i],vpos);display.print(wavesample_cc_a[edit_wavesample]);}
       if (i==8){  display.setCursor(hpos+more2_strings_width[i],vpos);display.print(wavesample_cc_b[edit_wavesample]);}
        if (i==9){  display.setCursor(hpos+more2_strings_width[i],vpos);display.print(wavesample_init_offset[edit_wavesample]);}
  if (hovercontext==i+MORE2_DUPLICATE2ZONE){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)more2_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+MORE2_DUPLICATE2ZONE){
      if (i<3){  display.setCursor(2+more2_strings_width[i],vpos);display.print(edit_wavesample_2);};
 
   display.fillRect(hpos-2,vpos-2,(int16_t)more2_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
   display.setCursor(hpos,vpos);
 display.print(*(more2_strings+i));

 }
 
 }*/

if (displaycontext==ERROR_NOSD){display.setCursor(2,17);display.print("Insert SD then click");
  display.drawRect(45,32,32,11,WHITE);
   display.setCursor(56,34);
 display.print(*(dialog_strings));
}

if (displaycontext==ERROR_NOSDB){display.setCursor(2,17);display.print("Do not remove SD while ON. Reset with SD inserted.");
  display.drawRect(45,42,32,11,WHITE);
   display.setCursor(56,44);
 display.print(*(dialog_strings));
}

if (displaycontext>=900){
display.setCursor(2,17);display.print("WARNING: May result in loss of info");

for (uint8_t i=0;i<2;i++){
 if (hovercontext==menuoffset+4+i){
    
   display.drawRect(32+(48*i),32,32,11,WHITE);}
   display.setCursor(34+(48*i),32);
 display.print(*(dialog_strings+i));

}

}


 if ((displaycontext==FILTER_D) or ((displaycontext>=FILTER_D_CTF) and (displaycontext<=FILTER_D_PROCESS))){
 display.setTextColor(WHITE);

   for (byte i=0;i<5;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%5)*10)+17;
  if (i==0){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print(String(f));};
 if (i==1){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print(String(q));};

   if (i==2){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print(width);}
 if (i==3){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print((*(filter_type_strings+filter_d_type)));}

 
 if (i<5){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+FILTER_D_CTF){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)filter_d_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+FILTER_D_CTF){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)filter_d_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(filter_d_strings+i));

 }
 
 }

 

 if ((displaycontext==FILTER) or ((displaycontext>=FILTER_TYPE) and (displaycontext<=FILTER_RES))){
 display.setTextColor(WHITE);

   for (byte i=0;i<5;i++){
      display.setTextColor(WHITE);
byte hpos=70;
 byte vpos=((i%5)*10)+17;
  if (i==1){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print(String(wavesample_filter_offset[edit_wavesample]));};
 if (i==2){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print(String(wavesample_filter_reso[edit_wavesample]));};


 if (i==0){  display.setCursor(9+filter_d_strings_width[i],vpos);display.print((*(filter_type_strings+wavesample_filter_type[edit_wavesample])));}

 

   hpos=2;
 
   
    display.setCursor(hpos,vpos);
  if (hovercontext==i+FILTER_TYPE){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)filter_d_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+FILTER_TYPE){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)filter_d_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(filter_strings+i));

 }
 
 }
if  (displaycontext==SETT_SOFTWARE){
 display.setTextColor(WHITE);
    display.setCursor(2,17);
     display.print(FIRMWARE_VERSION);
   for (byte i=0;i<2;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%5)*10)+27;
 
 

    
 if (i<5){
   hpos=2;
 
   } 
 
   
    display.setCursor(hpos,vpos);
  if (hovercontext==i+SW_UPDATE){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)sw_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+SW_UPDATE){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)sw_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(sw_strings+i));
   }
 
 }

 if (((displaycontext==SETTINGS) or  (displaycontext==SETT_MIDIRC)) or  (displaycontext==SETT_PBRANGE)){
 display.setTextColor(WHITE);

   for (byte i=0;i<6;i++){
      display.setTextColor(WHITE);
byte hpos=70;
 byte vpos=((i%5)*10)+17;
  if (i==3){  display.setCursor(9+sett_strings_width[i],vpos);display.print(midi_channel);};
 
 if (i==4){  display.setCursor(9+sett_strings_width[i],vpos);display.print((pb_range+1)*2);};
 

    
 if (i<5){
   hpos=2;
 
   } 
 
   
    display.setCursor(hpos,vpos);
  if (hovercontext==i+SETT_CLEARBNK){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)sett_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+SETT_CLEARBNK){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)sett_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(sett_strings+i));
   }
 }

 if ((displaycontext==TOOLS) ){
 display.setTextColor(WHITE);

   for (byte i=0;i<4;i++){
      display.setTextColor(WHITE);
byte hpos=2;
 byte vpos=((i%5)*10)+19;
 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+SAVE_AS_WAV){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)tools_strings_width[i]+2,11,WHITE);}
   
 display.print(*(tools_strings+i));

 }
 
 }

 
 if (displaycontext==SAVE_AS_WAV){
 display.setTextColor(WHITE);

   for (byte i=0;i<4;i++){
      display.setTextColor(WHITE);
byte hpos=2;
 byte vpos=((i%5)*10)+16;
  if (i==0){  display.setCursor(2+save_strings_width[i]+7,vpos);display.print(sample_name);};
 if (i==1){  display.setCursor(2+save_strings_width[i]+7,vpos);display.print(root.path());};


 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+SAVE_WAV_NAME){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)save_strings_width[i]+2,11,WHITE);}
   
 display.print(*(save_strings+i));

 }
 
 }

 if ((displaycontext==SAVE_WAV_NAME) or ((displaycontext>=SAVE_WAV_CHAR1) and (displaycontext<=WAV_NAME_CONFIRM))){
 display.setTextColor(WHITE);

   for (byte i=0;i<8;i++){
     byte hpos=(i*7)+18;
 byte vpos=24;
      
      if (displaycontext==i+SAVE_WAV_CHAR1){
        display.fillRect(hpos,vpos,(int16_t)7,11,WHITE);
      display.setTextColor(BLACK);} else{display.setTextColor(WHITE);}



 
    display.setCursor(hpos,vpos);
 
  if (hovercontext==i+SAVE_WAV_CHAR1){
    
 display.drawFastHLine(hpos,vpos+11,7,WHITE);}
   
 display.print(sample_name[i]);

 }
  display.setCursor(32,50);
  display.print("BACK");
  if (hovercontext==WAV_NAME_CONFIRM){
    
 display.drawFastHLine(32,61,7,WHITE);}
 }

  if (displaycontext==SETT_SAVEBNK){
 display.setTextColor(WHITE);

   for (byte i=0;i<4;i++){
      display.setTextColor(WHITE);
byte hpos=2;
 byte vpos=((i%5)*10)+16;
  if (i==0){  display.setCursor(2+save_strings_width[i]+7,vpos);display.print(bankname);};
 if (i==1){  display.setCursor(2+save_strings_width[i]+7,vpos);display.print(root.path());};


 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+SAVE_BNK_NAME){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)save_strings_width[i]+2,11,WHITE);}
   
 display.print(*(save_strings+i));

 }
 
 }

 if ((displaycontext==SAVE_BNK_NAME) or ((displaycontext>=SAVE_BNK_CHAR1) and (displaycontext<=BNK_NAME_CONFIRM))){
 display.setTextColor(WHITE);

   for (byte i=0;i<8;i++){
     byte hpos=(i*7)+18;
 byte vpos=24;
      
      if (displaycontext==i+SAVE_BNK_CHAR1){
        display.fillRect(hpos,vpos,(int16_t)7,11,WHITE);
      display.setTextColor(BLACK);} else{display.setTextColor(WHITE);}



 
    display.setCursor(hpos,vpos);
 
  if (hovercontext==i+SAVE_BNK_CHAR1){
    
 display.drawFastHLine(hpos,vpos+11,7,WHITE);}
   
 display.print(bankname[i]);

 }
  display.setCursor(32,50);
  display.print("BACK");
  if (hovercontext==BNK_NAME_CONFIRM){
    
 display.drawFastHLine(32,61,7,WHITE);}
 }


if ((displaycontext==PROG) or ((displaycontext>=PROG_NUMBER) and (displaycontext<=PROG_NEWPROG))){
 display.setTextColor(WHITE);

   for (byte i=0;i<6;i++){
      display.setTextColor(WHITE);
byte hpos=60;
 byte vpos=((i%5)*10)+17;
  if (i==0){  display.setCursor(2+prog_strings_width[i],vpos);display.print(String(program));};
 if (i==1){  display.setCursor(2+prog_strings_width[i],vpos);display.print((*(polymode_strings+program_polymode[program])));};

   if (i==2){  display.setCursor(2+prog_strings_width[i],vpos);display.print((*(oscmode_strings+program_osc_mode[program])));}
 if (i==3){  display.setCursor(2+prog_strings_width[i],vpos);display.print(String(program_init_ws[program]));};
 if (i==4){  display.setCursor(2+prog_strings_width[i],vpos);display.print(program_oscbdetune[program]);};

 
 if (i<5){
   hpos=2;
 
   } 
    display.setCursor(hpos,vpos);
  if (hovercontext==i+PROG_NUMBER){
    
   display.drawRect(hpos-2,vpos-2,(int16_t)prog_strings_width[i]+2,11,WHITE);}
    if (displaycontext==i+PROG_NUMBER){
    
   display.fillRect(hpos-2,vpos-2,(int16_t)prog_strings_width[i]+2,11,WHITE);
   display.setTextColor(BLACK);
   }
 display.print(*(prog_strings+i));

 }
 
 }

 if ((displaycontext==LOOP) or ((displaycontext >= LOOP_LOOPST) and (displaycontext<=LOOP_SAMP_END))){
   int menu_start=LOOP_LOOPST;
  genWaveform();


 switch(displaycontext){
case LOOP:
case LOOP_LOOPST:
case LOOP_LOOPEN:
menu_start=LOOP_LOOPST;
break;

case LOOP_VIEW:
case LOOP_VIEW_ZOOM:
case LOOP_VIEW_SEEK:
menu_start=LOOP_VIEW_ZOOM;
break;

case LOOP_SAMP:
case LOOP_SAMP_START:
case LOOP_SAMP_END:
menu_start=LOOP_SAMP_START;
break;

 }
  
for(byte i=0;i<4;i++){
        byte xpos=1+(i*36);
        byte yoffset=0;
        if (i==3){yoffset=10; xpos=73;};
    if (hovercontext==i+menu_start){ display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
 
if (displaycontext==i+menu_start){display.fillRect(xpos,42+yoffset,18,11,WHITE);  display.setTextColor(BLACK);}
    display.setCursor(xpos,44+yoffset);
       if (menu_start==LOOP_LOOPST){
    if ((i==2) and (hovercontext==LOOP_VIEW)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    if ((i==3) and (hovercontext==LOOP_SAMP)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    
    if (i<2){
  display.print(*(loop_strings+i));} else{
    if (i==2){display.print("VIEW");}
    if (i==3){display.print("SAMP");}
  }
   display.setCursor(xpos,54);
     display.setTextColor(WHITE);
   
   switch (i){

     case 0:
   display.print(wavesample_loopst[edit_wavesample]-wavesample_start[edit_wavesample]);
   break;
     case 1:
   display.print(wavesample_loopen[edit_wavesample]-wavesample_start[edit_wavesample]);
   break;
   }
       }
             if (menu_start==LOOP_VIEW_ZOOM){
    if( (i==2) and (hovercontext==LOOP)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    if ((i==3) and (hovercontext==LOOP_SAMP)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    
    if (i<2){
  display.print(*(editor_strings+i));} else{
    if (i==2){display.print("LOOP");}
    if (i==3){display.print("SAMP");}
  }
   display.setCursor(xpos,54);
     display.setTextColor(WHITE);
   
   switch (i){

     case 0:
   display.print(zoom);
   break;
     case 1:
   display.print(waveform_seek);
   break;
   }
       }

             if (menu_start==LOOP_SAMP_START){
    if ((i==2) and (hovercontext==LOOP_VIEW)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    if ((i==3) and (hovercontext==LOOP)){display.drawFastHLine(xpos,51+yoffset,18,WHITE);}
    
    if (i<2){
  display.print(*(loop_strings+i));} else{
    if (i==2){display.print("VIEW");}
    if (i==3){display.print("LOOP");}
  }
       
   display.setCursor(xpos,54);
     display.setTextColor(WHITE);
   
   switch (i){

     case 0:
   display.print(wavesample_start[edit_wavesample]);
   break;
     case 1:
   display.print(wavesample_end[edit_wavesample]);
   break;
   }
   }
  }

 
 
  

 }

  display.display();
  drawing=false;
 }}
 

void checkModEnv(){
  
    if (voices_init==true){
  int mod_env_now=0;
  int mod_env_nowt=0;
   int mod_env_rn=0;
  char my_wavesample=0;

for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];
 men[j]=p_mod_env[j+1]->tick();
mod_env_now=(men[j]*wavesample_menv_depth[my_wavesample])>>4;
 if (mod_env_now<0){
 mod_env_rn=mod_env_now+130048;} else {mod_env_rn=mod_env_now;};
 //men[j]=mod_env_nowt;
if (wavesample_cutoff_mod[my_wavesample]==MOD_ENV){ partials[j]->setCutoffFreq((mod_env_rn>>10)+wavesample_filter_offset[my_wavesample]);}
if (wavesample_reso_mod[my_wavesample]==MOD_ENV){        partials[j]->setResonance(mod_env_rn>>9);}
/*if (wavesample_o2index_mod[my_wavesample]==MOD_ENV){       
  
 partials[j]->setO2Index(mod_env_rn>>10);
  }*/
if (wavesample_o2detune_mod[my_wavesample]==MOD_ENV){                 if (osc_mode>0){ dualosc[j]->setLFM((int)((mod_env_now)));}}
if (wavesample_oap_mod[my_wavesample]==MOD_ENV){                 partials[j]->setLFM((int)((mod_env_now)));} 
if (wavesample_amp_mod[my_wavesample]==MOD_ENV){    int64_t amp_calc= (logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]]);
   //amp_calc=constrain(((amp_calc<<2)+(((lfo_now)>>1)*amp_calc))>>12,0,32767);
   int gdepth=(int)((((wavesample_menv_depth[my_wavesample]+128)<<6)*(amp_calc))>>6)/100;
   amp_calc=((amp_calc*5)+(((men[j]-8192)*(int16_t)gdepth)>>14))-(int16_t)gdepth;
    partials[j]->setVolume((int16_t)amp_calc);}
if (wavesample_pan_mod[my_wavesample]==MOD_ENV){  partials[j]->setPan(mod_env_rn>>3);}
//if (wavesample_so_mod[my_wavesample]==MOD_ENV){  }
if (wavesample_lfr_mod[my_wavesample]==MOD_ENV){ lfo_rate[j]=(mod_env_rn>>10);}
if (wavesample_lfd_mod[my_wavesample]==MOD_ENV){ lfo_depth[j]=(mod_env_rn>>10);}
}

}

}
void checkVelo(){
    if (voices_init==true){
  int16_t velocity_now;
  char my_wavesample;
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];
 velocity_now=p_velo[j];
 
if (wavesample_cutoff_mod[my_wavesample]==VELOCITY){ int8_t cf=(int8_t)constrain(((((int16_t)velocity_now-64))+(int16_t)wavesample_filter_offset[my_wavesample]),-127,127);partials[j]->setCutoffFreq(cf);}
if (wavesample_reso_mod[my_wavesample]==VELOCITY){  int8_t rs=(uint8_t)constrain(((((int16_t)velocity_now))+(int16_t)wavesample_filter_reso[my_wavesample]),0,255); partials[j]->setResonance(rs);}
if (wavesample_o2index_mod[my_wavesample]==VELOCITY){        if (osc_mode!=FM_MODE) { partials[j]->setO2Index(velocity_now);}}
if (wavesample_o2detune_mod[my_wavesample]==VELOCITY){                 if (osc_mode>0){ dualosc[j]->setLFM((int)((velocity_now-64)<<8));}} 
if (wavesample_oap_mod[my_wavesample]==VELOCITY){           partials[j]->setLFM((int)((velocity_now-64)<<8));} 
//if (wavesample_amp_mod[my_wavesample]==VELOCITY){  p_amp_env[j]->setVolume(((logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]])>>7)+(velocity_now>>6));}
if (wavesample_pan_mod[my_wavesample]==VELOCITY){  partials[j]->setPan(velocity_now<<7);}
//if (wavesample_so_mod[my_wavesample]==VELOCITY){   partials[j]->setOffset(velocity_now<<6);}
if (wavesample_lfr_mod[my_wavesample]==VELOCITY){ lfo_rate[j]=(velocity_now);}
if (wavesample_lfd_mod[my_wavesample]==VELOCITY){ lfo_depth[j]=(velocity_now);}
}}
}

void checkLFO(){
  if (voices_init==true){
  int16_t lfo_now;
  char my_wavesample;
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];
  int divider=7;
 // if (lfo_shape[my_wavesample]==1){divider=6;};
 lfo_now=(int)(lfo[j] * lfo_depth[j])>>divider;
 //Serial.println(lfo_now);
//  Serial.println(lfo_depth[j]);
if (wavesample_cutoff_mod[my_wavesample]==LFO){ partials[j]->setCutoffFreq((lfo_now>>9)+wavesample_filter_offset[my_wavesample]);}
if (wavesample_reso_mod[my_wavesample]==LFO){        partials[j]->setResonance((lfo_now>>8)+128);}
if (wavesample_o2index_mod[my_wavesample]==LFO){         if (osc_mode!=FM_MODE)  {partials[j]->setO2Index((int16_t)(lfo_now>>9)+64);}}
if (wavesample_o2detune_mod[my_wavesample]==LFO){                 if (osc_mode>0){ dualosc[j]->setLFM((lfo_now));}}
if (wavesample_oap_mod[my_wavesample]==LFO && partials[j]->isActive()){partials[j]->setLFM((lfo_now));}
if (wavesample_amp_mod[my_wavesample]==LFO){
  int64_t amp_calc= (logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]]);
   //amp_calc=constrain(((amp_calc<<2)+(((lfo_now)>>1)*amp_calc))>>12,0,32767);
   int gdepth=(int)(((lfo_depth[j]<<7)*(amp_calc))>>6)/100;
   amp_calc=((amp_calc*5)+((lfo[j]*(int16_t)gdepth)>>15))-(int16_t)gdepth;
    partials[j]->setVolume((int16_t)amp_calc);}
if (wavesample_pan_mod[my_wavesample]==LFO){  partials[j]->setPan(8192+(lfo_now>>2));}


//new pitch bend testing
 //rsh 13
  int32_t multiplier;
  int32_t mather;
   int32_t mather2;
  if (pitchBend>=0) {
    multiplier = 8192 + (pb_power[pb_range+12])*pitchBend;
      mather=((partials[j]->getInc()*multiplier)>>13)-partials[j]->getInc();
      mather2=((dualosc[j]->getInc()*multiplier)>>13)-dualosc[j]->getInc();
     } else {

int32_t ng=pb_power[11-pb_range]*pitchBend;
       multiplier = 8192 - ng;
        mather=(((int32_t)((int32_t)partials[j]->getInc() * multiplier)>>13)-partials[j]->getInc());
         mather2=(((int32_t)((int32_t)dualosc[j]->getInc() * multiplier)>>13)-dualosc[j]->getInc());
        //Serial.println(mather);
    }
   
     partials[j]->setPB(mather);
     dualosc[j]->setPB(mather2);
 






/* old pb
if (pitchBend>=0) {
  
                   partials[j]->setPB((abs(pitchBend)*( partials[j]->getPbFinal()-partials[j]->getPortaInc()))/127); 
if (osc_mode>0){dualosc[j]->setPB((abs(pitchBend)*( dualosc[j]->getPbFinal()-dualosc[j]->getPortaInc()))/127); }
              
              } else {
                    
                 partials[j]->setPB((abs(pitchBend)*( partials[j]->getPbFinalN()-partials[j]->getPortaInc()))/127);   
                 if (osc_mode>0){dualosc[j]->setPB((abs(pitchBend)*( dualosc[j]->getPbFinalN()-dualosc[j]->getPortaInc()))/127); }
                 } */
}}}


 



void checkCCA(int8_t cc,int8_t data){
  //int16_t lfo_now;
    if (voices_init==true){
  char my_wavesample;
  
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];
  
  if (cc==wavesample_cc_a[my_wavesample]){
     wavesample_cca_data[my_wavesample]=data;
 //data=(int)(lfo[j]*lfo_depth[j])>>7;
if (wavesample_cutoff_mod[my_wavesample]==CCA){ int8_t cfmw=(int8_t)constrain((((int16_t)data-64)+(int16_t)wavesample_filter_offset[my_wavesample]),-127,127);partials[j]->setCutoffFreq(cfmw);
wavesample_cca_data[my_wavesample]=data;}
if (wavesample_reso_mod[my_wavesample]==CCA){        partials[j]->setResonance(data>>1);wavesample_cca_data[my_wavesample]=data;}
if (wavesample_o2index_mod[my_wavesample]==CCA){         if (osc_mode!=FM_MODE) {partials[j]->setO2Index(data);}}
if (wavesample_o2detune_mod[my_wavesample]==CCA){                 if (osc_mode>0){ dualosc[j]->setLFM((int)(((int)data-64)<<7));wavesample_cca_data[my_wavesample]=data;}}
if (wavesample_oap_mod[my_wavesample]==CCA){                partials[j]->setLFM((int)(((int)data-64)<<7));wavesample_cca_data[my_wavesample]=data;} 
if (wavesample_amp_mod[my_wavesample]==CCA){  
  int64_t amp_calc= (logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]]);
   amp_calc=(amp_calc*data)>>5;
    partials[j]->setVolume((int16_t)amp_calc);
    wavesample_cca_data[my_wavesample]=data;}
if (wavesample_pan_mod[my_wavesample]==CCA){  partials[j]->setPan(data<<7);wavesample_cca_data[my_wavesample]=data;}

if (wavesample_so_mod[my_wavesample]==CCA){   wavesample_cca_data[my_wavesample]=data;}

if (wavesample_lfd_mod[my_wavesample]==CCA){ lfo_depth[j]=(data);wavesample_cca_data[my_wavesample]=data;}
if (wavesample_lfr_mod[my_wavesample]==CCA){ lfo_rate[j]=(data);wavesample_cca_data[my_wavesample]=data;}
  }
}}
}

void checkCCB(int8_t cc,int8_t data){
    if (voices_init==true){
  //int16_t lfo_now;
  char my_wavesample;
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];

  if (cc==wavesample_cc_b[my_wavesample]){
     wavesample_ccb_data[my_wavesample]=data;
 //data=(int)(lfo[j]*lfo_depth[j])>>7;
if (wavesample_cutoff_mod[my_wavesample]==CCB){ int8_t cfmw=(int8_t)constrain((((int16_t)data-64)+(int16_t)wavesample_filter_offset[my_wavesample]),-127,127);partials[j]->setCutoffFreq(cfmw);
wavesample_ccb_data[my_wavesample]=data;}
if (wavesample_reso_mod[my_wavesample]==CCB){        partials[j]->setResonance(data>>1);wavesample_ccb_data[my_wavesample]=data;}
if (wavesample_o2index_mod[my_wavesample]==CCB){         if (osc_mode!=FM_MODE) {partials[j]->setO2Index(data);}}
if (wavesample_o2detune_mod[my_wavesample]==CCB){                 if (osc_mode>0){  dualosc[j]->setLFM((int)(((int)data-64)<<7));wavesample_ccb_data[my_wavesample]=data;}}
if (wavesample_oap_mod[my_wavesample]==CCB){                partials[j]->setLFM((int)(((int)data-64)<<7));wavesample_ccb_data[my_wavesample]=data;}  
if (wavesample_amp_mod[my_wavesample]==CCB){   int64_t amp_calc= (logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]]);
   amp_calc=(amp_calc*data)>>5;
    partials[j]->setVolume((int16_t)amp_calc);
    wavesample_ccb_data[my_wavesample]=data;}
if (wavesample_pan_mod[my_wavesample]==CCB){ partials[j]->setPan(data<<7);wavesample_ccb_data[my_wavesample]=data;}

if (wavesample_so_mod[my_wavesample]==CCB){  wavesample_ccb_data[my_wavesample]=data;}
if (wavesample_lfd_mod[my_wavesample]==CCB){ lfo_depth[j]=(data);wavesample_ccb_data[my_wavesample]=data;}
if (wavesample_lfr_mod[my_wavesample]==CCB){ lfo_rate[j]=(data);wavesample_ccb_data[my_wavesample]=data;}
  }
}}
}

void checkMW(){
    if (voices_init==true){
 // int16_t velocity_now;
  char my_wavesample;
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];

if (wavesample_cutoff_mod[my_wavesample]==MODWHEEL){ int8_t cfmw=(int8_t)constrain((((int16_t)mw_now-64)+(int16_t)wavesample_filter_offset[my_wavesample]),-127,127);partials[j]->setCutoffFreq(cfmw);}
if (wavesample_reso_mod[my_wavesample]==MODWHEEL){        partials[j]->setResonance(mw_now<<1);}
//if (wavesample_o2index_mod[my_wavesample]==MODWHEEL){       if (osc_mode==FM_MODE) {partials[j]->setO2Index(mw_now<<8);} else {partials[j]->setO2Index(mw_now);}}
if (wavesample_o2detune_mod[my_wavesample]==MODWHEEL){                 if (osc_mode>0){  dualosc[j]->setLFM((int)(((int)mw_now-64)<<7));}}
if (wavesample_oap_mod[my_wavesample]==MODWHEEL){                partials[j]->setLFM((int)(((int)mw_now-64)<<7));}
if (wavesample_amp_mod[my_wavesample]==MODWHEEL){ int64_t amp_calc= (logtableb[p_velo[j]]*logtableb[wavesample_vol[my_wavesample]]);
   amp_calc=(amp_calc*mw_now)>>5;
   partials[j]->setVolume((int16_t)amp_calc);
  }
if (wavesample_pan_mod[my_wavesample]==MODWHEEL){  partials[j]->setPan(mw_now<<7);}
//if (wavesample_so_mod[my_wavesample]==MODWHEEL){   partials[j]->setOffset(mw_now<<6);}
if (wavesample_lfr_mod[my_wavesample]==MODWHEEL){ lfo_rate[j]=(mw_now);}
if (wavesample_lfd_mod[my_wavesample]==MODWHEEL){ lfo_depth[j]=(mw_now);}
}
    }
}

void checkKF(byte key, byte voice){
 // int16_t velocity_now;
   if (voices_init==true){
  char my_wavesample;

  my_wavesample=voice2ws[voice];

if (wavesample_cutoff_mod[my_wavesample]==KEYFOLLOW){int8_t cf=(int8_t)constrain(((((int16_t)key-64))+(int16_t)wavesample_filter_offset[my_wavesample]),-127,127);partials[voice]->setCutoffFreq(cf);}
if (wavesample_reso_mod[my_wavesample]==KEYFOLLOW){        partials[voice]->setResonance(key<<1);}
if (wavesample_o2index_mod[my_wavesample]==KEYFOLLOW){          if (osc_mode!=FM_MODE) {partials[voice]->setO2Index(key);}}
if (wavesample_o2detune_mod[my_wavesample]==KEYFOLLOW){                 if (osc_mode>0){ dualosc[voice]->setLFM((int)(((int)key-64)<<7));}}
if (wavesample_oap_mod[my_wavesample]==KEYFOLLOW){                partials[voice]->setLFM((int)(((int)key-64)<<7));}
if (wavesample_amp_mod[my_wavesample]==KEYFOLLOW){  int64_t amp_calc= (logtableb[p_velo[voice]]*logtableb[wavesample_vol[my_wavesample]]);
   amp_calc=(amp_calc*key)>>5;
    partials[voice]->setVolume((int16_t)amp_calc);
    }
if (wavesample_pan_mod[my_wavesample]==KEYFOLLOW){  partials[voice]->setPan(key<<7);}
//if (wavesample_so_mod[my_wavesample]==KEYFOLLOW){   partials[voice]->setOffset(key<<6);}
if (wavesample_lfr_mod[my_wavesample]==KEYFOLLOW){ lfo_rate[voice]=(key);}
if (wavesample_lfd_mod[my_wavesample]==KEYFOLLOW){ lfo_depth[voice]=(key);}

}}



void handleProgramChange(byte chan,byte prog){

    if (chan==midi_channel){    
        if (prog<=user_programs){
      program=prog;
    prev_osc_mode=osc_mode;
osc_mode=program_osc_mode[program];
poly_mode=program_polymode[program];
for (int i=0;i<max_voices;i++){
  partials[i]->setMode(oscmode_to_partialmode[osc_mode]);
}
   next_voice=0;
 for (char i;i<128;i++){handleNoteOff(midi_channel,i, 0);key2voice[i]=255;}
//stop i2s bus here please!
//for (char i;i<max_voices;i++){voice2key[i]=255;};
for (char i;i<max_voices;i++){voice2key[i]=255;partials[i]->keyOff();};
//for (char i;i<128;i++){key2voice[i]=255;}
Serial.println(program);
refreshDisplay();
}}

}
LogarithmicVolumeControl lvc(2);
void IRAM_ATTR onTimer() {

  portENTER_CRITICAL_ISR(&timerMux);



if (sampling==false){
  for (char i;i<max_voices;i++){
    saw[i]+=lfo_rate[i];
   if (saw[i]>16383){saw[i]=0;}

    switch (lfo_shape[i]){
case 0: 
lfo[i]=(saw[i]-8192)*2;
//lfo[i]=saw[i];
break;
case 2:
if (saw[i]<=8192) {  lfo[i]=-32768;} else {lfo[i]=32767;}
break;
case 1:
lfo[i]=(abs(saw[i]-8191)*8)-32768;
//if (saw[i]<=0) { 
//lfo[i]=abs(constrain(saw[i]-8192,-8192,8192))<<2;
break;
/*
case 2:
case 1:
if (saw[i]>=0) {
  if (lfo_shape[i]==2){
  lfo[i]=-32766;}
  if (lfo_shape[i]==1){
    if (lfo[i]>16384){lfo[i]=16384;}
    if ((lfo[i]+(lfo_rate[i]*2))<16384){lfo[i]+=lfo_rate[i]*2;}
  }
  } else {
     if (lfo_shape[i]==2){
    lfo[i]=32766;}
    if (lfo_shape[i]==1){
        if (lfo[i]<-16384){lfo[i]=-16384;}
     if (lfo[i]-(lfo_rate[i]*2)>-16384){lfo[i]-=lfo_rate[i]*2;}
  }}
break;
*/
case 3:
if (random_counter[i]==(255-lfo_rate[i])){
lfo[i]=random(65536)-32768;
random_counter[i]=0;
}
random_counter[i]++;
break;
    }
  }

oscy=oscy^1;
checkModEnv();
//checkVelo();
checkLFO();



/*
for (byte i=0;i<16;i++){

 if (wavesample_menv_dest[i] & FILTER_CUTOFF == FILTER_CUTOFF){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
fc_modstream[j]+=(mod_env_stream[j]);
fc_modstream_cables[j]++;
    }
    }
  }

   if (wavesample_menv_dest[i] & FILTER_RESO == FILTER_RESO){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
fr_modstream[j]+=(mod_env_stream[j]);
fr_modstream_cables[j]++;
    }
    }
  }

     if ((wavesample_menv_dest[i] & OSC_A_PITCH) == OSC_A_PITCH){
    
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
        
oap_modstream[j]+=(mod_env_stream[j]);
oap_modstream_cables[j]++;
    }
    }
  }

     if (wavesample_menv_dest[i] & AMP == AMP){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
amp_modstream[j]+=(mod_env_stream[j]);
amp_modstream_cables[j]++;
    }
    }
  }

     if (wavesample_menv_dest[i] & LFO_RATE == LFO_RATE){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
lfr_modstream[j]+=(mod_env_stream[j]);
lfr_modstream_cables[j]++;
    }
    }
  }


     if (wavesample_menv_dest[i] & LFO_DEPTH == LFO_DEPTH){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
lfd_modstream[j]+=(mod_env_stream[j]);
lfd_modstream_cables[j]++;
    }
    }
  }

    if (wavesample_menv_dest[i] & SAMP_OFFSET == SAMP_OFFSET){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
so_modstream[j]+=(mod_env_stream[j]);
so_modstream_cables[j]++;
    }
    }
  }

    if (wavesample_menv_dest[i] & SAMP_OFFSET == SAMP_OFFSET){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
so_modstream[j]+=(mod_env_stream[j]);
so_modstream_cables[j]++;
    }
    }
  }

    if (wavesample_menv_dest[i] & PAN == PAN){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
pan_modstream[j]+=(mod_env_stream[j]);
pan_modstream_cables[j]++;
    }
    }
  }

     if (wavesample_menv_dest[i] & AMP_ENV_SCALE == AMP_ENV_SCALE){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
aes_modstream[j]+=(mod_env_stream[j]);
aes_modstream_cables[j]++;
    }
    }
  } 

 
     if (wavesample_menv_dest[i] & FILT_ENV_SCALE == FILT_ENV_SCALE){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
fes_modstream[j]+=(mod_env_stream[j]);
fes_modstream_cables[j]++;
    }
    }
  }  

 
     if (wavesample_menv_dest[i] & OSC_B_DETUNE == OSC_B_DETUNE){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
obd_modstream[j]+=(mod_env_stream[j]);
obd_modstream_cables[j]++;
    }
    }
  }  

  if (wavesample_menv_dest[i] & OSC_B_INDEX == OSC_B_INDEX){
    for (byte j=0;j<max_voices;j++){
    if (voice2ws[j]==i){
oscb_modstream[j]+=(mod_env_stream[j]);
oscb_modstream_cables[j]++;
    }
    }
  }

  
}

for (byte i=0;i<max_voices;i++){
  partials[i]->setCutoffFreq((fc_modstream[i]>>(8*fc_modstream_cables[i]))+wavesample_fcoffset[voice2ws[i]]);
  partials[i]->setPB(((oap_modstream[i]<<2)>>(oap_modstream_cables[i])));
  //partials[i]->setPB(mod_env_stream[i]);



}

    /*
    if (osc_mode==0){
  for (char i;i<VOICE_QT;i++){


//mod wheel Pitch LFO / Vibrato
 partials[i]->setPB((pitchBend)*127+(int)((lfo*lfo_rate)>>8));}

  
  }
 if (osc_mode==FM_MODE && voices_init==true){
  for (char i;i<max_voices;i++){


if (wavesample_menv_dest[i] & OSC_B_IFint16_t
NDEX == OSC_B_INDEX){
//partials[i]->setO2Index((p_mod_env[i]->tick()*lfo_rate)>>6);}
 
  }}

   if (osc_mode==2 && voices_init==true){
     
  for (char i;i<5;i++){

partials[i]->setO2Index(lfo_rate);
//p_mixers[i]->setIndex(lfo_rate>>4);

  
  }}
 
 if (osc_mode==1 && voices_init==true){
    for (char i;i<5;i++){
//int env=((p_mod_env[i]->tick()*lfo_rate)>>13);
//int sound_ind=partials[i]->getSoundIndex();
partials[i]->setOffset((lfo>>8)+127);


//partials[i]->setSoundIndex(((sound_ind)+(env<<12))>>12);}}
    }}
    /* moved
for (byte j=0;j<10;j+=5){
partials[i+j]->setPB((pitchBend*127)+(int)((lfo*lfo_rate)>>8));}
 
    */}
  portEXIT_CRITICAL_ISR(&timerMux);
 


}

void IRAM_ATTR handleControlChange(byte channel, byte number,byte data ){
  

  if (channel==midi_channel){    
if (number==1){

  /*
  for(byte i;i<6;i++){
  lfo_rate[i]=data;
  lfo_depth[i]=data;}*/
//lfo_rate6b=data>>1;
mw_now=data;
checkMW();
}

/*
if (number==10){pan=(((float)data+1.0)/128.0);
  volume.setVolume(pan,0);
  Serial.println(pan);
    volume.setVolume(1.0-pan,1);
}*/
checkCCA(number,data);
checkCCB(number,data);
}
//sustain checker
if (number==64){
  if (data>63){
sustain=true;Serial.println("sustain on");} else {sustain=false;Serial.println("sustain off");
for (int i=0;i<max_voices;i++){
  if (keystate[voice2key[i]]==false){


    releaseVoice(i);

  }
}
}
}}
void IRAM_ATTR handlePitchBend(byte channel, int bend ){
   if (channel==midi_channel){    

     
   pitchBend=(int16_t)(bend>>6);



}
Serial.println(pitchBend);
}


 void playNote(byte voice,byte pitch, byte velocity){

partials[voice]->setLFM(0);
saw[voice]=0;
//tri[voice]=0;
lfo[voice]=0;

 if (osc_mode>0){  dualosc[voice]->setLFM(0);}
   
   
  char wavesample;
  byte true_ws;
  int16_t true_detune=-1;
  byte poffset;
  //bool is_dual_mode=false;
                     // if (osc_mode>=DUAL_MODE){is_dual_mode=true;}
                for (signed char i=WAVESAMPLE_AMOUNT;i>program_init_ws[program]-1;i--){
                  if (pitch<wavesample_topkey[i]){
                    wavesample=i;}}
                    //byte truews;
                    // Serial.println("ws");
                   // Serial.println(String((int)wavesample));
                    

                    

 poffset=voice ;
                      //if (j==1 && osc_mode>=DUAL_MODE){true_ws=wavesample_wsb[wavesample];
                      
                           //poffset=voice ;
                           
                          if (osc_mode>=CHORUS_MODE){  if (osc_mode>=DUAL_MODE){true_ws=wavesample_wsb[wavesample];} else {true_ws=wavesample;}
                   dualosc[poffset]->setEnd(wavesample_end[true_ws]);
                    dualosc[poffset]->setStart(wavesample_start[true_ws]);
                    dualosc[poffset]->setLoopStart(wavesample_loopst[true_ws]);
                    dualosc[poffset]->setLoopEnd(wavesample_loopen[true_ws]);
                    dualosc[poffset]->setLoopMode(wavesample_loopmode[true_ws]);} 
                 true_ws=wavesample;
                    partials[poffset]->setEnd(wavesample_end[true_ws]);
                    partials[poffset]->setStart(wavesample_start[true_ws]);
                    partials[poffset]->setLoopStart(wavesample_loopst[true_ws]);
                    partials[poffset]->setLoopEnd(wavesample_loopen[true_ws]);
                    partials[poffset]->setLoopMode(wavesample_loopmode[true_ws]);
                                          partials[poffset]->setSqrTune(wavesample_sqr_ctune[true_ws]);

                     // Serial.println("ws:");Serial.println(true_ws);
                      //if (j==1 && osc_mode==CHORUS_MODE){true_detune=wavesample_detune[true_ws]+program_oscbdetune[program];} else {true_detune=wavesample_detune[true_ws];};
                 
                    
                    
                
                
                
                 //p_amp_env[voice]->keyOn(4194304);
                if (wavesample_pan_mod[wavesample]==0) {partials[voice]->setPan(wavesample_pan[wavesample]<<7);}
                                if (wavesample_o2index_mod[wavesample]==0) {partials[voice]->setO2Index(wavesample_init_o2index[wavesample]);}

                voice2ws[voice]=wavesample;
                 voice2pitch[voice]=pitch;
                checkMW();
               checkKF(pitch,voice);
                
               

int8_t  dual_ws=-1;
int16_t  dual_detune=-1;

                 if (osc_mode>=DUAL_MODE){dual_ws=wavesample_wsb[wavesample];dual_detune=master_tune+program_oscbdetune[program]+wavesample_detune[dual_ws];} else {dual_ws=wavesample;dual_detune=master_tune+program_oscbdetune[program]+wavesample_detune[true_ws];}  ;
                 true_ws=wavesample;true_detune=master_tune+wavesample_detune[true_ws];
              
                 
                  //partials[poffset]->begin2();
                  if (poly_mode!=PER_WAVESAMPLE){
                  if (wavesample_portatime[wavesample]>0 && pitch!=last_key){
                    
                    partials[voice]->setIncrement((int)(midinotes.midiNoteToFrequency(last_key-(57-wavesample_ctune[true_ws]))*(true_detune))); 
                   
                      dualosc[voice]->setIncrement((int)(midinotes.midiNoteToFrequency(last_key-(57-wavesample_ctune[dual_ws]))*(dual_detune))); 
                int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  partials[voice]->setPBfinal(pb_final_p); 
                   pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                  dualosc[voice]->setPBfinal(pb_final_p); 
                    
                 // Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                   // Serial.println(pb_final_p);
                partials[poffset]->setPBfinalN(pb_final_p); 
                 pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                   // Serial.println(pb_final_p);
                dualosc[poffset]->setPBfinalN(pb_final_p); 
                   // partials[poffset]->setIncrement((int)(midinotes.midiNoteToFrequency(last_key-(57+wavesample_ctune[true_ws]))*(true_detune))); 
                   //set porta to porta + newly calculated picth bend value
                      partials[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[true_ws]))*(true_detune)));

                      
                       partials[poffset]->setPortaTime(wavesample_portatime[wavesample]);

                        dualosc[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune)));

                      
                       dualosc[poffset]->setPortaTime(wavesample_portatime[true_ws]);
                      } else{
                        partials[poffset]->setIncrement((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[true_ws]))*(true_detune)));
                  dualosc[poffset]->setIncrement((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune)));
                int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  partials[poffset]->setPBfinal(pb_final_p); 

                  //int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                  dualosc[poffset]->setPBfinal(pb_final_p); 
                //  Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  //  Serial.println(pb_final_p);
                partials[poffset]->setPBfinalN(pb_final_p); 

 pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                  //  Serial.println(pb_final_p);
                dualosc[poffset]->setPBfinalN(pb_final_p); 
                
                 }
             
               } else {
                partials[poffset]->setIncrement((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[true_ws]))*(true_detune))); 
               
                  
                int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  partials[poffset]->setPBfinal(pb_final_p); 
                //  Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                 //   Serial.println(pb_final_p);
                partials[poffset]->setPBfinalN(pb_final_p); 
//s
                 dualosc[poffset]->setIncrement((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune))); 
               
                  
                //int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                  dualosc[poffset]->setPBfinal(pb_final_p); 
                //  Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[dual_ws]))*(dual_detune));
                 //   Serial.println(pb_final_p);
                dualosc[poffset]->setPBfinalN(pb_final_p); 
                
                 }

               

               partials[voice]->begin(sampy);
               if (osc_mode!=NORMAL){dualosc[voice]->trueBegin();}
               
                 if (wavesample_cutoff_mod[wavesample]==0) {partials[voice]->setCutoffFreq(wavesample_filter_offset[wavesample]);}
                     
              if (wavesample_so_mod[wavesample]==0){ 
float test1=(wavesample_init_offset[wavesample]/127.00);
 int looplen=wavesample_loopen[wavesample]-wavesample_loopst[wavesample];
float test2=(test1*((wavesample_end[wavesample]-wavesample_start[wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;
               partials[voice]->setOffsetX((int)(test2));
                 dualosc[voice]->setOffsetX((int)(test2));
               }
               partials[voice]->setAttackAmpRate(wavesample_amp_atk[wavesample]);
          
               partials[voice]->setDecayAmpRate(wavesample_amp_dec[wavesample]);
                partials[voice]->setReleaseAmpRate(wavesample_amp_rel[wavesample]);
                int testoo=(int)wavesample_amp_sus[wavesample]<<14;
                partials[voice]->setSustainAmpLevel(testoo);
                 partials[voice]->setFilterType(wavesample_filter_type[wavesample]);
                    partials[voice]->setAttackFilterRate(wavesample_filter_atk[wavesample]);
          
               partials[voice]->setDecayFilterRate(wavesample_filter_dec[wavesample]);
               if (wavesample_reso_mod[wavesample]==0){ partials[voice]->setResonance(wavesample_filter_reso[wavesample]);}
               partials[voice]->setReleaseFilterRate(wavesample_filter_rel[wavesample]);
               partials[voice]->setSustainFilterLevel(wavesample_filter_sus[wavesample]<<6);
                 
                if (wavesample_amp_mod[wavesample]==0){ partials[voice]->setVolume((logtableb[velocity]*logtableb[wavesample_vol[wavesample]])<<2);}
                partials[voice]->keyOn(16384);
               p_velo[voice]=velocity;
               lfo_shape[voice]=wavesample_lfoshape[wavesample];
                if (wavesample_lfr_mod[wavesample]==0){lfo_rate[voice]=wavesample_lforate[wavesample];}
                if (wavesample_lfd_mod[wavesample]==0){lfo_depth[voice]=wavesample_lfodepth[wavesample];}
               checkVelo();
               //p_amp_env[voice]->keyOn(16384);
               
                 // partials[voice]->setCutoffFreq(64);
        
                 //wavetable testing
                  //int env=((p_mod_env[voice]->tick()*lfo_rate)>>13);
              //partials[voice]->setOffset((lfo>>8)+127);
              
  
                  //wqavetable testing
                // if (osc_mode==FM_MODE){partials[voice]->setO2Index((p_mod_env[voice]->tick()*lfo_rate)>>6);};
                                            // correponding to the program number
                                            // (0 to 127, it can last a while..)
                                            if (wavesample_mod_atk[wavesample]==255){ p_mod_env[voice+1]->setAttackRate(16384);} else {
              p_mod_env[voice+1]->setAttackRate(wavesample_mod_atk[wavesample]);}
          
               p_mod_env[voice+1]->setDecayRate(wavesample_mod_dec[wavesample]);
                p_mod_env[voice+1]->setReleaseRate(wavesample_mod_rel[wavesample]);
                p_mod_env[voice+1]->setSustainLevel(wavesample_mod_sus[wavesample]<<6);
                 if (wavesample_cca_data[wavesample]>-1 && wavesample_cc_a[wavesample]>-1){checkCCA(wavesample_cc_a[wavesample],wavesample_cca_data[wavesample]);};
                if (wavesample_ccb_data[wavesample]>-1 && wavesample_cc_b[wavesample]>-1){checkCCB(wavesample_cc_b[wavesample],wavesample_ccb_data[wavesample]);};
            p_mod_env[voice+1]->keyOn(16384);    
            p_mod_env[0]->keyOn(16384);    
            checkModEnv();            
               checkMW();
               checkKF(pitch,voice);
                              
              last_key=pitch;
             
}


void handleNoteOn(byte channel, byte pitch, byte velocity)
{

  if (channel==midi_channel){      
  keystate[pitch]=true;
  handleNoteOnX(channel,pitch,velocity);
  

    if (displaycontext==KEYSET_TOPKEY){
       
     
      wavesample_topkey[edit_wavesample]=pitch+1;}

     /*  if (displaycontext==KEYSET_KEYT){
       
     
      wavesample_ctune[edit_wavesample]=(pitch-34)*-1;}*/
  }
  //Serial.println(String(pitch));

}
uint8_t polynum=0;
void handleNoteOnX(byte channel, byte pitch, byte velocity){
if (keystate[pitch]==true){
  if (poly_mode==DEFAULT){
  uint8_t vtest=0;
for (int i=0;i<VOICE_QT;i++){

  if (partials[i]->isPressed()){
    if (polynum==i){polynum++;
    if(polynum==VOICE_QT){polynum=0;};
    }
    vtest++;
    }
  }
  if (vtest==8){
    polynum=random(8);
    }
    key2voice[voice2key[polynum]]=255;
    voice2key[polynum]=pitch;
    key2voice[pitch]=polynum;
    
    playNote(polynum,pitch,velocity);
    Serial.println(polynum);
    polynum++;
    if(polynum==VOICE_QT){polynum=0;};
 
}




if (poly_mode==PER_WAVESAMPLE){
  char wavesample;
  bool is_dual_mode=false;
   int16_t true_detune=-1;
   char true_ws;
   byte poffset;

                    //char current_voice=wavesample_vassign[wavesample];
                       for (signed char i=WAVESAMPLE_AMOUNT;i>program_init_ws[program]-1;i--){
                  if (pitch<wavesample_topkey[i]){
                    wavesample=i;}}
    char current_voice=wavesample_vassign[wavesample];
    if ( partials[current_voice]->isActive()==true && keystate[voice2key[current_voice]]==true){
      if (wavesample_portatime[wavesample]>0 && (pitch!=voice2key[current_voice]))
      {
         //if (osc_mode>=DUAL_MODE){is_dual_mode=true;}
         

           // if (osc_mode==NORMAL && j==1){break;}
    

           //if (j==1 && osc_mode>=DUAL_MODE){true_ws=wavesample_wsb[wavesample];} else {true_ws=wavesample;};
           
             //if (j==1){true_detune=master_tune+program_oscbdetune[program]+wavesample_detune[true_ws];}
          true_ws=wavesample;true_detune=master_tune+wavesample_detune[true_ws];
         poffset=current_voice;
          int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  partials[poffset]->setPBfinal(pb_final_p); 
                  Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                    Serial.println(pb_final_p);
                partials[poffset]->setPBfinalN(pb_final_p); 
        partials[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[true_ws]))*(true_detune)));
                      //partials[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57+wavesample_ctune[true_ws]))*(true_detune)));
                       partials[poffset]->setPortaTime(wavesample_portatime[wavesample]);

                        if (osc_mode>=CHORUS_MODE){
                          
                          if (osc_mode>=DUAL_MODE){ true_ws=wavesample_wsb[wavesample];}
           
             true_detune=master_tune+program_oscbdetune[program]+wavesample_detune[true_ws];
          //if (j==0) {true_ws=wavesample;true_detune=master_tune+wavesample_detune[true_ws];};
         //poffset=current_voice;
          int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                  dualosc[poffset]->setPBfinal(pb_final_p); 
                  Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+pitch-(57-wavesample_ctune[true_ws]))*(true_detune));
                    Serial.println(pb_final_p);
               dualosc[poffset]->setPBfinalN(pb_final_p); 
        dualosc[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57-wavesample_ctune[true_ws]))*(true_detune)));
                      //partials[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(pitch-(57+wavesample_ctune[true_ws]))*(true_detune)));
                       dualosc[poffset]->setPortaTime(wavesample_portatime[wavesample]);
      }
       
        }
      else {
      //p_amp_env[current_voice]->setReleaseQN(STEAL_FADE_TIME,pitch,velocity,current_voice);
      playNote(current_voice,pitch,velocity);
      }
      } else {playNote(current_voice,pitch,velocity); }
     // key2voice[voice2key[current_voice]]=255;
      //keystate[voice2key[current_voice]]=false;
      key2voice[pitch]=current_voice;
      voice2key[current_voice]=pitch;
       voice_order[pitch]=voice_counter++;
      
       //keystate[pitch]=true;
       
       
       last_velo=velocity;
}

if (poly_mode==POLY2){
//simplified polyphony without any stealing
for (int8_t i=0;i<max_voices;i++){
if ((partials[i]->isActive()==false) or (partials[i]->isRunning()==false)){
    key2voice[voice2key[i]]=255;
    voice2key[i]=pitch;
    key2voice[pitch]=i;
    playNote(i,pitch,velocity);
    Serial.println(i);
    i=max_voices;
  break;
}

}

}


} else if (poly_mode==PER_WAVESAMPLE){
  bool notesHeld=false;
  char wavesample;
 for (signed char i=WAVESAMPLE_AMOUNT;i>program_init_ws[program]-1;i--){
                  if (pitch<wavesample_topkey[i]){
                    wavesample=i;}}
                    char current_voice=wavesample_vassign[wavesample];
                    
         for (char i;i<127;i++){
       if (keystate[i]==true && key2voice[i]==current_voice){
         notesHeld=true;
       }

     }
      if (notesHeld==false){
playNote(current_voice,pitch,velocity);

releaseVoice(key2voice[pitch]);

} else {
   
          int16_t lt,latest=-1;
          for (byte i=127;i>0;i--){
            if (keystate[i]==true && key2voice[i]==current_voice){
              if (voice_order[i]>=lt){
                latest=i;
                lt=voice_order[i];
              }
            }

          }

          if (latest>-1 && keystate[latest]==true){
playNote(current_voice,latest,velocity);

          }
}


} else {Serial.println("too shirt");}




}
void releaseVoice(byte voice){
  bool dont=false;
  if (voice==255){dont=true;}
  if (osc_mode==NORMAL && voice>max_voices){dont=true;}
  if (dont==false){
partials[voice]->keyOff();
       //partials[voice]->keyOff();
          p_mod_env[voice+1]->keyOff();
          //Serial.println("voiceoff");
          //Serial.println(voice);
          }

}
void handleNoteOff(byte channel, byte pitch, byte velocity)
{  
  if (channel==midi_channel){    
 keystate[pitch]=false;
if ((poly_mode==POLY2) || (poly_mode==DEFAULT)){
  for (int8_t i=0;i<max_voices;i++){
    if (voice2key[i]==pitch){
     if (sustain==false){  releaseVoice(i);}
    }
  }
  return;
} 
  char wavesample;
  int16_t true_detune=-1;
  byte true_ws;
  byte poffset;
  bool is_dual_mode=false;
   for (signed char i=WAVESAMPLE_AMOUNT;i>program_init_ws[program]-1;i--){
                  if (pitch<wavesample_topkey[i]){
                    wavesample=i;}}
  //poly--;
  char my_voice=key2voice[pitch];
  if (my_voice==255){return;}
  bool notesHeld=false;
  //Serial.println(pitch);
   //keystate[pitch]=false;
  
    
      if (poly_mode==PER_WAVESAMPLE){


         for (char i;i<127;i++){
       if (keystate[i]==true && key2voice[i]==my_voice){
         notesHeld=true;
       }

     }
      if (notesHeld==false){
   if (sustain==false) {releaseVoice(my_voice);}
    
    } else {
         // keystate[pitch]=false;
         
          int16_t lt,latest=-1;
          for (byte i=127;i>0;i--){
            if (keystate[i]==true && key2voice[i]==my_voice){
              if (voice_order[i]>=lt){
                latest=i;
                lt=voice_order[i];
              }
            }

          }
Serial.println(latest);
//Serial.println(keystate[latest]);
//Serial.println(voice2key[i]);
          if (latest>-1 && keystate[latest]==true){
            bool do_switch=true;
              for (char i;i<max_voices;i++){
    if ( voice2key[i]==latest){do_switch=false;}
   }          
              if (do_switch==true){
                 for (signed char i=WAVESAMPLE_AMOUNT;i>program_init_ws[program]-1;i--){
                  if (voice2pitch[my_voice]<wavesample_topkey[i]){
                   wavesample=i;
                    
                    }
                    }
                 if (wavesample_portatime[wavesample]>0)
      {
        Serial.println("success");
        voice2key[my_voice]=latest;
         if (osc_mode>=DUAL_MODE){is_dual_mode=true;}
         
            //if (osc_mode==NORMAL && j==1){break;}
            

         
         true_ws=wavesample;true_detune=master_tune+wavesample_detune[true_ws];
         poffset=wavesample_vassign[wavesample];
          int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+latest-(57-wavesample_ctune[true_ws]))*(true_detune));
                  partials[poffset]->setPBfinal(pb_final_p); 
                 // Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+latest-(57-wavesample_ctune[true_ws]))*(true_detune));
                  //  Serial.println(pb_final_p);
                partials[poffset]->setPBfinalN(pb_final_p); 
          
        partials[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(latest-(57-wavesample_ctune[true_ws]))*(true_detune)));
                                 partials[poffset]->setPortaTime(wavesample_portatime[wavesample]);

                                 //f

                                 if (osc_mode>=CHORUS_MODE){
                                  
                                  if (osc_mode>=DUAL_MODE){true_ws=wavesample_wsb[wavesample];}
           
             true_detune=master_tune+program_oscbdetune[program]+wavesample_detune[true_ws];
          //if (j==0) {true_ws=wavesample;true_detune=master_tune+wavesample_detune[true_ws];};
         //poffset=wavesample_vassign[wavesample] + (j*5);
          int pb_final_p;
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(2))+latest-(57-wavesample_ctune[true_ws]))*(true_detune));
                  dualosc[poffset]->setPBfinal(pb_final_p); 
                 // Serial.println(pb_final_p);
                pb_final_p=(int)(midinotes.midiNoteToFrequency(((pb_range+1)*(-2))+latest-(57-wavesample_ctune[true_ws]))*(true_detune));
                  //  Serial.println(pb_final_p);
                dualosc[poffset]->setPBfinalN(pb_final_p); 
          
       dualosc[poffset]->setPorta((int)(midinotes.midiNoteToFrequency(latest-(57-wavesample_ctune[true_ws]))*(true_detune)));
                                 dualosc[poffset]->setPortaTime(wavesample_portatime[wavesample]);}
             
        } else{
               //partials[key2voice[pitch]]->setReleaseQN(STEAL_FADE_TIME,latest,last_velo,key2voice[pitch]);
               playNote(poffset,latest,last_velo);
               }}}
          }}  else if (my_voice<max_voices) {

   if (sustain==false){releaseVoice(my_voice);}
    
    } 
          
       //voice2key[key2voice[pitch]]=255;
     //key2voice[pitch]=255;
      }
}
void switchDualMode(){
   //max_voices=VOICE_QT-1;
   
  for (byte i=0;i<max_voices;i++){
   // partials[i]->end();
    //partials[i]->setReleaseAmpRate(65536);
    
      partials[i]->setO2(dualosc[i]);
   // voice_mixer.add(*pstreams[i],400);
voice2key[i]=255;
    if (osc_mode==FM_MODE){
  partials[i]->setMode(2);;}else {
partials[i]->setMode(1);

  }}
  for (char i;i<128;i++){key2voice[i]=255;}

Serial.println("Dual Mode Initialized");

  }

  void switchNormalMode(){
   max_voices=VOICE_QT;
   voice_mixer.end();
  for (byte i;i<max_voices;i++){
    
    voice_mixer.add(*pstreams[i],400);
partials[i]->setMode(0);

  }
 

  }
void initVoices(){
AudioInfo dynachange(sample_rate,2,16);
out.setAudioInfo(dynachange);

  for (byte i;i<VOICE_QT;i++){
    if (i<VOICE_QT){
partials[i]=new WomprisSynthVoice <int16_t> (samp_memory,1,true,0);
dualosc[i]=new GeneratorFromArray <int16_t> (samp_memory,1,true,0);}



/*if (osc_mode>0){
partials[i+max_voices]=new GeneratorFromArray <int8_t> (samp_memory,1,true,0);
partials[i+max_voices]->begin();
    partials[i+max_voices]->setAudioInfo(sampy);
partials[i+max_voices]->setArray(samp_memory,88064);

  partials[i]->setO2(dualosc[i]);
  if (osc_mode==FM_MODE){
  partials[i]->setMode(2);;}else {
partials[i]->setMode(1);

  }
  pstreams[i]=new GeneratedSoundStream <int8_t> (*partials[i]);
sample_rate=20000;
}
else {*/
  pstreams[i]=new GeneratedSoundStream <int16_t> (*partials[i]);
//}

//pconv[i]=new NumberFormatConverterStream (*pstreams[i]);
//p_fx_stream[i]=new AudioEffectStream (*pstreams[i]);
//p_amp_env[i]=new ADSRGainXP (100,100,12888,5,1);
//p_amp_env[i]->setOsc(partials[i]);
//partials[i]=new ADSRFilter (16384,16384,12888,50);
//p_fx_stream[i]->addEffect(*partials[i]);
//p_fx_stream[i]->addEffect(*p_amp_env[i]);

  partials[i]->begin();
  partials[i]->setO2(dualosc[i]);
  dualosc[i]->setVoice(1);
   // partials[i]->setAudioInfo(sampy);
partials[i]->setArray(samp_memory,88064);

 //auto fxcfg = out.defaultConfig();
 //fxcfg.channels=1;
 //fxcfg.sample_rate=sample_rate;
//p_fx_stream[i]->begin(fxcfg);
//p_pan[i]=new ChannelFormatConverterStreamT <int16_t> (*pstreams[i]);
//p_pan[i]->setIntMode();
auto config = out.defaultConfig(TX_MODE);
  config.buffer_count=4;
  config.buffer_size=128;
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.port_no=1;
  config.bits_per_sample =16;
 //p_pan[i]->begin(1,2);
voice_mixer.add(*pstreams[i],16);

//voice_mixer.add(*pconv[i],400);


 

}

copier.begin(out,voice_mixer);
voices_init=true;

};

void newVoices(byte voicect){
  voices_init=false;
  copier.end();
   
voice_mixer.end();
  
      
 for (char i;i<128;i++){key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};


for (int i = 0; i <10; i++) {
    delete partials[i];
  }

  




 for (int i = 0; i < max_voices; i++) {
    //delete p_amp_env[i];
  }

 for (int i = 0; i < max_voices; i++) {
   // delete partials[i];
  }

   for (int i = 0; i < max_voices; i++) {
    //delete p_fx_stream[i];
  }
  
for (int i = 0; i <max_voices; i++) {
   // delete p_pan[i];
  }
for (int i = 0; i <max_voices; i++) {
    delete p_mod_env[i];
  }




}


void endVoices(){

 for (char i;i<128;i++){key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};


for (int i = 0; i <PARTIAL_QT; i++) {
    partials[i]->end();
  }

}

// Arduino Setup
void setup(void) {  
  //refreshDisplay();
//pinMode(19, OUTPUT);
for (int8_t i=0;i<VOICE_QT+1;i++){
p_mod_env[i]=new ADSRxp (16384,200,0,200);
}
  	//rotaryEncoder.setEncoderValue(500);
  
   //DIN_MIDI.setHandlePitchBend(handlePitchBend);
//Serial.begin(115200);
xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      5000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task1,  /* Task handle. */
      0); /* Core where the task should run */

 for (char i;i<127;i++){key2voice[i]=255;}
 



  // Open Serial 
  Serial.begin(115200);
  
 // while(!Serial);
 // AudioLogger::instance().begin(Serial, AudioLogger::Info);
 //out.I2SConfig
 //filtered.setFilter(0, new FIR<float>(coef));
 pinMode(13, INPUT_PULLUP);
   //pinMode(17, INPUT_PULLUP);
 
  auto config_in = adc.defaultConfig(RX_MODE);
config_in.buffer_count=8;
  config_in.buffer_size=512;

   config_in.sample_rate =15000; 
  config_in.bits_per_sample = (i2s_bits_per_sample_t)24; 
 //config_in.bits_per_sample =16;
  config_in.is_master =false;
  config_in.port_no = 1;
    config_in.channels = channels;
    config_in.i2s_format = I2S_MSB_FORMAT;
  config_in.pin_bck = 15;
  config_in.pin_ws = 2;
  config_in.pin_data = 34;
      config_in.use_apll = true; 
 config_in.auto_clear=true;
adc.begin(config_in);
  converter.begin(from, to);
  
  //csvStream.begin(from);
 auto config = out.defaultConfig(TX_MODE);
  config.buffer_count=4;
  
  config.buffer_size=64;
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.port_no=0;
   config.pin_bck = 33;
  config.pin_ws = 26;
  config.pin_data = 32;
 config.fixed_mclk = sample_rate * 128;
    config.pin_mck = 0; //CHANGE TO 0 POSSIBLY
      config.use_apll = true; 
  config.bits_per_sample = (i2s_bits_per_sample_t)16;


  out.begin(config);
  //partials[1].setAudioInfo(sampy);
  // Setup sine wave

  //Serial.println("started...");

for (byte i; i<WAVESAMPLE_AMOUNT;i++){
	wavesample_cc_a[i]=-1;
	wavesample_cc_b[i]=-1;
	wavesample_cca_data[i]=-1;
	wavesample_ccb_data[i]=-1;
  	wavesample_vol[i]=127;
    	wavesample_pan[i]=64;
  	wavesample_amp_atk[i]=255;
  	wavesample_amp_dec[i]=255;
  	wavesample_amp_sus[i]=255;
  	wavesample_amp_rel[i]=254;
      	wavesample_mod_atk[i]=255;
  	wavesample_mod_dec[i]=10;
  	wavesample_mod_sus[i]=0;
  	wavesample_mod_rel[i]=4;
    	wavesample_filter_atk[i]=255;
      wavesample_menv_depth[i]=64;
            //wavesample_menv_depth[i]=91;
     wavesample_filter_offset[i]=127;
  	wavesample_filter_dec[i]=255;
  	wavesample_filter_sus[i]=255;
  	wavesample_filter_rel[i]=1;
   wavesample_init_o2index[i]=64;
wavesample_lforate[i]=32;
wavesample_lfoshape[i]=1;//TRIANGLE
wavesample_oap_mod[i]=0;
//wavesample_lfd_mod[i]=MODWHEEL;
wavesample_lfd_mod[i]=MODWHEEL;
wavesample_oap_mod[i]=LFO;
wavesample_o2detune_mod[i]=LFO;
wavesample_loopmode[i]=0;//ONE-SHOT
wavesample_topkey[i]=127;
wavesample_end[i]=512;
wavesample_loopen[i]=512;
}
 for (byte i; i<PROGRAM_AMOUNT;i++){
program_polymode[i]=DEFAULT;

  
 }
   
initVoices();



Serial.println(copier.bufferSize());
// we need to provide the bits_per_sample and channels
 //volume.setVolumeControl(lvc);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  

  //refreshDisplay();



/*
 display.clearDisplay();
 display.print("Loading..");
  display.display();
loadBank(SD, "/default.wsb");*/


displaycontext=EDIT;
refreshDisplay();

}


void Task1code( void * parameter) {

   timer = timerBegin(0, 40, true);
 
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 975, true);
  timerAlarmEnable(timer);
rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
	//set boundaries and if values should cycle or not
	//in this example we will set possible values between 0 and 1000;
	bool circleValues = true;
	rotaryEncoder.setBoundaries(0,3, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)

	/*Rotary acceleration introduced 25.2.2021.
   * in case range to select is huge, for example - select a value between 0 and 1000 and we want 785
   * without accelerateion you need long time to get to that number
   * Using acceleration, faster you turn, faster will the value raise.
   * For fine tuning slow down.
   */
	//rotaryEncoder.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
	rotaryEncoder.setAcceleration(0);
   DIN_MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function

    // Do the same for NoteOffs
    DIN_MIDI.setHandleNoteOff(handleNoteOff);
DIN_MIDI.setHandlePitchBend(handlePitchBend);
DIN_MIDI.setHandleControlChange(handleControlChange);
DIN_MIDI.setHandleProgramChange(handleProgramChange);

    // Initiate MIDI communications, listen to all channels
    DIN_MIDI.begin(MIDI_CHANNEL_OMNI);
 init_sd();
  for(;;) {



    DIN_MIDI.read();
 rotary_loop();
 
  }
    

    

}



void init_sd(){
  sd_spi.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS, sd_spi)) {
        Serial.println("Card Mount Failed");
        
        //return;
    }

    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        //return;
        sd_init=false;
    } else {

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (512 * 512);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
 sd_init=true;}
}

// Arduino loop - copy sound to out 
void loop() {
  //Serial.println(is_replace);
  if (sampling==true){

  if (samp_num<sample_length ){
  // sample_copy.copy();
   sample_copy.copy();
   samp_num++;
   Serial.println(samp_num);
   } 

   if (samp_num==sample_length){
      displaycontext=SAMPLE_EDITOR;
      sampling=false;
      uint8_t user_wavesamples_cache=user_wavesamples;
      if (is_replace==true){user_wavesamples=edit_wavesample;}
       wavesample_start[user_wavesamples]=(sample_pos*512);
      wavesample_loopst[user_wavesamples]=wavesample_start[user_wavesamples];
      wavesample_end[user_wavesamples]=wavesample_start[user_wavesamples]+(sample_length*512)-1;
       wavesample_loopen[user_wavesamples]=wavesample_end[user_wavesamples];
       edit_wavesample=user_wavesamples;
       if (is_replace==false){ user_wavesamples++;} else{
user_wavesamples=user_wavesamples_cache;
       }
       if (nprog==true){
         program=user_programs;
         program++;
         program_init_ws[program]=edit_wavesample;
          user_programs++;
          prev_osc_mode=osc_mode;
          osc_mode=0;
  poly_mode=0;
       }
       menuoffset=ZOOM-4;
       hovermode=true;
       hovercontext=ZOOM;
       rotaryEncoder.setEncoderValue(4);
       rotaryEncoder.setBoundaries(0,7, true); 
      refreshDisplay();
     
      sample_pos+=sample_length;
     
      
      copier.begin(out,voice_mixer);
   }
   
   /*else {
     if (ld_default==false){
loadBank(SD, "/default.wsb");
refreshDisplay();
  copier.begin(out,voice_mixer);
ld_default=true;

     }
     sampling=false;
   
   }*/
  
  } else {
  
  if (sampling_level==false){
    copier.copy();}
    
if (sampling_level==true){
  //copier1.copy();
level_copy.copy();

samp_level_average=samp_level_average_cache;
samp_level_average_cache=0;
for(int i=0;i<128;i++){

samp_level_average_cache+=abs(samp_level_buffer[i]);

}
samp_level_average_cache=samp_level_average_cache/128;
samp_level_average=((samp_level_average_cache + samp_level_average)/2);
 samp_level.reset();
  if (sample_ready==false){

  refreshDisplay();
  }

if ((sample_ready==true) && (samp_level_average>=sample_threshold)){
   // copier.begin(out,converter);
    sampling=true;
  refreshDisplay();
      samp_mem_out.setPosX(((int)(sample_pos)*512));
      samp_mem_out.begin();
     
      //samp_level.begin();
      //bitcrush.begin();
      //final_samp.begin(2,1);
      sample_copy.begin();
     // level_copy.end();
     samp_num=0;
      sampling_level=false;
     sample_ready=false;
    // sampling=true;
      auto config_in = adc.defaultConfig(RX_MODE);
config_in.buffer_count=4;
  config_in.buffer_size=512;

     

}


 

}
   


//recording a sample



  /*
  Serial.println(String(esp_get_free_heap_size()));
  out.end();
 
 int sample_timer=0;

 while (sample_timer<512){
   
   if (adc.available()==512){

 size_t len = adc.readBytes(samp_buff, 512); 
  // move center to 0 and scale the values
  scaler.convert(samp_buff, len);

  int16_t *sample = (int16_t*) samp_buff; 
 
  for (int j=0; j<len/2; j++){
    int xj=j+(sample_timer*256);
    samp_memory[xj]=(*sample++);
  ;
  }
sample_timer++;
}
}
/*
 auto config = out.defaultConfig(TX_MODE);
  config.buffer_count=4;
  config.buffer_size=64;
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.port_no=1;
  config.bits_per_sample = 16;
out.begin(config);

 for (int i;i<65536;i++){
  Serial.println( samp_memory[i] ); };

   size_t len = adc.readBytes(samp_buff, 256); 
  // move center to 0 and scale the values
  scaler.convert(samp_buff, len);

  int16_t *sample = (int16_t*) samp_buff; 
  int size = len;// / 4;
  for (int j=0; j<size; j++){
    samp_memory[j+(m*4096)]=(*sample++);
  }
Wire.beginTransmission(0x55);
  Wire.write(200);
Wire.endTransmission(); 
  m++; 
//Write message to the slave

  // Clear the buffer.
 
 
  
 
 

 */

 
if (laststate==HIGH && digitalRead(13)==LOW){

onButtonClick();

}
laststate=digitalRead(13);

//splash animation frames
if ((user_wavesamples==0) && (displaycontext==EDIT)){
  gp_reg8b+=4;
if (gp_reg8b==0){gp_reg8a+=64;refreshDisplay();}//to slow it down further. 7 is the speed
}

//rotary_loop();
for (byte i=0;i<max_voices;i++){
    
    if (queueVelo[i]>0){
     // Serial.println(queuePitch[i]);
  
      if ((partials[i]->isActive()==false) || (queuePitch[i]!=voice2key[i])){
        //Serial.println("bsteal");
     handleNoteOnX(midi_channel,queuePitch[i],queueVelo[i]);
     queue_test=true;
      queuePitch[i]=0;
      queueVelo[i]=0;}
    }
    
    }



//digitalWrite(19,oscy);
  /*  bool notesHeld;
for (char i;i<127;i++){
       if (keystate[i]==true){
         notesHeld=true;
       }}

if (notesHeld==false){
for (char i;i<max_voices;i++){
if (p_amp_env[i]->isActive)
}*/
/*
if (prev_osc_mode!=osc_mode){


if (prev_osc_mode==NORMAL && osc_mode!=NORMAL){
//sample_rate=20000;
switchDualMode();
  //prev_osc_mode=osc_mode;
 prev_osc_mode= osc_mode;
 dual_init=true;
} else if (prev_osc_mode!=NORMAL && osc_mode==NORMAL) {
//sample_rate=30000;
switchNormalMode();
  //prev_osc_mode=NORMAL;
prev_osc_mode= osc_mode;
dual_init=false;
  } 


  if (prev_osc_mode!=FM_MODE && osc_mode==FM_MODE){
for (int i=0;i<max_voices;i++){
  partials[i]->setMode(2);
}

  }

   if (prev_osc_mode!=CHORUS_MODE && osc_mode==CHORUS_MODE){
for (int i=0;i<max_voices;i++){
  partials[i]->setMode(1);
}

  }

  if (prev_osc_mode!=DUAL_MODE && osc_mode==DUAL_MODE){
for (int i=0;i<max_voices;i++){
  partials[i]->setMode(1);
}

  }

} else if ((prev_osc_mode!=NORMAL) && (dual_init==false)){
  switchDualMode();
  prev_osc_mode= osc_mode;
  dual_init=true;
}
*/




//offset mod fixes
   char my_wavesample;
for (byte j=0;j<max_voices;j++){
  my_wavesample=voice2ws[j];

  if (wavesample_so_mod[my_wavesample]==LFO){ 
  
  //byte shiftornot=0;
  //if (lfo_shape[j]==1){shiftornot=1;}
  float test1;

  if ((lfo_shape[j])==0){ //saw
     test1=(((((lfo[j]+16384))) * lfo_depth[j])/4194304.0);
    } else {
        test1=(((((lfo[j]+32768))>>1) * lfo_depth[j])/4161536.0);
      }
 
  //float test1=(((((lfo[j]+32768))>>1) * lfo_depth[j])/4161536.0);
  /* classic offset mod (non-wavetable)
float test2=(wavesample_end[my_wavesample]-wavesample_loopen[my_wavesample]);
test2=test1*test2;
//Serial.println(test2);
 partials[j]->setOffset((int)test2);*/
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;
  partials[j]->setOffset((int)test2);
  if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
  }

   if (wavesample_so_mod[my_wavesample]==MOD_ENV){ 
  
float test1=(((men[j]) * (wavesample_menv_depth[my_wavesample]+127))/4161536.0);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;

 /*
  float test1=(((men[j]) * (wavesample_menv_depth[my_wavesample]+127))/4161536.0);
 
 
float test2=(wavesample_end[my_wavesample]-wavesample_loopen[my_wavesample]);
 float max_os=test2/looplen;
test2=test1*test2;
//Serial.println(test2);*/
 partials[j]->setOffset((int)test2);
   if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
  }

  
   if (wavesample_so_mod[my_wavesample]==VELOCITY){ 
  
float test1=(p_velo[j]/127.00);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;
 if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
 /*
  float test1=(((men[j]) * (wavesample_menv_depth[my_wavesample]+127))/4161536.0);
 
 
float test2=(wavesample_end[my_wavesample]-wavesample_loopen[my_wavesample]);
 float max_os=test2/looplen;
test2=test1*test2;
//Serial.println(test2);*/
 partials[j]->setOffset((int)test2);
  
  }

   if (wavesample_so_mod[my_wavesample]==CCA){ 
 // my_wavesample=voice2ws[j];
float test1=(wavesample_cca_data[my_wavesample]/127.00);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;

 partials[j]->setOffset((int)test2);
   if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
  }
  
  if (wavesample_so_mod[my_wavesample]==CCB){ 
 // my_wavesample=voice2ws[j];
float test1=(wavesample_ccb_data[my_wavesample]/127.00);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;

 partials[j]->setOffset((int)test2);
   if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
  }

   
  if (wavesample_so_mod[my_wavesample]==MODWHEEL){ 
 // my_wavesample=voice2ws[j];
float test1=(mw_now/127.00);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;

 partials[j]->setOffset((int)test2);
   if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffset((int)test2);} else { dualosc[j]->setOffset((int)test2);}
  }
  }

  if (wavesample_so_mod[my_wavesample]==KEYFOLLOW){ 
 // my_wavesample=voice2ws[j];
float test1=(voice2key[j]/127.00);
 int looplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
float test2=(test1*((wavesample_end[my_wavesample]-wavesample_start[my_wavesample])-looplen))/looplen;
test2=floor(test2)*looplen;
 
 partials[j]->setOffsetX((int)test2);
   if (osc_mode!=NORMAL){
    if (osc_mode!=CHORUS_MODE){
    int8_t my_wsb=wavesample_wsb[my_wavesample];
looplen=wavesample_loopen[my_wsb]-wavesample_loopst[my_wsb];
test2=(test1*((wavesample_end[my_wsb]-wavesample_start[my_wsb])-looplen))/looplen;
test2=floor(test2)*looplen;
      dualosc[j]->setOffsetX((int)test2);} else { dualosc[j]->setOffsetX((int)test2);}
  }
  }

  //FM FIXES (making the sample length proportional to maximum mod index)

if (wavesample_o2index_mod[my_wavesample]==MOD_ENV){       if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
 int modenv=(men[j]*(wavesample_menv_depth[my_wavesample]+127));
 float fracc = (modenv/4161536.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

} else { 
  int modenv=(men[j]*(wavesample_menv_depth[my_wavesample]))>>4;
  int mod_env_rn;
  if ((wavesample_menv_depth[my_wavesample])<0){
 mod_env_rn=modenv+130048;} else {mod_env_rn=modenv;};
  partials[j]->setO2Index(mod_env_rn>>10);
  }

}


if (wavesample_o2index_mod[my_wavesample]==MODWHEEL){       if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
 //int modw=(men[j]*(wavesample_menv_depth[my_wavesample]+127));
 float fracc = (mw_now/127.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

} else { 
  partials[j]->setO2Index(mw_now);}

}


if (wavesample_o2index_mod[my_wavesample]==LFO){    
  
 
     if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
  int lfod=((lfo[j]+32768)*(lfo_depth[j]));
 float fracc = (lfod/8323072.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

}

}

if (wavesample_o2index_mod[my_wavesample]==VELOCITY){    
  
 
     if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
  //int lfod=((lfo[j]+32768)*(wavesample_lfodepth[my_wavesample]));
 float fracc = (p_velo[j]/127.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

}}

if (wavesample_o2index_mod[my_wavesample]==CCA){    
  
 
     if (osc_mode==FM_MODE) { 
 if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
 //int modw=(men[j]*(wavesample_menv_depth[my_wavesample]+127));
 float fracc = (wavesample_cca_data[my_wavesample]/127.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));


}

}

if (wavesample_o2index_mod[my_wavesample]==CCB){    
  
 
     if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
  //int lfod=((lfo[j]+32768)*(wavesample_lfodepth[my_wavesample]));
 float fracc = (wavesample_ccb_data[my_wavesample]/127.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

}

}

if (wavesample_o2index_mod[my_wavesample]==KEYFOLLOW){    
  
 
     if (osc_mode==FM_MODE) { 
 int mlooplen=wavesample_loopen[my_wavesample]-wavesample_loopst[my_wavesample];
  //int lfod=((lfo[j]+32768)*(wavesample_lfodepth[my_wavesample]));
 float fracc = (voice2key[j]/127.00)*mlooplen;
 partials[j]->setO2Index((int)(fracc));

}

}


//click fix
/*
if (partials[j]->isActive()==false){
 // partials[j]->setRZ(true);
} else{
  //partials[j]->setRZ(false);
}
*/


}
  }
}
}

void wavSmpl(){
  
  }

void doNothing(byte poo,int pee){};


void writeDWORD(File file, int dword){
file.write(dword>>24);
file.write((dword>>16)&255);
file.write((dword>>8)&255);
file.write((dword)&255);
}

void writeDWORDle(File file, int dword){
  file.write((dword)&255);
  file.write((dword>>8)&255);
  file.write((dword>>16)&255);
file.write(dword>>24);
}

int readDWORD(File file){
  int dword;
 dword+=file.read()<<24;
  dword+=file.read()<<16;
    dword+=file.read()<<8;
      dword+=file.read();
return dword;
}

int readDWORDle(File file){
  int dword;
 dword+=file.read();
  dword+=file.read()<<8;
    dword+=file.read()<<16;
      dword+=file.read()<<24;
return dword;
}
int readWORD(File file){
  int word;

    word+=file.read()<<8;
      word+=file.read();
return word;
}

int readWORDle(File file){
  int word;

    word+=file.read();
      word+=file.read()<<8;
return word;
}

void writeWORD(File file, int word){

file.write((word>>8));
file.write((word)&255);
}

void writeWORDle(File file, int word){
file.write((word)&255);
file.write((word>>8));

}

void writeBank(fs::FS &fs, const char * path){
   enable_click=false;
  displaycontext=SAVE_SCRN;
  hovercontext=EDIT;
  hovermode=false;
  display.clearDisplay();
    display.setCursor(2,0);
    display.println("Saving File..");
    display.display();
   // Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        display.setCursor(2,16);
    display.println("ERROR: Problem saving file");
    display.display();
    enable_click=true;
        return;
    }


    if(file.print("WSB3")){
        display.setCursor(2,16);
    display.println("Writing Samples");
    display.display();
        Serial.println("Header written");
    } else {
        display.setCursor(2,16);
    display.println("Save Failed");
    display.display();
        Serial.println("Header Write failed");
        enable_click=true;
        return;
    }
file.print(bankname);
    //sample memory
    
 bool success=true;
for (int i=0;i<88064;i++){
    if (file.write(samp_memory[i])){
      success=true;
    } else {success=false;}
}
if (success==true){
     display.setCursor(2,24);
    display.println("Sample Write Success");
    display.display();
Serial.println("Sample Write Success");} else {Serial.println("Sample Write Failiure");
 display.setCursor(2,24);
    display.println("Sample Write Fail");
    display.display();
    enable_click=true;
    return;
}


 //wavesample/zone data
if (file.write(user_wavesamples)){
      success=true;
    } else {success=false;}
for (int i=0;i<user_wavesamples;i++){
file.write(wavesample_topkey[i]);
writeDWORD(file,wavesample_start[i]);
writeDWORD(file,wavesample_end[i]);
writeDWORD(file,wavesample_loopst[i]);
writeDWORD(file,wavesample_loopen[i]);
file.write(wavesample_loopmode[i]);
file.write(wavesample_vol[i]);
file.write(wavesample_pan[i]);

file.write(wavesample_amp_atk[i]);
file.write(wavesample_amp_dec[i]);
file.write(wavesample_amp_sus[i]);
file.write(wavesample_amp_rel[i]);

file.write(wavesample_filter_atk[i]);
file.write(wavesample_filter_dec[i]);
file.write(wavesample_filter_sus[i]);
file.write(wavesample_filter_rel[i]);

file.write(wavesample_mod_atk[i]);
file.write(wavesample_mod_dec[i]);
file.write(wavesample_mod_sus[i]);
file.write(wavesample_mod_rel[i]);

file.write(wavesample_filter_type[i]);
file.write(wavesample_filter_offset[i]);
file.write(wavesample_filter_reso[i]);
file.write(wavesample_wsb[i]);
file.write(wavesample_init_o2index[i]);
file.write(wavesample_vassign[i]);
file.write(wavesample_ctune[i]);
file.write(wavesample_detune[i]);
file.write(wavesample_lfoshape[i]);
file.write(wavesample_lfodepth[i]);
file.write(wavesample_lforate[i]);
file.write(wavesample_cc_a[i]);
file.write(wavesample_cca_data[i]);
file.write(wavesample_cc_b[i]);
file.write(wavesample_ccb_data[i]);
file.write(wavesample_cutoff_mod[i]);
file.write(wavesample_reso_mod[i]);
file.write(wavesample_o2index_mod[i]);
file.write(wavesample_o2detune_mod[i]);
file.write(wavesample_amp_mod[i]);
file.write(wavesample_oap_mod[i]);
file.write(wavesample_lfr_mod[i]);
file.write(wavesample_lfd_mod[i]);
file.write(wavesample_so_mod[i]);
file.write(wavesample_pan_mod[i]);
file.write(wavesample_menv_depth[i]);
file.write(wavesample_init_offset[i]);
file.write(wavesample_sqr_ctune[i]);
writeWORD(file,wavesample_portatime[i]);
}
if (success==true){Serial.println("Wavesample Write Success");
 display.setCursor(2,32);
    display.println("Zone Write Success");
    display.display();
} else {Serial.println("Wavesample Write Failiure");
 display.setCursor(2,32);
    display.println("Zone Write Failed");
    display.display();
    enable_click=true;
return;
}

if (file.write(user_programs)){
      success=true;
    } else {success=false;}
for (int i=0;i<=user_programs;i++){
file.write(program_init_ws[i]);
file.write(program_osc_mode[i]);
file.write(program_polymode[i]);
file.write(program_oscbdetune[i]);
}


if (success==true){
   display.setCursor(2,40);
    display.println("Program Write Success");
    display.display();
Serial.println("Program Write Success");} else {Serial.println("Program Write Failiure");
 display.setCursor(2,40);
    display.println("Program Write Failed");
    display.display();
    enable_click=true;
    return;
}
file.write(midi_channel);
file.write(pb_range);
file.write(sample_pos);
 display.setCursor(2,48);
    display.println("Finished Saving Bank");
    display.display();
Serial.println("Finished Saving Bank");
    file.close();
        enable_click=true;
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
   
   for (uint8_t i=0;i<3;i++){
        Serial.write(file.read());}
        
    
    file.close();
}

void loadBank(fs::FS &fs, const char * path){

 
    Serial.printf("Loading Bank: %s\n", path);
drawing=true;
    
    display.clearDisplay();
    display.setCursor(2,0);
    display.println("Loading File..");
    display.display();
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        display.setCursor(2,8);
    display.println("ERROR: Problem reading file");
    display.display();
    enable_click=true;
        return;
    } 

    Serial.print("Read from file: ");
   int header_check=0;
  
        header_check+=file.read();
        header_check+=file.read()<<8;
        header_check+=file.read()<<16;
        header_check+=file.read()<<24;

        if ((header_check==WSB_HEADER) || (header_check==WSB2_HEADER) ||  (header_check==WSB3_HEADER)){
 Serial.println("CERTIFIED WOMPRIS SOUND BANK");
 display.setCursor(2,16);
    display.println("Wompris Sound Bank");
    display.display();
        } else {display.setCursor(2,16);
    display.println("ERROR: Non-WSB file");
    display.display();enable_click=true;return;}

        file.readBytes(bankname,8);
        Serial.println(bankname);
display.setCursor(2,24);
    display.println(bankname);
    display.display();
display.setCursor(2,32);
    display.println("Loading Samples");
    display.display();
    if ((header_check==WSB2_HEADER) ||(header_check==WSB3_HEADER)) {
     for(int i=0;i<88064;i++){
        samp_memory[i]=file.read();}} else if (header_check==WSB_HEADER) {
 for(int i=0;i<87832;i++){
        samp_memory[i]=file.read();}

        }
 Serial.println("Loaded Sample Memory");

user_wavesamples=file.read();
Serial.println(user_wavesamples);

display.setCursor(2,40);
    display.println("Loading Zones");
    display.display();
    //Serial.printf("Loading %s zones\n", user_wavesamples);
    uint8_t ws=0;
for(uint8_t i=0;i<user_wavesamples;i++){
  ws=i;
  if (i>WAVESAMPLE_AMOUNT){
  ws=WAVESAMPLE_AMOUNT;
  }
wavesample_topkey[ws]=file.read();
wavesample_start[ws]=readDWORD(file);
wavesample_end[ws]=readDWORD(file);
wavesample_loopst[ws]=readDWORD(file);
wavesample_loopen[ws]=readDWORD(file);
wavesample_loopmode[ws]=file.read();
wavesample_vol[ws]=file.read();
wavesample_pan[ws]=file.read();
wavesample_amp_atk[ws]=file.read();
wavesample_amp_dec[ws]=file.read();
wavesample_amp_sus[ws]=file.read();
wavesample_amp_rel[ws]=file.read();
wavesample_filter_atk[ws]=file.read();
wavesample_filter_dec[ws]=file.read();
wavesample_filter_sus[ws]=file.read();
wavesample_filter_rel[ws]=file.read();
wavesample_mod_atk[ws]=file.read();
wavesample_mod_dec[ws]=file.read();
wavesample_mod_sus[ws]=file.read();
wavesample_mod_rel[ws]=file.read();
wavesample_filter_type[ws]=file.read();
wavesample_filter_offset[ws]=file.read();
wavesample_filter_reso[ws]=file.read();
wavesample_wsb[ws]=file.read();
wavesample_init_o2index[ws]=file.read();
wavesample_vassign[ws]=file.read();
wavesample_ctune[ws]=file.read();
wavesample_detune[ws]=file.read();
wavesample_lfoshape[ws]=file.read();
wavesample_lfodepth[ws]=file.read();
wavesample_lforate[ws]=file.read();
wavesample_cc_a[ws]=file.read();
wavesample_cca_data[ws]=file.read();
wavesample_cc_b[ws]=file.read();
wavesample_ccb_data[ws]=file.read();
wavesample_cutoff_mod[ws]=file.read();
wavesample_reso_mod[ws]=file.read();
wavesample_o2index_mod[ws]=file.read();
wavesample_o2detune_mod[ws]=file.read();
wavesample_amp_mod[ws]=file.read();
wavesample_oap_mod[ws]=file.read();
wavesample_lfr_mod[ws]=file.read();
wavesample_lfd_mod[ws]=file.read();
wavesample_so_mod[ws]=file.read();
wavesample_pan_mod[ws]=file.read();
wavesample_menv_depth[ws]=file.read();
wavesample_init_offset[ws]=file.read();
if ((header_check==WSB3_HEADER)){
wavesample_sqr_ctune[ws]=file.read();}
wavesample_portatime[ws]=readWORD(file);
}
   Serial.println("Loaded Zones");

user_programs=file.read();
display.setCursor(2,48);
    display.println("Loading Programs");
    display.display();
   // Serial.printf("Loading %s programs \n", user_programs);
  
for(uint8_t i=0;i<=user_programs;i++){
uint8_t p=i;
   if (i>PROGRAM_AMOUNT) {
    p=PROGRAM_AMOUNT;
    }
program_init_ws[p]=file.read();
program_osc_mode[p]=file.read();
program_polymode[p]=file.read();

program_oscbdetune[p]=file.read();

}
osc_mode=program_osc_mode[program];
poly_mode=program_polymode[program];
   
 for (char i;i<128;i++){key2voice[i]=255;}
//stop i2s bus here please!
for (char i;i<max_voices;i++){voice2key[i]=255;};
 //Serial.printf("%s programs loaded \n", user_programs);
  Serial.println("Loaded Programs");
  display.setCursor(2,54);
    display.println("Loading Settings");
    display.display();
  midi_channel=file.read();
 pb_range=file.read();
  sample_pos=file.read();
  Serial.println("Loaded Global Settings");
    file.close();
hovermode=true;
hovercontext=EDIT;
    displaycontext=EDIT;
menuoffset=0;
     rotaryEncoder.setEncoderValue(0);
     // rotaryEncoder.setPosition(0);
     drawing=false;
       rotaryEncoder.setBoundaries(0,11, true); 
       enable_click=true;
       handleProgramChange(midi_channel,0);
      
}


void loadWAV(fs::FS &fs, const char * path){
  int sample_pos_cache_b;
  if ((preview!=true) && (is_replace==true)){  
    //old_sample_pos=sample_pos;
sample_pos=(uint8_t)ceil((float)wavesample_start[edit_wavesample] / 512.00);
} 
if (preview && is_replace) {
    sample_pos_cache_b=sample_pos;
  sample_pos=old_sample_pos;

  }
  if (preview){
 if (sample_pos+sample_length>172){
                
                errorPreviewMemory();
                return;
                }}

uint8_t import_channels=0;
uint8_t import_bitdepth=0;
int import_data_size=0;
drawing=true;
    Serial.printf("Loading WAVE: %s\n", path);
    display.clearDisplay();
    display.setCursor(2,0);
    display.println("Loading File..");
    display.display();
    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    } 

    //read RIFF Header
   int header_check=readDWORD(file);
  

        if (header_check==RIFF){
 Serial.println("RIFF");
//display.setCursor(2,16);
    //display.println("RIFF Detected..");
    display.display();
        } else { Serial.println("NOT RIFF");
        display.setCursor(2,16);
    display.println("ERROR: Non-WAVE File"); display.display();enable_click=true;return;}


//read/throw away chunksize
header_check=readDWORD(file);


//read WAVE header
header_check=readDWORD(file);

        if (header_check==WAVE){
          
 Serial.println("WAVE");
//display.setCursor(2,24);
display.setCursor(2,16);
    display.println("WAVE Detected");
    display.display();
        } else { Serial.println("NOT WAVE");
        display.setCursor(2,16);
    display.println("ERROR: Non-WAVE file");
    display.display();enable_click=true;
         return;}
//subchunk1 id
readDWORD(file);
//subchunk1 size
header_check=readDWORDle(file);//16 for non-extended waves
Serial.println(header_check);
bool fact=false;
if (header_check==18){fact=true;}
//"audio format" (if compresssed, do not continue)
header_check=readWORDle(file);
 if (header_check==1){Serial.println("Uncompressed");
 //display.setCursor(2,32);
 display.setCursor(2,24);
    display.println("Verified Uncompressed");
    display.display();
 } else {
Serial.println("Compression unsupported");
 display.setCursor(2,24);
    display.println("Unsupported WAV");
    display.display();
    enable_click=true;
 return;

 }
//channels #
import_channels=readWORDle(file);

//samp_rate
int samp_rate = readDWORDle(file);
//byterate
readDWORD(file);
//block align
readWORD(file);


//bit depth
import_bitdepth=readWORDle(file);
 display.setCursor(2,32);
    display.println(String(import_bitdepth)+" bits");
display.display();
Serial.println(String(import_bitdepth));
//fact skipping with extended akai waves
if (fact==true){
  readWORD(file);//skip cbSize
  //readDWORD(file);//skip "fact"
 // readDWORD(file);//skip factSize
   // readDWORD(file);//skip sample num


  } 

//fact or smpl checker  (used to be data section or smpl? checker)
header_check=readDWORD(file);
//check for sampler chunk
//header_check=readDWORD(file);
Serial.println(header_check);
  if(header_check==FACT){
     readDWORD(file);//skip factSize
    readDWORD(file);//skip sample num
    header_check=readDWORD(file);
  } 

         uint8_t user_wavesamples_cache=user_wavesamples;
    if(preview==false){ if ((is_replace)==true){user_wavesamples=edit_wavesample;}}
//smpl or data?
bool smpl_ld=false;
  if(header_check==SMPL){
     readDWORD(file);//skip chunkSize
     readDWORD(file);//skip mma id
     readDWORD(file);//skip "product"
     readDWORD(file);//skip nanosecond count
     //get root
     int rootnote=(readDWORDle(file));//root midi note'
     float s2=(float)samp_rate;
     float multiplier=30000.00/s2;
      
     float freq=440.00*(pow(2.00,(rootnote-69.00)/12.00));
     freq=multiplier * freq;
     freq=freq/440.00;
    
     float multi2= log2(freq);
     
     freq=floor((multi2*12)+69);
     Serial.println(freq);
     uint8_t midi_note=(int8_t)freq;
     wavesample_ctune[user_wavesamples]=(58-midi_note);
     wavesample_detune[user_wavesamples]=15;
     readDWORD(file);//tuning
     readDWORD(file);//skip smpte
     readDWORD(file);//skip smpte
     int skip_xtra_loop_chunks =readDWORDle(file)-1;//loop number, if greater than 1 then skip all but the first
     int skip_xtra_data_bytes = readDWORDle(file);//number of bytes to skip after loop data
     //Serial.println(readDWORD(file));//skip "loop" tag
      header_check=readDWORD(file);
      Serial.println(header_check);
     if((header_check==16777216) or (header_check==1819242352)){
     //get loop type
     int looptype=readDWORDle(file);//loop type | 0=forward, 1=ping pong, 2=backward
     switch (looptype){
      case 0:
      wavesample_loopmode[user_wavesamples]=1;
      break;
       case 1:
      wavesample_loopmode[user_wavesamples]=4;
      break;
       case 2:
      wavesample_loopmode[user_wavesamples]=3;
      break;
      }
     int loopp = readDWORDle(file);//loop start
      wavesample_loopst[user_wavesamples]=1+(sample_pos * 512)+loopp;
    loopp = readDWORDle(file);//loop end
     wavesample_loopen[user_wavesamples]=1+(sample_pos * 512)+loopp;
          readDWORD(file);//loop frac
     readDWORD(file);//loop number | 0=infinite 
      if(skip_xtra_loop_chunks>0){
        for (int j=0;j<skip_xtra_loop_chunks;j++){
        for (int i=0;i<6;i++){
           readDWORD(file);
          }
        }}
     if(skip_xtra_data_bytes>0){
      for (int i=0;i<skip_xtra_data_bytes;i++){file.read();}
      }
       header_check=readDWORD(file);
       smpl_ld=true;
      } 

     
    }
//Serial.println
Serial.println(header_check);
 if (header_check==DATA_ID){Serial.println("Starting Audio Data Transfer");
  //display.setCursor(2,48);
    //display.println("Begin Audio Data TX");
    //display.display();
 } else {
     display.setCursor(2,40);
    display.println("ERROR: Unsupported");
    display.display();
Serial.println("Wave Extensions Unsupported"); enable_click=true;return;

 }
//data szie
import_data_size=readDWORDle(file);
display.setCursor(2,40);
 display.println("Length:"+(String((float)(import_data_size)/(float)samp_rate)+"sec"));
 display.display();
if (import_bitdepth==16){import_data_size/=2;};
if (import_channels>1){import_data_size/=2;};
//start import of audio data
display.setCursor(2,48);

 display.println("Begin Audio Data TX");
for (int i=0;i<sample_length*512;i++){
  //Serial.println(i);
  if (i<import_data_size){
int actual_pos=1+(sample_pos * 512) + i;
if (actual_pos>88063){actual_pos=88063;
 //errorPreviewMemory();
 //return;
};

if (import_bitdepth==8){
 
int16_t samp_value=(file.read())-128;
/*
if (samp_value<0){
  samp_value+=128;
} else {(samp_value=(samp_value * -1)+128);}*/
samp_memory[actual_pos]=(int8_t)samp_value;

} else {
  //throw away useless byte for 16 bit
file.read();

}


if (import_bitdepth >= 16){
  
samp_memory[actual_pos]=file.read();
}

//throw away bytes
if (import_channels>1){
  file.read();

  if (import_bitdepth>=16){
    file.read();
  }

}
  } else {
file.close();
      break;
      break;
  }


}

Serial.println("Import Success");
     display.setCursor(2,56);
    display.println("Import Success");
    display.display();


    
//go to sample editor
 
 int old_samp_len=sample_length;
      sampling=false;
       //sampling_level=false;
       

       wavesample_start[user_wavesamples]=(sample_pos * 512);
      if (smpl_ld!=true){wavesample_loopst[user_wavesamples]=wavesample_start[user_wavesamples];}
       if ((sample_length * 512) < import_data_size){
      wavesample_end[user_wavesamples]=wavesample_start[user_wavesamples]+(sample_length * 512);
} else {
 wavesample_end[user_wavesamples]=wavesample_start[user_wavesamples]+(import_data_size);

 sample_length=ceil((float)import_data_size / 512.00);

      }
      if (smpl_ld!=true){ wavesample_loopen[user_wavesamples]=wavesample_end[user_wavesamples];} else if ( wavesample_loopen[user_wavesamples]==wavesample_loopst[user_wavesamples]){wavesample_loopen[user_wavesamples]+=1;}
            
            if (preview==true){
               
              byte progcache=program;
               prev_osc_mode=osc_mode;
               program=63;
osc_mode=program_osc_mode[program];
poly_mode=program_polymode[program];
  // next_voice=0;
 //for (char i;i<128;i++){handleNoteOff(midi_channel,i, 0);key2voice[i]=255;}
//stop i2s bus here please!
//for (char i;i<max_voices;i++){voice2key[i]=255;};
              switchNormalMode();
              program_init_ws[program]=user_wavesamples;
             // wavesample_loopmode[user_wavesamples]=0;
             wavesample_amp_sus[user_wavesamples]=0;
             wavesample_amp_dec[user_wavesamples]=5;
             wavesample_amp_rel[user_wavesamples]=5;
              playNote(0,60,100);
               enable_click=true;
               drawing=false;
               displaycontext=SAMPLE_LOAD;
               rotaryEncoder.setBoundaries(0,9998, true);
               rotaryEncoder.setEncoderValue(next_dir_pos);
               //handleProgramChange(midi_channel,progcache);
               sample_length=old_samp_len;
               program=0;
              return;
              }
              drawing=false;
 displaycontext=SAMPLE_EDITOR;
 
              sample_pos+=sample_length;
            
       edit_wavesample=user_wavesamples;
         if (is_replace==false){ user_wavesamples++;} else{
user_wavesamples=user_wavesamples_cache;
       }
       if (nprog==true){
         program=user_programs;
         program++;
         program_init_ws[program]=edit_wavesample;
          user_programs++;
             prev_osc_mode=osc_mode;
          osc_mode=0;
  poly_mode=0;
       }
       menuoffset=ZOOM-4;
       hovermode=true;
      hovercontext=5;
       rotaryEncoder.setEncoderValue(0);
     // rotaryEncoder.setPosition(0);
       rotaryEncoder.setBoundaries(0,7, true); 
       if (drawing==false){
      refreshDisplay();}
     
      
      if (is_replace && !preview){sample_pos=old_sample_pos;is_replace=false;}
      if (preview && is_replace) {
  sample_pos= sample_pos_cache_b;
  }
      enable_click=true;
      //copier.begin(out,voice_mixer);


    
}



void saveWAV(fs::FS &fs, const char * path){
  hovercontext=EDIT;
  hovermode=false;
  displaycontext=SAVE_SCRN;
  rotaryEncoder.setBoundaries(0,3, true);
  int subchunk2size=wavesample_end[edit_wavesample]-wavesample_start[edit_wavesample];
int chunk_size=subchunk2size+36;
    Serial.printf("Saving WAVE: %s\n", path);

   
    display.clearDisplay();
    display.setCursor(2,0);
    display.println("Saving WAV..");
    display.display();
   File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
         display.setCursor(2,16);
    display.println("ERROR: Unable to save");
    display.display();
    enable_click=true;
        return;
    }

 display.setCursor(2,16);
    display.println("Writing WAV Header");
    display.display();
    //write RIFF Header
   writeDWORD(file,RIFF);
  



//writechunksize
writeDWORDle(file,chunk_size);


//write WAVE header
   writeDWORD(file,WAVE);

     
//subchunk1 id
   writeDWORD(file,FMTT);
//subchunk1 size
writeDWORDle(file,16);

//"audio format" (if compresssed, do not continue)
writeWORDle(file,1);
 
//channels #
writeWORDle(file,1);

//samp_rate
writeDWORDle(file,30000);
//byterate
writeDWORDle(file,30000);
//block align
writeWORDle(file,1);


//bit depth
writeWORDle(file,8);

//data section check
 writeDWORD(file,DATA_ID);
 
//data szie
writeDWORDle(file,subchunk2size);
display.setCursor(2,24);
    display.println("Writing Audio Data");
    display.display();
for (int i=0;i<subchunk2size;i++){
 int16_t samp_value=(samp_memory[wavesample_start[edit_wavesample]+i]);
 samp_value+=128;
file.write(samp_value);

}
display.setCursor(2,32);
    display.println("Success");
    display.display();
file.close();
}
