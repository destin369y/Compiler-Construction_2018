	read => r1		// read statement
	loadI 0 => r0		// index variable initialization
	subI r1, 1 => r12
	cmp_LE r0, r12 => r13	// index < UB
	cbr r13 -> L0, L1	// branch around loop
L0:	nop			// loop header label
	loadI 0 => r14		// start accum at 0
	subI r0, 0 => r15
	add r14, r15 => r14
	multI r14, 4 => r14	//  x sizeof(int)
	loadI 20 => r16		// base address of array DATA
	add r16, r14 => r16	// base + offset
	read => r17		// read statement
	store r17 => r16		// value from read
	addI r0, 1 => r0	// update index
	cmp_LE r0, r12 => r18	// index < UB
	cbr r18 -> L0, L1	// branch back to loop header
L1:	nop			// loop exit label
	loadI 0 => r20		// int load of integer literal
	subI r20, 1 => r19
	i2i r19 => r11		// source-level assignment
	loadI 0 => r21		// start accum at 0
	loadI 0 => r22		// result of fold
	add r21, r22 => r21
	multI r21, 4 => r21	//  x sizeof(int)
	loadI 40024 => r23		// base address of array STACK
	add r23, r21 => r23	// base + offset
	store r11 => r23		// int array store
	loadI 0 => r24		// start accum at 0
	loadI 1 => r25		// result of fold
	add r24, r25 => r24
	multI r24, 4 => r24	//  x sizeof(int)
	loadI 40024 => r26		// base address of array STACK
	add r26, r24 => r26	// base + offset
	store r11 => r26		// int array store
	loadI 0 => r27		// start accum at 0
	loadI 2 => r28		// result of fold
	add r27, r28 => r27
	multI r27, 4 => r27	//  x sizeof(int)
	loadI 40024 => r29		// base address of array STACK
	add r29, r27 => r29	// base + offset
	loadI 0 => r30		// int load of integer literal
	store r30 => r29		// int array store
	loadI 0 => r31		// start accum at 0
	loadI 3 => r32		// result of fold
	add r31, r32 => r31
	multI r31, 4 => r31	//  x sizeof(int)
	loadI 40024 => r33		// base address of array STACK
	add r33, r31 => r33	// base + offset
	subI r1, 1 => r34
	store r34 => r33		// int array store
	loadI 4 => r9		// source-level assignment
	loadI 0 => r10		// source-level assignment
	loadI 1 => r35		// int load of integer literal
	cmp_GT r9, r35 => r36
	cbr r36 -> L2, L3
L2:	nop			// top of while loop
	subI r9, 2 => r37
	loadI 0 => r38		// start accum at 0
	subI r37, 0 => r39
	add r38, r39 => r38
	multI r38, 4 => r38	//  x sizeof(int)
	loadI 40024 => r40		// base address of array STACK
	add r40, r38 => r40	// base + offset
	load r40 => r41		// LValue -> RValue
	i2i r41 => r4		// source-level assignment
	subI r9, 1 => r42
	loadI 0 => r43		// start accum at 0
	subI r42, 0 => r44
	add r43, r44 => r43
	multI r43, 4 => r43	//  x sizeof(int)
	loadI 40024 => r45		// base address of array STACK
	add r45, r43 => r45	// base + offset
	load r45 => r46		// LValue -> RValue
	i2i r46 => r5		// source-level assignment
	subI r9, 2 => r47
	i2i r47 => r9		// source-level assignment
	addI r10, 1 => r48
	i2i r48 => r10		// source-level assignment
	loadI 1000 => r49		// int load of integer literal
	cmp_GT r10, r49 => r50
	cbr r50 -> L4, L5	// if-then or if-then-else
L4:	nop			// then part
	i2i r11 => r9		// source-level assignment
L5:	nop			// exit label for if-then
	cmp_LT r4, r5 => r51
	cbr r51 -> L7, L8	// if-then or if-then-else
L7:	nop			// then part
	loadI 0 => r52		// start accum at 0
	subI r4, 0 => r53
	add r52, r53 => r52
	multI r52, 4 => r52	//  x sizeof(int)
	loadI 20 => r54		// base address of array DATA
	add r54, r52 => r54	// base + offset
	load r54 => r55		// LValue -> RValue
	loadI 0 => r56		// start accum at 0
	subI r5, 0 => r57
	add r56, r57 => r56
	multI r56, 4 => r56	//  x sizeof(int)
	loadI 20 => r58		// base address of array DATA
	add r58, r56 => r58	// base + offset
	load r58 => r59		// LValue -> RValue
	add r55, r59 => r60
	add r4, r5 => r61
	divI r61, 2 => r62
	loadI 0 => r63		// start accum at 0
	subI r62, 0 => r64
	add r63, r64 => r63
	multI r63, 4 => r63	//  x sizeof(int)
	loadI 20 => r65		// base address of array DATA
	add r65, r63 => r65	// base + offset
	load r65 => r66		// LValue -> RValue
	add r60, r66 => r67
	divI r67, 3 => r68
	i2i r68 => r6		// source-level assignment
	i2i r4 => r2		// source-level assignment
	i2i r5 => r3		// source-level assignment
	sub r3, r2 => r69
	loadI 1 => r70		// int load of integer literal
	cmp_EQ r69, r70 => r71
	cbr r71 -> L10, L11	// if-then or if-then-else
