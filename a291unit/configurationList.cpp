#include "configurationList.h"

a291unit::ConfigurationList::ConfigurationList() {
	mListSize = CFG_LIST_INITIAL_SIZE;
	mListCount = 0;
	mList = new Configurabel*[mListSize];
	for(int i=0; i<mListSize; i++) { mList[i] = NULL; }
}

a291unit::Configurabel* a291unit::ConfigurationList::append(Configurabel* configurabel) {
	Configurabel* c = NULL;
	
	if(mListCount >= mListSize) {
		expandList();
	}
	for(int i=0; i<mListSize; i++) {
		if(mList[i] == NULL) {
			mList[i] = configurabel;
			c = configurabel;
			mListCount++;
			break;
		}
	}
	
	return c;
}

void a291unit::ConfigurationList::expandList() {
	mListSize += CFG_LIST_INCREMENT_SIZE;
	
	Configurabel** newList = new Configurabel*[mListSize];
	for(int i=0; i<mListSize; i++) { newList[i] = (i< mListCount) ? mList[i] : NULL; }
	
	delete mList;
	mList = newList;
}

a291unit::Configurabel* a291unit::ConfigurationList::getFirst() {
	return getNextConfigurable(NULL);
}

a291unit::Configurabel* a291unit::ConfigurationList::getNext(Configurabel* nextOfThis) {
	return getNextConfigurable(nextOfThis);
}

void a291unit::ConfigurationList::deleteAllContent() {
	for(int i=0; i<mListSize; i++) {
		if(mList[i] != NULL) delete(mList[i]);
		mList[i] = NULL;
	}
	
	mListCount = 0;
}

a291unit::Configurabel* a291unit::ConfigurationList::getNextConfigurable(Configurabel* nextOfThis) {
	Configurabel* c = NULL;
	
	if(nextOfThis == NULL) {
		// get first configurable
		c = mList[0];
	}
	else {
		// get following configurable
		int i=0;
		while(i<mListCount) {
			if(mList[i] == nextOfThis) {
				c = mList[i+1];
				break;
			}
			
			i++;
		}
	}
	
	return c;
}
