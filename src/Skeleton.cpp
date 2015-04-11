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
#include "Skeleton.hpp"
#include <sstream>
#include "math.h"
#include <iostream>
#include <fstream>
#include <algorithm>
namespace skl
{
	Skeleton::Skeleton(void)
		: root(0.0f,0.0f,0.0f,0.0f,0.0f,6.283f,false,"ROOT"), boneAddedCount(0)
	{
	}

	Skeleton::~Skeleton(void)
	{
	}

	Bone* Skeleton::add( float x, float y, float angle,
						float length, float minAngle, float maxAngle,
						const std::string& name, Bone* parent /*= NULL*/ )
	{
		boneAddedCount++;
		std::string actualName = name;
		if(actualName == "" )
		{
			std::stringstream ss;
			ss << "Bone ";
			if(boneAddedCount + 1 < 10)
			{
				ss << "0";
			}

			ss << boneAddedCount;

			actualName = ss.str();
		}

		if(!parent)
		{
			parent = &root;
		}

		int count = 0;
		while(contains(actualName) && count < 10)
		{
			actualName += "_1";
		}


		bones[actualName] = parent->add(x,y,angle,length,minAngle,maxAngle,actualName);
		return bones[actualName];


	}

	bool Skeleton::remove( Bone* bone )
	{
		for(std::map<std::string,Bone*>::iterator it = bones.begin();
			it != bones.end(); ++it)
		{
			if(it->second == bone)
			{
				if(bone->getParent() == NULL)
				{
					return false; //Cannot remove root
				}
				else
				{
					bone->getParent()->remove(bone);
					bones.erase(it);
					return true;
				}
			}
		}

		return false;
	}

	Bone* Skeleton::getByName( const std::string& name )
	{
		if(contains(name))
		{
			return bones[name];
		}

		return NULL;
	}

	int Skeleton::count() const
	{
		return bones.size();
	}

	Bone* Skeleton::getRoot()
	{
		return &root;
	}

	void Skeleton::updateBones()
	{
		_updateBones(&root,0.0f,0.0f,0.0f);
	}

	void Skeleton::_updateBones( Bone* root,float realStartX, float realStartY, float realStartAngle )
	{
		if(!root->isRelative())
		{
			realStartX = 0.0f;
			realStartY = 0.0f;
			realStartAngle = 0.0f;
		}
		realStartX += root->getX();
		realStartY += root->getY();
		realStartAngle += root->getAngle();
		float vecX = cos(realStartAngle);
		float vecY = sin(realStartAngle);

		realStartX += (vecX * root->getLength());
		realStartY += (vecY * root->getLength());

		root->setFrame(realStartX,realStartY,realStartAngle);
		
		float angle = fmod(realStartAngle,SK_TWO_PI);

		if( angle < -SK_PI)
			angle += (SK_TWO_PI);
		else if( angle > SK_PI)
			angle -= (SK_TWO_PI);
		for(std::list<Bone>::iterator it = root->begin(); it != root->end(); ++it)
		{
			_updateBones(&(*it),realStartX,realStartY,angle);
		}
	}

	bool Skeleton::save( const std::string& fileName ) const
	{
		std::ofstream file = std::ofstream(fileName.c_str());

		if(!file.is_open())
		{
			return false;
		}
		
		std::stringstream ss;
	
		//Add the root
		//clear stream
		ss.clear();
		ss.str("");

		//Level
		ss << 0;
		ss << " ";

		//X Coordinate
		ss << root.getX();
		ss << " ";

		//Y Coordinate
		ss << root.getY();
		ss << " ";

		//Angle
		ss << fmod(root.getAngle(),6.283f);
		ss << " ";

		//Length
		ss << root.getLength();
		ss << " ";

		//Minimum Angle
		ss << fmod(root.getMinAngle(),6.283f);
		ss << " ";

		//Maximum Angle
		ss << root.getMaxAngle(),6.283f;
		ss << " ";

		//isRelative
		ss << root.isRelative();
		ss << " ";

		//Name
		ss << "\"" << root.getName().c_str() << "\"";
		ss << " ";

		//Parent Name
		ss << "\"\"";

		ss << std::endl;
		file << ss.str().c_str();


		for (std::map<std::string,Bone*>::const_iterator it = bones.begin();
			it != bones.end(); ++it)
		{
			if(!(it->second)->getParent())
			{
				continue;
			}
			//clear stream
			ss.clear();
			ss.str("");

			//Level
			ss << findLevel((it->second));
			ss << " ";

			//X Coordinate
			ss << (it->second)->getX();
			ss << " ";

			//Y Coordinate
			ss << (it->second)->getY();
			ss << " ";

			//Angle
			ss << fmod((it->second)->getAngle(),6.283f);
			ss << " ";

			//Length
			ss << (it->second)->getLength();
			ss << " ";

			//Minimum Angle
			ss << fmod((it->second)->getMinAngle(),6.283f);
			ss << " ";

			//Maximum Angle
			ss << fmod((it->second)->getMaxAngle(),6.283f);
			ss << " ";

			//isRelative
			ss << (it->second)->isRelative();
			ss << " ";

			//isFixture
			ss << (it->second)->isFixture();
			ss << " ";


			//Name
			ss << "\"" <<(it->second)->getName().c_str() << "\"";
			ss << " ";

			//Parent Name
			ss << "\"" <<(it->second)->getParent()->getName().c_str() << "\"";

			ss << std::endl;
			file << ss.str().c_str();

		}

		file.close();

		return true;
	}

