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
VELM1: 				DS.B   		1
VELM2:				DS.B			1
;
; code section
;
            				ORG    		ROMStart
            

_Startup:            LDHX   	#RAMEnd+1        ; initialize the stack pointer
        					 TXS
        					 LDA				#$03							;FREE DOG, DESABILITA EL COP, habilita el boton de reset
        					STA				SOPT1
        					LDA				$FFAF							; sincronizar el reloj a 8Mhz
        					STA				ICSTRM
        					LDA				$FFAE
        					STA				ICSSC
        				
        					BSR				CONFIG
        					 MOV			#$05,PTCD
            				 CLI											; enable interrupts
					
								
								
mainLoop:          BRA    mainLoop
			
CONFIG:					MOV			#$00,SCIBDH				; inicializa puerto serial 19200,8,1,n
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$2C,SCIC2					;interrupcion de  recibir

								MOV			#$0F,PTCDD
								
								MOV			#$4B,TPM1SC
								MOV			#$03,TPM1MODH		;03E8
								MOV			#$E8,TPM1MODL
								
								MOV			#50,VELM1
								MOV			#50,VELM2
								MOV			#$68,TPM1C0SC
								
								
								LDA				VELM1
								LDX				#$0A
								MUL
								STX				TPM1C0VH		
								STA				TPM1C0VL
									
								LDA				VELM2
								LDX				#$0A
								MUL
								STX				TPM1C1VH		
								STA				TPM1C1VL
								
								
								RTS

ISR_SCIR:				PSHH
								LDA				SCIS1
								LDA				SCID
								CBEQA			#'A',DER
								CBEQA			#'D',IZQ
								CBEQA			#'W',FOR
								CBEQA			#'S',BACK
								CBEQA			#'+',SUBE
								CBEQA			#'-',BAJA
								CBEQA			#'X',STOP
								BRA    			FIN_ISR_SCIR

DER							MOV			#$09,PTCD
								BRA    			FIN_ISR_SCIR
IZQ							MOV			#$06,PTCD								
								BRA    			FIN_ISR_SCIR
FOR							MOV			#$05,PTCD
								BRA    			FIN_ISR_SCIR
BACK						MOV			#$0A,PTCD								
								BRA    			FIN_ISR_SCIR
STOP						CLR				VELM1
								CLR				VELM2
								BRA    			FIN_ISR_SCIR								
SUBE						INC				VELM1
								INC				VELM2
							    BRA    			FIN_ISR_SCIR
BAJA						DEC				VELM1	 
   								DEC				VELM2 
FIN_ISR_SCIR:		PULH
								RTI			


								
ISR_T1_TOF:			PULH
								LDA				TPM1SC
								BCLR			7,TPM1SC
								PSHH
								RTI




ISR_T1C0_CHOF:	PULH
								LDA				TPM1C0SC
								BCLR			7,TPM1C0SC
								LDA				VELM1
								LDX				#$0A
								MUL
								STX				TPM1C0VH		
								STA				TPM1C0VL
								PSHH
								RTI
								
ISR_T1C1_CHOF:	PULH
								LDA				TPM1C1SC
								BCLR			7,TPM1C1SC
								LDA				VELM2
								LDX				#$0A
								MUL
								STX				TPM1C1VH		
								STA				TPM1C1VL
								PSHH
								RTI								
;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************
							ORG			$FFDC
							DC.W		ISR_SCIR				;SCI R
							
							
							ORG			$FFE8
							DC.W		ISR_T1_TOF
							ORG			$FFF2
							DC.W		ISR_T1C1_CHOF
							ORG			$FFF4
							DC.W		ISR_T1C0_CHOF
            				ORG			$FFFE
							DC.W  		_Startup			; Reset
