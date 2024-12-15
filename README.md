### A3 - Interrupts and Timers
## Timers
Modern MCUs include hardware timers to keep precise timing without the need of software delay loops. This allows the CPU to process other instructions while “waiting” for the specified time. The STM32L4 has several different timers. 
- Advanced Control Timers - TIM1, TIM8
- General Purpose Timers (32-bit) - TIM2, TIM5
- General Purpose Timers (16-bit) - TIM3, TIM4
- General Purpose Timers (Reduced features, 16-bit) - TIM15, TIM16, TIM17
- Basic Timers - TIM6, TIM7
- Low Power Timer - LPTIM1, LPTIM2
For this assignment, the most basic functionality of the general purpose timer TIM2 will be used. The core of any timer is a hardware counter that counts clock ticks. The most basic functionality of a timer is to trigger an interrupt when the count reaches a specified value. This allows easy and precise timing with an MCU without software delay loops. Timers typically include other features for multiple interterrupt time events, creating PWM signals, and measuring the timing of input signals.  

## Interrupts
An interrupt is a hardware mechanism in a CPU that can interrupt executing the current sequence of instructions and jump to a specific subroutine. Modern CPUs have multiple interrupts with each able to connect to a separate subroutine to allow each interrupt to handle a specific task (82 on the STM32L4). The advanced timers (TIM1, TIM8) have 4 different interrupts (some are shared with other timers). The general purpose timers each have a single interrupt. TIM2_IRQHandler is the interrupt handler for TIM2.  

## Instructions
# Part A: 5 kHz with 25% Duty Cycle Square Wave
Setup TIM2 to count in up mode. Calculate the necessary ARR values to create a 5 kHz square wave using a 4 MHz input clock. Set CCR1 to interrupt at a 25% duty cycle. It is recommended to draw out the 5 kHz wave along with a 4 MHz wave for reference. Be sure to specify period, high, and low time values of the 5 kHz wave.
Create a 5 kHz with a 25% duty cycle square wave by setting a pin high and low with the TIM2 ISR. The code in main should only set up TIM2 and not keep track of any timing. No software delays should be used. 
View this output on an oscilloscope and take a screen capture with measurements verifying timing.
# Part B: ISR Processing Measurement
Change your code above to run TIM2 continuously (ARR = 0xFFFFFFFF) and change CCR1 to generate a 5 kHz square wave with a 50% duty cycle. Verify this on the oscilloscope.
Add a second GPIO output to your system to measure ISR processing time. This can be done by driving a pin high immediately when entering the ISR and driving it low just before exiting. This bit should go high and low around every transition of the 5 kHz wave.
PA8 can be used to output an internal clock signal, microcontroller clock output (MCO). The code below will configure PA8 to output the 4 MHz system clock.

// Enable MCO, select MSI (4 MHz source)
RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));

// Configure MCO output on PA8
RCC->AHB2ENR   |=  (RCC_AHB2ENR_GPIOAEN);
GPIOA->MODER   &= ~(GPIO_MODER_MODE8);		// alternate function mode
GPIOA->MODER   |=  (2 << GPIO_MODER_MODE8_Pos);
GPIOA->OTYPER  &= ~(GPIO_OTYPER_OT8);		// Push-pull output
GPIOA->PUPDR   &= ~(GPIO_PUPDR_PUPD8);		// no resistor
GPIOA->OSPEEDR |=  (GPIO_OSPEEDR_OSPEED8);		// high speed
GPIOA->AFR[1]  &= ~(GPIO_AFRH_AFSEL8);		// select MCO function

Watch the ISR execution timing bit on the oscilloscope while watching MCO on another trace. Count how many MCO clock cycles it takes to execute your ISR. Take a scope capture from the oscilloscope. (The 5 kHz square wave is not needed for this capture)
# Part C: Shortest Pulse and Failing ISR
Using the same code from Part B, watch the 5 kHz, 50% duty cycle square wave signal and lower the value of CCR1. The resulting square wave should have a smaller period. Continue to lower the CCR1 value until the resulting square wave “breaks” and is no longer shorter. It will be obvious when this happens. Record the smallest value of CCR1 that works and take a scope capture of the resulting 50% duty cycle square wave along with the ISR timing bit output. Does this CCR1 value correlate to the number of clocks that the ISR requires to execute from Part B?

## Hints
Include counting 0 when calculating CCR / AAR values
Be sure to clear the correct interrupt flag in the ISR.
