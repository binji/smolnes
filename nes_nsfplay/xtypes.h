#ifndef _XTYPES_H_
#define _XTYPES_H_

#if defined(_MSC_VER)
#define INLINE __forceinline
#elif defined(__GNUC__)
#define INLINE __inline__
#elif defined(_MWERKS_)
#define INLINE inline
#else
#define INLINE
#endif

namespace xgm
{
    /** 8bit�����Ȃ��̐����^ */
    typedef unsigned char UINT8;
    /** 16bit�����Ȃ��̐����^ */
    typedef unsigned short UINT16;
    /** 32bit�����Ȃ��̐����^ */
    typedef unsigned int UINT32;
    /** 64bit unsigned */
    typedef unsigned long long UINT64;
    /** 8bit�����t�������^ */
    typedef signed char INT8;
    /** 16bit�����t�������^ */
    typedef signed short INT16;
    /** 32bit�����t�������^ */
    typedef signed int INT32;
    /** 64bit signed */
    typedef signed long long INT64;
}

#endif
