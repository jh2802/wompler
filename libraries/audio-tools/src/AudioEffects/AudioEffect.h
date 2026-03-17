#pragma once
#include "AudioEffects/AudioParameters.h"
#include "AudioEffects/PitchShift.h"
#include "AudioTools/AudioLogger.h"
#include <stdint.h>
#define FX_SHIFT 8
#define SHIFTED_1 ((uint8_t)255)
#define LOWPASS 0
#define HIPASS 1
namespace audio_tools {

// we use int16_t for our effects
typedef int16_t effect_t;

/**
 * @brief Abstract Base class for Sound Effects
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class AudioEffect {
public:
  AudioEffect() = default;
  virtual ~AudioEffect() = default;

  /// calculates the effect output from the input
  virtual effect_t process(effect_t in) = 0;

  /// sets the effect active/inactive
  virtual void setActive(bool value) { active_flag = value; }

  /// determines if the effect is active
  virtual bool active() { return active_flag; }

  virtual AudioEffect *clone() = 0;

  /// Allows to identify an effect
  int id() { return id_value; }

  /// Allows to identify an effect
  void setId(int id) { this->id_value = id; }

protected:
  bool active_flag = true;
  int id_value = -1;

  void copyParent(AudioEffect *copy) {
    id_value = copy->id_value;
    active_flag = copy->active_flag;
  }

  /// generic clipping method
  int16_t clip(int32_t in, int16_t clipLimit = 32767,
               int16_t resultLimit = 32767) {
    int32_t result = in;
    if (result > clipLimit) {
      result = resultLimit;
    }
    if (result < -clipLimit) {
      result = -resultLimit;
    }
    return result;
  }
};

/**
 * @brief Boost AudioEffect
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 *
 */

class Boost : public AudioEffect {
public:
  /// Boost Constructor: volume 0.1 - 1.0: decrease result; volume >0: increase
  /// result
  Boost(float volume = 1.0) { effect_value = volume; }

  Boost(const Boost &copy) = default;

  float volume() { return effect_value; }

  void setVolume(float volume) { effect_value = volume; }

  effect_t process(effect_t input) {
    if (!active())
      return input;
    int32_t result = effect_value * input;
    // clip to int16_t
    return clip(result);
  }

