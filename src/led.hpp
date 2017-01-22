#ifndef _LED_H
#define _LED_H

#include <Arduino.h>

// base class for LEDs
// this is what they all need
class CLed_base
{
	public:
		CLed_base(const uint8_t pin);
		virtual ~CLed_base();

	protected:
		uint8_t pin;				// which pin to use

};

// Normal on and off LED
class CLed : public CLed_base
{
	public:
		CLed(const uint8_t pin);

		// get state of LED
		virtual bool isOn();
		// set power
		virtual void power(bool state = true);
		virtual void switch_on();
		virtual void switch_off();

	protected:
		bool state;

};

// fading is implemented in a loop
// every call of Cled_fade::fade will 
// change the leds brigthness
class CLed_fade : public CLed
{
	public:
		// internal representation of fading
		typedef struct brightness_s
		{
			public:
				uint8_t max;
				uint8_t min;
				uint8_t val;
				int8_t fadeAmount;    // how many steps to fade
		} brightness_t, *brightness_p;


		CLed_fade(const uint8_t pin);

		void fade();
		void setUp(const brightness_t &prog);
		virtual void power(bool state = true);

	private:
		brightness_t brightness;
		bool init;

};

#endif // _LED_H

