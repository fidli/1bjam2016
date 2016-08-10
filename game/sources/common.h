#ifndef COMMON_H
#define COMMON_H

  #ifdef RELEASE
      #define ASSERT
  #else
      #define ASSERT(expression) if(!(expression)) {*(int *)0 = 0;}
  #endif


  #define ARRAYSIZE(array) sizeof(array) /sizeof(*(array))

  #define KILOBYTE(n) 1024*n
  #define MEGABYTE(n) 1024*KILOBYTE(n)
  #define GIGABYTE(n) 1024*MEGABYTE(n)

  struct memory{
      void * persistent;
      void * stack;
      void * temp;
      Uint64 tempOffset;
      Uint64 stackOffset;
  };
  

  
#define PUSH(strct) *((strct *)(((Byte *) mem.temp) + mem.tempOffset)); mem.tempOffset += sizeof(strct); ASSERT(mem.tempOffset < TEMP_MEM);
#define PUSHS(strct, size) *((strct *)(((Byte *) mem.temp) + mem.tempOffset)); mem.tempOffset += size; ASSERT(mem.tempOffset < TEMP_MEM);
  
 #define PPUSH(strct) *((strct *)(((Byte *) mem.stack) + mem.stackOffset)); mem.stackOffset += sizeof(strct); ASSERT(mem.stackOffset < STACK_MEM);
#define PPUSHS(strct, size) *((strct *)(((Byte *) mem.stack) + mem.stackOffset)); mem.stackOffset += size; ASSERT(mem.stackOffset < STACK_MEM);

#endif
