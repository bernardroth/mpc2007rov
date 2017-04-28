#include <16f877A.h>
#include <stdlib.h>
//#fuses   HS, NOLVP, WDT, debug
#fuses   HS, NOLVP, NOWDT, PUT, BROWNOUT, NOCPD, NOPROTECT
#use delay (clock=15000000) 
#use RS232 (baud=115200, xmit=PIN_C6, rcv=PIN_C7)
//#use RS232 (DEBUGGER)

#define R_switch PIN_B0
#define L_switch PIN_B1

signed int16 forbak_cal;
signed int16 crablr_cal;
signed int16 updown_cal;
signed int16 turnlr_cal;
signed int16 openclose_cal;
signed int16 forbak=0;
signed int16 crablr=0;
signed int16 updown=0;
signed int16 turnlr=0;
signed int16 openclose = 0;
signed int16 HR=0;
signed int16 HL=0;
signed int16 VR=0;
signed int16 VL=0;
signed int16 Manip = 0;
signed int8 Checksum=0;
signed int8 SendByte;


void main () {
   //Turn on LED
   output_high (pin_D7);

   port_b_pullups(TRUE);
   setup_adc_ports( ALL_ANALOG );
   setup_adc ( ADC_CLOCK_INTERNAL );


      //Read for Calibrating Joystick
      set_adc_channel ( 0 );
      //yellow wire
      delay_ms (5);
      forbak_cal = read_adc();
      forbak_cal = 255 - forbak_cal;

      set_adc_channel ( 1 );
      //red wire
      delay_ms (5);
      crablr_cal = read_adc();
      crablr_cal = 255 - crablr_cal;
      
      set_adc_channel ( 2 );
      //white wire
      delay_ms (5);
      updown_cal = read_adc();
      updown_cal = 255 - updown_cal;
      
      set_adc_channel ( 3 );
      //black wire
      delay_ms (5);
      turnlr_cal = read_adc();
      turnlr_cal = 255 - turnlr_cal;


   while (TRUE) {
   
      //restart_wdt();
      
      //setup_timer_0(RTCC_INTERNAL);
      //setup_wdt(WDT_72MS);    //2.5 sec watchdog  
   
   
      //delay_ms(100);
    
      //Read data
      set_adc_channel ( 0 );
      delay_ms (5);
      forbak = read_adc();
      
      set_adc_channel ( 1 );
      delay_ms (5);
      crablr = read_adc();
      
      
      set_adc_channel ( 2 );
      delay_ms (5);
      updown = read_adc();
      
      
      set_adc_channel ( 3 );
      delay_ms (5);
      turnlr = read_adc();
      
      //read digital pulse
      openclose = 0;
      if (!input (R_switch)){
         openclose = 125;}
         
      if (!input(L_switch)){
         openclose = -125;}      
      
      //Invert
      forbak = 255-forbak;
      crablr = 255-crablr;
      updown = 255-updown;
      turnlr = 255-turnlr;
      
         //Change values to optimal numbers
       //If direction is backwards, change it here:

         forbak=forbak-forbak_cal;
         crablr=crablr-crablr_cal;
         updown=updown-updown_cal;
         turnlr=turnlr-turnlr_cal;
         openclose=openclose;
      
      //Zero Area
      if (forbak<15 && forbak>-15){
      forbak=0;
      }
      
      if (crablr<15 && crablr>-15){
      crablr=0;
      }
      
      if (updown<15 && updown>-15){
      updown=0;
      }
      
      if (turnlr<15 && turnlr>-15){
      turnlr=0;
      }
      
      //if (openclose<10 && openclose>-10){
      //openclose = 0;
      //}
      
      
      
      //Fix the data
      
      HR=forbak+turnlr;
      HL=forbak-turnlr;
      
      VR=updown+crablr;
      VL=updown-crablr;
      Manip=openclose; 
       
      //Send data

      //Make sure it is -126< x <126
      if (HR>126){
   HR = 126;} 
      if (HR< -126){
   HR = -126;}
      if (HL>126){
   HL = 126;} 
      if (HL< -126){
   HL = -126;}
      if (VR>126){
   VR = 126;} 
      if (VR< -126){
   VR = -126;}
      if (VL>126){
   VL = 126;} 
      if (VL< -126){
   VL = -126;}
      if (Manip>126){
   Manip = 126;}
      if (Manip< -126){
   Manip = -126;}
      
      
      Checksum=HR+HL+VR+VL+Manip;
      
      putc (255);
      delay_ms (3);    
      SendByte = HR;
      putc (SendByte);
      delay_ms (3);
      SendByte = HL;
      putc (SendByte);
      delay_ms (3);
      SendByte = VR;
      putc (SendByte);
      delay_ms (3);
      SendByte = VL;
      putc (SendByte);
      delay_ms (3);
      SendByte = Manip;
      putc (SendByte);
      delay_ms (3);
      putc (Checksum);
      delay_ms (100);
      
      
      
      /*
      putc (255);
      delay_ms (3);     
      putc (HR);
      delay_ms (3);
      putc (HL);
      delay_ms (3);
      putc (VR);
      delay_ms (3);
      putc (VL);
      delay_ms (3);
      putc (Checksum);
      delay_ms (100);
      */
      
      //Debug
      /*
      delay_ms (50);
      putc (0x0C);
      printf("forbak %Ld\r\n", (forbak));
      printf("crablr %Ld\r\n", (crablr));     
      printf("updown %Ld\r\n", (updown));    
      printf("turnlr %Ld\r\n", (turnlr));     
      printf("Checksum %d\r\n", (Checksum));
      printf("HR %Ld\r\n", (HR));    
      printf("HL %Ld\r\n", (HL));     
      printf("VR %Ld\r\n", (VR));   
      printf("VL %Ld\r\n", (VL));
      */      
   }
}
   

