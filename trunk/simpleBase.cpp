/*
 *  SimpleBase.cpp
 *  Watch
 *
 *  Created by JanBird on 5/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "simpleBase.h"


void 
SimpleBase::IOLog()
{
	::IOLog("object: %p; ref: %d\n", this, this->references);
}

int 
SimpleBase::retain()
{
	//::IOLog("simple object retain\n");
	//IOLog();
	return OSIncrementAtomic(&references);
}

int 
SimpleBase::release()
{
	//::IOLog("simple object release\n");
	//IOLog();
	if(OSDecrementAtomic(&references) == 1)
	{
		//::IOLog("try delete object in release %p \n", this);
		free();
		return 0;
	}
	
	return references;
}

void 
SimpleBase::free()
{
	//::IOLog("delete object %p \n", this);
	delete this;
}

