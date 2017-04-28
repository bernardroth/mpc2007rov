#include <16f877A.h>
#include <stdlib.h>
#fuses   HS, NOLVP, WDT, PUT, BROWNOUT, NOCPD, NOPROTECT

//#use delay (clock=3686800)
//#fuses   XT, NOLVP, NOWDT, PUT, BROWNOUT, NOCPD, NOPROTECT

#use delay (clock=15000000)
#use RS232 (baud=115200, xmit=PIN_C6, rcv=PIN_C7)

#BYTE RCREG=0x1A
#BYTE TMR1H=0x0F

#define HR_FOR PIN_B2
#define HR_PWM PIN_B4
#define HR_BAK PIN_B5

#define HL_FOR PIN_B1
#define HL_PWM PIN_B0
#define HL_BAK PIN_D7

#define VR_FOR PIN_D6
#define VR_PWM PIN_D5
#define VR_BAK PIN_D4

#define VL_FOR PIN_C4
#define VL_PWM PIN_D3
#define VL_BAK PIN_D2

#define Manip_open PIN_A0   
#define Manip_PWM PIN_A1
#define Manip_close PIN_A2

signed int8 rx_counter;
unsigned int8 error_counter = 0;
unsigned int8 HR_ISR;
unsigned int8 HL_ISR;
unsigned int8 VR_ISR;
unsigned int8 VL_ISR;
unsigned int8 checksum_ISR;
unsigned int8 Manip_ISR;
signed int8 checksum;

signed int8 HR=0;
signed int8 HL=0;
signed int8 VR=0;
signed int8 VL=0;
signed int8 Manip = 0;

unsigned int8 HR_ABS=0;
unsigned int8 HL_ABS=0;
unsigned int8 VR_ABS=0;
unsigned int8 VL_ABS=0;
unsigned int8 Manip_ABS = 0;

#int_timer2
void timer2_isr() {
   
   static unsigned int8 counter;
 
   //Begining of loop - Turn on all motors
   if (counter==0){
      output_high (HR_PWM);
      output_high (HL_PWM);
      output_high (VR_PWM);
      output_high (VL_PWM);
      output_high (Manip_PWM);
      counter++;
      }
   
   //Check motor values - if less than counter, turn off
   else {
      if (counter > HR_ABS){
         output_low(HR_PWM);
         }
      if (counter > HL_ABS){
         output_low(HL_PWM);
         }
      if (counter > VR_ABS){
         output_low(VR_PWM);
      }
      if (counter > VL_ABS){
         output_low(VL_PWM);
      }
      if (counter > Manip_ABS){
         output_low(Manip_PWM);
      }
      counter+=10;}   //adjusting this value will be the
                      //same as dividing all the varibles by 10
                      //Increasing this value will increase 
                      //PWM Freq and decrease speed resolution
   
   //End of loop - reset counter
   
   //if (counter>127){
   if (counter>120){
      counter=0;}
   
   //TMR1H = 0xFE;   // skip to the end for max speed 
}


#int_RDA                                             
  void rx_isr(){
  
  //restart_wdt();
 
  if (RCREG == 255){
  rx_counter = 0;
  error_counter ++;
    if (error_counter>100){
        error_counter = 101;
                HR = 0;
               HL = 0;
               VR = 0;
               VL = 0;
               Manip = 0;         
               HR_ABS=0;
               HL_ABS=0;
               VR_ABS=0;
               VL_ABS=0;
               Manip_ABS = 0;
      }
  }
else
{
 
   switch (rx_counter){
      case 0:
              HR_ISR=RCREG;
              rx_counter++;
         break;

      case 1:
              HL_ISR=RCREG;
              rx_counter++;
         break;

      case 2:
              VR_ISR=RCREG;
              rx_counter++;
         break;

      case 3:
              VL_ISR=RCREG;
              rx_counter++;
         break;

      case 4:
              Manip_ISR=RCREG;
              rx_counter++;
         break;

      case 5:
         checksum_ISR=RCREG;
         checksum = HR_ISR + HL_ISR + VR_ISR + VL_ISR + Manip_ISR;
         if (checksum == checksum_ISR){   
            restart_wdt();
            error_counter = 0;
            HR = HR_ISR;
            HL = HL_ISR;
            VR = VR_ISR;
            VL = VL_ISR;
            Manip = Manip_ISR;         
            HR_ABS=ABS(HR);
            HL_ABS=ABS(HL);
            VR_ABS=ABS(VR);
            VL_ABS=ABS(VL);
            Manip_ABS = ABS(Manip);
         }
              rx_counter = 0;               
         break;

      default:
         rx_counter = 0;
         break;
        }
}
}




void main () {
  
   //Power LED
   output_high (PIN_D1);
   
   /*
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
   enable_interrupts(int_RDA);
   enable_interrupts(int_timer1);
   enable_interrupts(global);
   */
   
   setup_timer_0(RTCC_INTERNAL);
   setup_wdt(WDT_1152MS);    //watchdog                         
   
   setup_timer_2(T2_DIV_BY_16,200,1);

   enable_interrupts(int_RDA);
   enable_interrupts(int_timer2);
   enable_interrupts(global);
   
   while(TRUE){

      //Direction

      if (HR < 0) {                            
         output_high(HR_FOR);
         output_low (HR_BAK);
         }
      
      else if (HR == 0) {
         output_low(HR_FOR); 
         output_low(HR_BAK);
         //output_high (PIN_C5);
         }
      
      else if (HR > 0)  {
         output_low (HR_FOR); 
         output_high(HR_BAK);
         }
             
      if (HL < 0) {
         output_high(HL_FOR); 
         output_low (HL_BAK);
         }
         
      else if (HL == 0) {                          
         output_low(HL_FOR); 
         output_low(HL_BAK);
         //output_high (PIN_C5);
         }
        
      else if (HL > 0) {
         output_high(HL_BAK); 
         output_low (HL_FOR);
         }
      
      if (VR < 0) {
         output_high(VR_FOR);
         output_low (VR_BAK);
         }
         
      else if (VR == 0) {
         output_low(VR_FOR);
         output_low(VR_BAK);
         //output_high (PIN_C5);
         }
         
      else if (VR > 0) {
         output_high(VR_BAK);
         output_low (VR_FOR);
         }
      
      if (VL < 0) {
         output_high(VL_FOR);
         output_low (VL_BAK);
         }
         
      else if (VL == 0) {
         output_low(VL_FOR);
         output_low(VL_BAK);
         //output_high (PIN_C5);
         }
         
      else if (VL > 0) {
         output_high(VL_BAK);
         output_low (VL_FOR);
         }
      
      if (Manip < 0) {
         output_high(Manip_open);
         output_low (Manip_close);
         }
         
      else if (Manip == 0) {
         output_low(Manip_open);
         output_low(Manip_close);
         //output_high (PIN_C5);
         }
         
      else if (Manip > 0) {
         output_high(Manip_close);
         output_low (Manip_open);
         }         
   }
}
   
