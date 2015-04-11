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
#ifndef SKALE_IK_SOLVER_HPP
#define SKALE_IK_SOLVER_HPP
#include "SKALE/platform.hpp"
#include "SKALE/Bone.hpp"

namespace skl
{
	class Skeleton;
	class IKSolver
	{
		float mSolvedRadiusSquared;
		bool mConstraints;
		bool mFixtures;
		size_t mMaxIter;
		float _simplifyAngle(float angle) const;
		float _constrainAngle(Bone* bone, float angle) const;
	public:
		IKSolver(void);
		void setSolvedRadius(float radius);
		float getSolvedRadiusSquared() const;
		void setAngleLimits(bool limited);
		bool isAngleLimited() const;
		void setStopAtFixture(bool stopping);
		bool isStoppingAtFixture() const;
		void setMaxIterations(size_t iterations);
		size_t getMaxIterations() const;
		bool solveIteration(Bone* targetBone, float targetX, float targetY) const;
		bool solve(Skeleton* skeleton,Bone* targetBone, float targetX, float targetY);
		bool solve(Skeleton* skeleton,const std::string& boneName, float targetX, float targetY);

		virtual ~IKSolver(void);
	};
}

#endif
