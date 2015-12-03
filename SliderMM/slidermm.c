
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>


#define LED_COUNT 10

uint8_t led_strip[1][10] = {{PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PD0, PD1}};
volatile uint8_t* led_port[1][10] = {{&PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTB, &PORTD, &PORTD }};
volatile uint8_t* led_ddr[1][10] = {{&DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRB, &DDRD, &DDRD}};


/**
 * Sleep amount of milliseconds.
 * Used to pause ADC between readouts.
 */
void sleep_ms( uint16_t ms )
{
  while ( ms )
  {
    ms--;
    _delay_ms( 1 );
  }
}

/**
 * Initialize A/D converter.
 */
void adc_init(void)
{
  /* reference voltage from AVcc */
  ADMUX = (1<<REFS0);    
  
  /* single conversion */
  ADCSRA = (1<<ADPS1) | (1<<ADPS0); /* frequency */
  ADCSRA |= (1<<ADEN);              /* activate ADC */
 
  /* dummy readout of ADC to get it running */
  ADCSRA |= (1<<ADSC);
  while (ADCSRA & (1<<ADSC) ) {} /* wait for conversion */
  
  /* touch ADCW once */
  (void) ADCW;
}
 
/**
 * ADC single read.
 * 
 * @return converted AD value for channel as integer - 10 bit conversion, value range [0..1023] for [0..AVcc]
 */
uint16_t adc_read( uint8_t channel )
{
  /* set channel bit */
  ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
  ADCSRA |= (1<<ADSC); /* single conversion */
  while (ADCSRA & (1<<ADSC) ) {} /* wait conversion */
  return ADCW; /* read and return */
}

/**
 * Set DDR for LED ports by LED strip number (channel)
 */
void init_led_strip( uint8_t channel )
{
  for ( uint8_t i = 0; i < LED_COUNT; ++i )
  {
    *led_ddr[channel][i] |= (1 << led_strip[channel][i]);
  }
}
 
/**
 * Set LED output port for 10 LED strip number #channel to val in [0..1000].
 */
void set_led_strip( uint8_t channel, uint16_t val )
{
  uint8_t i = 0;
  /* Turn on LEDs up to val/100 */
  for ( ; i < val / 100; ++i )
  {
    if ( i >= LED_COUNT ) break;
    *led_port[channel][i] |= (1<<led_strip[channel][i]);
  }
  
  /*  turn off all other LEDs in strip*/
  for ( ; i < LED_COUNT; ++i )
  {
    *led_port[channel][i] &= ~(1<<led_strip[channel][i]);
  }
}

/**
 * Set 2 LED strip vals between [0..1023] by 3 ADC inputvalues.
 */
void set_led_vals( uint16_t* invals, uint16_t *outval1, uint16_t *outval2 )
{
  *outval1 = invals[0];
  *outval2 = invals[1];
}
 

/**
 * Init all stuff and run event loop
 */
int main( void )
{
  adc_init();
  init_led_strip(0);
  // TODO init_led_strip(1);
  
  uint16_t adc[3];
  uint16_t ledvals[2];
  uint16_t adcval;
  uint8_t adcchanged = 1;
  uint8_t i = 0;
  
  while ( 1 ) 
  {
    // TODO some ms sleep not to stress ADC the whole time
    for ( i = 0; i < 3; i++ )
    {
      adcval = adc_read( i );
      if ( adcval != adc[i] ) // TODO ignore minor changed (flapping values) 
      {
        adc[i] = adcval;
        adcchanged = 1;
      }  
    }
    
    if ( adcchanged )
    {
      set_led_vals( adc, &ledvals[0], &ledvals[1] );
      set_led_strip( 0, ledvals[0] );
      // TODO set_led_strip( 1, ledvals[1] );
    }
    adcchanged = 0;
  }
}
