#ifndef _CODECS_H_
#define _CODECS_H_

#include "config.h"
#include "text.h"
#include <map>


class AudioCodec
{
public:
	enum Type {PCMA=8,PCMU=0,GSM=3,SPEEX16=117,AMR=118,TELEPHONE_EVENT=100,NELLY8=130,NELLY11=131};
	typedef std::map<int,Type> RTPMap;
	
public:
	static const char* GetNameFor(Type codec);
	static AudioCodec* CreateCodec(Type codec);
	static AudioCodec* CreateDecoder(Type codec);
	
public:
	virtual ~AudioCodec(){};
	virtual int Encode(WORD *in,int inLen,BYTE* out,int outLen)=0;
	virtual int Decode(BYTE *in,int inLen,WORD* out,int outLen)=0;
	Type type;
	int numFrameSamples;
	int frameLength;
};

class VideoCodec
{
public:
	enum Type {H263_1996=34,H263_1998=103,MPEG4=104,H264=99, H264_RTSP=111, SORENSON=100};
	static const char* GetNameFor(Type type)
	{
		switch (type)
		{
			case H263_1996:	return "H263_1996";
			case H263_1998:	return "H263_1998";
			case MPEG4:	return "MPEG4";
			case H264:	return "H264";
			// rtsp addon by liuhong
			case H264_RTSP:	return "H264_RTSP";
			case SORENSON:  return "SORENSON";
			default:	return "unknown";
		}
	}
	typedef std::map<int,Type> RTPMap;
};


class TextCodec
{
public:
	enum Type {T140=106,T140RED=105};
	static const char* GetNameFor(Type type)
	{
		switch (type)
		{
			case T140:	return "T140";
			case T140RED:	return "T140RED";
			default:	return "unknown";
		}
	}
	typedef std::map<int,Type> RTPMap;
};

#endif
