	read => r5		// read statement
	mult r5, r5 => r8
	mult r8, r5 => r9
	i2i r9 => r7		// source-level assignment
	loadI 0 => r6		// source-level assignment
	loadI 1 => r0		// source-level assignment
	loadI 1 => r1		// source-level assignment
	loadI 1 => r2		// source-level assignment
	loadI 1 => r3		// index variable initialization
	cmp_LE r3, r7 => r10	// index < UB
	cbr r10 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	mult r5, r5 => r11
	mult r11, r5 => r12
	mult r0, r12 => r13
	mult r5, r5 => r14
	mult r1, r14 => r15
	add r13, r15 => r16
	add r16, r2 => r17
	i2i r17 => r6		// source-level assignment
	addI r2, 1 => r18
	i2i r18 => r2		// source-level assignment
	cmp_GT r2, r5 => r19
	cbr r19 -> L2, L3	// if-then or if-then-else
L2:	nop			// then part
	loadI 1 => r2		// source-level assignment
	addI r1, 1 => r20
	i2i r20 => r1		// source-level assignment
L3:	nop			// exit label for if-then
	cmp_GT r1, r5 => r21
	cbr r21 -> L5, L6	// if-then or if-then-else
L5:	nop			// then part
	loadI 1 => r1		// source-level assignment
	addI r0, 1 => r22
	i2i r22 => r0		// source-level assignment
L6:	nop			// exit label for if-then
	addI r3, 1 => r3	// update index
	cmp_LE r3, r7 => r23	// index < UB
	cbr r23 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	write r6	
	halt			// stop execution
