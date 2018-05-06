	read => r6		// read statement
	loadI 47 => r7		// int load of integer literal
	cmp_GT r6, r7 => r8
	cbr r8 -> L0, L1	// if-then or if-then-else
L0:	nop			// then part
	loadI 0 => r9		// int load of integer literal
	write r9	
	br -> L2		// branch to exit label
L1:	nop			// else part
	loadI 0 => r2		// index variable initialization
	subI r6, 1 => r10
	cmp_LE r2, r10 => r11	// index < UB
	cbr r11 -> L3, L4	// branch around loop
L3:	nop			// loop header label
	loadI 0 => r3		// source-level assignment
	loadI 1 => r4		// source-level assignment
	loadI 0 => r5		// source-level assignment
	loadI 2 => r12		// int load of integer literal
	cmp_LT r2, r12 => r13
	cbr r13 -> L5, L6	// if-then or if-then-else
L5:	nop			// then part
	i2i r2 => r0		// source-level assignment
	br -> L7		// branch to exit label
L6:	nop			// else part
	loadI 2 => r1		// index variable initialization
	cmp_LE r1, r2 => r14	// index < UB
	cbr r14 -> L8, L9	// branch around loop
L8:	nop			// loop header label
	add r4, r5 => r15
	i2i r15 => r3		// source-level assignment
	i2i r4 => r5		// source-level assignment
	i2i r3 => r4		// source-level assignment
	addI r1, 1 => r1	// update index
	cmp_LE r1, r2 => r16	// index < UB
	cbr r16 -> L8, L9	// branch back to loop header
L9:	nop			// loop exit label
	i2i r3 => r0		// source-level assignment
	br -> L7		// no fall-through in iloc
L7:	nop			// exit label for if-then-else
	write r0	
	addI r2, 1 => r2	// update index
	cmp_LE r2, r10 => r17	// index < UB
	cbr r17 -> L3, L4	// branch back to loop header
L4:	nop			// loop exit label
	br -> L2		// no fall-through in iloc
L2:	nop			// exit label for if-then-else
	halt			// stop execution
