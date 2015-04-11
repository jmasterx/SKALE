#include "SKALE/IKSolver.hpp"
#include "SKALE/Skeleton.hpp"
#include "math.h"
#include <algorithm>

namespace skl{

	IKSolver::IKSolver(void)
		: mSolvedRadiusSquared(1.0f),mConstraints(true),
		mFixtures(true),mMaxIter(20)
	{
	}

	IKSolver::~IKSolver(void)
	{
	}

	//IK Implementation based on: http://www.ryanjuckett.com/programming/cyclic-coordinate-descent-in-2d/
	bool IKSolver::solveIteration( Bone* targetBone, float targetX, float targetY ) const
	{
		//Track the end effector position (the final bone)
		float endX = targetBone->getFrameX();
		float endY = targetBone->getFrameY();

		bool modifiedBones = false;

		//Stop at the root
		while(targetBone->getParent())
		{
			//Get the vector from the current bone to the end effector position.
			float curToEndX = endX - targetBone->getParent()->getFrameX();
			float curToEndY = endY - targetBone->getParent()->getFrameY();
			float curToEndMag = sqrt( curToEndX*curToEndX + curToEndY*curToEndY );

			//Get the vector from the current bone to the target position.
			float curToTargetX = targetX - targetBone->getParent()->getFrameX();
			float curToTargetY = targetY - targetBone->getParent()->getFrameY();
			float curToTargetMag = sqrt(   curToTargetX*curToTargetX
				+ curToTargetY*curToTargetY );

			//Get rotation to place the end effector on the line from the current
			//joint position to the target position.
			float cosRotAng;
			float sinRotAng;
			float endTargetMag = (curToEndMag*curToTargetMag);

			if( endTargetMag <= 0.00001f )
			{
				cosRotAng = 1.0f;
				sinRotAng = 0.0f;
			}
			else
			{
				cosRotAng = (curToEndX*curToTargetX + curToEndY*curToTargetY) / endTargetMag;
				sinRotAng = (curToEndX*curToTargetY - curToEndY*curToTargetX) / endTargetMag;
			}

			//Clamp the cosine into range when computing the angle
			float rotAng = acosf( std::max(-1.0f, std::min(1.0f,cosRotAng) ) );
			if( sinRotAng < 0.0f )
				rotAng = -rotAng;

			//Rotate the end effector position.
			endX = targetBone->getParent()->getFrameX() + cosRotAng*curToEndX - sinRotAng*curToEndY;
			endY = targetBone->getParent()->getFrameY() + sinRotAng*curToEndX + cosRotAng*curToEndY;

			//Rotate the current bone in local space
			targetBone->setAngle(_simplifyAngle(_simplifyAngle(targetBone->getAngle())) + _simplifyAngle(rotAng));

			//Check for termination
			float endToTargetX = (targetX-endX);
			float endToTargetY = (targetY-endY);

			if( endToTargetX*endToTargetX + endToTargetY*endToTargetY <= mSolvedRadiusSquared )
			{
				//We found a valid solution.
				return true;
			}

			//Track if the arc length that we moved the end effector was
			//a nontrivial distance.
			if( !modifiedBones && fabs(rotAng)*curToEndMag > 0.0001f )
			{
				modifiedBones = true;
			}

			//Stop at fixtures
			if(targetBone->isFixture())
			{
				break;
			}

			targetBone = targetBone->getParent();
		}
		return false;
	}

	float IKSolver::_simplifyAngle( float angle ) const
	{
		angle = fmod(angle,SK_TWO_PI);

		if( angle < -SK_PI )
			angle += SK_TWO_PI;
		else if( angle > SK_PI )
			angle -= SK_TWO_PI;
		return angle;
	}

	void IKSolver::setSolvedRadius( float radius )
	{
		mSolvedRadiusSquared = radius * 2.0f;
	}

	float IKSolver::getSolvedRadiusSquared() const
	{
		return mSolvedRadiusSquared;
	}

	void IKSolver::setAngleLimits( bool limited )
	{
		mConstraints = limited;
	}

	bool IKSolver::isAngleLimited() const
	{
		return mConstraints;
	}

	void IKSolver::setStopAtFixture(bool stopping)
	{
		mFixtures = stopping;
	}

	bool IKSolver::isStoppingAtFixture() const
	{
		return mFixtures;
	}

	void IKSolver::setMaxIterations( size_t iterations )
	{
		mMaxIter = iterations;
	}

	size_t IKSolver::getMaxIterations() const
	{
		return mMaxIter;
	}

	bool IKSolver::solve( Skeleton* skeleton,Bone* targetBone, float targetX, float targetY )
	{
		bool solved = false;
		for(size_t i = 0; i < mMaxIter && !solved; ++i)
		{
			solved = solveIteration(targetBone,targetX,targetY);
			skeleton->updateBones();
		}

		return solved;
	}

	bool IKSolver::solve( Skeleton* skeleton,const std::string& boneName, float targetX, float targetY )
	{
		if(!skeleton->contains(boneName))
		{
			return false;
		}

		bool solved = false;

		Bone* targetBone = skeleton->getByName(boneName);

		for(size_t i = 0; i < mMaxIter && !solved; ++i)
		{
			solved = solveIteration(targetBone,targetX,targetY);
			skeleton->updateBones();
		}

		return solved;
	}

	float IKSolver::_constrainAngle( Bone* bone, float angle ) const
	{
		if(angle > bone->getMaxAngle())
		{
			angle = bone->getMaxAngle();
		}

		if(angle < bone->getMinAngle())
		{
			angle = bone->getMinAngle();
		}

		return angle;
	}

}

