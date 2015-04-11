/* SKALE - 2D SKeletal Animation Layer for Entities
 * Copyright (c) 2011 Joshua Larouche
 * 
 *
 * License: (BSD)
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of SKALE nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "SKALE/Bone.hpp"
#include <math.h>
#include <algorithm>

namespace skl
{

	Bone::~Bone(void)
	{
	}

	Bone::Bone( float x, float y, float angle, float length, float minAngle,
		float maxAngle, bool relative, const std::string& name, Bone* parent /*= NULL*/ )
		: mX(x),mY(y),mAngle(angle),mLength(length),mName(name),
		mMinAngle(minAngle),mMaxAngle(maxAngle),mRelative(relative),
		mFrameX(0),mFrameY(0),mFrameAngle(0),mParent(parent),
		currentFrame(0),currentKeyFrameIndex(0),startKeyFrame(NULL),
		endKeyFrame(NULL),framesPerSecond(60),curIncreaseAngle(0.0f),
		remainingInterpolationFrames(0),mFixture(false)
	{
		mMinAngle = fmod(mMinAngle,SK_TWO_PI);
		mMaxAngle = fmod(mMaxAngle,SK_TWO_PI);
		mAngle = fmod(mAngle,SK_TWO_PI);
	}

	bool Bone::remove( Bone* child )
	{
		for(std::list<Bone>::iterator it = children.begin(); it != children.end(); ++it)
		{
			if(&(*it) == child)
			{
				children.erase(it);
				return true;
			}
		}

		return false;
	}

	void Bone::setAngle( float angle )
	{
		mAngle = angle;
	}

	const float& Bone::getAngle() const
	{
		return mAngle;
	}

	void Bone::setX( float x )
	{
		mX = x;
	}

	void Bone::setY( float y )
	{
		mY = y;
	}

	void Bone::set( float x, float y )
	{
		mX = x;
		mY = y;
	}

	void Bone::set( float x, float y, float angle )
	{
		set(x,y);
		setAngle(angle);
	}

	const float& Bone::getX() const
	{
		return mX;
	}

	const float& Bone::getY() const
	{
		return mY;
	}

	void Bone::setMinAngle( float minAngle )
	{
		mMinAngle = minAngle;
	}

	void Bone::setMaxAngle( float maxAngle )
	{
		mMaxAngle = maxAngle;
	}

	const float& Bone::getMinAngle() const
	{
		return mMinAngle;
	}

	const float& Bone::getMaxAngle() const
	{
		return mMaxAngle;
	}

	void Bone::setLength( float length )
	{
		mLength = abs(length);
	}

	void Bone::clear()
	{
		children.clear();
	}

	void Bone::setRelative( bool relative )
	{
		mRelative = relative;
	}

	bool Bone::isRelative() const
	{
		return mRelative;
	}

	Bone* Bone::add( float x, float y, float angle, float length,
					float minAngle, float maxAngle,
					const std::string& name /*= ""*/ )
	{
		children.push_back(Bone(x,y,angle,length
			,minAngle,maxAngle,true,name,this));
		return &children.back();
	}

	std::list<Bone>::iterator Bone::begin()
	{
		return children.begin();
	}

	std::list<Bone>::iterator Bone::end()
	{
		return children.end();
	}

	Bone* Bone::getParent() const
	{
		return mParent;
	}

	const std::string& Bone::getName() const
	{
		return mName;
	}

	int Bone::count() const
	{
		return children.size();
	}

	void Bone::setFrame( float frameX, float frameY, float FrameAngle )
	{
		mFrameX = frameX;
		mFrameY = frameY;
		mFrameAngle = FrameAngle;
	}

	const float& Bone::getFrameX() const
	{
		return mFrameX;
	}

	const float& Bone::getFrameY() const
	{
		return mFrameY;
	}

	const float& Bone::getLength() const
	{
		return mLength;
	}

	const float& Bone::getFrameAngle() const
	{
		return mFrameAngle;
	}

	void Bone::setName( const std::string &name )
	{
		mName = name;
		for(int i = 0; i < (int)mName.length(); ++i)
		{
			if(mName[i] == '\"')
			{
				mName[i] = '\'';
			}
		}
	}

	void Bone::addKeyFrame( const KeyFrame& keyFrame )
	{
		mKeyFrames.push_back(keyFrame);
		std::sort(mKeyFrames.begin(),mKeyFrames.end());
	}

	void Bone::addKeyFrames( const std::vector<KeyFrame>& keyFrames )
	{
		mKeyFrames = keyFrames;
		std::sort(mKeyFrames.begin(),mKeyFrames.end());
	}

	void Bone::interpolateIncreaseAngle()
	{
		if(!startKeyFrame && !endKeyFrame)
		{
			resetAnimation();
		}
		if(startKeyFrame && endKeyFrame && currentFrame >= startKeyFrame->getFrame())
		{
			bool CCW = abs(endKeyFrame->getValue() - 
				startKeyFrame->getValue() ) > SK_PI;

			remainingInterpolationFrames = endKeyFrame->getFrame() - startKeyFrame->getFrame();

			//Linear interpolation
			if(CCW)
			{
				curIncreaseAngle = (SK_TWO_PI - endKeyFrame->getValue() - 
					startKeyFrame->getValue()) / remainingInterpolationFrames;

				if(endKeyFrame->getValue() - startKeyFrame->getValue() > 0.0f) 
				curIncreaseAngle = -curIncreaseAngle;
			}
			else
			{
				curIncreaseAngle = (endKeyFrame->getValue() - 
					startKeyFrame->getValue()) / remainingInterpolationFrames;
			}
		}
		else
		{
			remainingInterpolationFrames = -1;
			curIncreaseAngle = 0.0f;
		}
	}

	void Bone::resetAnimation()
	{
		curIncreaseAngle = 0.0f;
		currentFrame = 0;
		currentKeyFrameIndex = 0;

		remainingInterpolationFrames = 0;
		if(mKeyFrames.size() >= 2)
		{
			startKeyFrame = &mKeyFrames[0];
			endKeyFrame = &mKeyFrames[1];
			currentKeyFrameIndex = 1;
			setAngle(startKeyFrame->getValue());
		}
		else
		{
			startKeyFrame = NULL;
			endKeyFrame = NULL;
		}
	}

	void Bone::processAnimation()
	{
		if(mKeyFrames.size() == 0)
		{
			return;
		}
		if (remainingInterpolationFrames > 0)
		{
			remainingInterpolationFrames--;
			setAngle(getAngle() + curIncreaseAngle);
		}

		if(remainingInterpolationFrames == 0)
		{
			interpolateIncreaseAngle();

			if(startKeyFrame && currentFrame < startKeyFrame->getFrame())
			{
				currentFrame++;
				return;
			}
			if(currentKeyFrameIndex + 1 < (int)mKeyFrames.size())
			{
				currentKeyFrameIndex++;
				startKeyFrame = endKeyFrame;

				endKeyFrame = &mKeyFrames[currentKeyFrameIndex];

			}
			else //End of animation
			{
				startKeyFrame = endKeyFrame;
				endKeyFrame = NULL;
			}
		}
		currentFrame++;
	}

	void Bone::setAsFixture( bool fixture )
	{
		mFixture = fixture;
	}

	bool Bone::isFixture() const
	{
		return mFixture;
	}

}