  Boost *clone() { return new Boost(*this); }

protected:
  float effect_value;
};

/**
 * @brief Distortion AudioEffect
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class Distortion : public AudioEffect {
public:
  /// Distortion Constructor: e.g. use clipThreashold 4990 and maxInput=6500
  Distortion(int16_t clipThreashold = 4990, int16_t maxInput = 6500) {
    p_clip_threashold = clipThreashold;
    max_input = maxInput;
  }

  Distortion(const Distortion &copy) = default;

  void setClipThreashold(int16_t th) { p_clip_threashold = th; }

  int16_t clipThreashold() { return p_clip_threashold; }

  void setMaxInput(int16_t maxInput) { max_input = maxInput; }

  int16_t maxInput() { return max_input; }

  effect_t process(effect_t input) {
    if (!active())
      return input;
    // the input signal is 16bits (values from -32768 to +32768
    // the value of input is clipped to the distortion_threshold value
    return clip(input, p_clip_threashold, max_input);
  }

  Distortion *clone() { return new Distortion(*this); }

protected:
  int16_t p_clip_threashold;
  int16_t max_input;
};

/**
 * @brief Fuzz AudioEffect
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class Fuzz : public AudioEffect {
public:
  /// Fuzz Constructor: use e.g. effectValue=6.5; maxOut = 300
  Fuzz(float fuzzEffectValue = 6.5, uint16_t maxOut = 300) {
    p_effect_value = fuzzEffectValue;
    max_out = maxOut;
  }

  Fuzz(const Fuzz &copy) = default;

  void setFuzzEffectValue(float v) { p_effect_value = v; }

  float fuzzEffectValue() { return p_effect_value; }

  void setMaxOut(uint16_t v) { max_out = v; }

  uint16_t maxOut() { return max_out; }

  effect_t process(effect_t input) {
    if (!active())
      return input;
    float v = p_effect_value;
    int32_t result = clip(v * input);
    return map(result * v, -32768, +32767, -max_out, max_out);
  }

  Fuzz *clone() { return new Fuzz(*this); }

protected:
  float p_effect_value;
  uint16_t max_out;
};

/**
 * @brief Tremolo AudioEffect
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class Tremolo : public AudioEffect {
public:
  /// Tremolo constructor -  use e.g. duration_ms=2000; depthPercent=50;
  /// sampleRate=44100
  Tremolo(int16_t duration_ms = 2000, uint8_t depthPercent = 50,
          uint32_t sampleRate = 44100) {
    this->duration_ms = duration_ms;
    this->sampleRate = sampleRate;
    this->p_percent = depthPercent;
    int32_t rate_count = sampleRate * duration_ms / 1000;
    rate_count_half = rate_count / 2;
  }

  Tremolo(const Tremolo &copy) = default;

  void setDuration(int16_t ms) {
    int32_t rate_count = sampleRate * ms / 1000;
    rate_count_half = rate_count / 2;
  }

  int16_t duration() { return duration_ms; }

  void setDepth(uint8_t percent) { p_percent = percent; }

  uint8_t depth() { return p_percent; }

  effect_t process(effect_t input) {
    if (!active())
      return input;

    // limit value to max 100% and calculate factors
    float tremolo_depth = p_percent > 100 ? 1.0 : 0.01 * p_percent;
    float signal_depth = (100.0 - p_percent) / 100.0;

    float tremolo_factor = tremolo_depth / rate_count_half;
    int32_t out = (signal_depth * input) + (tremolo_factor * count * input);

    // saw tooth shaped counter
    count += inc;
    if (count >= rate_count_half) {
      inc = -1;
    } else if (count <= 0) {
      inc = +1;
    }

    return clip(out);
  }

  Tremolo *clone() { return new Tremolo(*this); }

protected:
  int16_t duration_ms;
  uint32_t sampleRate;
  int32_t count = 0;
  int16_t inc = 1;
  int32_t rate_count_half; // number of samples for on raise and fall
  uint8_t p_percent;
};

/**
 * @brief Delay/Echo AudioEffect. See
 * https://wiki.analog.com/resources/tools-software/sharc-audio-module/baremetal/delay-effect-tutorial
 * Howver the dry value and wet value were replace by the depth parameter.
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
class Delay : public AudioEffect {
public:
  /// e.g. depth=0.5, ms=1000, sampleRate=44100
  Delay(uint16_t duration_ms = 1000, float depth = 0.5,
        float feedbackAmount = 1.0, uint32_t sampleRate = 44100,
        bool zeroIfBufferEmpty = false) {
    setSampleRate(sampleRate);
    setFeedback(feedbackAmount);
    setDepth(depth);
    setDuration(duration_ms);
  }

  Delay(const Delay &copy) {
    setSampleRate(copy.sampleRate);
    setFeedback(copy.feedback);
    setDepth(copy.depth);
    setDuration(copy.duration);
  };

  void setDuration(int16_t dur) {
    duration = dur;
    updateBufferSize();
  }

  int16_t getDuration() { return duration; }

  void setDepth(float value) {
    depth = value;
    if (depth > 1.0)
      depth = 1.0;
    if (depth < 0)
      depth = 0.0;
  }

  float getDepth() { return depth; }

  void setFeedback(float feed) {
    feedback = feed;
    if (feedback > 1.0)
      feedback = 1.0;
    if (feedback < 0)
      feedback = 0.0;
  }

  float getFeedback() { return feedback; }

  void setSampleRate(int32_t sample) {
    sampleRate = sample;
    updateBufferSize();
  }

  float getSampleRate() { return sampleRate; }

  effect_t process(effect_t input) {
    if (!active())
      return input;

    // Read last audio sample in each delay line
    int32_t delayed_value = buffer[delay_line_index];

    // Mix the above with current audio and write the results back to output
    int32_t out = ((1.0 - depth) * input) + (depth * delayed_value);

    // Update each delay line
    buffer[delay_line_index] = clip(feedback * (delayed_value + input));

    // Finally, update the delay line index
    if (delay_line_index++ >= delay_len_samples) {
      delay_line_index = 0;
    }
    return clip(out);
  }

  Delay *clone() { return new Delay(*this); }

protected:
  Vector<effect_t> buffer{0};
  float feedback = 0.0, duration = 0.0, sampleRate = 0.0, depth = 0.0;
  size_t delay_len_samples = 0;
  size_t delay_line_index = 0;

  void updateBufferSize() {
    if (sampleRate > 0 && duration > 0) {
      size_t newSampleCount = sampleRate * duration / 1000;
      if (newSampleCount != delay_len_samples) {
        delay_len_samples = newSampleCount;
        buffer.resize(delay_len_samples);
        memset(buffer.data(),0,delay_len_samples*sizeof(effect_t));
        LOGD("sample_count: %u", (unsigned)delay_len_samples);
      }
    }
  }
};

/**
 * @brief ADSR Envelope: Attack, Decay, Sustain and Release.
 * Attack is the time taken for initial run-up oeffect_tf level from nil to
 peak, beginning when the key is pressed.
 * Decay is the time taken for the subsequent run down from the attack level to
 the designated sustainLevel level.
 * Sustain is the level during the main sequence of the sound's duration, until
 the key is released.
 * Release is the time taken for the level to decay from the sustainLevel level
 to zero after the key is released.[4]
 * @ingroup effects
 * @author Phil Schatzmann
 * @copyright GPLv3
 */

class ADSRGain : public AudioEffect {
public:
  ADSRGain(float attack = 0.001, float decay = 0.001, float sustainLevel = 0.5,
           float release = 0.005, float boostFactor = 1.0) {
    this->factor = boostFactor;
    adsr = new ADSR(attack, decay, sustainLevel, release);
  }

