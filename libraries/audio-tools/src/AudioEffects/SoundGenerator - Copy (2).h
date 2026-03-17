#pragma once

#include "AudioTools/AudioLogger.h"
#include "AudioTools/AudioTypes.h"
#include "AudioBasic/Collections.h"
#define ONESHOT 0
#define LOOP 1
#define REVERSE 2
#define REVERSELOOP 3
#define PINGPONG 4
/** 
 * @defgroup generator Generators
 * @ingroup dsp
 * @brief Sound Generators  
**/


namespace audio_tools {

/**
 * @brief Base class to define the abstract interface for the sound generating classes
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 * @tparam T 
 */
template <class T>
class SoundGenerator  {
    public:
        SoundGenerator() {
            info.bits_per_sample = sizeof(T)*8;
            info.channels = 1;
            info.sample_rate = 44100;
        }

        virtual ~SoundGenerator() {
            end();
        }

        virtual bool begin(AudioInfo info) {
            this->info = info;
            return begin();
        }

        virtual bool begin() {
            TRACED();
            active = true;
            activeWarningIssued = false;
            LOGI("bits_per_sample: %d", info.bits_per_sample);
            LOGI("channels: %d", info.channels);
            LOGI("sample_rate: %d", info.sample_rate);

            // support bytes < framesize
            ring_buffer.resize(info.channels*sizeof(T));

            return true;
        }

        /// ends the processing
        virtual void end(){
            active = false;
        }

        /// Checks if the begin method has been called - after end() isActive returns false
        virtual bool isActive() {
            return active;
        }

        /// Provides a single sample
        virtual  T readSample() = 0;

        /// Provides the data as byte array with the requested number of channels
        virtual size_t readBytes( uint8_t *buffer, size_t lengthBytes){
            LOGD("readBytes: %d", (int)lengthBytes);
            int channels = audioInfo().channels;
            int frame_size = sizeof(T) * channels;
            int frames = lengthBytes / frame_size;
            if (lengthBytes>=frame_size){
                return readBytesFrames(buffer, lengthBytes, frames, channels);
            }
            return readBytesFromBuffer(buffer, lengthBytes, frame_size, channels);
        }

        virtual AudioInfo defaultConfig(){
            AudioInfo def;
            def.bits_per_sample = sizeof(T)*8;
            def.channels = 1;
            def.sample_rate = 44100;
            return def;
        }

        virtual void setFrequency(float frequency) {
            LOGE("setFrequency not supported");
        }


        virtual AudioInfo audioInfo() {
            return info;
        }

        virtual void setAudioInfo(AudioInfo info){
            this->info = info;
            if (info.bits_per_sample!=sizeof(T)*8){
                LOGE("invalid bits_per_sample: %d", info.channels);
            }   
        }

    protected:
        bool active = false;
        bool activeWarningIssued = false;
        int output_channels = 1;
        AudioInfo info;
        RingBuffer<uint8_t> ring_buffer{0};

        size_t readBytesFrames(uint8_t *buffer, size_t lengthBytes, int frames, int channels ){
            T* result_buffer = (T*)buffer;
            for (int j=0;j<frames;j++){
                T sample = readSample();
                for (int ch=0;ch<channels;ch++){
                    *result_buffer++ = sample;
                }
            }
            return frames*sizeof(T)*channels;
        }

        size_t readBytesFromBuffer(uint8_t *buffer, size_t lengthBytes, int frame_size, int channels) {
            // fill ringbuffer with one frame
            if (ring_buffer.isEmpty()){
                uint8_t tmp[frame_size];
                readBytesFrames(tmp, frame_size, 1, channels);
                ring_buffer.writeArray(tmp, frame_size);
            }
            // provide result
            return ring_buffer.readArray(buffer, lengthBytes);
        }


        
};


/**
 * @brief Generates a Sound with the help of sin() function. If you plan to change the amplitude or frequency (incrementally),
 * I suggest to use SineFromTable instead.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */
template <class T>
class SineWaveGenerator : public SoundGenerator<T>{
    public:

        // the scale defines the max value which is generated
        SineWaveGenerator(float amplitude = 0.9 * NumberConverter::maxValueT<T>(), float phase = 0.0){
            LOGD("SineWaveGenerator");
            m_amplitude = amplitude;
            m_phase = phase;
        }

        bool begin() override {
            TRACEI();
            SoundGenerator<T>::begin();
            this->m_deltaTime = 1.0 / SoundGenerator<T>::info.sample_rate;
            return true;
        }

        bool begin(AudioInfo info) override {
            LOGI("%s::begin(channels=%d, sample_rate=%d)","SineWaveGenerator", info.channels, info.sample_rate);
            SoundGenerator<T>::begin(info);
            this->m_deltaTime = 1.0 / SoundGenerator<T>::info.sample_rate;
            return true;
        }

        bool begin(AudioInfo info, float frequency){
            LOGI("%s::begin(channels=%d, sample_rate=%d, frequency=%.2f)","SineWaveGenerator",info.channels, info.sample_rate,frequency);
            SoundGenerator<T>::begin(info);
            this->m_deltaTime = 1.0 / SoundGenerator<T>::info.sample_rate;
            if (frequency>0){
                setFrequency(frequency);
            }
            return true;
        }

        bool begin(int channels, int sample_rate, float frequency=0.0){
            SoundGenerator<T>::info.channels  = channels;
            SoundGenerator<T>::info.sample_rate = sample_rate;
            return begin(SoundGenerator<T>::info, frequency);
        }

        // update m_deltaTime
        virtual void setAudioInfo(AudioInfo info) override {
            SoundGenerator<T>::setAudioInfo(info);
            this->m_deltaTime = 1.0 / SoundGenerator<T>::info.sample_rate;
        }

        virtual AudioInfo defaultConfig() override {
            return SoundGenerator<T>::defaultConfig();
        }

        /// Defines the frequency - after the processing has been started
        void setFrequency(float frequency)  override {
            LOGI("setFrequency: %.2f", frequency);
            LOGI( "active: %s", SoundGenerator<T>::active ? "true" : "false" );
            m_frequency = frequency;
        }

        /// Provides a single sample
        virtual T readSample() override {
            float angle = double_Pi * m_cycles + m_phase;
            T result = m_amplitude * sinf(angle);
            m_cycles += m_frequency * m_deltaTime;
            if (m_cycles > 1.0) {
                m_cycles -= 1.0;
            }
            return result;
        }

        void setAmplitude(float amp){
            m_amplitude = amp;
        }

