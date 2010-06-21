#ifdef __cplusplus
extern "C" {
#endif

#ifndef GD_IO_H
#define GD_IO_H 1

#include <stdio.h>

#ifdef VMS
#define Putchar gdPutchar
#endif

typedef struct gdIOCtx
{
  int (*getC) (struct gdIOCtx *);
  int (*getBuf) (struct gdIOCtx *, void *, size_t);

  void (*putC) (struct gdIOCtx *, int);
  int (*putBuf) (struct gdIOCtx *, const void *, size_t);

  /* seek must return 1 on SUCCESS, 0 on FAILURE. Unlike fseek! */
  int (*seek) (struct gdIOCtx *, const size_t);

  long (*tell) (struct gdIOCtx *);

  void (*gd_free) (struct gdIOCtx *);

}
gdIOCtx;

typedef struct gdIOCtx *gdIOCtxPtr;

void Putword (int w, gdIOCtx * ctx);
void Putchar (int c, gdIOCtx * ctx);

 void gdPutC (const unsigned char c, gdIOCtx * ctx);
 int gdPutBuf (const void *, size_t, gdIOCtx *);
 void gdPutWord (int w, gdIOCtx * ctx);
 void gdPutInt (int w, gdIOCtx * ctx);

 int gdGetC (gdIOCtx * ctx);
 int gdGetBuf (void *, size_t, gdIOCtx *);
 int gdGetByte (int *result, gdIOCtx * ctx);
 int gdGetWord (int *result, gdIOCtx * ctx);
 int gdGetInt (int *result, gdIOCtx * ctx);

 int gdSeek (gdIOCtx * ctx, const size_t);
 long gdTell (gdIOCtx * ctx);

#endif

#ifdef __cplusplus
}
#endif