  ADSRGain(const ADSRGain &ref) {
    adsr = new ADSR(*(ref.adsr));
    factor = ref.factor;
    copyParent((AudioEffect *)&ref);
  };

  virtual ~ADSRGain() { delete adsr; }

  void setAttackRate(float a) { adsr->setAttackRate(a); }

  float attackRate() { return adsr->attackRate(); }

  void setDecayRate(float d) { adsr->setDecayRate(d); }

  float decayRate() { return adsr->decayRate(); }

  void setSustainLevel(float s) { adsr->setSustainLevel(s); }

  float sustainLevel() { return adsr->sustainLevel(); }

  void setReleaseRate(float r) { adsr->setReleaseRate(r); }

  float releaseRate() { return adsr->releaseRate(); }

  void keyOn(float tgt = 0) { adsr->keyOn(tgt); }

  void keyOff() { adsr->keyOff(); }

  effect_t process(effect_t input) {
    effect_t result = factor * adsr->tick() * input;
    return result;
  }

  bool isActive() { return adsr->isActive(); }

  ADSRGain *clone() { return new ADSRGain(*this); }

protected:
  ADSR *adsr;
  float factor;
};

class ADSRGainXP : public AudioEffect {
public:
  ADSRGainXP(int16_t attack = 1, int16_t decay = 1, int16_t sustainLevel = 4096,
           int16_t release = 1, int16_t boostFactor = 32767) {
   // this->factor = boostFactor;
    adsrxp = new ADSRxp(attack, decay, sustainLevel, release);
  }

  ADSRGainXP(const ADSRGainXP &ref) {
    adsrxp = new ADSRxp(*(ref.adsrxp));
    //factor = ref.factor;
    copyParent((AudioEffect *)&ref);
  };
	
  virtual ~ADSRGainXP() { delete adsrxp; }
  
  //void setPartial(char partial) {adsrxp->setPartial(partial);}
	void setVolume(int16_t vol){volumeb=vol;}
  void setAttackRate(int16_t a) { adsrxp->setAttackRate(a<<4);if (a==255){adsrxp->setAttackRate(32768);} }

  int16_t attackRate() { return adsrxp->attackRate(); }

  void setDecayRate(int16_t d) { adsrxp->setDecayRate(d<<4); if (d==255){adsrxp->setDecayRate(2097152);}}

  int16_t decayRate() { return adsrxp->decayRate(); }

  void setSustainLevel(int32_t s) { adsrxp->setSustainLevel(s); }
  
 

  int16_t sustainLevel() { return adsrxp->sustainLevel(); }

  void setReleaseRate(int16_t r) { adsrxp->setReleaseRate(r<<4); if (r==255){adsrxp->setReleaseRate(131072);};o_release=r; divider=1;}
  