    protected:
        volatile float m_frequency = 0;
        float m_cycles = 0.0; // Varies between 0.0 and 1.0
        float m_amplitude = 1.0;  
        float m_deltaTime = 0.0;
        float m_phase = 0.0;
        float double_Pi = PI * 2.0;


        void logStatus() {
            SoundGenerator<T>::info.logStatus();
            LOGI( "amplitude: %f", this->m_amplitude );
            LOGI( "active: %s", SoundGenerator<T>::active ? "true" : "false" );
        }

};

/**
 * @brief Generates a square wave sound.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */
template <class T>
class SquareWaveGenerator : public SineWaveGenerator<T> {
    public:
        SquareWaveGenerator(float amplitude = 32767.0, float phase = 0.0) : SineWaveGenerator<T>(amplitude, phase) {
            LOGD("SquareWaveGenerator");
        }

        virtual  T readSample() {
            return value(SineWaveGenerator<T>::readSample(), SineWaveGenerator<T>::m_amplitude);
        }

    protected:
        // returns amplitude for positive vales and -amplitude for negative values
        T value(T value, T amplitude) {
            return (value >= 0) ? amplitude : -amplitude;
        }
};


/**
 * @brief Sine wave which is based on a fast approximation function. 
 * @ingroup generator
 * @author Vivian Leigh Stewart
 * @copyright GPLv3
 * @tparam T 
 */
template <class T>
class FastSineGenerator : public SineWaveGenerator<T> {
    public:
        FastSineGenerator(float amplitude = 32767.0, float phase = 0.0) : SineWaveGenerator<T>(amplitude, phase) {
            LOGD("FastSineGenerator");
        }

        virtual T readSample() override {
            float angle = SineWaveGenerator<T>::m_cycles +
                          SineWaveGenerator<T>::m_phase;
            T result = SineWaveGenerator<T>::m_amplitude * sine(angle);
            SineWaveGenerator<T>::m_cycles += SineWaveGenerator<T>::m_frequency *
                                                SineWaveGenerator<T>::m_deltaTime;
            if (SineWaveGenerator<T>::m_cycles > 1.0) {
                SineWaveGenerator<T>::m_cycles -= 1.0;
            }
            return result;
        }
        
    protected:
        /// sine approximation.
        inline float sine(float t) {
            float p = (t - (int)t) - 0.5f; // 0 <= p <= 1
            float pp = p * p;
            return (p - 6.283211f * pp * p + 9.132843f * pp * pp * p) * -6.221086f;
        }
};

/**
 * @brief Generates a random noise sound with the help of rand() function.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */
template <class T>
class WhiteNoiseGenerator : public SoundGenerator<T> {
    public:
        /// the scale defines the max value which is generated
        WhiteNoiseGenerator(T amplitude = 32767) {
            this->amplitude = amplitude;
        }

        /// Provides a single sample
        T readSample() {
            return (random(-amplitude, amplitude));
        }

    protected:
        T amplitude;
        // //range : [min, max]
        int random(int min, int max)  {
            return min + rand() % (( max + 1 ) - min);
        }


};

/**
 * @brief Generates pink noise.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 *
 */
template <class T> class PinkNoiseGenerator : public SoundGenerator<T> {
public:
  /// the amplitude defines the max value which is generated
  PinkNoiseGenerator(T amplitude = 32767) {
    this->amplitude = amplitude;
    max_key = 0x1f; // Five bits set
    key = 0;
    for (int i = 0; i < 5; i++)
      white_values[i] = rand() % (amplitude / 5);
  }

  /// Provides a single sample
  T readSample() {
    T last_key = key;
    unsigned int sum;

    key++;
    if (key > max_key)
      key = 0;
    // Exclusive-Or previous value with current value. This gives
    // a list of bits that have changed.
    int diff = last_key ^ key;
    sum = 0;
    for (int i = 0; i < 5; i++) {
      // If bit changed get new random number for corresponding
      // white_value
      if (diff & (1 << i))
        white_values[i] = rand() % (amplitude / 5);
      sum += white_values[i];
    }
    return sum;

  }

protected:
  T max_key;
  T key;
  unsigned int white_values[5];
  unsigned int amplitude;
};


/**
 * @brief Provides a fixed value (e.g. 0) as sound data. This can be used e.g. to test the output functionality which should optimally just output
 * silence and no artifacts.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * 
 */
template <class T>
class SilenceGenerator : public SoundGenerator<T> {
    public:
        // the scale defines the max value which is generated
        SilenceGenerator(T value=0) {
            this->value = value;
        }

        /// Provides a single sample
        T readSample() {
            return  value; // return 0
        }

    protected:
        T value;

};

/**
 * @brief An Adapter Class which lets you use any Stream as a Generator
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * @tparam T 
 */
template <class T>
class GeneratorFromStream : public SoundGenerator<T> {
    public:
        GeneratorFromStream() {
            maxValue = NumberConverter::maxValue(sizeof(T)*8);
        };

        /**
         * @brief Constructs a new Generator from a Stream object that can be used e.g. as input for AudioEffectss.
         * 
         * @param input Stream
         * @param channels number of channels of the Stream
         * @param volume factor my which the sample value is multiplied - default 1.0; Use it e.g. to reduce the volume (e.g. with 0.5)
         */
        GeneratorFromStream(Stream &input, int channels=1, float volume=1.0){
            maxValue = NumberConverter::maxValue(sizeof(T)*8);
            setStream(input);
            setVolume(volume);
            setChannels(channels);
        }

        /// (Re-)Assigns a stream to the Adapter class
        void setStream(Stream &input){
            this->p_stream = &input;
        }

        void setChannels(int channels){
            this->channels = channels;
        }

        /// Multiplies the input with the indicated factor (e.g. )
        void setVolume(float factor){
            this->volume = factor;
        }
        
        
        /// Provides a single sample from the stream
        T readSample() {
            T data = 0;
            float total = 0;
            if (p_stream!=nullptr) {
                for (int j=0;j<channels;j++){
                    p_stream->readBytes((uint8_t*)&data, sizeof(T));
                    total += data;
                }
                float avg = (total / channels) * volume;
                if (avg>maxValue){
                    data = maxValue;
                } else if (avg < -maxValue){
                    data = -maxValue;
                } else {
                    data = avg;
                }
            }
            return data;
        }

    protected:
        Stream *p_stream = nullptr;
        int channels=1;
        int volume=1.0;
        float maxValue;

};

/**
 * @brief We generate the samples from an array which is provided in the constructor
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * @tparam T 
 */
 #define SHIFTER 12
template <class T>
class GeneratorFromArray : public SoundGenerator<T> {
  public:

