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
ExampleVar: DS.B   1

;
; code section
;
            					ORG    			ROMStart
  MSG:						DC.B			"HOLA MUNDO",$0A,$0D        
								DC.B			"CRUEL Y DESPIADADO",$0A,$0D,$00 
								
MSG1:						DC.B			"QUE BIEN",$0A,$0D,$00
MSG2:						DC.B			"QUE MAL",$0A,$0D,$00       								
								
								
_Startup:
            					LDHX   		#RAMEnd+1        ; initialize the stack pointer
            					TXS
            					LDA				#$02					;FREE DOG, DESABILITA EL COP
        						STA				SOPT1
        						
        						BSR				INIT_SCI
        						
        						LDHX			#MSG
								BSR				PRINT_SCI
								
								
mainLoop:				BRCLR			5,SCIS1,mainLoop					;SCIS1_RDRF
								LDA				SCIS1
								LDA				SCID
								CBEQ			'A',ESA
								CBEQ			'a',ESA
								CBEQ			'B',ESB
								BRA    			mainLoop

ESA							LDHX			#MSG1
								BSR				PRINT_SCI
								BRA    			mainLoop
ESB							LDHX			#MSG2
								BSR				PRINT_SCI
								BRA    			mainLoop								
		

INIT_SCI:				MOV			#$00,SCIBDH
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$0C,SCIC2
								RTS
								
PRINT_SCI:			BRCLR			7,SCIS1,*
								LDA				,X
								BEQ				FIN_SCI
								STA				SCID
								INCX
								BRA				PRINT_SCI
FIN_SCI					RTS							
								
								
;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************

            					ORG				$FFFE
								DC.W  			_Startup			; Reset
