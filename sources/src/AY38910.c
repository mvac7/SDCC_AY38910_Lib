/* =============================================================================
   SDCC AY-3-8910 Functions Library (object type)
   Version: 1.4
   Date: 17/06/2018
   Author: mvac7/303bcn
   Architecture: MSX
   Format: C Object (SDCC .rel)
   Programming language: C
   WEB: 
   mail: mvac7303b@gmail.com

   Description:                                                              
     Opensource library for acces to PSG AY-3-8910
     It does not use the BIOS so it can be used to program ROMs or 
     MSX-DOS executables.
     
  History of versions:
   >v1.4 (17/06/2018)<
    v1.3 (11/02/2018)
    v1.1 (14/02/2014) 
============================================================================= */
#include "../include/AY38910.h"

//intern MSX AY
#define AY0index 0xA0
#define AY0write 0xA1
#define AY0read  0xA2

//AY from MEGAFLASHROM-SCC+
//#define AY1index 0x10
//#define AY1write 0x11
//#define AY1read  0x12



/* =============================================================================
 SOUND(register, value)

 Function : Write into a register of PSG
 Input    : [char] register number (0 to 13)
            [char] value
 Output   : -
============================================================================= */
void SOUND(char reg, char value){
reg;value;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   C,4(IX) ;reg
  ld   B,5(IX) ;value
  
  ;control del registro 7
  ld   A,C
  cp   #7   ;IF reg=7
  jr   NZ,writeAY      ;NO
  
  ld   A,B
  AND  #0b00111111
  ld   B,A
      
  ;YES
  ld   A,#7
  out  (#AY0index),A
  in   A,(#AY0read)  
  and	 #0b11000000	; Mascara para coger dos bits de joys 
	or	 B		        ; A�ado Byte de B
  ld   B,A

writeAY:
  ld   A,C    
  out  (#AY0index),A
  ld   A,B
  out  (#AY0write),A

  pop  IX
__endasm;  
}

    


/* =============================================================================
 GetSound(register) 

 Function : Read PSG register value
 Input    : [char] register number (0 to 13)
 Output   : [char] value 
============================================================================= */
char GetSound(char reg){
reg;
__asm
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   A,4(IX)
  out  (#AY0index),A
  in   A,(#AY0read)
  
  ld   L,A
  
  pop  IX  
__endasm;
}



/* =============================================================================
 SetTonePeriod(channel, period) 

 Function : Set Tone Period for any channel
 Input    : [char] channel (0, 1 or 2)
            [unsigned int] period (0 - 4095)
 Output   : -
============================================================================= */
void SetTonePeriod(char channel, unsigned int period){
  channel=channel*2;
  SOUND(channel++,period & 0xFF);
  SOUND(channel,(period & 0xFF00)/0xFF);
  return;
}



/* =============================================================================
 SetNoisePeriod(period) 

 Function : Set Noise Period
 Input    : [char] Noise period (0 - 31) 
 Output   : - 
============================================================================= */
void SetNoisePeriod(char period){
  SOUND(6,period);
  return;
}



/* =============================================================================
 SetEnvelopePeriod(period) 

 Function : Set Envelope Period
 Input    : [unsigned int] Envelope period (0 - 65535) 
 Output   : - 
============================================================================= */
void SetEnvelopePeriod(unsigned int period){
  SOUND(11,period & 0xFF);
  SOUND(12,(period & 0xFF00)/0xFF);
  return;
}



/* =============================================================================
 SetVolume(channel, volume) 

 Function : Set volume channel
 Input    : [char] channel (0, 1 or 2)
            [char] volume, 0 to 15 or 16 for activate envelope
 Output   : -
============================================================================= */
void SetVolume(char channel, char volume){
  SOUND(8+channel,volume);
  return;
}



/* =============================================================================
 SetChannel(channel, isTone, isNoise)

 Function : Mixer. Enable/disable Tone and Noise channels.
 Input    : [char] channel (0, 1 or 2)
            [boolean] tone state
            [boolean] noise state
 Output   : -
============================================================================= */
void SetChannel(char channel, boolean isTone, boolean isNoise)
{
  char newValue=0;
  newValue = GetSound(7); 
  //el control de los dos bits de I/O del registro 7 
  //se hace en la funci�n Sound
  if(channel==0) 
  {
      if(isTone==true){newValue&=254;}else{newValue|=1;}
      if(isNoise==true){newValue&=247;}else{newValue|=8;}
  }
  if(channel==1)    
  {
      if(isTone==true){newValue&=253;}else{newValue|=2;}
      if(isNoise==true){newValue&=239;}else{newValue|=16;}
  }
  if(channel==2)
  { 
      if(isTone==true){newValue&=251;}else{newValue|=4;}
      if(isNoise==true){newValue&=223;}else{newValue|=32;}
  }
  SOUND(7,newValue);
  return;
}



/* =============================================================================
 PlayEnvelope(shape) 

 Function : Set envelope type.
            Plays the sound on channels that have a volume of 16.
 Input    : [char] Envelope shape (0-15) (see envelope shapes definitions)
 Output   : -
============================================================================= */
void PlayEnvelope(char shape){
  SOUND(13,shape);
  return;
}




/*  
// DUMP 

;buffer to intern AY fast copy  ####################
_RAM2PSG::
  ld HL,#AY0_RAM
  call RAM2AY0
  ret


;RAM2PSG fast copy  ####################
RAM2AY0:
  ;internal PSG
  xor A	
	ld BC,#0x0DA1  ;0D=13 > num de regs; A1 > port
ILOOP:
  out (#0xA0),A
  inc A
  outi  
  JR NZ,ILOOP
  
  ;Envelope wave (reg 13)    
  ld A,(HL)
  cp #0  
  ret Z
  
  dec A  ;el valor esta incrementado para usar 0 como nulo
  ld E,A
  ld D,#13
  jp SET_SND_PSG  ;si no es 0, dispara la envolvente

*/