L10:	nop			// then part
	loadI 0 => r72		// start accum at 0
	subI r2, 0 => r73
	add r72, r73 => r72
	multI r72, 4 => r72	//  x sizeof(int)
	loadI 20 => r74		// base address of array DATA
	add r74, r72 => r74	// base + offset
	load r74 => r75		// LValue -> RValue
	loadI 0 => r76		// start accum at 0
	subI r3, 0 => r77
	add r76, r77 => r76
	multI r76, 4 => r76	//  x sizeof(int)
	loadI 20 => r78		// base address of array DATA
	add r78, r76 => r78	// base + offset
	load r78 => r79		// LValue -> RValue
	cmp_GT r75, r79 => r80
	cbr r80 -> L13, L14	// if-then or if-then-else
L13:	nop			// then part
	loadI 0 => r81		// start accum at 0
	subI r2, 0 => r82
	add r81, r82 => r81
	multI r81, 4 => r81	//  x sizeof(int)
	loadI 20 => r83		// base address of array DATA
	add r83, r81 => r83	// base + offset
	load r83 => r84		// LValue -> RValue
	i2i r84 => r8		// source-level assignment
	loadI 0 => r85		// start accum at 0
	subI r2, 0 => r86
	add r85, r86 => r85
	multI r85, 4 => r85	//  x sizeof(int)
	loadI 20 => r87		// base address of array DATA
	add r87, r85 => r87	// base + offset
	loadI 0 => r88		// start accum at 0
	subI r3, 0 => r89
	add r88, r89 => r88
	multI r88, 4 => r88	//  x sizeof(int)
	loadI 20 => r90		// base address of array DATA
	add r90, r88 => r90	// base + offset
	load r90 => r91		// LValue -> RValue
	store r91 => r87		// int array store
	loadI 0 => r92		// start accum at 0
	subI r3, 0 => r93
	add r92, r93 => r92
	multI r92, 4 => r92	//  x sizeof(int)
	loadI 20 => r94		// base address of array DATA
	add r94, r92 => r94	// base + offset
	store r8 => r94		// int array store
L14:	nop			// exit label for if-then
	br -> L12		// branch to exit label
L11:	nop			// else part
	sub r3, r2 => r95
	loadI 1 => r96		// int load of integer literal
	cmp_GT r95, r96 => r97
	cbr r97 -> L16, L17	// if-then or if-then-else
L16:	nop			// then part
	loadI 1 => r7		// source-level assignment
	cbr r7 -> L19, L20
L19:	nop			// top of while loop
	loadI 0 => r98		// start accum at 0
	subI r2, 0 => r99
	add r98, r99 => r98
	multI r98, 4 => r98	//  x sizeof(int)
	loadI 20 => r100		// base address of array DATA
	add r100, r98 => r100	// base + offset
	load r100 => r101		// LValue -> RValue
	cmp_LT r101, r6 => r102
	cbr r102 -> L21, L22
L21:	nop			// top of while loop
	addI r2, 1 => r103
	i2i r103 => r2		// source-level assignment
	loadI 0 => r98		// start accum at 0
	subI r2, 0 => r99
	add r98, r99 => r98
	multI r98, 4 => r98	//  x sizeof(int)
	loadI 20 => r100		// base address of array DATA
	add r100, r98 => r100	// base + offset
	load r100 => r101		// LValue -> RValue
	cmp_LT r101, r6 => r102
	cbr r102 -> L21, L22
L22:	nop			// bottom of while loop
	loadI 0 => r104		// start accum at 0
	subI r3, 0 => r105
	add r104, r105 => r104
	multI r104, 4 => r104	//  x sizeof(int)
	loadI 20 => r106		// base address of array DATA
	add r106, r104 => r106	// base + offset
	load r106 => r107		// LValue -> RValue
	cmp_GT r107, r6 => r108
	cbr r108 -> L23, L24
