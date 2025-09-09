#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>

/* --------- Pin Map (Arduino Uno / ATmega328P) ---------
   Ultrasonic:
     TRIG -> D9  = PB1 (output)
     ECHO -> D10 = PB2 (input, PCINT2)   <-- keep your wiring here
   LEDs:
     RED   -> D2  = PD2 (output)
     GREEN -> D3  = PD3 (output)
   Buttons (active LOW, internal pullups):
     HEALTH -> D4 = PD4
     ENTRY  -> D5 = PD5
     EXIT   -> D6 = PD6
--------------------------------------------------------*/

#define TRIG     PB1
#define ECHO     PB2
#define RED_LED  PD2
#define GRN_LED  PD3
#define BTN_HEALTH PD4
#define BTN_ENTRY  PD5
#define BTN_EXIT   PD6

#define MAX_CAPACITY 10
#define APPROACH_CM_THRESHOLD 50        // “approaching” if < 50 cm
#define DETECTION_COOLDOWN_MS 6000UL    // 6 s (your sketch said 3 in comment, used 6000 before)

static volatile uint32_t ms_counter = 0;        // millis()
static volatile uint16_t t_start=0, t_end=0;    // Timer1 capture values
static volatile uint32_t ovf_count=0, ovf_start=0, ovf_end=0;
static volatile uint8_t  edge_state=0;          // 0=wait rise,1=wait fall,2=done

// -------- USART @ 9600 --------
static void usart_init(void){
    UBRR0H = 0; UBRR0L = 103;                   // 9600 @16MHz
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}
static void tx(char c){ while(!(UCSR0A&(1<<UDRE0))); UDR0=c; }
static void prints(const char*s){ while(*s) tx(*s++); }
static void printi(int v){ char b[16]; sprintf(b,"%d",v); prints(b); }
static void println(const char*s){ prints(s); tx('\r'); tx('\n'); }

// -------- Timer0 -> millis() (1 kHz) --------
ISR(TIMER0_COMPA_vect){ ms_counter++; }
static void timer0_init_millis(void){
    TCCR0A = (1<<WGM01);         // CTC
    OCR0A  = 249;                // 1ms @16MHz /64
    TCCR0B = (1<<CS01)|(1<<CS00);
    TIMSK0 = (1<<OCIE0A);
}

// -------- Timer1 free-run + overflow for high-res timing --------
ISR(TIMER1_OVF_vect){ ovf_count++; }
static void timer1_init_free_run(void){
    // Timer1 @ 2 MHz (0.5us/tick): prescaler /8
    TCCR1A = 0;
    TCCR1B = (1<<CS11);          // /8
    TCNT1  = 0;
    TIMSK1 = (1<<TOIE1);         // enable overflow interrupt
}

// -------- Pin Change Interrupt on PB2 (ECHO) --------
ISR(PCINT0_vect){
    uint8_t high = (PINB & (1<<ECHO)) != 0;
    if(edge_state==0 && high){
        t_start = TCNT1; ovf_start = ovf_count; edge_state = 1;
    } else if(edge_state==1 && !high){
        t_end   = TCNT1; ovf_end   = ovf_count; edge_state = 2;
    }
}

static inline uint32_t millis(void){ return ms_counter; }

// -------- I/O init --------
static void io_init(void){
    // TRIG out, ECHO in
    DDRB  |= (1<<TRIG);
    PORTB &= ~(1<<TRIG);
    DDRB  &= ~(1<<ECHO);
    PORTB &= ~(1<<ECHO); // no pull-up

    // LEDs
    DDRD |= (1<<RED_LED) | (1<<GRN_LED);
    PORTD &= ~((1<<RED_LED)|(1<<GRN_LED));

    // Buttons with pull-ups
    DDRD  &= ~((1<<BTN_HEALTH)|(1<<BTN_ENTRY)|(1<<BTN_EXIT));
    PORTD |=  (1<<BTN_HEALTH)|(1<<BTN_ENTRY)|(1<<BTN_EXIT);

    // Enable pin-change interrupt for PORTB, PB2
    PCICR  |= (1<<PCIE0);
    PCMSK0 |= (1<<PCINT2);
}

