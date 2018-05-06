	read => r3		// read statement
	loadI 0 => r4		// source-level assignment
	loadI 1 => r0		// index variable initialization
	cmp_LE r0, r3 => r5	// index < UB
	cbr r5 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	loadI 1 => r1		// index variable initialization
	cmp_LE r1, r3 => r6	// index < UB
	cbr r6 -> L2, L3	// branch around loop
L2:	nop			// loop header label
	loadI 1 => r2		// index variable initialization
	cmp_LE r2, r3 => r7	// index < UB
	cbr r7 -> L4, L5	// branch around loop
L4:	nop			// loop header label
	mult r3, r3 => r8
	mult r8, r3 => r9
	mult r0, r9 => r10
	mult r3, r3 => r11
	mult r1, r11 => r12
	add r10, r12 => r13
	add r13, r2 => r14
	i2i r14 => r4		// source-level assignment
	addI r2, 1 => r2	// update index
	cmp_LE r2, r3 => r15	// index < UB
	cbr r15 -> L4, L5	// branch back to loop header
L5:	nop			// loop exit label
	addI r1, 1 => r1	// update index
	cmp_LE r1, r3 => r16	// index < UB
	cbr r16 -> L2, L3	// branch back to loop header
L3:	nop			// loop exit label
	addI r0, 1 => r0	// update index
	cmp_LE r0, r3 => r17	// index < UB
	cbr r17 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	write r4	
	halt			// stop execution
