	read => r4		// read statement
	loadI 0 => r0		// index variable initialization
	subI r4, 1 => r5
	cmp_LE r0, r5 => r6	// index < UB
	cbr r6 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	loadI 0 => r1		// index variable initialization
	subI r4, 1 => r7
	cmp_LE r1, r7 => r8	// index < UB
	cbr r8 -> L2, L3	// branch around loop
L2:	nop			// loop header label
	loadI 0 => r9		// start accum at 0
	subI r0, 0 => r10
	multI r10, 100 => r10
	add r9, r10 => r9
	subI r1, 0 => r10
	add r9, r10 => r9
	multI r9, 4 => r9	//  x sizeof(int)
	loadI 20 => r11		// base address of array A
	add r11, r9 => r11	// base + offset
	loadI 0 => r12		// int load of integer literal
	store r12 => r11		// int array store
	loadI 0 => r13		// start accum at 0
	subI r0, 0 => r14
	multI r14, 100 => r14
	add r13, r14 => r13
	subI r1, 0 => r14
	add r13, r14 => r13
	multI r13, 4 => r13	//  x sizeof(int)
	loadI 40020 => r15		// base address of array B
	add r15, r13 => r15	// base + offset
	loadI 0 => r16		// int load of integer literal
	store r16 => r15		// int array store
	loadI 0 => r17		// start accum at 0
	subI r0, 0 => r18
	multI r18, 100 => r18
	add r17, r18 => r17
	subI r1, 0 => r18
	add r17, r18 => r17
	multI r17, 4 => r17	//  x sizeof(int)
	loadI 80020 => r19		// base address of array C
	add r19, r17 => r19	// base + offset
	loadI 0 => r20		// int load of integer literal
	store r20 => r19		// int array store
	addI r1, 1 => r1	// update index
	cmp_LE r1, r7 => r21	// index < UB
	cbr r21 -> L2, L3	// branch back to loop header
L3:	nop			// loop exit label
	loadI 0 => r22		// start accum at 0
	subI r0, 0 => r23
	multI r23, 100 => r23
	add r22, r23 => r22
	subI r0, 0 => r23
	add r22, r23 => r22
	multI r22, 4 => r22	//  x sizeof(int)
	loadI 20 => r24		// base address of array A
	add r24, r22 => r24	// base + offset
	loadI 1 => r25		// int load of integer literal
	store r25 => r24		// int array store
	loadI 0 => r26		// start accum at 0
	subI r0, 0 => r27
	multI r27, 100 => r27
	add r26, r27 => r26
	subI r0, 0 => r27
	add r26, r27 => r26
	multI r26, 4 => r26	//  x sizeof(int)
	loadI 40020 => r28		// base address of array B
	add r28, r26 => r28	// base + offset
	loadI 1 => r29		// int load of integer literal
	store r29 => r28		// int array store
	addI r0, 1 => r0	// update index
	cmp_LE r0, r5 => r30	// index < UB
	cbr r30 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	loadI 0 => r0		// index variable initialization
	subI r4, 1 => r31
	cmp_LE r0, r31 => r32	// index < UB
	cbr r32 -> L4, L5	// branch around loop
L4:	nop			// loop header label
	loadI 0 => r1		// index variable initialization
	subI r4, 1 => r33
	cmp_LE r1, r33 => r34	// index < UB
	cbr r34 -> L6, L7	// branch around loop
L6:	nop			// loop header label
	loadI 0 => r2		// index variable initialization
	subI r4, 1 => r35
	cmp_LE r2, r35 => r36	// index < UB
	cbr r36 -> L8, L9	// branch around loop
L8:	nop			// loop header label
	loadI 0 => r37		// start accum at 0
	subI r0, 0 => r38
	multI r38, 100 => r38
	add r37, r38 => r37
	subI r1, 0 => r38
	add r37, r38 => r37
	multI r37, 4 => r37	//  x sizeof(int)
	loadI 80020 => r39		// base address of array C
	add r39, r37 => r39	// base + offset
	loadI 0 => r40		// start accum at 0
	subI r0, 0 => r41
	multI r41, 100 => r41
	add r40, r41 => r40
	subI r1, 0 => r41
	add r40, r41 => r40
	multI r40, 4 => r40	//  x sizeof(int)
	loadI 80020 => r42		// base address of array C
	add r42, r40 => r42	// base + offset
	load r42 => r43		// LValue -> RValue
	loadI 0 => r44		// start accum at 0
	subI r0, 0 => r45
	multI r45, 100 => r45
	add r44, r45 => r44
	subI r2, 0 => r45
	add r44, r45 => r44
	multI r44, 4 => r44	//  x sizeof(int)
	loadI 20 => r46		// base address of array A
	add r46, r44 => r46	// base + offset
	load r46 => r47		// LValue -> RValue
	loadI 0 => r48		// start accum at 0
	subI r2, 0 => r49
	multI r49, 100 => r49
	add r48, r49 => r48
	subI r1, 0 => r49
	add r48, r49 => r48
	multI r48, 4 => r48	//  x sizeof(int)
	loadI 40020 => r50		// base address of array B
	add r50, r48 => r50	// base + offset
	load r50 => r51		// LValue -> RValue
	mult r47, r51 => r52
	add r43, r52 => r53
	store r53 => r39		// int array store
	addI r2, 1 => r2	// update index
	cmp_LE r2, r35 => r54	// index < UB
	cbr r54 -> L8, L9	// branch back to loop header