L23:	nop			// top of while loop
	subI r3, 1 => r109
	i2i r109 => r3		// source-level assignment
	loadI 0 => r104		// start accum at 0
	subI r3, 0 => r105
	add r104, r105 => r104
	multI r104, 4 => r104	//  x sizeof(int)
	loadI 20 => r106		// base address of array DATA
	add r106, r104 => r106	// base + offset
	load r106 => r107		// LValue -> RValue
	cmp_GT r107, r6 => r108
	cbr r108 -> L23, L24
L24:	nop			// bottom of while loop
	cmp_LT r2, r3 => r110
	cbr r110 -> L25, L26	// if-then or if-then-else
L25:	nop			// then part
	loadI 0 => r111		// start accum at 0
	subI r2, 0 => r112
	add r111, r112 => r111
	multI r111, 4 => r111	//  x sizeof(int)
	loadI 20 => r113		// base address of array DATA
	add r113, r111 => r113	// base + offset
	load r113 => r114		// LValue -> RValue
	i2i r114 => r8		// source-level assignment
	loadI 0 => r115		// start accum at 0
	subI r2, 0 => r116
	add r115, r116 => r115
	multI r115, 4 => r115	//  x sizeof(int)
	loadI 20 => r117		// base address of array DATA
	add r117, r115 => r117	// base + offset
	loadI 0 => r118		// start accum at 0
	subI r3, 0 => r119
	add r118, r119 => r118
	multI r118, 4 => r118	//  x sizeof(int)
	loadI 20 => r120		// base address of array DATA
	add r120, r118 => r120	// base + offset
	load r120 => r121		// LValue -> RValue
	store r121 => r117		// int array store
	loadI 0 => r122		// start accum at 0
	subI r3, 0 => r123
	add r122, r123 => r122
	multI r122, 4 => r122	//  x sizeof(int)
	loadI 20 => r124		// base address of array DATA
	add r124, r122 => r124	// base + offset
	store r8 => r124		// int array store
	br -> L27		// branch to exit label
L26:	nop			// else part
	loadI 0 => r7		// source-level assignment
	br -> L27		// no fall-through in iloc
L27:	nop			// exit label for if-then-else
	cbr r7 -> L19, L20
L20:	nop			// bottom of while loop
	loadI 0 => r125		// start accum at 0
	subI r9, 0 => r126
	add r125, r126 => r125
	multI r125, 4 => r125	//  x sizeof(int)
	loadI 40024 => r127		// base address of array STACK
	add r127, r125 => r127	// base + offset
	store r4 => r127		// int array store
	addI r9, 1 => r128
	loadI 0 => r129		// start accum at 0
	subI r128, 0 => r130
	add r129, r130 => r129
	multI r129, 4 => r129	//  x sizeof(int)
	loadI 40024 => r131		// base address of array STACK
	add r131, r129 => r131	// base + offset
	subI r2, 1 => r132
	store r132 => r131		// int array store
	addI r9, 2 => r133
	loadI 0 => r134		// start accum at 0
	subI r133, 0 => r135
	add r134, r135 => r134
	multI r134, 4 => r134	//  x sizeof(int)
	loadI 40024 => r136		// base address of array STACK
	add r136, r134 => r136	// base + offset
	store r3 => r136		// int array store
	addI r9, 3 => r137
	loadI 0 => r138		// start accum at 0
	subI r137, 0 => r139
	add r138, r139 => r138
	multI r138, 4 => r138	//  x sizeof(int)
	loadI 40024 => r140		// base address of array STACK
	add r140, r138 => r140	// base + offset
	store r5 => r140		// int array store
	addI r9, 4 => r141
	i2i r141 => r9		// source-level assignment
L17:	nop			// exit label for if-then
	br -> L12		// no fall-through in iloc
L12:	nop			// exit label for if-then-else
L8:	nop			// exit label for if-then
	loadI 1 => r35		// int load of integer literal
	cmp_GT r9, r35 => r36
	cbr r36 -> L2, L3
L3:	nop			// bottom of while loop
	loadI 0 => r0		// index variable initialization
	subI r1, 1 => r142
	cmp_LE r0, r142 => r143	// index < UB
	cbr r143 -> L28, L29	// branch around loop
L28:	nop			// loop header label
	loadI 0 => r144		// start accum at 0
	subI r0, 0 => r145
	add r144, r145 => r144
	multI r144, 4 => r144	//  x sizeof(int)
	loadI 20 => r146		// base address of array DATA
	add r146, r144 => r146	// base + offset
	load r146 => r147		// LValue -> RValue
	write r147	
	addI r0, 1 => r0	// update index
	cmp_LE r0, r142 => r148	// index < UB
	cbr r148 -> L28, L29	// branch back to loop header
L29:	nop			// loop exit label
	halt			// stop execution