    void setReleaseQN(int16_t r,byte pitch,byte velo,byte partial) { 
	
	 
	
		divider=1;
	adsrxp->setReleaseRate(9000); 
	qn_pitch=pitch;
	qn_velo=velo;
	qn_partial=partial;
	//adsrxp->keyOff();
	cache = (adsrxp->tick())>>8;
	factor=prev_val;
	}

  int16_t releaseRate() { return adsrxp->releaseRate(); }

  void keyOn(int32_t tgt = 0) { adsrxp->setMax(tgt);adsrxp->keyOn(tgt);divider=1;  cache = (adsrxp->tick())>>8;m_osc->begin();qn_pitch=0;cacheb=cache;volume=volumeb;}

  void keyOff() { adsrxp->keyOff(); }
  
  //void forceInactive(){};

  effect_t process(effect_t input) {
	 // adsrxp->tick();
	  
	  	zcd=false;
				
	  wait++;
	 if (wait==divider){
	 

			
					cache = (adsrxp->tick())>>8;
	 
	 wait=0;
	if (isActive()!=true) {
		m_osc->end();
	}
	
	 } 
	
	  
					
	   // if (qn_pitch>0){input=factor;cacheb=cache;}
		    int64_t calc=(((cacheb * input)>>14)*volume)>>14;
	 //if ( adsrxp->isActive()){Serial.println(calc);};
	   effect_t result = calc;
	   //if (((prev_val>0) && (result<=0)) || (prev_val<0) && (result>=0)) {
		    if (qn_velo>0 && getKeystate(qn_pitch)==true){
		   // if (zcd==true){
				//divider=1;
				adsrxp->setReleaseRate(9000); 
		  adsrxp->keyOff();
		    setQueuedNote(qn_partial,qn_pitch,qn_velo);
		//qn_pitch=0;
		   qn_velo=0;
		  
		
		cache = (adsrxp->tick())>>8;
		 //setQueuedNote(qn_partial,qn_pitch,qn_velo);
		//qn_pitch=0;
		   //qn_velo=0;
	  // }
	} else if (qn_velo>0) {
		qn_velo=0;
		//qn_partial=0;
		//qn_pitch=0;
		adsrxp->setReleaseRate(o_release);
		divider=1;
	}
		 
	   	if ((prev_val>0) && (input<0)) {zcd=true;}
					if ((prev_val<0) && (input>0)) {zcd=true;}
					if (zcd==true){volume=volumeb;cacheb=cache;};
					  calc=(((cacheb * input)>>14)*volume)>>14;
					  result = calc;
	prev_val=input;
	   
    return result;
	//return input;
  }

void setOsc(SoundGenerator<int16_t> *generator){m_osc=generator;}
  bool isActive() { return adsrxp->isActive(); }
 bool isRel() { return adsrxp->isRel(); }
  ADSRGainXP *clone() { return new ADSRGainXP(*this); }

protected:
  ADSRxp *adsrxp;
  int32_t factor;
  int32_t cache=0;
  char wait=0;
  int16_t volume;
  int16_t volumeb;
int16_t prev_val=0;
bool zcd=false;
  byte qn_pitch=0;
  byte qn_velo=0;
  byte qn_partial=0;
  byte divider=1;
  int32_t cacheb;
  SoundGenerator<int16_t> *m_osc;
  int16_t o_release;
 
};


class ADSRFilter : public AudioEffect {
public:
  ADSRFilter(int16_t attack = 1, int16_t decay = 1, int16_t sustainLevel = 4096,
           int16_t release = 1) {
    //this->factor = boostFactor;
    adsrxp = new ADSRxp(attack, decay, sustainLevel, release);
  }

  ADSRFilter(const ADSRFilter &ref) {
    adsrxp = new ADSRxp(*(ref.adsrxp));
    factor = ref.factor;
    copyParent((AudioEffect *)&ref);
  };

  virtual ~ADSRFilter() { delete adsrxp; }
  
  void setType(byte typ) { type=typ; }

 void setAttackRate(int16_t a) { adsrxp->setAttackRate(a);if (a==255){adsrxp->setAttackRate(16384);} }

  int16_t attackRate() { return adsrxp->attackRate(); }