// -------- Ultrasonic read (PCINT + Timer1), returns microseconds --------
static int32_t echo_pulse_us(uint32_t* us_out){
    // prepare edge capture
    edge_state = 0;
    ovf_count = 0;
    TIFR1 = (1<<TOV1);

    // 10us TRIG
    PORTB &= ~(1<<TRIG); _delay_us(3);
    PORTB |=  (1<<TRIG); _delay_us(10);
    PORTB &= ~(1<<TRIG);

    // wait for both edges with timeout (~60ms total)
    uint32_t guard=0;
    while(edge_state<2){
        _delay_us(10);
        if(++guard>6000){ if(us_out)*us_out=0; return -1; }
    }

    uint32_t ticks_start = ((uint32_t)ovf_start<<16) + t_start;
    uint32_t ticks_end   = ((uint32_t)ovf_end  <<16) + t_end;
    uint32_t ticks = (ticks_end>=ticks_start)?(ticks_end - ticks_start)
                                            : (0x100000000ULL - ticks_start + ticks_end);

    uint32_t us = ticks/2;   // 0.5us/tick
    if(us_out) *us_out = us;
    return (int32_t)us;
}

static int read_distance_cm(void){
    uint32_t us=0;
    int32_t ok = echo_pulse_us(&us);
    if(ok<0) return -1;
    return (int)(us/58);      // HC-SR04 conversion
}

// -------- Simple OLED hook (replace with real SSD1306 later) --------
static void oled_show_count(int count){
    prints("[OLED] People Count: "); printi(count); println("");
}

// -------- Occupancy status print --------
static void print_occupancy_status(int peopleCount){
    prints("Current People Count: "); printi(peopleCount); println("");

    if (peopleCount >= MAX_CAPACITY) {
        println("Occupancy: FULL");
        return;
    }

    // scale to percentage (0–100)
    int pct = (peopleCount * 100) / MAX_CAPACITY;

    if (pct <= 40) {
        println("Occupancy: LOW");
    } else if (pct <= 80) {
        println("Occupancy: MEDIUM");
    } else {
        println("Occupancy: HIGH");
    }
}

/* ======================= MAIN LOGIC ======================= */
int main(void){
    usart_init();
    io_init();
    timer0_init_millis();
    timer1_init_free_run();
    sei();

    int peopleCount = 0;
    bool healthOK = false;
    bool canEnter = false;        // set after approach detect
    uint32_t lastDetectMs = 0;

    println("System Initialized");
    prints("Current People Count: "); printi(peopleCount); println("");

    while(1){
        uint32_t now = millis();

        /* ---- Approach detect with cooldown ---- */
        if(now - lastDetectMs > DETECTION_COOLDOWN_MS){
            int cm = read_distance_cm();
            if(cm > 0 && cm < APPROACH_CM_THRESHOLD){
                lastDetectMs = now;
                println("Individual approaching");

                if(peopleCount >= MAX_CAPACITY){
                    println("Maximum Capacity Reached. Access Denied.");
                    PORTD |=  (1<<RED_LED);
                    PORTD &= ~(1<<GRN_LED);
                    // lock out entry flow until space frees
                    canEnter = false;
                    healthOK = false;
                }else{
                    canEnter = true;   // window open to press buttons
                }
            }
        }

        /* ---- Buttons (active LOW) ---- */
        uint8_t pinsD = PIND;

        // If max capacity, any button press just shows max-capacity denial + RED
        if(peopleCount >= MAX_CAPACITY){
            if(!(pinsD & (1<<BTN_HEALTH)) || !(pinsD & (1<<BTN_ENTRY)) || !(pinsD & (1<<BTN_EXIT))){
                println("Maximum Capacity Reached. Access Denied.");
                PORTD |=  (1<<RED_LED);
                PORTD &= ~(1<<GRN_LED);
                _delay_ms(250);    // debounce
            }
            continue; // skip normal flow while full
        }

        // Health
        if(!(pinsD & (1<<BTN_HEALTH))){
            healthOK = true;
            println("Health Check Passed.");
            _delay_ms(250); // debounce
        }

        // Entry
        if(!(pinsD & (1<<BTN_ENTRY))){
            if(canEnter){
                if(healthOK){
                    peopleCount++;
                    println("Health Check Passed. Access Granted.");
                    print_occupancy_status(peopleCount);
                    oled_show_count(peopleCount);
                    PORTD |=  (1<<GRN_LED);
                    PORTD &= ~(1<<RED_LED);
                    healthOK = false;    // consume
                }else{
                    println("Health Check Failed. Access Denied.");
                    PORTD |=  (1<<RED_LED);
                    PORTD &= ~(1<<GRN_LED);
                }
                canEnter = false;        // consume the “approach”
            }else{
                println("No recent person detected. Ignoring entry.");
            }
            _delay_ms(250); // debounce
        }

        // Exit
        if(!(pinsD & (1<<BTN_EXIT))){
            if(peopleCount > 0){
                peopleCount--;
                println("Exit Button Pressed.");
                print_occupancy_status(peopleCount);
                oled_show_count(peopleCount);
            }else{
                println("Exit Pressed but no one inside.");
            }
            PORTD &= ~((1<<GRN_LED)|(1<<RED_LED));
            _delay_ms(300); // debounce
        }
    }
}
