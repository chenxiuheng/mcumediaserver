#ifndef _MOSAIC_H_
#define _MOSAIC_H_
#include "config.h"
#include "framescaler.h"
#include "overlay.h"
#include <map>

class Mosaic
{
public:
	typedef enum
	{
		mosaic1x1	= 0,
		mosaic2x2	= 1,
		mosaic3x3	= 2,
		mosaic3p4	= 3,
		mosaic1p7	= 4,
		mosaic1p5	= 5,
		mosaic1p1	= 6,
		mosaicPIP1	= 7,
		mosaicPIP3	= 8
	} Type;
public:
	Mosaic(Type type,DWORD size);
	virtual ~Mosaic();

	int GetWidth()		{ return mosaicTotalWidth;}
	int GetHeight()		{ return mosaicTotalHeight;}
	int HasChanged()	{ return mosaicChanged; }
	void Reset()		{ mosaicChanged = true; }
	
	BYTE* GetFrame();
	virtual int Update(int index,BYTE *frame,int width,int heigth) = 0;
	virtual int Clean(int index) = 0;

	int AddParticipant(int id);
	int HasParticipant(int id);
	int RemoveParticipant(int id);
	int SetSlot(int num,int id);
	int CalculatePositions();
	int GetPosition(int id);
	int* GetPositions();
	int* GetSlots();
	int GetNumSlots();
	void SetSlots(int *slots,int num);
	
	static int GetNumSlotsForType(Type type);
	static Mosaic* CreateMosaic(Type type,DWORD size);

	int SetOverlayPNG(const char* filename);
	int SetOverlaySVG(const char* svg);
	int ResetOverlay();

protected:
	void SetChanged()	{ mosaicChanged = true; overlayNeedsUpdate = true; }
	
protected:
	typedef std::map<int,int> Participants;

protected:
	Participants participants;
	int mosaicChanged;
	int *mosaicSlots;
	int *mosaicPos;
	int numSlots;
	
	FrameScaler** resizer;
	BYTE* 	mosaic;
	BYTE*	mosaicBuffer;
	int 	mosaicTotalWidth;
	int 	mosaicTotalHeight;
	Type	mosaicType;
	int     mosaicSize;

	Overlay* overlay;
	bool	 overlayNeedsUpdate;
};

#endif 