    GeneratorFromArray() = default;
    /**
     * @brief Construct a new Generator From Array object
     * 
     * @tparam array array of audio data of the the type defined as class template parameter 
     * @param repeat number of repetions the array should be played (default 1), set to 0 for endless repeat.
     * @param setInactiveAtEnd  defines if the generator is set inactive when the array has played fully. Default is true.
     * @param startIndex  defines if the phase. Default is 0.
     */

    template  <size_t arrayLen> 
    GeneratorFromArray(int8_t(&array)[arrayLen], int repeat=0, bool setInactiveAtEnd=true, size_t startIndex=0) {
        TRACED();
        this->max_repeat = repeat;
        this->inactive_at_end = setInactiveAtEnd;
        this->sound_index = startIndex;
		 this->reverse = false;
        setArray(array, arrayLen);
    }
/*
    ~GeneratorFromArray(){
        if (owns_data){
            delete[] table;
        }
    }*/

    template  <int arrayLen> 
    void setArray(int8_t(&array)[arrayLen]){
        TRACED();
        setArray(array, arrayLen);
    }

    void setArray(int8_t*array, size_t size){
      this->table_length = size;
      this->table = array;
      LOGI("table_length: %d", (int)size);
    }
	
	virtual void end() override{
	is_running=false;
	
	
}

    virtual bool begin(AudioInfo info) override {
		
        return SoundGenerator<T>::begin(info);
		
    }
	void setMode(byte m){
		mode=m;
		
	}
	
	void setPBfinal(int pb_final){	
		index_final=pb_final;
	}
	
	void setPBfinalN(int pb_final){	
		index_final_n=pb_final;
	}
	

	
	int getPbFinal(){	
		return index_final;
	}
	
		int getPbFinalN(){	
		return index_final_n;
	}
	
	
		int getInc(){	
		//return t_ind;
		return index_increment;
	}
	
		int getPortaInc(){	
		//return t_ind;
		if (porta>0){
		return porta; }else {
			
		return index_increment;}}
	
    /// Starts the generation of samples
    bool begin() override {
      //TRACEI();
      SoundGenerator<T>::begin();
		newNote=true;
		    is_running = true;
		  rz=false;
	 

	   //Serial.println(rz);
      return true;
	 
    }
	
