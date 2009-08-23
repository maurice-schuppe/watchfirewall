#include "simpleBase.h"


//void 
//SimpleBase::IOLog()
//{
//	::IOLog("object: %p; ref: %d\n", this, this->references);
//}

int 
SimpleBase::Retain()
{
	//::IOLog("simple object retain\n");
	//IOLog();
	return OSIncrementAtomic(&references);
}

int 
SimpleBase::Release()
{
	//::IOLog("simple object release\n");
	//IOLog();
	if(OSDecrementAtomic(&references) == 1)
	{
		//::IOLog("try delete object in release %p \n", this);
		Free();
		return 0;
	}
	
	return references;
}

void 
SimpleBase::Free()
{
	//::IOLog("delete object %p \n", this);
	delete this;
}

