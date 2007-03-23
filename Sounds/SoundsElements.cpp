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

#include "SoundsElements.h"

SoundsDefinition::SoundsDefinition(bool verbose) : SoundsElement(verbose) {}

SoundsDefinition::~SoundsDefinition() {}


BigEndianBuffer& SoundsDefinition::SaveObject(BigEndianBuffer& buffer)
{
	return buffer;
}

BigEndianBuffer& SoundsDefinition::LoadObject(BigEndianBuffer& buffer)
{	
	mSoundCode = buffer.ReadShort();

	mBehaviorIndex = buffer.ReadShort();
	mFlags = buffer.ReadUShort();
	
	mChance = buffer.ReadUShort();
	
	if ((mBehaviorIndex > NUMBER_OF_SOUND_BEHAVIOR_DEFINITIONS) || (mChance > _ten_percent)) {
		wxLogError("[SoundsDefinition] incorrect Behavior/Chance");
		return buffer;
	}
	
	wxInt32		low_pitch_fixed, high_pitch_fixed;
	
	low_pitch_fixed = buffer.ReadLong();
	high_pitch_fixed = buffer.ReadLong();
	
	mLowPitch = ((low_pitch_fixed >> 16) & 0xffff) + (float)(low_pitch_fixed & 0xffff) / 65536.0;	// convert fixed point [0,1] to float
	mHighPitch = ((high_pitch_fixed >> 16) & 0xffff) + (float)(high_pitch_fixed & 0xffff) / 65536.0;	// convert fixed point [0,1] to float

	short permutations = buffer.ReadShort();
	
	if (permutations < 0 || permutations > MAXIMUM_PERMUTATIONS_PER_SOUND) {
		wxLogError("[SoundsDefinition] incorrect permutation count");
		return buffer;
	}
	
	unsigned short permutationsPlayed = buffer.ReadUShort();
	int groupOffset = buffer.ReadLong();
	int singleLength = buffer.ReadLong();
	int totalLength = buffer.ReadLong();
	
	std::vector<int> soundOffsets;
	
	soundOffsets.resize(MAXIMUM_PERMUTATIONS_PER_SOUND);
	for (unsigned int i = 0; i < MAXIMUM_PERMUTATIONS_PER_SOUND; i++) {
		soundOffsets[i] = buffer.ReadLong();
	}
	
	mLastPlayed = buffer.ReadULong();
	
	if (IsVerbose()) {
		wxLogDebug("[SoundsDefinition] Sound Code:			%d", mSoundCode);
		wxLogDebug("[SoundsDefinition] Behavior Index:		%d", mBehaviorIndex);
		wxLogDebug("[SoundsDefinition] Flags:				%d", mFlags);
		wxLogDebug("[SoundsDefinition] Chance:				%d", mChance);
		wxLogDebug("[SoundsDefinition] Low Pitch:			%f", mLowPitch);
		wxLogDebug("[SoundsDefinition] High Pitch:			%f", mHighPitch);
		wxLogDebug("[SoundsDefinition] Permutations:		%d", permutations);
		wxLogDebug("[SoundsDefinition] Permutations Played:	%d", permutationsPlayed);
		wxLogDebug("[SoundsDefinition] Group Offset:		%d", groupOffset);
		wxLogDebug("[SoundsDefinition] Single Length:		%d", singleLength);
		wxLogDebug("[SoundsDefinition] Total Length:		%d", totalLength);
		wxLogDebug("[SoundsDefinition] Last Played:			%d", mLastPlayed);
	}
	
	// Now we load actual sound data
	unsigned int oldpos = buffer.Position();
	
	for (short i = 0; i < permutations; i++) {
	
		unsigned int size = 0;
		if (permutations == 1)
			size = singleLength;
		else if (i == permutations - 1)
			size = totalLength - soundOffsets[i];
		else
			size = soundOffsets[i + 1] - soundOffsets[i];
		
		BigEndianBuffer *sndbuffer = new BigEndianBuffer(size);
		
		buffer.Position(groupOffset + soundOffsets[i]);
		buffer.ReadBlock(sndbuffer->Size(), sndbuffer->Data());
		
		mSounds.push_back(sndbuffer);
	}

	buffer.Position(oldpos);
	
	mGoodData = true;
	return buffer;
}

BigEndianBuffer* SoundsDefinition::GetPermutation(unsigned int permutation_index)
{
	if (permutation_index > mSounds.size())
		return NULL;
	
	return mSounds[permutation_index];
}

/*SoundHeader::SoundHeader(bool verbose): SoundsElement(verbose), mLength(0), mData(NULL)
{}

SoundHeader::~SoundHeader()
{
	if (mData)
		mData = NULL;
}

BigEndianBuffer& SoundHeader::SaveObject(BigEndianBuffer& buffer)
{
	return buffer;
}

BigEndianBuffer& SoundHeader::LoadObject(BigEndianBuffer& buffer)
{
	return buffer;
}*/