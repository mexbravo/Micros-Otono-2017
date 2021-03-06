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
            ORG    			Z_RAMStart         ; Insert your data definition here
SEG: 						DS.B   	1
MIN: 						DS.B   	1
HRS: 						DS.B   	1
FLAG:						DS.B		1
EDOS:						DS.B		1
CRONO:					DS.B		10
;
; code section
;
            					ORG    				ROMStart
 WELCOME:			DC.B 				"CRONO IBERO",$0A,$0D,$00          

_Startup:				LDHX   			#RAMEnd+1       	 ; initialize the stack pointer
            					TXS
            					LDA				#$03							;FREE DOG, DESABILITA EL COP, habilita el boton de reset
        						STA				SOPT1
        						LDA				$FFAF							; sincronizar el reloj a 8Mhz
        						STA				ICSTRM
        						LDA				$FFAE
        						STA				ICSSC
            					
            					
Config:					BSR				INIT							; CONFIGURAR LOS PERIFERICOS

								CLI													; enable interrupts

								BSR				WEL_DIS


mainLoop:				LDA				EDOS
								CBEQA			#$3B,EDO_3B
								BRA    			mainLoop
								
EDO_3B:					BSR				DISPLAY
								CLR				EDOS
								BRA				mainLoop								
								
;**************************************************************
;*    *
;**************************************************************		
WEL_DIS:				LDHX			#WELCOME
								BSR				PRINT_SCI
								RTS
			
;**************************************************************
;*    *
;**************************************************************		
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
;**************************************************************
;*    *
;**************************************************************										
PRINT_SCI:			BRCLR			7,SCIS1,*
								LDA				,X
								BEQ				FIN_SCI
								STA				SCID
								INCX
								BRA				PRINT_SCI
FIN_SCI:					RTS									
		
;**************************************************************
;* INIT - Inicializo los perifericos.             *
;**************************************************************		
INIT:						
INIT_RTC:				MOV			#100,RTCMOD			; inicializa el real time counter
								MOV			#$1B,RTCSC	
								MOV			#':',CRONO+2
        						MOV			#':',CRONO+5
        						MOV			#$0D,CRONO+8
        						MOV			#$00,CRONO+9
        						CLR				SEG
        						CLR				MIN  						;MOV			#$05,MIN
        						CLR				HRS
        						CLR				FLAG
        						MOV			#$3B,EDOS
								


INIT_SCI:				MOV			#$00,SCIBDH				; inicializa puerto serial 19200,8,1,n
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$2C,SCIC2					;interrupcionde de recibir
								RTS				

;**************************************************************
;* ISR_SCIR: - rutina de atencion al al interrupcion del SCI Recibe           *
;**************************************************************
ISR_SCIR:				PSHH
								LDA				SCIS1							;LIMPIA ;LA BANDERA RDRF
								LDA				SCID
								CBEQA			#'S',CMD_S
								CBEQA			#'s',CMD_S
								CBEQA			#'C',CMD_C
								CBEQA			#'c',CMD_C
								BRA				FIN_ISR_SCIR
;START/STOP								
CMD_S:					LDA				FLAG							
								EOR				#%00000100					;#$04
								STA				FLAG
								BRA				FIN_ISR_SCIR
								
;CLEAR
CMD_C:					CLR				SEG
        						CLR				MIN  						
        						CLR				HRS
								BRA				FIN_ISR_SCIR								


FIN_ISR_SCIR:		PULH
								RTI




;**************************************************************
;* ISR_RTC: - rutina de atencion al al interrupcion del RTC             *
;**************************************************************
ISR_RTC:					PSHH
								BSET			7,RTCSC					;LIMPIA LA BANDERA DE INTERRUPCION
								BRSET			2,FLAG,INIC_RTC
								BRA				FIN_ISR_RTC
								
INIC_RTC:				MOV			#$3B,EDOS
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


FIN_ISR_RTC:			PULH
								RTI
			


;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************

            ORG					$FFCC
			DC.W  				ISR_RTC			;25 Real-time interrupt
			ORG					$FFDC
			DC.W  				ISR_SCIR			;17 SCI receive
			ORG					$FFFE
			DC.W  				_Startup			;00 Reset
