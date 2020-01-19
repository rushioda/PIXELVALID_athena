/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COLLECTIONBASE_ICOLLECTIONDATAEDITOR_H
#define COLLECTIONBASE_ICOLLECTIONDATAEDITOR_H

#include <string>


namespace coral {
  class AttributeList;
}

namespace pool {

  class CollectionRowBuffer;
  class TokenList;

  /** 
   * @class ICollectionDataEditor ICollectionDataEditor.h CollectionBase/ICollectionDataEditor.h
   *
   * An interface used to add, update or delete data in a collection.
   */
  class ICollectionDataEditor
  {
  public:
    /**
     * Sets the number of rows to cache before insertion into the collection. If the input
     * argument is 0 (the default) bulk row insertion will not be applied.
     *
     * @param rowCacheSize Number of rows to cache before insertion into collection.
     */
    virtual void setRowCacheSize( int rowCacheSize = 0 ) = 0; 

    /**
     * Returns a reference to a collection row buffer for adding rows of data to the collection.
     * If data for one or more columns will not be provided use the method `emptyRowBuffer' instead.
     */
    virtual CollectionRowBuffer& rowBuffer() = 0;

    /**
     * Returns a reference to an empty collection row buffer for adding rows of data to the collection.
     * If data will be provided for all columns use the method `rowBuffer' instead since buffers do
     * not need to be cleared in this case.
     */
    virtual CollectionRowBuffer& emptyRowBuffer() = 0;

    /**
     * Clears all row buffers. Should be applied before inserting rows for which some columns are
     * expected to have null values.
     */
    virtual void clearRowBuffers() = 0;

    /**
     * Adds a new row of data to the collection. An option is provided to turn off the
     * written records counter to improve performance.
     *
     * @param inputRowBuffer Buffer containing data for row to be added.
     * @param updateRecordsCounter Flag indicating whether to increment written records counter. 
     */
    virtual void insertRow( const pool::CollectionRowBuffer& inputRowBuffer, bool updateRecordsCounter = true ) = 0;

    /**
     * Updates the rows of the collection that satisfy a query. Returns the number of rows updated.
     * Throws an exception if both the Attribute and Token set lists provided as input are empty.
     *
     * @param tokenSetList List of Tokens to set with associated values.
     * @param attributeSetList List of Attributes to set with associated values.
     * @param whereClause Predicates of query. 
     * @param attributeBindData Attribute bind data for where clause.
     * @param tokenBindData Token bind data for where clause.
     */
    virtual int updateRows( coral::AttributeList* attributeSetList = 0,
                             TokenList* tokenSetList = 0,
		             const std::string& whereClause = "",
		             coral::AttributeList* attributeBindData = 0,
                             pool::TokenList* tokenBindData = 0 ) = 0;

    /**
     * Deletes the rows of the collection that satisfy a query. Returns the number of rows deleted.
     * Throws an exception if no where clause is provided as input. An option is provided to turn
     * off the written and deleted records counters if the written records counter was turned off
     * during record insertion to improve performance.
     *
     * @param whereClause Predicates of query.
     * @param attributeBindData Attribute bind data for where clause.
     * @param tokenBindData Token bind data for where clause.
     * @param updateRecordsCounter Flag indicating whether to decrement written records counter and increment deleted records counter.
     */
    virtual int deleteRows( const std::string& whereClause,
			     coral::AttributeList* attributeBindData = 0,
                             pool::TokenList* tokenBindData = 0,
                             bool updateRecordsCounter = true ) = 0;

    // TODO protected:
    /// Empty destructor.
    virtual ~ICollectionDataEditor() {}
  };

}

#endif