  void setDecayRate(int16_t d) { adsrxp->setDecayRate(d); if (d==255){adsrxp->setDecayRate(16384);}}

  int16_t decayRate() { return adsrxp->decayRate(); }

  void setSustainLevel(int16_t s) { adsrxp->setSustainLevel(s); }

  int16_t sustainLevel() { return adsrxp->sustainLevel(); }

  void setReleaseRate(int16_t r) { adsrxp->setReleaseRate(r); if (r==255){adsrxp->setReleaseRate(16384);};}

  void keyOn(int16_t tgt = 0) { adsrxp->keyOn(tgt); }

  void keyOff() { adsrxp->keyOff();}

  effect_t process(effect_t input) {
	 // return input;
	  
	 if (wait==64){
	 f=constrain((adsrxp->tick()>>7) + ucutoff,0,255);
	
	 fb = (q) + ucfxmul(q, SHIFTED_1 - f);
	 wait=0;
	 } 
	 wait++;
	 effect_t result;
	 switch (type){
		 case LOWPASS:
  result = dcBlock(nextLP(input));
  break;
  case HIPASS:
  result = nextHP(input);
  break;		
  }
  
  
    return result;
	//return input<<8;
  }

  bool isActive() { return adsrxp->isActive(); }

  ADSRFilter *clone() { return new ADSRFilter(*this); }
  
  void setCutoffFreq(int8_t cutoff)
	{
    ucutoff = cutoff;
	f=constrain((adsrxp->tick()>>7) + ucutoff,0,255);
    fb = q + ucfxmul(q, SHIFTED_1 - cutoff);
  }
  
  void setCenterFreq(int cutoff)
	{
		
    f = constrain(cutoff+width,0,255);
	f2 = constrain(cutoff-width,0,255);
    fb = q + ucfxmul(q, SHIFTED_1 - f);
	fb2 = q + ucfxmul(q, SHIFTED_1 - f2);
  }
void setType(char ftype){
	type=ftype;
}
void setWidth(uint8_t w) {width=w;}

  /** deprecated.  Use setCutoffFreqAndResonance(uint8_t cutoff, uint8_t
  resonance).

  Set the resonance. If you hear unwanted distortion, back off the resonance.
  After setting resonance, you need to call setCuttoffFreq() to hear the change!
  @param resonance in the range 0-255, with 255 being most resonant.
  @note	Remember to call setCuttoffFreq() after resonance is changed!
  */
  void setResonance(uint8_t resonance) { q = resonance; }

  /**
  Set the cut off frequency and resonance.  Replaces setCutoffFreq() and
  setResonance().  (Because the internal calculations need to be done whenever either parameter changes.)
  @param cutoff range 0-255 represents 0-8191 Hz (AUDIO_RATE/2).
  Be careful of distortion at the lower end, especially with high resonance.
  @param resonance range 0-255, 255 is most resonant.
  */
  void setCutoffFreqAndResonance(uint8_t cutoff, uint8_t resonance)
	{
    ucutoff = cutoff;
    q = resonance; // hopefully optimised away when compiled, just here for
                   // backwards compatibility
    fb = (q) + ucfxmul(q, SHIFTED_1 - cutoff);
  }

  /** Calculate the next sample, given an input signal.
  @param in the signal input.
  @return the signal output.
  @note Timing: about 11us.
  */
  //	10.5 to 12.5 us, mostly 10.5 us (was 14us)
  inline int nextLP(int in){
	  
	//in=in<<7;
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f);
	buf2 += fxmul(((buf1 - buf2) + fxmul(fb, buf2 - buf3)), f);
    buf3 += ifxmul(buf2 - buf3, f);	// could overflow if input changes fast
    // setPin13Low();
	//return buf1>>1;
	if (((prev_b3>0) && (buf3<=0)) || (prev_b3<0) && (buf3>=0)) {
		
		nf=f;
	}
	prev_b3=buf3;
	return ((buf3*(512-((((255-nf)*q)>>7))))>>9);
    //return buf1>>(8+(q/127));*/
	//return in;*/
	