	bool trueBegin() {
		newNote=false;
		  rz=false;
	  if (reverse==false){
      sound_index = start_Index;} else {
		   sound_index = end_Index;
		  
	  }
      repeat_counter = 0;
      is_running = true;
		
		
		return true;
	}

	
 void setO2(SoundGenerator<T> *generator){
            o2=generator;
        }
    /// Provides a single sample
    T IRAM_ATTR readSample() override {
      // at end deactivate output
	  if (loop_mode==ONESHOT){
      if (sound_index > end_Index) {
        // LOGD("reset index - sound_index: %d, table_length: %d",sound_index,table_length);
        sound_index = 0;
        // deactivate when count has been used up
        if (max_repeat>=1 && ++repeat_counter>=max_repeat){
            LOGD("atEnd");
            this->is_running = false;
            if (inactive_at_end){
                this->active = false;
            }
        }
      }
	  
	  }
	  
	   if (loop_mode==REVERSE){
      if (sound_index < start_Index) {
        // LOGD("reset index - sound_index: %d, table_length: %d",sound_index,table_length);
        sound_index = end_Index;
        // deactivate when count has been used up
        if (max_repeat>=1 && ++repeat_counter>=max_repeat){
            LOGD("atEnd");
            this->is_running = false;
            if (inactive_at_end){
                this->active = false;
            }
        }
      }}
	  
	   if ((sound_index > loop_end)) {
		   if ((loop_mode==LOOP) || ((loop_mode==PINGPONG)&&(reverse==false))){
			   int div1 = (sound_index-loop_end);
			   int div2 = (loop_end-loop_start);
			   if (div1!=0 and div2!=0){
			   sound_index=loop_start+(div1%div2);
			   if (loop_mode==PINGPONG){
			   reverse=true;
			sound_index=loop_end-(div1%div2);
		   }
			   }
			   
		   }
		
		   offset=offseti;
		 
	   } 

	   if ((sound_index < loop_start)) {
		    if ((loop_mode==REVERSELOOP) || ((loop_mode==PINGPONG)&&(reverse==true))){
				 int div1 = (loop_start-sound_index);
			   int div2 = (loop_end-loop_start);
			   if (div1!=0 and div2!=0){
		  //sound_index=loop_end-((loop_start-sound_index)%(loop_end-loop_start));
		   sound_index=loop_end-(div1%div2);
		    if (loop_mode==PINGPONG){
			   reverse=false;
			 sound_index=loop_start+(div1%div2);
		   }
			   }
		  
	   }
	    
	   }
	   
	   if (sound_index<start_Index){
		   	 int div1 = (start_Index-sound_index);
			   int div2 = (end_Index-start_Index);
			   if (div1!=0 and div2!=0){
		   sound_index=end_Index-(div1%div2);
			   }
		   }
		if (sound_index>end_Index){
			 int div1 = (sound_index-end_Index);
			   int div2 = (end_Index-start_Index);
			   if (div1!=0 and div2!=0){
			sound_index=start_Index+(div1%div2);
			   }
			}
	   

      //LOGD("index: %d - active: %d", sound_index, this->active);
      int16_t result = 0;
      if (this->is_running) {
		  /*
		    int tmp=(sound_index+((pitch_bend*index_increment)>>12));
			  if ((tmp > loop_end)) {
				   tmp=loop_start+((tmp-loop_end)%(loop_end-loop_start));}
		  if ((tmp < loop_start)) {
		  tmp=loop_end-((loop_start-tmp)%(loop_end-loop_start));}
		  sound_index=tmp;*/
		  
		
		  if (porta_time>0){
		  	if (porta!=index_increment){
			if ((greaterPorta==true) and (index_increment>porta)){
				index_increment=porta;
				porta_time=0;
			}
			if ((greaterPorta==false) and (index_increment<porta)){
				index_increment=porta;
				porta_time=0;
			}
			super_inc=super_inc+step;
			index_increment=(super_inc)>>12;
		 } else {porta_time=0;}
		
		 } 
		  if (mode==1 ){
			 // int o2_indexb;
			//  if (invert==false){
				  int my_index=(sound_index>>SHIFTER)+offset;
			  int my_indexb;
			  int res_1=table[my_index]<<7;
			  
			   int res_2=table[my_index+1]<<7;
			 int frac=sound_index & 4095;
			  result=res_1+((frac*(res_2-res_1))>>12);
			  if (reverse==true){result*=-1;}
			 /*  //zero cross check osc 1
			   if (((prev_o1>0) && (result<=0)) || (prev_o1<0) && (result>=0)) {
           o2_index=o2i;
        } 
		prev_o1=result;
		
			
			 //int my_index=(sound_index>>SHIFTER)+offset;
			 
			 //zero cross check osc 2
			int in1=o2->readSample();
			if (((prev_o2>0) && (in1<=0)) || (prev_o2<0) && (in1>=0)) {
           o2_indexb=o2i;
        } 
		prev_o2=in1;*/
		int in1=o2->readSample();
		
			  result=(result*(127-o2_index))+(in1 * o2_index)>>7;
			  if (((prev_o2>0) && (result<=0)) || (prev_o2<0) && (result>=0)) {
				   zcd=true;
           o2_index=o2i;
        } 
		prev_o2=result;
			  //} else {
				//  result=((table[(sound_index>>SHIFTER)+offset]*(127-o2_index))-(o2->readSample() * o2_index))>>7;
				  
			 // }
			  //

			  }
		  if (mode==0) {

			  int my_index=(sound_index>>SHIFTER)+offset;
			 // int my_indexb;
			  int res_1=table[my_index]<<7;
			  //int res_2;
			  //if (reverse==false){
			 // if ((my_index+1)>(loop_end>>SHIFTER)+offset){res_2=0;}else{res_2=table[my_index+1]<<7;}} else {
				 //  if (my_index-1<loop_start>>SHIFTER){res_2=0;}else{res_2=table[my_index-1]<<7;} 
			 // }
			   int res_2=table[my_index+1]<<7;
			  
			 // int res_3=(res_1-res_2);
			  
		
			 //int frac=sound_index % 4096;
			 int frac=sound_index & 4095;
			  result=res_1+((frac*(res_2-res_1))>>12);
			  
			    if (((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) {zcd=true;}
		  }
			  
			 
			  
		 if (mode==2){
			 int in1=o2->readSample();
			 //int my_index=(sound_index>>SHIFTER)+offset;
			if (((prev_o2>0) && (in1<=0)) || (prev_o2<0) && (in1>=0)) {
           o2_index=o2i;
        } 
		prev_o2=in1;
			 int tmp1=((in1) * (o2_index));
			  //if (reverse==true){tmp1*=-1;}
			  int tmp=((sound_index)+(tmp1));
			  if ((tmp > loop_end)) {
			  //tmp=loop_end;}
				   tmp=loop_start+((tmp-loop_end)%(loop_end-loop_start));}
		  if ((tmp < loop_start)) {
		  //tmp=loop_start;}
		 tmp=loop_end-((loop_start-tmp)%(loop_end-loop_start));}
		   int my_index=(tmp>>SHIFTER)+offset;
			  int my_indexb;
			  int res_1=table[my_index]<<7;
			 
			   int res_2=table[my_index+1]<<7;
			    //int res_1=SIN1024_DATA[my_index];
			   //int res_2=SIN1024_DATA[my_index+1];
			 int frac=sound_index & 4095;
			 result=res_1+((frac*(res_2-res_1))>>12);
			 if (reverse==true){result*=-1;}
			 // result=table[(tmp>>SHIFTER)+offset];
			// result=SIN1024_DATA[my_index];
		  }
		
		
		int64_t ic2=((icache>>2)*(index_increment+pitch_bend>>2))>>13;
		tii=(index_increment+pitch_bend)+(ic2);
        if (reverse==false){
			
			sound_index+=tii;
			//sound_index+=index_increment*pow(2.00,(2.00*pitch_bend)/1524.00);
			} else {sound_index-=tii;
		//t_ind=(index_increment+pitch_bend)+(ic2);
		if (mode==0){result*=-1;}

		}
		
		
	 
		
      }
	   if ((((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) || (result==0 && prev_val[0]==0)) {zcd1=true;
if (newNote==true){

	  if (reverse==false){
      sound_index = start_Index;} else {
		   sound_index = end_Index;
		  
	  }
      repeat_counter = 0;
      is_running = true;
		
newNote=false;
}
	  
	  }
		//zcd=false;
		
		 //for (int i = 1; i < N; ++i) {
       // if (((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) {
       //    zcd=true;
      //  } 
           /* for (int l = 0; l <= 3; ++l) {
               if ((result<0) && (prev_val[l]==0) && (prev_val[3]>0)){
				   zcd=true;
				   break;
			   }
			  if ((result>0) && (prev_val[l]==0) && (prev_val[3]<0)){  
			  zcd=true;
				   break;
			  }
            }*/
        
		
		//o2_index=o2i;
					//if ((prev_val[0]>0) && (result<0)) {zcd=true;}
					//if ((prev_val[0]<0) && (result>0)) {zcd=true;}
					//if (zcd==true){offset=offseti;}
					//prev_val[4]=prev_val[3];
					//prev_val[3]=prev_val[2];
					//prev_val[2]=prev_val[1];
					//prev_val[1]=prev_val[0];
					prev_val[0]=result;
					if (rz==true){result=0;sound_index=loop_start;}
					//if (zcd==true){checkQueuedNote(voice_num);}
      return result;
}



void setRZ(bool rzv){
	
	rz=rzv;
}
    // step size the sound index is incremented (default = 1)
    void setIncrement(int inc){
        index_increment = inc;
		porta=inc;
		//icache=inc;
    }
	
	void setPorta(int inc){
		porta=inc;
		
		super_inc=index_increment<<12;
		if (porta>index_increment){greaterPorta=true;} else {greaterPorta=false;}
	}
	
	void setPortaTime(int16_t portatime){
		
		porta_time=portatime;
		step=(((porta)-(index_increment))<<12)/porta_time;
	}
	
	void setPB(int pb){
        pitch_bend=pb;
		//Serial.println("PB:");
		//Serial.print(pb);
    }
	
	int getPB(){
		return pitch_bend;
	}
	
	int getSoundIndex(){
		
		return sound_index;
	}
	
	void setOffset(int offs){
		
		offseti=offs;
	}
	
	
	void setOffsetX(int offs){
		offseti=offs;
		offset=offs;
	}
	void IRAM_ATTR add2Increment(int inc){
		
        index_increment = icache + inc;
    }
	
	void setLFM(int lfm){
		icache=lfm;
		
	}
	
	void setVoice(byte vn){
		voice_num=vn;
	}
	
	void setStart(int index){
        start_Index = index<<SHIFTER;
    }
	
	void setEnd(int index){
        end_Index = index<<SHIFTER;
    }
	
	void setLoopStart(int index){
        loop_start = index<<SHIFTER;
    }
	void setO2Index(int16_t ind){
		o2i=ind;
	}
	void setLoopEnd(int index){
        loop_end = index<<SHIFTER;
    }
	
	
	void setSoundIndex(int index){
		sound_index=index<<SHIFTER;
	}
	void setLoopMode(char index){
        loop_mode=index;
		switch (loop_mode){
			case REVERSE:
			reverse=true;
			sound_index = end_Index;
			break;
			case REVERSELOOP:
			reverse=true;
			sound_index = end_Index;
			break;
			default:
			reverse=false;
		}
		}
    

    

    // Similar like is active to check if the array is still playing.  
    bool isRunning() {
        return is_running;
    }
	uint8_t note;
  protected:
    int sound_index = 0;
	int end_Index = 32768;
	int start_Index = 0;
	int loop_start=0;
	int loop_end=87000;
	char loop_mode=0;
    int max_repeat = 0;
    int repeat_counter = 0;
	int pitch_bend=0;
	bool reverse=false;
    bool inactive_at_end;
    bool is_running = false;
    //bool owns_data = false;
	bool newNote = false;
    int8_t *table;
    size_t table_length = 0;
    int index_increment = 1;
	int icache=0; 
	SoundGenerator<int16_t> *o2;
	int32_t o2_index=64;
	int32_t o2i=64;
	byte mode=0;
	bool greaterPorta=false;
	bool rz=true;
	int porta;
	//int t_ind=0;
	int16_t porta_time;
	int super_inc;
	bool invert=false;
	int step;
	byte wait=0;
	int offset=0;
	int offseti=0;
	int prev_o2;
	//int prev_o1;
	bool zcd=false;
	bool zcd1=false;
	int16_t prev_val[4]={1,0,0,0};
	int index_final;
		int index_final_n;
	byte voice_num=0;
	int tii;
};






template <class T>
class GeneratorFromCArray : public SoundGenerator<T> {
  public:

    GeneratorFromCArray() = default;
    /**
     * @brief Construct a new Generator From Array object
     * 
     * @tparam array array of audio data of the the type defined as class template parameter 
     * @param repeat number of repetions the array should be played (default 1), set to 0 for endless repeat.
     * @param setInactiveAtEnd  defines if the generator is set inactive when the array has played fully. Default is true.
     * @param startIndex  defines if the phase. Default is 0.
     */

    template  <size_t arrayLen> 
    GeneratorFromCArray(const int8_t(&array)[arrayLen], int repeat=0, bool setInactiveAtEnd=true, size_t startIndex=0) {
        TRACED();
        this->max_repeat = repeat;
        this->inactive_at_end = setInactiveAtEnd;
        this->sound_index = startIndex;
		 this->reverse = false;
        setArray(array, arrayLen);
    }
/*
    ~GeneratorFromArray(){
        if (owns_data){
            delete[] table;
        }
    }*/

    template  <int arrayLen> 
    void setArray(const int8_t(&array)[arrayLen]){
        TRACED();
        setArray(array, arrayLen);
    }

    void setArray(const int8_t*array, size_t size){
      this->table_length = size;
      this->table = array;
      LOGI("table_length: %d", (int)size);
    }
	
	virtual void end() override{
	is_running=false;
	
	
}

    virtual bool begin(AudioInfo info) override {
		
        return SoundGenerator<T>::begin(info);
		
    }
	void setMode(byte m){
		mode=m;
		
	}
	
	void setPBfinal(int pb_final){	
		index_final=pb_final;
	}
	
	void setPBfinalN(int pb_final){	
		index_final_n=pb_final;
	}
	

	
	int getPbFinal(){	
		return index_final;
	}
	
		int getPbFinalN(){	
		return index_final_n;
	}
	
	
		int getInc(){	
		//return t_ind;
		return index_increment;
	}
	
		int getPortaInc(){	
		//return t_ind;
		if (porta>0){
		return porta; }else {
			
		return index_increment;}}
	
    /// Starts the generation of samples
    bool begin() override {
      //TRACEI();
      SoundGenerator<T>::begin();
		newNote=true;
		    is_running = true;
		  rz=false;
	 

	   //Serial.println(rz);
      return true;
	 
    }
	
	bool trueBegin() {
		newNote=false;
		  rz=false;
	  if (reverse==false){
      sound_index = start_Index;} else {
		   sound_index = end_Index;
		  
	  }
      repeat_counter = 0;
      is_running = true;
		
		
		return true;
	}

	
 void setO2(SoundGenerator<T> *generator){
            o2=generator;
        }
    /// Provides a single sample
    T IRAM_ATTR readSample() override {
      // at end deactivate output
	  if (loop_mode==ONESHOT){
      if (sound_index > end_Index) {
        // LOGD("reset index - sound_index: %d, table_length: %d",sound_index,table_length);
        sound_index = 0;
        // deactivate when count has been used up
        if (max_repeat>=1 && ++repeat_counter>=max_repeat){
            LOGD("atEnd");
            this->is_running = false;
            if (inactive_at_end){
                this->active = false;
            }
        }
      }
	  
	  }
	  
	   if (loop_mode==REVERSE){
      if (sound_index < start_Index) {
        // LOGD("reset index - sound_index: %d, table_length: %d",sound_index,table_length);
        sound_index = end_Index;
        // deactivate when count has been used up
        if (max_repeat>=1 && ++repeat_counter>=max_repeat){
            LOGD("atEnd");
            this->is_running = false;
            if (inactive_at_end){
                this->active = false;
            }
        }
      }}
	  
	   if ((sound_index > loop_end)) {
		   if ((loop_mode==LOOP) || ((loop_mode==PINGPONG)&&(reverse==false))){
			   int64_t div1 = (sound_index-loop_end);
			   int64_t div2 = (loop_end-loop_start);
			   if (div1!=0 and div2!=0){
			   sound_index=loop_start+(div1%div2);
			   if (loop_mode==PINGPONG){
			   reverse=true;
			sound_index=loop_end-(div1%div2);
		   }
			   }
			   
		   }
		
		   offset=offseti;
		 
	   } 

	   if ((sound_index < loop_start)) {
		    if ((loop_mode==REVERSELOOP) || ((loop_mode==PINGPONG)&&(reverse==true))){
				 int64_t div1 = (loop_start-sound_index);
			   int64_t div2 = (loop_end-loop_start);
			   if (div1!=0 and div2!=0){
		  //sound_index=loop_end-((loop_start-sound_index)%(loop_end-loop_start));
		   sound_index=loop_end-(div1%div2);
		    if (loop_mode==PINGPONG){
			   reverse=false;
			 sound_index=loop_start+(div1%div2);
		   }
			   }
		  
	   }
	    
	   }
	   
	   if (sound_index<start_Index){
		   	 int64_t div1 = (start_Index-sound_index);
			   int64_t div2 = (end_Index-start_Index);
			   if (div1!=0 and div2!=0){
		   sound_index=end_Index-(div1%div2);
			   }
		   }
		if (sound_index>end_Index){
			 int64_t div1 = (sound_index-end_Index);
			   int64_t div2 = (end_Index-start_Index);
			   if (div1!=0 and div2!=0){
			sound_index=start_Index+(div1%div2);
			   }
			}
	   

      //LOGD("index: %d - active: %d", sound_index, this->active);
      int16_t result = 0;
      if (this->is_running) {
		  /*
		    int tmp=(sound_index+((pitch_bend*index_increment)>>12));
			  if ((tmp > loop_end)) {
				   tmp=loop_start+((tmp-loop_end)%(loop_end-loop_start));}
		  if ((tmp < loop_start)) {
		  tmp=loop_end-((loop_start-tmp)%(loop_end-loop_start));}
		  sound_index=tmp;*/
		  
		
		  if (porta_time>0){
		  	if (porta!=index_increment){
			if ((greaterPorta==true) and (index_increment>porta)){
				index_increment=porta;
				porta_time=0;
			}
			if ((greaterPorta==false) and (index_increment<porta)){
				index_increment=porta;
				porta_time=0;
			}
			super_inc=super_inc+step;
			index_increment=(super_inc)>>12;
		 } else {porta_time=0;}
		
		 } 
		  if (mode==1 ){
			 // int o2_indexb;
			//  if (invert==false){
					int64_t true_sound_index= sound_index>>SHIFTER;
			  int64_t my_index=true_sound_index+offset;
			  int my_indexb;
			  int res_1=table[my_index]<<7;
			  
			   int res_2=table[my_index+1]<<7;
			  int64_t frac=sound_index % 4096;
			  result=res_1+((frac*(res_2-res_1))>>12);
			  if (reverse==true){result*=-1;}
			 /*  //zero cross check osc 1
			   if (((prev_o1>0) && (result<=0)) || (prev_o1<0) && (result>=0)) {
           o2_index=o2i;
        } 
		prev_o1=result;
		
			
			 //int my_index=(sound_index>>SHIFTER)+offset;
			 
			 //zero cross check osc 2
			int in1=o2->readSample();
			if (((prev_o2>0) && (in1<=0)) || (prev_o2<0) && (in1>=0)) {
           o2_indexb=o2i;
        } 
		prev_o2=in1;*/
		int in1=o2->readSample();
		
			  result=(result*(127-o2_index))+(in1 * o2_index)>>7;
			  if (((prev_o2>0) && (result<=0)) || (prev_o2<0) && (result>=0)) {
				   zcd=true;
           o2_index=o2i;
        } 
		prev_o2=result;
			  //} else {
				//  result=((table[(sound_index>>SHIFTER)+offset]*(127-o2_index))-(o2->readSample() * o2_index))>>7;
				  
			 // }
			  //

			  }
		  if (mode==0) {
			int64_t true_sound_index= sound_index>>SHIFTER;
			  int64_t my_index=true_sound_index+offset;
			 // int my_indexb;
			  int res_1=table[my_index]<<7;
			  //int res_2;
			  //if (reverse==false){
			 // if ((my_index+1)>(loop_end>>SHIFTER)+offset){res_2=0;}else{res_2=table[my_index+1]<<7;}} else {
				 //  if (my_index-1<loop_start>>SHIFTER){res_2=0;}else{res_2=table[my_index-1]<<7;} 
			 // }
			   int res_2=table[my_index+1]<<7;
			  
			 int64_t frac=sound_index % 4096;
			  result=res_1+((frac*(res_2-res_1))>>12);
			  
			    if (((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) {zcd=true;}
		  }
			  
			 
			  
		 if (mode==2){
			 int in1=o2->readSample();
			 //int my_index=(sound_index>>SHIFTER)+offset;
			if (((prev_o2>0) && (in1<=0)) || (prev_o2<0) && (in1>=0)) {
           o2_index=o2i;
        } 
		prev_o2=in1;
			 int tmp1=((in1) * (o2_index));
			  //if (reverse==true){tmp1*=-1;}
			  int64_t tmp=((sound_index)+(int64_t)(tmp1));
			  if ((tmp > loop_end)) {
			  //tmp=loop_end;}
				   tmp=loop_start+((tmp-loop_end)%(loop_end-loop_start));}
		  if ((tmp < loop_start)) {
		  //tmp=loop_start;}
		 tmp=loop_end-((loop_start-tmp)%(loop_end-loop_start));}
		   int64_t true_sound_index= tmp>>SHIFTER;
			  int64_t my_index=true_sound_index+offset;
			  int my_indexb;
			  int res_1=table[my_index]<<7;
			 
			   int res_2=table[my_index+1]<<7;
			    //int res_1=SIN1024_DATA[my_index];
			   //int res_2=SIN1024_DATA[my_index+1];
			 int64_t frac=sound_index % 4096;
			 result=res_1+((frac*(res_2-res_1))>>12);
			 if (reverse==true){result*=-1;}
			 // result=table[(tmp>>SHIFTER)+offset];
			// result=SIN1024_DATA[my_index];
		  }
		
		
		int64_t ic2=((icache>>2)*(index_increment+pitch_bend>>2))>>13;
        if (reverse==false){
			
			sound_index+=(index_increment+pitch_bend)+(ic2);
			//sound_index+=index_increment*pow(2.00,(2.00*pitch_bend)/1524.00);
			} else {sound_index-=(index_increment+pitch_bend)+ic2;
		//t_ind=(index_increment+pitch_bend)+(ic2);
		if (mode==0){result*=-1;}

		}
		
		
	 
		
      }
	   if ((((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) || (result==0 && prev_val[0]==0)) {zcd1=true;
if (newNote==true){

	  if (reverse==false){
      sound_index = start_Index;} else {
		   sound_index = end_Index;
		  
	  }
      repeat_counter = 0;
      is_running = true;
		
newNote=false;
}
	  
	  }
		//zcd=false;
		
		 //for (int i = 1; i < N; ++i) {
       // if (((prev_val[0]>0) && (result<=0)) || (prev_val[0]<0) && (result>=0)) {
       //    zcd=true;
      //  } 
           /* for (int l = 0; l <= 3; ++l) {
               if ((result<0) && (prev_val[l]==0) && (prev_val[3]>0)){
				   zcd=true;
				   break;
			   }
			  if ((result>0) && (prev_val[l]==0) && (prev_val[3]<0)){  
			  zcd=true;
				   break;
			  }
            }*/
        
		
		//o2_index=o2i;
					//if ((prev_val[0]>0) && (result<0)) {zcd=true;}
					//if ((prev_val[0]<0) && (result>0)) {zcd=true;}
					//if (zcd==true){offset=offseti;}
					//prev_val[4]=prev_val[3];
					//prev_val[3]=prev_val[2];
					//prev_val[2]=prev_val[1];
					//prev_val[1]=prev_val[0];
					prev_val[0]=result;
					if (rz==true){result=0;sound_index=loop_start;}
					//if (zcd==true){checkQueuedNote(voice_num);}
      return result;
}



void setRZ(bool rzv){
	
	rz=rzv;
}
    // step size the sound index is incremented (default = 1)
    void setIncrement(int inc){
        index_increment = inc;
		porta=inc;
		//icache=inc;
    }
	
	void setPorta(int inc){
		porta=inc;
		
		super_inc=index_increment<<12;
		if (porta>index_increment){greaterPorta=true;} else {greaterPorta=false;}
	}
	
	void setPortaTime(int16_t portatime){
		
		porta_time=portatime;
		step=(((porta)-(index_increment))<<12)/porta_time;
	}
	
	void setPB(int pb){
        pitch_bend=pb;
		//Serial.println("PB:");
		//Serial.print(pb);
    }
	
	int getPB(){
		return pitch_bend;
	}
	
	int getSoundIndex(){
		
		return sound_index;
	}
	
	void setOffset(int offs){
		
		offseti=offs;
	}
	
	
	void setOffsetX(int offs){
		offseti=offs;
		offset=offs;
	}
	void IRAM_ATTR add2Increment(int inc){
		
        index_increment = icache + inc;
    }
	
	void setLFM(int lfm){
		icache=lfm;
		
	}
	
	void setVoice(byte vn){
		voice_num=vn;
	}
	
	void setStart(int index){
        start_Index = (uint64_t)index<<(uint64_t)SHIFTER;
    }
	
	void setEnd(int index){
        end_Index = (uint64_t)index<<SHIFTER;
    }
	
	void setLoopStart(int index){
        loop_start = (uint64_t)index<<SHIFTER;
    }
	void setO2Index(int16_t ind){
		o2i=ind;
	}
	void setLoopEnd(int index){
        loop_end = (uint64_t)index<<SHIFTER;
    }
	
	
	void setSoundIndex(int index){
		sound_index=index<<SHIFTER;
	}
	void setLoopMode(char index){
        loop_mode=index;
		switch (loop_mode){
			case REVERSE:
			reverse=true;
			sound_index = end_Index;
			break;
			case REVERSELOOP:
			reverse=true;
			sound_index = end_Index;
			break;
			default:
			reverse=false;
		}
		}
    

    

    // Similar like is active to check if the array is still playing.  
    bool isRunning() {
        return is_running;
    }
	uint8_t note;
  protected:
    uint64_t sound_index = 0;
	uint64_t end_Index = 32768;
	uint64_t start_Index = 0;
	uint64_t loop_start=0;
	uint64_t loop_end=87000;
	char loop_mode=0;
    int max_repeat = 0;
    int repeat_counter = 0;
	int pitch_bend=0;
	bool reverse=false;
    bool inactive_at_end;
    bool is_running = false;
    //bool owns_data = false;
	bool newNote = false;
    const int8_t *table;
    size_t table_length = 0;
    int index_increment = 1;
	int icache=0; 
	SoundGenerator<int16_t> *o2;
	int32_t o2_index=64;
	int32_t o2i=64;
	byte mode=0;
	bool greaterPorta=false;
	bool rz=true;
	int porta;
	//int t_ind=0;
	int16_t porta_time;
	int super_inc;
	bool invert=false;
	int step;
	byte wait=0;
	int offset=0;
	int offseti=0;
	int prev_o2;
	//int prev_o1;
	bool zcd=false;
	bool zcd1=false;
	int16_t prev_val[4]={1,0,0,0};
	uint64_t index_final;
		uint64_t index_final_n;
	byte voice_num=0;
	
};








/**
 * @brief Just returns a constant value
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * @tparam T 
 */
template <class T>
class GeneratorFixedValue : public SoundGenerator<T> {
  public:

    GeneratorFixedValue() = default;

    virtual bool begin(AudioInfo info) {
        return SoundGenerator<T>::begin(info);
    }

    void setValue(T value){
        value_set = value;
    }

    /// Starts the generation of samples
    bool begin() override {
      TRACEI();
      SoundGenerator<T>::begin();
      is_running = true;
      value_return = value_set;
      return true;
    }

    /// Provides a single sample
    T readSample() override {
        return value_return;
    }

    // Similar like is active to check if the array is still playing.  
    bool isRunning() {
        return is_running;
    }

  protected:
    T value_set = 0;
    T value_return = 0;
    bool is_running = false;
};

/**
 * @brief A sine generator based on a table. The table is created based using degrees where one full wave is 360 degrees.
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
template <class T>
class SineFromTable  : public SoundGenerator<T> {
    public:
        SineFromTable(float amplitude = 32767.0){
            this->amplitude = amplitude;
            this->amplitude_to_be = amplitude;
        }

        /// Defines the new amplitude (volume)
        void setAmplitude(float amplitude){
            this->amplitude_to_be = amplitude;
        }

        /// To avoid pops we do not allow to big amplitude changes at once and spread them over time
        void setMaxAmplitudeStep(float step){
            max_amplitude_step = step;
        }

        T readSample() {
            // update angle
            angle += step;
            if (angle >= 360){
               while(angle>=360.0){
                angle -= 360.0; 
               }
               // update frequency at start of circle (near 0 degrees)
               step = step_new;

               updateAmplitudeInSteps();
               //amplitude = amplitude_to_be;
            }
            return interpolate(angle);
        }

        bool begin() {
            is_first = true;
            SoundGenerator<T>::begin();
            base_frequency = SoundGenerator<T>::audioInfo().sample_rate / 360.0; //122.5 hz (at 44100); 61 hz (at 22050)
            return true;
        }

        bool begin(AudioInfo info, float frequency) {
            SoundGenerator<T>::begin(info);
            base_frequency = SoundGenerator<T>::audioInfo().sample_rate / 360.0; //122.5 hz (at 44100); 61 hz (at 22050)
            setFrequency(frequency);
            return true;
        }

        bool begin(int channels, int sample_rate, uint16_t frequency=0){
            SoundGenerator<T>::info.channels  = channels;
            SoundGenerator<T>::info.sample_rate = sample_rate;
            return begin(SoundGenerator<T>::info, frequency);
        }

        void setFrequency(float freq) {
            step_new = freq / base_frequency;
            if (is_first){
                step = step_new;
                is_first = false;
            }
            LOGD("step: %f", step_new);
        }

    protected:
        bool is_first = true;
        float amplitude;
        float amplitude_to_be;
        float max_amplitude_step = 50;
        float base_frequency = 1.0;
        float step = 1.0; 
        float step_new = 1.0; 
        float angle = 0;
        //122.5 hz (at 44100); 61 hz (at 22050)
        const float values[181] = {0, 0.0174524, 0.0348995, 0.052336, 0.0697565, 0.0871557, 0.104528, 0.121869, 0.139173, 0.156434, 0.173648, 0.190809, 0.207912, 0.224951, 0.241922, 0.258819, 0.275637, 0.292372, 0.309017, 0.325568, 0.34202, 0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.45399, 0.469472, 0.48481, 0.5, 0.515038, 0.529919, 0.544639, 0.559193, 0.573576, 0.587785, 0.601815, 0.615661, 0.62932, 0.642788, 0.656059, 0.669131, 0.681998, 0.694658, 0.707107, 0.71934, 0.731354, 0.743145, 0.75471, 0.766044, 0.777146, 0.788011, 0.798636, 0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.87462, 0.882948, 0.891007, 0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.93358, 0.939693, 0.945519, 0.951057, 0.956305, 0.961262, 0.965926, 0.970296, 0.97437, 0.978148, 0.981627, 0.984808, 0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.99863, 0.999391, 0.999848, 1, 0.999848, 0.999391, 0.99863, 0.997564, 0.996195, 0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627, 0.978148, 0.97437, 0.970296, 0.965926, 0.961262, 0.956305, 0.951057, 0.945519, 0.939693, 0.93358, 0.927184, 0.920505, 0.913545, 0.906308, 0.898794, 0.891007, 0.882948, 0.87462, 0.866025, 0.857167, 0.848048, 0.838671, 0.829038, 0.819152, 0.809017, 0.798636, 0.788011, 0.777146, 0.766044, 0.75471, 0.743145, 0.731354, 0.71934, 0.707107, 0.694658, 0.681998, 0.669131, 0.656059, 0.642788, 0.62932, 0.615661, 0.601815, 0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038, 0.5, 0.48481, 0.469472, 0.45399, 0.438371, 0.422618, 0.406737, 0.390731, 0.374607, 0.358368, 0.34202, 0.325568, 0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951, 0.207912, 0.190809, 0.173648, 0.156434, 0.139173, 0.121869, 0.104528, 0.0871557, 0.0697565, 0.052336, 0.0348995, 0.0174524, 0}; 

        T interpolate(float angle){
            bool positive = (angle<=180);
            float angle_positive = positive ? angle : angle - 180.0f;
            int angle_int1 = angle_positive;
            int angle_int2 = angle_int1+1;
            T v1 = values[angle_int1] * amplitude;
            T v2 = values[angle_int2] * amplitude;
            T result = v1 < v2 ? map(angle_positive,angle_int1,angle_int2,v1, v2) : map(angle_positive,angle_int1,angle_int2,v2, v1)  ;
            //float result = v1;
            return positive ? result : -result;
        }

        T map(T x, T in_min, T in_max, T out_min, T out_max) {
          return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        void updateAmplitudeInSteps() {
            float diff = amplitude_to_be - amplitude; 
            if (abs(diff) > max_amplitude_step){
                diff = (diff<0) ? -max_amplitude_step : max_amplitude_step;
            } 
            if (abs(diff)>=1.0f){
                amplitude += diff;
            }
        }
};

/**
 * @brief Mixer which combines multiple sound generators into one output
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * @tparam T 
 */
 #define BLEND 0
 #define AM 1
template <class T>
class GeneratorMixer : public SoundGenerator<T> {
    public:
        GeneratorMixer() = default;

        void add(SoundGenerator<T> &generator){
            vector.push_back(&generator);
        }
        void add(SoundGenerator<T> *generator){
            vector.push_back(generator);
        }
		void setIndex(byte ind){
			index=ind;
			
		}
		
		void setMode(byte mo){
			mode=mo;
			if (mode==AM){
				shift=22;
			} else {shift=8;}
		}
        void clear() {
            vector.clear();
        }

        T readSample() {
            T result;
			int tmp=0;
            //int count = 0;
            
				if (mode==BLEND){
					
					//tmp += vector[0]->readSample() * (index);
				
					//tmp += vector[1]->readSample()*(127-index);
					tmp += vector[0]->readSample();
				
					tmp += vector[1]->readSample();
				} else {
					
					tmp = ((vector[0]->readSample())*127)*((32767-index*255)+((vector[1]->readSample())*index));}
					
				
                
                    
                
            
            result = tmp>>shift;
            return result;
        }

    protected:
        Vector<SoundGenerator<T>*> vector;
        int actualChannel=0;
		byte mode=BLEND;
		byte index=127;
		byte shift=1;
};

/**
 * @brief Generates a test signal which is easy to check because the values are incremented or decremented by 1
 * @ingroup generator
 * @author Phil Schatzmann
 * @copyright GPLv3
 * @tparam T 
 */
template <class T>
class TestGenerator : public SoundGenerator<T>{
    public:
        TestGenerator(T max=1000, T inc=1){
            this->max=max;
        }

        T readSample() override {
            value += inc;
            if (abs(value)>=max){
                inc = -inc;
                value += (inc * 2);
            }
            return value;
        }

    protected:
        T max;
        T value=0;
        T inc=1;

};

}
