# FosSupportLibs (Support libs for Fly Operating System)

This library consists of some functionally independent parts.

### Library parts
1. CircBuf - the ralization of circular buffer
2. CRC - the realization of CRC calculations
3. DMem - the realization of dynamic memory distribution (heap)
4. Libs - the realization of common used functions
5. Platform - the prototypes of platform dependent functions

All library parts can be found in the folfer `/src`

### Library dependencies
To work this library needs declaration some compiler depending macroses. The template of such macroses can be found in folder `/Templates` named `compiler_macros.h`. Also there can be found template of platform depended function `user_sl_platform.c`.

### Information
All paths are relative to the folfer `/src`






