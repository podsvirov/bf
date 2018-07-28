#ifndef _BF_H_
#define _BF_H_ 1

/** \file bf.h
    \brief Single bf library header file
 */

#ifdef __cplusplus
extern "C" {
#endif

/// Hi - initialize bf library
extern int bfhi();

/// Clear screen
extern void bfcs();
/// Fill screen
extern void bffs();

/// Character
extern int bfch();
/// Error
extern int bfer();

/// Output
extern int bfo(int nbuf, const char *fmt, ...);
/// Output always
extern int bfoa(int nbuf, const char *fmt, ...);

/// Input
extern int bfi(int nbuf, const char *fmt, ...);
/// Input always
extern int bfia(int nbuf, const char *fmt, ...);

/// Buffer string
extern char *bfb(int nbuf);
/// Buffer free to reuse
extern int bfre(int nbuf);

/// Non-bloking input
extern int bfas(int as);

/// Bye - terminate bf library
extern int bfbi();

#ifdef __cplusplus
}
#endif

#endif // _BF_H_
