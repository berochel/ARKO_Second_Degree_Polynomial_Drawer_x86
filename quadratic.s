;
;    Second Degree Polynomial Drawer by Jaroslaw Zabuski
;
;	 **LICENSE**
;
;	 This file is a part of Second Degree Polynomial .
;
;	 Second Degree Polynomial Drawer is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    Second Degree Polynomial Drawer is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
;    GNU General Public License for more details.
;
;   You should have received a copy of the GNU General Public License
;   along with Second Degree Polynomial Drawer.  If not, see <http://www.gnu.org/licenses/>.
;
	global		quadratic_draw_body
	section		.text



;				rdi		      rsi	  rdx       xmm0
; void quadratic_draw_body(unsigned char *pPixelBuffer, int width, int height, double A,
; double B, double C, double S);
;   xmm1      xmm2      xmm3 
  


quadratic_draw_body:
	push rbp
	mov rbp, rsp


; Registers meaning in this part:
;  ----------------------------------------------------------------------------
; | rax	| COUNTER			| rbx |	(must be preserved)		|
; | rcx	|				| rdx | (input)buffer height		|
; | rbp	| (must be preserved)		| rsp |					|
; | rsi	| (input)buffer width		| rdi | (input)buffer base address	|
; | r8	|				| r9  |					|
; | r10	| WRITE ADDRESS			| r11 |					|
;------------------------------------------------------------------------------

	;Draw y axis on position *(buf + width/2 * 4), where 4 is bytes per pixel
	

	mov		rax, rdx 

	lea		r10, [rdi + rsi*2]
	mov		[r10], DWORD 0x00000000

	dec		rax
	jz		xaxisinit

yaxisloop:

	lea		r10, [r10, rsi*4]
	mov		[r10], DWORD 0x00000000
	dec		rax
	jg		yaxisloop


; Registers meaning in this part:
;  ----------------------------------------------------------------------------
; | rax	| TEMPORARY REGISTER		| rbx |	(must be preserved)		|
; | rcx	|				| rdx | (DESTROYED BY MUL)buffer height	|
; | rbp	| (must be preserved)		| rsp |					|
; | rsi	| (input)buffer width		| rdi | (input)buffer base address	|
; | r8	|				| r9  |	(moved from rdx)buffer height	|
; | r10	| WRITE ADDRESS			| r11 |	COUNTER				|
;------------------------------------------------------------------------------

	;Draw x axis on position (height/2)*width*4

xaxisinit:
	
	mov		r11, rsi

	;Save buffer height from being destroyed by mul operation

	mov 		r9, rdx
	
	mov		rax, rsi
	mul		edx
	lea		r10, [rdi + rax*2]
	mov		[r10], DWORD 0x00000000

	
	dec		r11
	jz		graph

xaxisloop:

	add		r10, 4
	mov		[r10], DWORD 0x00000000
	dec		r11
	jg		xaxisloop

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Registers in this section:
;  ----------------------------------------------------------------------------
; | rax	| TEMPORARY REGISTER		| rbx |	(must be preserved)		|
; | rcx	| X OFFSET IN BUFFER 		| rdx | (DESTROYED BY MUL)		|
; | rbp	| (must be preserved)		| rsp |					|
; | rsi	| (input)buffer width		| rdi | (input)buffer base address	|
; | r8	| Y OFFSET IN BUFFER 		| r9  |	(moved from rdx)buffer height	|
; | r10	| WRITE ADDRESS			| r11 |					|
; | xmm0| (input)A			| xmm1| (input)B			|
; | xmm2| (input)C			| xmm3| (input)S			|
; | xmm4| X IN DOUBLE			| xmm5| Y IN DOUBLE			|
; | xmm6| TEMPORARY REGISTER		| xmm7|	TEMPORARY REGISTER		|
; | xmm8| TEMPORARY REGISTER		|

graph:
	
	;Setting up x value of currently drawn point

	mov		rcx, -1
	cvtsi2sd	xmm4, rcx

	;Setting up x offset in buffer from 0 (from left)

	mov		rcx, 0

iter_graph:

	;y = C

	movsd		xmm5, xmm2

	;y = Bx + C

	movsd		xmm7, xmm4
	mulsd		xmm7, xmm1
	addsd		xmm5, xmm7

	;y = Ax^2 + Bx + C

	movsd		xmm7, xmm0
	mulsd		xmm7, xmm4
	mulsd		xmm7, xmm4
	addsd		xmm5, xmm7

	;y = y + 1.0 to be consisent with x axis starting in the middle of buffer

	mov		rax, 1
	cvtsi2sd	xmm6, rax
	addsd		xmm5, xmm6

	;y *= (height/2) to get y value in relation to buffer height
	mov		rax, r9
	sar		rax, 1
	cvtsi2sd	xmm6, rax
	mulsd		xmm5, xmm6

	;Conversion from double to index
	cvtsd2si	r8, xmm5

	;Is y out of bounds? If yes, skip putting this pixel
	sub		r8, 0
	jl		calculateNewX
	mov		rax, r9
	sub		rax, r8
	jle		calculateNewX

	;Calculate address on buffer
	mov		rax, r8
	mul		rsi
	lea		r10, [rdi, rax*4]
	lea		r10, [r10, rcx*4]

	;Put pixel on buffer
	mov		[r10], DWORD 0x00FF0000

calculateNewX:
	;Calculating derivative of drawn function and using it to get the next x index

	;Derivative = B
	movsd		xmm6, xmm1

	;Derivative = 2*A*x + B
	movsd		xmm7, xmm4
	mulsd		xmm7, xmm0
	mov		rax, 2
	cvtsi2sd	xmm8, rax
	mulsd		xmm7, xmm8
	addsd		xmm6, xmm7

	;Calculating step of x index
	;Step = S / sqrt(1 + derivative * derivative)

	mulsd		xmm6, xmm6
	mov		rax, 1
	cvtsi2sd	xmm7, rax
	addsd		xmm6, xmm7
	sqrtsd		xmm6, xmm6
	movsd		xmm7, xmm3	
	divsd		xmm7, xmm6

	;Add step to x

	addsd		xmm4, xmm7

	;Move calculated x to temporary value

	movsd		xmm7, xmm4

	;Calculate new x offset position
	;x = x + 1.0

	mov		rax, 1
	cvtsi2sd	xmm6, rax
	addsd		xmm7, xmm6

	; x *= (width/2) to get x value in relation to buffer width

	mov		rax, rsi
	sar		rax, 1
	cvtsi2sd	xmm6, rax
	mulsd		xmm7, xmm6

	;Conversion from double to index

	cvtsd2si	rcx, xmm7

	;Is x out of bounds? if yes, stop this procedure

	mov		rax, rcx
	sub		rax, rsi
	jle		iter_graph

end:
	mov rsp, rbp
	pop rbp
	ret
