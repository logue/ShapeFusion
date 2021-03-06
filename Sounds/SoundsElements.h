/*
 * This file is part of ShapeFusion (Copyright 2000 Tito Dal Canton)
 *
 * ShapeFusion is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * ShapeFusion is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ShapeFusion; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef SOUNDSELEMENTS_H
#define SOUNDSELEMENTS_H

#include <fstream>
#include <vector>
using std::vector;

#include "../BigEndianBuffer.h"

/* ---------- constants */

#define MAXIMUM_PERMUTATIONS_PER_SOUND		5
#define SOUND_FILE_VERSION					1
#define SIZEOF_sound_file_header			260
#define SIZEOF_sound_definition				64

enum /* sound behaviors */
{
	_sound_is_quiet,
	_sound_is_normal,
	_sound_is_loud,
	NUMBER_OF_SOUND_BEHAVIOR_DEFINITIONS
};

enum // Sounds flags
{
	_sound_cannot_be_restarted= 0x0001,
	_sound_does_not_self_abort= 0x0002,
	_sound_resists_pitch_changes= 0x0004, // 0.5 external pitch changes
	_sound_cannot_change_pitch= 0x0008, // no external pitch changes
	_sound_cannot_be_obstructed= 0x0010, // ignore obstructions
	_sound_cannot_be_media_obstructed= 0x0020, // ignore media obstructions
	_sound_is_ambient= 0x0040 // will not be loaded unless _ambient_sound_flag is asserted
};

enum /* sound chances */
{
	_ten_percent= 32768*9/10,
	_twenty_percent= 32768*8/10,
	_thirty_percent= 32768*7/10,
	_fourty_percent= 32768*6/10,
	_fifty_percent= 32768*5/10,
	_sixty_percent= 32768*4/10,
	_seventy_percent= 32768*3/10,
	_eighty_percent= 32768*2/10,
	_ninty_percent= 32768*1/10,
	_always= 0
};

/* ---------- structures */

/*typedef struct {
	short sound_index;
}  ambient_sound_definition;

typedef struct {
	short sound_index;
} random_sound_definition;*/

class SoundsElement // : public wxObject
{
private:
	bool	mVerboseLoading;

protected:
	// So that subclasses can change their status
	bool	mGoodData;

public:
	SoundsElement(bool verbose) : mVerboseLoading(verbose), mGoodData(false) {}
	~SoundsElement(void) {}
	
	bool IsGood() const {return mGoodData;}
	bool IsVerbose() const {return mVerboseLoading;}
};

class AppleSoundHeader : public SoundsElement
{
private:
	// Stuff for reading System 7 Sound info
	/*sampled sound header encoding options*/
	enum {
		standardSoundHeader		= 0x00,
		extendedSoundHeader		= 0xFF,
		compressedSoundHeader	= 0xFE
	};

	bool mSixteenBit;
	bool mStereo;
	bool mSigned;
	int mBytesPerFrame;
	unsigned int mSampleRate; // 16.16 unsigned fixed
	int mLoopStart;
	int mLoopEnd;
	unsigned char mBaseFrequency;

	std::vector<unsigned char> mData;

	bool SaveToWaveOrAiff(wxString path, bool aiff);

public:
	AppleSoundHeader(bool verbose = false);
	~AppleSoundHeader();

	bool operator==(const AppleSoundHeader& right) const;
	
	bool LoadFromFile(wxString path);
	bool LoadFromWave(wxString path);
	bool SaveToWave(wxString path) { return SaveToWaveOrAiff(path, false); }
	bool LoadFromAiff(wxString path);
	bool SaveToAiff(wxString path) { return SaveToWaveOrAiff(path, true); }
	void PlaySound(void);

	bool IsSixteenBit() const { return mSixteenBit; }
	bool IsStereo() const { return mStereo; }
	bool IsSigned() const { return mSigned; }
	int GetBytesPerFrame() const { return mBytesPerFrame; }
	unsigned int GetSampleRate() const { return mSampleRate; }
	
	void SetSixteenBit(bool s) { mSixteenBit = s; }
	void SetStereo(bool s) { mStereo = s; }
	void SetSigned(bool s) { mSigned = s; }
	void SetBytesPerFrame(int b) { mBytesPerFrame = b; }
	void SetSampleRate(unsigned int sr) { mSampleRate = sr; }

	// Utilities
	unsigned int Size(void);
	unsigned char* Data(void);
    BigEndianBuffer& SaveObject(BigEndianBuffer& buffer);
    BigEndianBuffer& LoadObject(BigEndianBuffer& buffer);
};

