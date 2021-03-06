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
; export symbols, HOLA CLASE
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
            ORG    ROMStart
            

_Startup:
            LDHX   #RAMEnd+1        ; initialize the stack pointer
            TXS
           	LDA  	SOPT1
           	AND		#$3F
           	STA  	SOPT1
           	
           	LDA		#$01
           	STA		PTCDD			;$0005
           	
mainLoop:	LDA  	#$01
			STA		PTCD
			
			LDHX	#$FFFF
ACA:		AIX		#-1
			CPHX	#$0000
			BNE		ACA
			
			
			LDA  	#$00
			STA		PTCD		;$0004
			
			
			LDHX	#$FFFF
ACA1:		AIX		#-1
			CPHX	#$0000
			BNE		ACA1
			
			
			
		    BRA    mainLoop
			

;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************

            ORG	$FFFE
			DC.W  _Startup			; Reset
