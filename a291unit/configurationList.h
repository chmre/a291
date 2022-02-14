#ifndef CONFIGURATIONLIST_H
#define CONFIGURATIONLIST_H

#include "configurabel.h"

#define CFG_LIST_INITIAL_SIZE 20;
#define CFG_LIST_INCREMENT_SIZE 10;

namespace a291unit {
	
	class ConfigurationList {

	private:
		int mListSize;
		int mListCount;
		Configurabel** mList;
		
		void expandList();
		Configurabel* getNextConfigurable(Configurabel* nextOfThis);
		
	public:
		ConfigurationList();
		int getListCount() const {return mListCount;}
		void deleteAllContent();
		Configurabel* append(Configurabel* configurabel);
		Configurabel* getFirst();
		Configurabel* getNext(Configurabel* nextOfThis);
		
	};
	
}

#endif