class SoundsDefinition : public SoundsElement 
{
private:
	/* Marathon Sound Definition */
	short mSoundCode;
	short mBehaviorIndex;
	unsigned short mFlags;
	unsigned short mChance; // play sound if AbsRandom()>=chance
	/* if low_pitch==0, use FIXED_ONE;
	 * if high_pitch==0 use low pitch;
	 * else choose in [low_pitch,high_pitch]
	 */
	float mLowPitch, mHighPitch;
	/* filled in later */
//	short mPermutations;
	unsigned short mPermutationsPlayed;
//	int mGroupOffset, mSingleLength, mTotalLength; // magic numbers necessary to load sounds
//	std::vector<int> mSoundOffsets;
	std::vector<AppleSoundHeader> mSounds;
	unsigned int mLastPlayed; // machine ticks
	// Pointer to loaded sound and size of sound object pointed to
//	short *ptr;
//	int size;
	
	bool mRemap8bit;

public:
	SoundsDefinition(bool verbose = false);
	~SoundsDefinition();
	
	bool HaveSameAttributesAs(const SoundsDefinition& right) const;
	bool HaveSameSoundsAs(const SoundsDefinition& right) const;
	bool operator== (const SoundsDefinition& right) const;
	bool operator!=(const SoundsDefinition& right) const;
	
	short GetSoundCode(void) const {return mSoundCode;}
	short GetBehaviorIndex(void) const {return mBehaviorIndex;}
	
	bool IsNotRestartable(void) const {return mFlags & _sound_cannot_be_restarted;}
	bool IsNotSelfAbortable(void) const {return mFlags & _sound_does_not_self_abort;}
	bool IsPitchChangeResistant(void) const {return mFlags & _sound_resists_pitch_changes;}
	bool IsNotPitchChangeable(void) const {return mFlags & _sound_cannot_change_pitch;}
	bool IsNotObstructed(void) const {return mFlags & _sound_cannot_be_obstructed;}
	bool IsNotMediaObstructed(void) const {return mFlags & _sound_cannot_be_media_obstructed;}
	bool IsAmbient(void) const {return mFlags & _sound_is_ambient;}
	
	short GetChance(void) const;
	
	float GetLowPitch(void) const {return mLowPitch;}
	float GetHighPitch(void) const {return mHighPitch;}
	
	void SetSoundCode(short s) {mSoundCode = s;}
	void SetBehaviorIndex(int i) {mBehaviorIndex = i;}
	
	void SetNotRestartable(bool f) {
		if (f) mFlags |= _sound_cannot_be_restarted;
		else mFlags &= ~_sound_cannot_be_restarted;
	}

	void SetNotSelfAbortable(bool f) {
		if (f) mFlags |= _sound_does_not_self_abort;
		else mFlags &= ~_sound_does_not_self_abort;
	}

	void SetPitchChangeResistant(bool b) {
		if (b) mFlags |= _sound_resists_pitch_changes;
		else mFlags &= ~_sound_resists_pitch_changes;
	}

	void SetNotPitchChangeable(bool b) {
		if (b) mFlags |= _sound_cannot_change_pitch;
		else mFlags &= ~_sound_cannot_change_pitch;
	}

	void SetNotObstructed(bool b) {
		if (b) mFlags |= _sound_cannot_be_obstructed;
		else mFlags &= ~_sound_cannot_be_obstructed;
	}

	void SetNotMediaObstructed(bool b) {
		if (b) mFlags |= _sound_cannot_be_media_obstructed;
		else mFlags &= ~_sound_cannot_be_media_obstructed;
	}

	void SetAmbient(bool b) {
		if (b) mFlags |= _sound_is_ambient;
		else mFlags &= ~_sound_is_ambient;
	}
	
	void SetChance(short i);
	
	void SetLowPitch(int p) {mLowPitch = p;}
	void SetHighPitch(int p) {mHighPitch = p;}
	
	unsigned int GetPermutationCount(void) const {return mSounds.size();}
	void DeletePermutation(unsigned int permutation_index);
	AppleSoundHeader* GetPermutation(unsigned int permutation_index);
	AppleSoundHeader* NewPermutation(wxString path);
	
	// Utilities
	unsigned int GetSizeInFile(void);
    BigEndianBuffer& SaveObject(BigEndianBuffer& buffer, unsigned int& offset);
    BigEndianBuffer& LoadObject(BigEndianBuffer& buffer);
};

#endif