	//new test filter
	//ini
	/*
mX1 = 0;
mX2 = 0;
mY1 = 0;
mY2 = 0;
pi = 22/7;

//255=1 math
//coefficients
cutoff = f;
res = pow(10, 0.05 * -res_slider);
k = 0.5 * res * sin(pi * cutoff);
c1 = 0.5 * (1 - k) / (1 + k);
c2 = (0.5 + c1) * cos(pi * cutoff);
c3 = (0.5 + c1 - c2) * 0.25;

mA0 = 2 * c3;
mA1 = 2 * 2 * c3;
mA2 = 2 * c3;
mB1 = 2 * -c2;
mB2 = 2 * c1;

//loop
output = mA0*input + mA1*mX1 + mA2*mX2 - mB1*mY1 - mB2*mY2;

mX2 = mX1;
mX1 = input;
mY2 = mY1;
mY1 = output;
	
	return output;*/
  }
  
  inline int nextHP(int in)
	{
    // setPin13High();
	in=in<<7;
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
    // setPin13Low();
    return (in-buf1)>>9;
  }
  
  inline int dcBlock(int in)
	{
    // setPin13High();
	in=in<<2;
    buf4 += fxmul(((in - buf4) ), 10);
    buf5 += ifxmul(buf4 - buf5, 10); // could overflow if input changes fast
    // setPin13Low();
    return (in-buf5)>>4;
  }
  
    inline int nextBP(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (buf3)>>1;
  }

 inline int nextBS(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (in-buf3)>>1;
  }
  
   inline int nextPKG(int in)
	{
    // setPin13High();
    buf0 += fxmul(((in - buf0) + fxmul(fb, buf0 - buf1)), f);
    buf1 += ifxmul(buf0 - buf1, f); // could overflow if input changes fast
	buf2 += fxmul((((in-buf1) - buf2) + fxmul(fb2, buf2 - buf3)), f2);
    buf3 += ifxmul(buf2 - buf3, f2); // could overflow if input changes fast
    // setPin13Low();
    return (buf3+in)>>2;
  }

private:
  uint8_t q=200;
  uint8_t f,f2;
  uint8_t width=1;
  unsigned int fb=0;
  unsigned int fb2;
  int64_t buf0=0, buf1=0,buf2=0,buf3=0,buf4=0,buf5=0,prev_b3=0;
  int8_t ucutoff=50;
  uint8_t nf;
  byte type=0;
	bool keystate=false;
  // // multiply two fixed point numbers (returns fixed point)
  // inline
  // long fxmul(long a, long b)
  // {
  // 	return (a*b)>>FX_SHIFT;
  // }

  // multiply two fixed point numbers (returns fixed point)
  inline unsigned int ucfxmul(int16_t a, int16_t b)
	{
    return (((unsigned int)a * b) >> FX_SHIFT);
  }

  // multiply two fixed point numbers (returns fixed point)
  inline int ifxmul(int a, int16_t b) { return ((a * b) >> FX_SHIFT); }

  // multiply two fixed point numbers (returns fixed point)
  inline long fxmul(long a, int b) { return ((a * b) >> FX_SHIFT); }


protected:
  ADSRxp *adsrxp;
  int16_t factor;
  int16_t cache=0;
  char wait=0;
};


/**
 * @brief Shifts the pitch by the indicated step size: e.g. 2 doubles the pitch
 * @author Phil Schatzmann
 * @ingroup effects
 * @copyright GPLv3
 */
class PitchShift : public AudioEffect {
public:
  /// Boost Constructor: volume 0.1 - 1.0: decrease result; volume >0: increase
  /// result
  PitchShift(float shift_value = 1.0, int buffer_size = 1000) {
    effect_value = shift_value;
    size = buffer_size;
    buffer.resize(buffer_size);
    buffer.setIncrement(shift_value);
  }

  PitchShift(const PitchShift &ref) {
    size = ref.size;
    effect_value = ref.effect_value;
    buffer.resize(size);
    buffer.setIncrement(effect_value);
  };

  float value() { return effect_value; }

  void setValue(float value) {
    effect_value = value;
    buffer.setIncrement(value);
  }

  effect_t process(effect_t input) {
    if (!active())
      return input;
    buffer.write(input);
    return buffer.read();
  }

  PitchShift *clone() { return new PitchShift(*this); }

protected:
  VariableSpeedRingBuffer<int16_t> buffer;
  float effect_value;
  int size;
};
} // namespace audio_tools
