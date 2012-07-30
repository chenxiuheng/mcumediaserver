extern "C" {
#include "gsm.h"
} 
#include "codecs.h"

class GSMCodec : public AudioCodec
{
public:
	GSMCodec();
	virtual ~GSMCodec();
	virtual int Encode(WORD *in,int inLen,BYTE* out,int outLen);
	virtual int Decode(BYTE *in,int inLen,WORD* out,int outLen);
private:
	gsm g;

};
