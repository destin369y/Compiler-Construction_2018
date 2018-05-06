	loadI 1 => r0		// source-level assignment
	loadI 2 => r1		// source-level assignment
	loadI 3 => r2		// source-level assignment
	loadI 1 => r3		// source-level assignment
	loadI 0 => r4		// source-level assignment
	cmp_LE r0, r1 => r5
	cbr r5 -> L0, L1	// if-then or if-then-else
L0:	nop			// then part
	loadI 0 => r6		// address for store
	store r3 => r6	
	output 0		// print statement
	br -> L2		// branch to exit label
L1:	nop			// else part
	loadI 0 => r7		// address for store
	store r4 => r7	
	output 0		// print statement
	br -> L2		// no fall-through in iloc
L2:	nop			// exit label for if-then-else
