	read => r1		// read statement
	loadI 0 => r0		// index variable initialization
	subI r1, 1 => r4
	cmp_LE r0, r4 => r5	// index < UB
	cbr r5 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	loadI 0 => r6		// start accum at 0
	subI r0, 0 => r7
	add r6, r7 => r6
	multI r6, 4 => r6	//  x sizeof(int)
	loadI 20 => r8		// base address of array DATA
	add r8, r6 => r8	// base + offset
	read => r9		// read statement
	store r9 => r8		// value from read
	addI r0, 1 => r0	// update index
	cmp_LE r0, r4 => r10	// index < UB
	cbr r10 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	loadI 1 => r2		// source-level assignment
	loadI 1 => r11		// int load of integer literal
	cmp_EQ r2, r11 => r12
	cbr r12 -> L2, L3
L2:	nop			// top of while loop
	loadI 0 => r2		// source-level assignment
	loadI 0 => r0		// index variable initialization
	subI r1, 2 => r13
	cmp_LE r0, r13 => r14	// index < UB
	cbr r14 -> L4, L5	// branch around loop
L4:	nop			// loop header label
	loadI 0 => r15		// start accum at 0
	subI r0, 0 => r16
	add r15, r16 => r15
	multI r15, 4 => r15	//  x sizeof(int)
	loadI 20 => r17		// base address of array DATA
	add r17, r15 => r17	// base + offset
	load r17 => r18		// LValue -> RValue
	addI r0, 1 => r19
	loadI 0 => r20		// start accum at 0
	subI r19, 0 => r21
	add r20, r21 => r20
	multI r20, 4 => r20	//  x sizeof(int)
	loadI 20 => r22		// base address of array DATA
	add r22, r20 => r22	// base + offset
	load r22 => r23		// LValue -> RValue
	cmp_GT r18, r23 => r24
	cbr r24 -> L6, L7	// if-then or if-then-else
L6:	nop			// then part
	loadI 1 => r2		// source-level assignment
	loadI 0 => r25		// start accum at 0
	subI r0, 0 => r26
	add r25, r26 => r25
	multI r25, 4 => r25	//  x sizeof(int)
	loadI 20 => r27		// base address of array DATA
	add r27, r25 => r27	// base + offset
	load r27 => r28		// LValue -> RValue
	i2i r28 => r3		// source-level assignment
	loadI 0 => r29		// start accum at 0
	subI r0, 0 => r30
	add r29, r30 => r29
	multI r29, 4 => r29	//  x sizeof(int)
	loadI 20 => r31		// base address of array DATA
	add r31, r29 => r31	// base + offset
	addI r0, 1 => r32
	loadI 0 => r33		// start accum at 0
	subI r32, 0 => r34
	add r33, r34 => r33
	multI r33, 4 => r33	//  x sizeof(int)
	loadI 20 => r35		// base address of array DATA
	add r35, r33 => r35	// base + offset
	load r35 => r36		// LValue -> RValue
	store r36 => r31		// int array store
	addI r0, 1 => r37
	loadI 0 => r38		// start accum at 0
	subI r37, 0 => r39
	add r38, r39 => r38
	multI r38, 4 => r38	//  x sizeof(int)
	loadI 20 => r40		// base address of array DATA
	add r40, r38 => r40	// base + offset
	store r3 => r40		// int array store
L7:	nop			// exit label for if-then
	addI r0, 1 => r0	// update index
	cmp_LE r0, r13 => r41	// index < UB
	cbr r41 -> L4, L5	// branch back to loop header
L5:	nop			// loop exit label
	loadI 1 => r11		// int load of integer literal
	cmp_EQ r2, r11 => r12
	cbr r12 -> L2, L3
L3:	nop			// bottom of while loop
	loadI 0 => r0		// index variable initialization
	subI r1, 1 => r42
	cmp_LE r0, r42 => r43	// index < UB
	cbr r43 -> L9, L10	// branch around loop
L9:	nop			// loop header label
	loadI 0 => r44		// start accum at 0
	subI r0, 0 => r45
	add r44, r45 => r44
	multI r44, 4 => r44	//  x sizeof(int)
	loadI 20 => r46		// base address of array DATA
	add r46, r44 => r46	// base + offset
	load r46 => r47		// LValue -> RValue
	write r47	
	addI r0, 1 => r0	// update index
	cmp_LE r0, r42 => r48	// index < UB
	cbr r48 -> L9, L10	// branch back to loop header
L10:	nop			// loop exit label
	halt			// stop execution
