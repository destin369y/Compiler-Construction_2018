	read => r2		// read statement
	loadI 1 => r0		// index variable initialization
	cmp_LE r0, r2 => r4	// index < UB
	cbr r4 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	loadI 1 => r1		// index variable initialization
	cmp_LE r1, r2 => r5	// index < UB
	cbr r5 -> L2, L3	// branch around loop
L2:	nop			// loop header label
	loadI 0 => r6		// start accum at 0
	subI r0, 0 => r7
	multI r7, 100 => r7
	add r6, r7 => r6
	subI r1, 0 => r7
	add r6, r7 => r6
	multI r6, 4 => r6	//  x sizeof(int)
	loadI 4 => r8		// base address of array Array
	add r8, r6 => r8	// base + offset
	read => r9		// read statement
	store r9 => r8		// value from read
	addI r1, 1 => r1	// update index
	cmp_LE r1, r2 => r10	// index < UB
	cbr r10 -> L2, L3	// branch back to loop header
L3:	nop			// loop exit label
	addI r0, 1 => r0	// update index
	cmp_LE r0, r2 => r11	// index < UB
	cbr r11 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	loadI 0 => r3		// source-level assignment
	loadI 1 => r0		// index variable initialization
	cmp_LE r0, r2 => r12	// index < UB
	cbr r12 -> L4, L5	// branch around loop
L4:	nop			// loop header label
	loadI 1 => r1		// index variable initialization
	cmp_LE r1, r2 => r13	// index < UB
	cbr r13 -> L6, L7	// branch around loop
L6:	nop			// loop header label
	loadI 0 => r14		// start accum at 0
	subI r0, 0 => r15
	multI r15, 100 => r15
	add r14, r15 => r14
	subI r1, 0 => r15
	add r14, r15 => r14
	multI r14, 4 => r14	//  x sizeof(int)
	loadI 4 => r16		// base address of array Array
	add r16, r14 => r16	// base + offset
	load r16 => r17		// LValue -> RValue
	add r3, r17 => r18
	i2i r18 => r3		// source-level assignment
	addI r1, 1 => r1	// update index
	cmp_LE r1, r2 => r19	// index < UB
	cbr r19 -> L6, L7	// branch back to loop header
L7:	nop			// loop exit label
	addI r0, 1 => r0	// update index
	cmp_LE r0, r2 => r20	// index < UB
	cbr r20 -> L4, L5	// branch back to loop header
L5:	nop			// loop exit label
	write r3	
	halt			// stop execution
