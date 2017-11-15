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
            ORG    RAMStart         ; Insert your data definition here


;
; code section
;
            			ORG    			ROMStart
_Startup:		LDHX   		#RAMEnd+1       		 ; initialize the stack pointer
            			TXS
            			LDA				#$03							;FREE DOG, DESABILITA EL COP, habilita el boton de reset
        				STA				SOPT1
        				LDA				$FFAF							; sincronizar el reloj a 8Mhz
        				STA				ICSTRM
        				LDA				$FFAE
        				STA				ICSSC
        				
        				BSR				CONFIG
            
            
            			CLI			; enable interrupts

mainLoop:				BRA    			mainLoop
			
CONFIG:					MOV			#$07,PTADD				;A0..A2 DE SALIDA

								MOV			#$00,SCIBDH				; inicializa puerto serial 19200,8,1,n
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$2C,SCIC2					;interrupcion de  recibir

								RTS
								
								
ISR_SCIR:				PSHH
								LDA				SCIS1
								LDA				SCID
								CMP			#$30
								BHS				SIGUE
								BRA				FIN_ISR_SCIR
SIGUE						CMP			#$37
								BLE				SIGUE1
								BRA				FIN_ISR_SCIR
SIGUE1					SUB				#$30
								STA				PTAD
FIN_ISR_SCIR:		PULH
								RTI			
								
;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************
			ORG			$FFDC
			DC.W		ISR_SCIR				;SCI R


            ORG			$FFFE
			DC.W  		_Startup			; Reset