L9:	nop			// loop exit label
	addI r1, 1 => r1	// update index
	cmp_LE r1, r33 => r55	// index < UB
	cbr r55 -> L6, L7	// branch back to loop header
L7:	nop			// loop exit label
	addI r0, 1 => r0	// update index
	cmp_LE r0, r31 => r56	// index < UB
	cbr r56 -> L4, L5	// branch back to loop header
L5:	nop			// loop exit label
	loadI 0 => r2		// source-level assignment
	loadI 0 => r0		// index variable initialization
	subI r4, 1 => r57
	cmp_LE r0, r57 => r58	// index < UB
	cbr r58 -> L10, L11	// branch around loop
L10:	nop			// loop header label
	loadI 0 => r1		// index variable initialization
	subI r4, 1 => r59
	cmp_LE r1, r59 => r60	// index < UB
	cbr r60 -> L12, L13	// branch around loop
L12:	nop			// loop header label
	cmp_NE r0, r1 => r61
	loadI 0 => r62		// start accum at 0
	subI r0, 0 => r63
	multI r63, 100 => r63
	add r62, r63 => r62
	subI r1, 0 => r63
	add r62, r63 => r62
	multI r62, 4 => r62	//  x sizeof(int)
	loadI 20 => r64		// base address of array A
	add r64, r62 => r64	// base + offset
	load r64 => r65		// LValue -> RValue
	loadI 0 => r66		// start accum at 0
	subI r0, 0 => r67
	multI r67, 100 => r67
	add r66, r67 => r66
	subI r1, 0 => r67
	add r66, r67 => r66
	multI r66, 4 => r66	//  x sizeof(int)
	loadI 40020 => r68		// base address of array B
	add r68, r66 => r68	// base + offset
	load r68 => r69		// LValue -> RValue
	add r65, r69 => r70
	loadI 0 => r71		// start accum at 0
	subI r0, 0 => r72
	multI r72, 100 => r72
	add r71, r72 => r71
	subI r1, 0 => r72
	add r71, r72 => r71
	multI r71, 4 => r71	//  x sizeof(int)
	loadI 80020 => r73		// base address of array C
	add r73, r71 => r73	// base + offset
	load r73 => r74		// LValue -> RValue
	add r70, r74 => r75
	loadI 0 => r76		// int load of integer literal
	cmp_NE r75, r76 => r77
	// 'AND' case
	and r61, r77 => r78
	cbr r78 -> L14, L15	// if-then or if-then-else
L14:	nop			// then part
	loadI 1 => r2		// source-level assignment
L15:	nop			// exit label for if-then
	cmp_EQ r0, r1 => r79
	loadI 0 => r80		// start accum at 0
	subI r0, 0 => r81
	multI r81, 100 => r81
	add r80, r81 => r80
	subI r1, 0 => r81
	add r80, r81 => r80
	multI r80, 4 => r80	//  x sizeof(int)
	loadI 20 => r82		// base address of array A
	add r82, r80 => r82	// base + offset
	load r82 => r83		// LValue -> RValue
	loadI 0 => r84		// start accum at 0
	subI r0, 0 => r85
	multI r85, 100 => r85
	add r84, r85 => r84
	subI r1, 0 => r85
	add r84, r85 => r84
	multI r84, 4 => r84	//  x sizeof(int)
	loadI 40020 => r86		// base address of array B
	add r86, r84 => r86	// base + offset
	load r86 => r87		// LValue -> RValue
	add r83, r87 => r88
	loadI 0 => r89		// start accum at 0
	subI r0, 0 => r90
	multI r90, 100 => r90
	add r89, r90 => r89
	subI r1, 0 => r90
	add r89, r90 => r89
	multI r89, 4 => r89	//  x sizeof(int)
	loadI 80020 => r91		// base address of array C
	add r91, r89 => r91	// base + offset
	load r91 => r92		// LValue -> RValue
	add r88, r92 => r93
	loadI 3 => r94		// int load of integer literal
	cmp_NE r93, r94 => r95
	// 'AND' case
	and r79, r95 => r96
	cbr r96 -> L17, L18	// if-then or if-then-else
L17:	nop			// then part
	loadI 1 => r2		// source-level assignment
L18:	nop			// exit label for if-then
	addI r1, 1 => r1	// update index
	cmp_LE r1, r59 => r97	// index < UB
	cbr r97 -> L12, L13	// branch back to loop header
L13:	nop			// loop exit label
	addI r0, 1 => r0	// update index
	cmp_LE r0, r57 => r98	// index < UB
	cbr r98 -> L10, L11	// branch back to loop header
L11:	nop			// loop exit label
	write r2	
	halt			// stop execution
