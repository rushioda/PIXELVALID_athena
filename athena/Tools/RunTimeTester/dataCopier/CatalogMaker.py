# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#Make a new RTT dataset Catalog
import os.path, time, logging
from DataSetCatalog     import DataSetCatalog
from VersionedFileSaver import VersionedFileSaver
from formatCollection import formatCollection

logger = logging.getLogger('rtt')

class CatalogMaker:
    def __init__(self, oldCatalog,newCatalog):
        self.oldcat  = oldCatalog
        self.newcat  = newCatalog 
        self.catalog = DataSetCatalog(self.oldcat).catalog

    def makeCatalog(self):
        logger.debug('versioning old catalog')
        vfs =VersionedFileSaver(8, os.path.dirname(self.oldcat))
        vfs.saveFile(self.oldcat)

        logger.debug('making new catalog')
        fh = open(self.newcat, 'w')
        fh.write('<?xml version="1.0"?>'+'\n')
        fh.write('\n<!-- ********************************************************\n')
        fh.write('\n            RTT Dataset Catalog                              \n')
        fh.write('\n   Generated by  : DataSetManager                            \n')
        fh.write('\n   Time          : '+time.ctime()+'                          \n')
        fh.write('\n**********************************************************-->\n')
        fh.write('\n<dsns>'+'\n')
  
        cdict = self.catalog
        print 'No of datasets in new catalog: '+str(len(cdict.keys()))
        for key in cdict.keys():
            fh.write(' <dsn>'+'\n')
            fh.write('    <logical>' + key +'</logical>'+'\n')
            fh.write('    <physical>'+ cdict[key] +'</physical>'+'\n')
            fh.write(' </dsn>'+'\n')
        fh.write('</dsns>'+'\n')
        fh.close()

    def update(self, copiedDatasets, allDatasets):
        'datasets is a list dataset objects'
        datasets = copiedDatasets
        datasets.extend(self.datasetNotInCatalog(allDatasets))
        catalog  = self.catalog
        for ds in datasets:
            print 'new dataset added to catalog '+str(ds.logicalName)
            if ds.dsSource != 'dq2':
                catalog[ds.logicalName] = ds.physicalName
            else:
                for file in ds.physicalFiles:
                    lname = os.path.join(ds.logicalName, os.path.basename(file))
                    catalog[lname] = file
                
    def datasetNotInCatalog(self, requestedDatasets):
        nic = [ds for ds in requestedDatasets
               if ds.logicalName not in self.catalog.keys()]
        dsnic = [ds for ds in nic if ds.isInDestDir]

        return dsnic

if __name__ == '__main__':
    #pass
    cm = CatalogMaker('/home/en/RTT/dsmPoject/rttDataSetCatalog.xml', 'newCatalog.xml')
    cm.makeCatalog()
        
