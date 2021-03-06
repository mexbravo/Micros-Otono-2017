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
           			ORG    		Z_RAMStart         ; Insert your data definition here
CONT1:  		DS.B   		1



;
; code section
;
            		ORG     ROMStart
            		
_Startup:   	LDHX   	#RAMEnd+1        ; initialize the stack pointer
            		TXS
            
        			LDA			#$02					;FREE DOG, DESABILITA EL COP
        			STA			SOPT1
            
            
INIT_PORT:	LDA		#$FF				;PUERTO C DE SALIDA
						STA		PTCDD				;MOV		#$FF,PTCDD
					    
					    LDA		#$00				; PUERTO A DE ENTRADA
						STA		PTADD    
						
						CLR		CONT1     		

mainLoop:		LDA		PTAD
						AND	#$01
						BEQ		DECRE
INCRE				INC   	CONT1		
						LDA		CONT1
						CMPA	#09
						BLS     DISPLAY
						CLR		CONT1
						BRA		DISPLAY


DECRE				DEC		CONT1
						BPL		DISPLAY		
						MOV	#$09,CONT1
					
DISPLAY			LDA		CONT1
						STA		PTCD
						BRA    mainLoop
			

;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************

            ORG			$FFFE
			DC.W  		_Startup			; Reset
