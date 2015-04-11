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
#ifndef SKALE_BONE_HPP
#define SKALE_BONE_HPP
#include <list>
#include <vector>
#include "SKALE/platform.hpp"
#include "SKALE/KeyFrame.hpp"
namespace skl
{
	//NOTE: This class does not do any logical verifications when setting values
	class Bone
	{
		bool mRelative;
		float mX;
		float mY;
		float mAngle; //radians
		float mMinAngle;
		float mMaxAngle;
		float mLength;
		std::string mName;
		Bone* mParent;
		std::list<Bone> children;
		std::vector<KeyFrame> mKeyFrames;
		bool mFixture;

		float mFrameX;
		float mFrameY;
		float mFrameAngle;
		size_t currentFrame;
		int currentKeyFrameIndex;
		KeyFrame* startKeyFrame;
		KeyFrame* endKeyFrame;
		int remainingInterpolationFrames;
		float curIncreaseAngle;
		int framesPerSecond;
		void interpolateIncreaseAngle();
	public:
		Bone(float x, float y, float angle, float length,
			float minAngle, float maxAngle, bool relative,
			const std::string& name, 
			Bone* parent = NULL);
		Bone* add(float x, float y, float angle, float length,
			float minAngle, float maxAngle, const std::string& name = "");
		bool remove(Bone* child);
		void clear();
		void setAngle(float angle);
		const float& getAngle() const;
		void setX(float x);
		void setY(float y);
		const float& getX() const;
		const float& getY() const;
		void set(float x, float y);
		void set(float x, float y, float angle);
		void setMinAngle(float minAngle);
		void setMaxAngle(float maxAngle);
		const float& getMinAngle() const;
		const float& getMaxAngle() const;
		void setLength(float length);
		const float& getLength() const;
		void setRelative(bool relative);
		bool isRelative() const;
		std::list<Bone>::iterator begin();
		std::list<Bone>::iterator end();
		Bone* getParent() const;
		void setName(const std::string &name);
		const std::string& getName() const;
		int count() const;
		void setFrame(float frameX, float frameY, float FrameAngle);
		const float& getFrameX() const;
		const float& getFrameY() const;
		const float& getFrameAngle() const;

		void setAsFixture(bool fixture);
		bool isFixture() const;


		void addKeyFrame(const KeyFrame& keyFrame);
		void addKeyFrames(const std::vector<KeyFrame>& keyFrames);
		void resetAnimation();
		void processAnimation();
		
		virtual ~Bone(void);
	};
}

#endif
