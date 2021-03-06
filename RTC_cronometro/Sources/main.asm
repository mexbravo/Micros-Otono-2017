;*******************************************************************
;* This stationery serves as the framework for a user application. *
;* For a more comprehensive program that demonstrates the more     *
;* advanced functionality of this processor, please see the        *
;* demonstration applications, located in the examples             *
;* subdirectory of the "Freescale CodeWarrior for HC08" program    *
;* directory.                                                      *
;*******************************************************************

; Include derivative-specific definitions
            INCLUDE 'derivative.inc'
            
;
; export symbols
;
            XDEF _Startup
            ABSENTRY _Startup

;
; variable/data section
;
            			ORG    Z_RAMStart         ; Insert your data definition here
SEG: 				DS.B   	1
MIN: 				DS.B   	1
HRS: 				DS.B   	1
FLAG:				DS.B		1
CRONO:			DS.B		10
;
; code section
;
            					ORG    			ROMStart
       					
            					
            					
_Startup:				LDHX   		#RAMEnd+1        ; initialize the stack pointer
            					TXS
            					LDA				#$03					;FREE DOG, DESABILITA EL COP, habilita el boton de reset
        						STA				SOPT1
        						
        						LDA				$FFAF
        						STA				ICSTRM
        						LDA				$FFAE
        						STA				ICSSC
        						
        						JSR				init
        						
        						MOV			#':',CRONO+2
        						MOV			#':',CRONO+5
        						MOV			#$0A,CRONO+8
        						MOV			#$00,CRONO+9
        						
        						
        						CLR				SEG
        						CLR				MIN  						;MOV			#$05,MIN
        						CLR				HRS
        						CLR				FLAG
        						
        						CLI											; enable interrupts

mainLoop:				BRCLR			0,FLAG,*
								BSR				DISPLAY		
								BCLR			0,FLAG
								BRA    			mainLoop
			
DISPLAY:				LDA				HRS
								CLRH
								LDX				#$0A
								DIV
								ADD				#$30
								STA				CRONO
								PSHH
								PULA
								ADD				#$30
								STA				CRONO+1
								
								LDA				MIN
								CLRH
								LDX				#$0A
								DIV
								ADD				#$30
								STA				CRONO+3
								PSHH
								PULA
								ADD				#$30
								STA				CRONO+4	

								LDA				SEG
								CLRH
								LDX				#$0A
								DIV
								ADD				#$30
								STA				CRONO+6
								PSHH
								PULA
								ADD				#$30
								STA				CRONO+7	


								LDHX			#CRONO
								BSR				PRINT_SCI
								RTS



PRINT_SCI:			BRCLR			7,SCIS1,*
								LDA				,X
								BEQ				FIN_SCI
								STA				SCID
								INCX
								BRA				PRINT_SCI
FIN_SCI:					RTS	



ISR_RTC:					PSHH
								BSET			7,RTCSC					;LIMPIA LA BANDERA DE INTERRUPCION
								LDA				SEG
								INCA
								CBEQA			#60,CMIN
								STA				SEG
								BRA				FIN_ISR_RTC
CMIN:						CLR				SEG
								LDA				MIN
								INCA
								CBEQA			#60,CHRS
								STA				MIN
								BRA				FIN_ISR_RTC
CHRS:						CLR				MIN
								LDA				HRS
								INCA
								CBEQA			#25,CDAY
								STA				HRS
								BRA				FIN_ISR_RTC
CDAY:						CLR				HRS
FIN_ISR_RTC:			BSET			0,FLAG
								PULH
								RTI



init:							
INIT_RTC:				MOV			#100,RTCMOD
								MOV			#$1B,RTCSC	
INIT_SCI:				MOV			#$00,SCIBDH
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$0C,SCIC2
								RTS								
											
								RTS


;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************
 			ORG				$FFCC
			DC.W  			ISR_RTC			; REAL TIEM COUNTER

            ORG				$FFFE
			DC.W  			_Startup			; Reset