	int Skeleton::findLevel( const Bone* bone ) const
	{
		int level = 0;


		while(bone->getParent())
		{
			level++;
			bone = bone->getParent();
		}

		return level;
	}

	bool Skeleton::load( const std::string& fileName )
	{
		std::vector<std::string> lines;
		std::vector<std::pair<int,std::string*> > sortedList;

		if(!_getLinesFromFile(fileName,lines))
		{
			return false;
		}

		if(!_sortLinesByLevel(lines,sortedList))
		{
			return false;
		}

		_makeBonesFromSortedList(sortedList);

		return true;
	}

	bool Skeleton::_getLinesFromFile( const std::string& fileName,
		std::vector<std::string>& lines )
	{
		std::ifstream file = std::ifstream(fileName.c_str());

		if(!file.is_open())
		{
			return false;
		}

		const int BUFFER_SIZE = 2048;
		char buffer[BUFFER_SIZE];

		//read each line into a buffer, then into a string
		while(!file.eof())
		{
			for(int i = 0; i < BUFFER_SIZE; ++i)
			{
				buffer[i] = NULL;
			}
			file.getline(buffer,BUFFER_SIZE - 1);

			//comments are allowed
			if(buffer[0] != '#' && strlen(buffer) > 2)
			lines.push_back(buffer);
		}

		file.close();

		return true;
	}

	bool Skeleton::_sortLinesByLevel( std::vector<std::string>& lines,
		std::vector<std::pair<int,std::string*> >& sortedList )
	{
		std::stringstream ss;

		const int BUFFER_SIZE = 64;
		char buffer[BUFFER_SIZE];

		for (std::vector<std::string>::iterator it = lines.begin();
			it != lines.end(); ++it)
		{
			//clear buffer
			for(int i = 0; i < BUFFER_SIZE; ++i)
			{
				buffer[i] = NULL;
			}

			//Do until space or end of string
			for(int i = 0; (*it)[i] != ' ' && (*it)[i] != NULL && i < BUFFER_SIZE; ++i)
			{
				buffer[i] = (*it)[i];
			}

			ss.clear();
			ss.str("");

			ss << buffer;

			int level = 0;
			ss >> level;

			//Add to list
			sortedList.push_back(std::pair<int, std::string*>(level,&(*it) ));
		}

		std::sort(sortedList.begin(), sortedList.end());

		return true;
	}

	void Skeleton::_makeBonesFromSortedList( std::vector<std::pair<int,std::string*> >& sortedList )
	{
		root.clear();
		bones.clear();

		std::stringstream ss;

		int level = 0;
		float x = 0;
		float y = 0;
		float angle = 0;
		float length = 0;
		float minAngle = 0;
		float maxAngle = 0;
		int relative = 0;
		int fixture = 0;
		char name[256];
		char parentName[256];

		Bone* parent = NULL;
		Bone* child = NULL;
		for( std::vector<std::pair<int,std::string*> >::iterator it = sortedList.begin();
			it != sortedList.end(); ++it)
		{

			sscanf(it->second->c_str(),"%d %f %f %f %f %f %f %d %d \" %[^\"] \" \" %[^\"]",
				&level, &x, &y, &angle, &length, &minAngle, &maxAngle, &relative, &fixture, name,parentName);

			if(level == 1)
			{
				parent = &root;
			}
			else if(level == 0)
			{
				parent = NULL;
			}
			else
			{
				parent = getByName(parentName);
			}
			

			if(parent)
			{
				child = add(x,y,angle,length,minAngle,maxAngle,name,parent);
				child->setAsFixture(fixture == 1);
				if(relative == 0 && child)
				{
					child->setRelative(false);
				}
			}
			else if(level == 0)
			{
				//Add root
				root = Bone(x,y,angle,length,minAngle,maxAngle,relative == 1,"ROOT");
			}
		}
	}

	void Skeleton::setPosition( float x, float y )
	{
		root.set(x,y);
	}

	void Skeleton::setAngle( float angle )
	{
		root.setAngle(angle);
	}

	bool Skeleton::contains( const std::string& name ) const
	{
		return bones.find(name) != bones.end();
	}

	void Skeleton::renameBone( const std::string& oldName, const std::string& newName )
	{
		if(!contains(oldName))
		{
			return;
		}

		Bone* bone = bones[oldName];
		bones.erase(oldName);

		std::string actualName = newName;
		int count = 0;
		while(contains(actualName) && count < 10)
		{
			actualName += "_1";
		}

	
		bone->setName(actualName);
		bones[bone->getName()] = bone;


	}

	void Skeleton::renameBone( Bone* bone, const std::string& newName )
	{
		renameBone(bone->getName(),newName);
	}

	void Skeleton::_processAnimation( Bone* root )
	{
		root->processAnimation();
		for(std::list<Bone>::iterator it = root->begin(); it != root->end(); ++it)
		{
			_processAnimation(&(*it));
		}
	}

	void Skeleton::processAnimation()
	{
		_processAnimation(&root);
	}


}


