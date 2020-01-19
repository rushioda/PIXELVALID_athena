/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AGDDVolumeStore_H
#define AGDDVolumeStore_H

class AGDDVolume;

#include <map>
#include <string>

typedef std::map<std::string,AGDDVolume* > AGDDVolumeMap;

class AGDDVolumeStore: public AGDDVolumeMap {
public:
	AGDDVolumeStore();
	void RegisterVolume(AGDDVolume *);
	AGDDVolume* GetVolume(std::string);
	bool Exist(std::string);
	static AGDDVolumeStore* GetVolumeStore();
	void Clean();
private:

};

#endif
