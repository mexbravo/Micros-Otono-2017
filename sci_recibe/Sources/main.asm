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
C_HRS						DS.B				1
C_MIN						DS.B				1
C_SEG						DS.B				1

C_DAY						DS.B				1
C_MON					DS.B				1
C_YER						DS.B				1

R_BUFF					DS.B				15
R_EDO						DS.B				1
R_POINT					DS.B				2
;
; code section
;
            					ORG    				ROMStart
 
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

mainLoop:				BRA				mainLoop								
								

;**************************************************************
;* INIT - Inicializo los perifericos.             *
;**************************************************************		
INIT:		
INIT_SCI:				MOV			#$00,SCIBDH				; inicializa puerto serial 19200,8,1,n
								MOV			#$1A,SCIBDL
								MOV			#$00,SCIC1
								MOV			#$2C,SCIC2					;interrupcion de  recibir
								
								MOV			#$00,R_EDO
								
								
								RTS				

;**************************************************************
;* ISR_SCIR: - rutina de atencion al al interrupcion del SCI Recibe           *
;**************************************************************
ISR_SCIR:				PSHH
								LDA				SCIS1							;LIMPIA ;LA BANDERA RDRF
								LDA				R_EDO
								CBEQA			#$00,R_EDO_0
								CBEQA			#$01,R_EDO_1
								LDA				SCID
								JMP				FIN_ISR_SCIR
								
								
R_EDO_0					LDA				SCID
								STA				R_BUFF
								MOV			#$01,R_EDO
								LDHX			#R_BUFF+1
								STHX			R_POINT	
								JMP				FIN_ISR_SCIR
								
R_EDO_1					LDHX			R_POINT
								LDA				SCID
								CBEQA			#$0A,R_INST
								STA				0,X
								AIX				#$1
								STHX			R_POINT
								BRA				FIN_ISR_SCIR						

R_INST						LDA				R_BUFF
								CBEQA			#'S',CMD_S
								CBEQA			#'C',CMD_C
								CBEQA			#'T',CMD_T
								CBEQA			#'D',CMD_D
								BRA				FIN_ISR_SCIR
								
								
CMD_S					MOV			#$00,R_EDO
								BRA				FIN_ISR_SCIR					; TO DO

CMD_C					MOV			#$00,R_EDO
								BRA				FIN_ISR_SCIR					; to do

CMD_T					LDA				R_BUFF+1
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_HRS
								LDA				R_BUFF+2
								SUB				#$30
								ADD				C_HRS
															
								LDA				R_BUFF+3
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_MIN
								LDA				R_BUFF+4
								SUB				#$30
								ADD				C_MIN
								
								LDA				R_BUFF+5
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_SEG
								LDA				R_BUFF+6
								SUB				#$30
								ADD				C_SEG
								
								MOV			#$00,R_EDO
								BRA				FIN_ISR_SCIR
								
CMD_D					LDA				R_BUFF+1
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_DAY
								LDA				R_BUFF+2
								SUB				#$30
								ADD				C_DAY
															
								LDA				R_BUFF+3
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_MON
								LDA				R_BUFF+4
								SUB				#$30
								ADD				C_MON
								
								LDA				R_BUFF+5
								SUB				#$30
								LDX				#$0A
								MUL
								STA				C_YER
								LDA				R_BUFF+6
								SUB				#$30
								ADD				C_YER
								
								MOV			#$00,R_EDO
								BRA				FIN_ISR_SCIR							
								
								
								
									
FIN_ISR_SCIR:		PULH
								RTI

;**************************************************************
;*                 Interrupt Vectors                          *
;**************************************************************

			ORG					$FFDC
			DC.W  				ISR_SCIR			;17 SCI receive
			
			ORG					$FFFE
			DC.W  				_Startup			;00 Reset
