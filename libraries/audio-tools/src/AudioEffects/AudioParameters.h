#pragma once
#include "AudioTools/AudioLogger.h"

namespace audio_tools {

/**
 * @brief Base class for all parameters
 */
class AbstractParameter {
    public:
        virtual ~AbstractParameter() = default;

        virtual float value() {
            return act_value;
        };

        // triggers an update of the value
        virtual float tick() {
            act_value = update();
            return act_value;
        }

        // to manage keyboard related parameters
        virtual void keyOn(float tgt=0){}

        // to manage keyboard related parameters
        virtual void keyOff(){}

    protected:
        float act_value = 0;
        friend class ScaledParameter;

        virtual float update() = 0;
};

class AbstractParameterXP {
    public:
        virtual ~AbstractParameterXP() = default;

        virtual int32_t value() {
            return act_value;
        };

        // triggers an update of the value
        virtual int32_t tick() {
            act_value = update();
            return act_value;
        }

        // to manage keyboard related parameters
        virtual void keyOn(int32_t tgt=0){}

        // to manage keyboard related parameters
        virtual void keyOff(){}

    protected:
        int32_t act_value = 0;
        friend class ScaledParameter;

        virtual int32_t update() = 0;
};

/**
 * @brief A constant value
 * @ingroup effects
 */
class Parameter : public AbstractParameter {
    public:
        Parameter(float value){
            act_value = value;
        }
        virtual float update(){ return act_value;}
};

/**
 * @brief Generates ADSR values between 0.0 and 1.0
 */
class ADSR : public  AbstractParameter  {
    public:

        ADSR(float attack=0.001, float decay=0.001, float sustainLevel=0.5, float release= 0.005){
            this->attack = attack;
            this->decay = decay;
            this->sustain = sustainLevel;
            this->release = release;
        }

        ADSR(ADSR &copy) = default;

        void setAttackRate(float a){
            attack = a;
        }

        float attackRate(){
            return attack;
        }

        void setDecayRate(float d){
            decay = d;
        }

        float decayRate() {
            return decay;
        }

        void setSustainLevel(float s){
            sustain = s;
        }

        float sustainLevel(){
            return sustain;
        }

        void setReleaseRate(float r){
            release = r;
        }

        float releaseRate() {
            return release;
        }

        void keyOn(float tgt=0){
            LOGI("%s: %f", LOG_METHOD, tgt);
            state = Attack;
            this->target = tgt>0.0f && tgt<=1.0f ? tgt : sustain;
            this->act_value = 0;
        }

        void keyOff(){
            TRACEI();
            if (state!=Idle){
                state = Release;
                target = 0;
            }
        }

        bool isActive(){
            return state!=Idle;
        }

    protected:
        float attack,  decay,  sustain,  release;
        enum AdsrPhase {Idle, Attack, Decay, Sustain, Release};
        const char* adsrNames[5] = {"Idle", "Attack", "Decay", "Sustain", "Release"};
        AdsrPhase state = Idle;
        float target = 0;
        int zeroCount =  0;

        inline float update( ) {

            switch ( state ) {
                case Attack:
                    act_value += attack;
                    if ( act_value >= target ) {
                        act_value = target;
                        target = sustain;
                        state = Decay;
                    }
                    break;

                case Decay:
                    if ( act_value > sustain ) {
                        act_value -= decay;
                        if ( act_value <= sustain ) {
                            act_value = sustain;
                            state = Sustain;
                        }
                    }
                    else {
                        act_value += decay; // attack target < sustainLevel level
                        if ( act_value >= sustain ) {
                            act_value = sustain;
                            state = Sustain;
                        }
                    }
                    break;

                case Release:
                    act_value -= release;
                    if ( act_value <= 0.0f ) {
                        act_value = 0.0;
                        state = Idle;
                    }
                    break;

                default:
                    // nothing to be done
                    break;
            }
            return act_value;
        }

};

class ADSRxp : public  AbstractParameterXP  {
    public:
		//char this_partial=0;
        ADSRxp(int16_t attack=1, int16_t decay=1, int16_t sustainLevel=4096, int16_t release= 1){
            this->attack = attack;
            this->decay = decay;
            this->sustain = sustainLevel;
            this->release = release;
			 this->act_value = 0;
        }

        ADSRxp(ADSRxp &copy) = default;

        void setAttackRate(int32_t a){
            attack = a;
        }
		
		

        int16_t attackRate(){
            return attack;
        }

        void setDecayRate(int32_t d){
            decay = d;
        }

        int16_t decayRate() {
            return decay;
        }

        void setSustainLevel(int32_t s){
            sustain = s;
			//Serial.println(sustain);
        }

        int16_t sustainLevel(){
            return sustain;
        }

        void setReleaseRate(int32_t r){
            release = r;
        }

        int16_t releaseRate() {
            return release;
        }

        void keyOn(int32_t tgt=0){
            LOGI("%s: %f", LOG_METHOD, tgt);
            state = Attack;
			this->target = tgt;
			this->act_value = 0;
			target=tgt;
			if (attack>131000){
				state=Decay;
				
				this->act_value = max;
				
				this->target = sustain;
			}
            
            //this->act_value = 0;
			//Serial.println(tgt);
        }

        void keyOff(){
            TRACEI();
            if (state!=Idle){
                state = Release;
                target = 0;
            }
        }

        bool isActive(){
            return state!=Idle;
        }
		
		bool isRel(){
            return state!=Release;
        }
		
		void stop(){
			
			act_value=0;
			state=Idle;
		}
		
		void setMax(int my_max){
			
			max=my_max;
		}

    protected:
        int32_t attack,  decay,  sustain,  release;
        enum ADSRxpPhase {Idle, Attack, Decay, Sustain, Release};
        const char* ADSRxpNames[5] = {"Idle", "Attack", "Decay", "Sustain", "Release"};
        ADSRxpPhase state = Idle;
        int32_t target = 0;
        int zeroCount =  0;
		int max=16384;
        inline int32_t update( ) {

            switch ( state ) {
                case Attack:
                    act_value += attack;
				//Serial.println((int)act_value);
                    if ( act_value >= target ) {
                        act_value = target;
                        target = sustain;
                        state = Decay;
                    }
                    break;

                case Decay:
                    if ( act_value > sustain ) {
                        act_value -= decay;
                        if ( act_value <= sustain ) {
                            act_value = sustain;
                            state = Sustain;
                        }
                    }
                    else {
                        act_value += decay; // attack target < sustainLevel level
                        if ( act_value >= sustain ) {
                            act_value = sustain;
                            state = Sustain;
                        }
                    }
                    break;

                case Release:
                    act_value -= release;
                    if ( act_value <= 0 ) {
                        act_value = 0;
                        state = Idle;
						
						 
						
                    }
                    break;

                default:
                    // nothing to be done
                    break;
            }
			
			
            return act_value;
			
        }

};


/**
 * @brief Scales a dynamic parameter to the indicated range
 *
 */
class ScaledParameter : public AbstractParameter {
    public:
    ScaledParameter(AbstractParameter *parameter, float min, float max){
        this->min = min;
        this->max = max;
        this->p_parameter = parameter;
    }

    float update() {
        return p_parameter->update() + min * (max-min);
    }

    protected:
        float min=0, max=0;
        AbstractParameter *p_parameter;

};


}
