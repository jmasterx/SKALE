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
#ifndef SKALE_SKELETON_HPP
#define SKALE_SKELETON_HPP
#include "SKALE/platform.hpp"
#include "SKALE/Bone.hpp"
#include <map>
#include <vector>
namespace skl
{
	class Skeleton
	{
		Bone root;
		std::map<std::string,Bone*> bones;
		int boneAddedCount;

		void _updateBones(Bone* root,float realStartX, float realStartY, float realStartAngle);
		void _processAnimation(Bone* root);
		bool _getLinesFromFile(const std::string& fileName,
			std::vector<std::string>& lines );
		bool _sortLinesByLevel(std::vector<std::string>& lines,
			std::vector<std::pair<int,std::string*> >& sortedList);
		void _makeBonesFromSortedList(std::vector<std::pair<int,std::string*> >& sortedList);
	public:
		Skeleton(void);
		bool contains(const std::string& name) const;
		Bone* add(float x, float y, float angle, float length, float minAngle, float maxAngle,
			const std::string& name, Bone* parent = NULL);
		bool remove(Bone* bone);
		int count() const;
		Bone* getRoot();
		Bone* getByName(const std::string& name);
		void updateBones();
		void renameBone(const std::string& oldName, const std::string& newName);
		void renameBone(Bone* bone, const std::string& newName);
		int findLevel(const Bone* bone) const;
		bool save(const std::string& fileName) const;
		bool load(const std::string& fileName);
		void setPosition(float x, float y);
		void setAngle(float angle);
		void processAnimation();
		virtual ~Skeleton(void);
	};
}
#endif

