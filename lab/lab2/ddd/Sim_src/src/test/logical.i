  loadI 0 => r0
  andI  r0,1 => r1
  andI  r0,0 => r1
  loadI 1 => r0
  andI  r0,1 => r1
  andI  r0,0 => r1
  nop
  loadI 0 => r0
  orI  r0,1 => r1
  orI  r0,0 => r1
  loadI 1 => r0
  orI  r0,1 => r1
  orI  r0,0 => r1
  nop
  loadI 0 => r0
  loadI 1 => r1
  and r0,r0 => r2
  and r0,r1 => r2
  and r1,r0 => r2
  and r1,r1 => r2
  nop
  or  r0,r0 => r2
  or  r0,r1 => r2
  or  r1,r0 => r2
  or  r1,r1 => r2
  nop
  not r0 => r2
  not r1 => r2
  loadI 17 => r17
  not r17 => r17
