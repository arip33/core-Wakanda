/*
* This file is part of Wakanda software, licensed by 4D under
*  (i) the GNU General Public License version 3 (GNU GPL v3), or
*  (ii) the Affero General Public License version 3 (AGPL v3) or
*  (iii) a commercial license.
* This file remains the exclusive property of 4D and/or its licensors
* and is protected by national and international legislations.
* In any event, Licensee's compliance with the terms and conditions
* of the applicable license constitutes a prerequisite to any use of this file.
* Except as otherwise expressly stated in the applicable license,
* such license does not include any other license or rights on this file,
* 4D's and/or its licensors' trademarks and/or other proprietary rights.
* Consequently, no title, copyright or other proprietary rights
* other than those specified in the applicable license is granted.
*/
/**
 *
 * Updated September 27, 2011 - The Datastore API provides server-side JavaScript classes and methods to manage models, datastore classes, attributes, entity collections, and entities on Wakanda Server.
 *
 * @class Datastore
 * @extends Object
 *
 */
EntityAttribute = function EntityAttribute() {
    
    
    /**
     * 
     *
     * @property relatedDataClass
     * @attributes 
     * @type DatastoreClass | Null
     */
    this.relatedDataClass =  new DatastoreClass( ) || new Null( ); 
    
    /**
     * 
     *
     * @property dataClass
     * @attributes 
     * @type DatastoreClass
     */
    this.dataClass =  new DatastoreClass( ); 
    
    /**
     * 
     *
     * @property fullTextIndexed
     * @attributes 
     * @type Boolean
     */
    this.fullTextIndexed =  true; 
    
    /**
     * 
     *
     * @property indexed
     * @attributes 
     * @type Boolean
     */
    this.indexed =  true; 
    
    /**
     * 
     *
     * @property indexType
     * @attributes 
     * @type String
     */
    this.indexType =  ''; 
    
    /**
     * 
     *
     * @property type
     * @attributes 
     * @type String
     */
    this.type =  ''; 
    
    /**
     * 
     *
     * @property kind
     * @attributes 
     * @type String
     */
    this.kind =  ''; 
    
    /**
     * returns the name of the EntityAttribute object as a string
     *
     * @property name
     * @attributes 
     * @type String
     */
    this.name =  ''; 
    
    
    /**
     * returns the name of the EntityAttribute object as a string
     *
     * @method toString
     * @return {String}
     */
    this.toString = function toString() {        return '';     };
    
    /**
     * returns the name of the EntityAttribute object as a string
     *
     * @method getName
     * @return {String}
     */
    this.getName = function getName() {        return '';     };
    

};


Datastore = function Datastore() {
    var    className;
    
    /**
     * 
     *
     * @property dataClasses
     * @attributes ReadOnly
     * @type DataClassEnumerator
     */
    this.dataClasses =  new DataClassEnumerator( ); 
    
    
    /**
     * returns a Folder type reference to the datastore "temporary files" folder
     *
     * @method getTempFolder
     * @return {Folder}
     */
    this.getTempFolder = function getTempFolder() {        return new Folder( );     };
    
    /**
     * returns the name of the current datastore
     *
     * @method getName
     * @return {String}
     */
    this.getName = function getName() {        return '';     };
    
    /**
     * replaces the name of the current datastore with the one you pass in name
     *
     * @method setName
     * @param {String} name
     */
    this.setName = function setName(name) {             };
    
    /**
     * returns a reference, Folder, to the folder containing the datastore model file
     *
     * @method getModelFolder
     * @return {Folder}
     */
    this.getModelFolder = function getModelFolder() {        return new Folder( );     };
    
    /**
     * returns a reference, Folder, to the folder containing the datastore data file
     *
     * @method getDataFolder
     * @return {Folder}
     */
    this.getDataFolder = function getDataFolder() {        return new Folder( );     };
    
    /**
     * flushes the data cache to disk
     *
     * @method flushCache
     */
    this.flushCache = function flushCache() {             };
    
    /**
     * close a datastore file previously opened with the open4DBase( ) or openDataStore( ) method
     *
     * @method close
     * @param {String} syncEventName
     * @return {Object}
     */
    this.close = function close(syncEventName) {        return {};     };
    
    /**
     * cancels the transaction opened by the startTransaction() method at the corresponding level in the current context
     *
     * @method rollBack
     */
    this.rollBack = function rollBack() {             };
    
    /**
     * returns the level of the current transaction for the context
     *
     * @method transactionLevel
     * @return {Number}
     */
    this.transactionLevel = function transactionLevel() {        return 0;     };
    
    /**
     * starts a  transaction in the current context
     *
     * @method startTransaction
     */
    this.startTransaction = function startTransaction() {             };
    
    /**
     * accepts the transaction opened by the startTransaction() method  at the corresponding level in the current context
     *
     * @method commit
     */
    this.commit = function commit() {             };
    
    /**
     * compacts the datastore's data file designated by model and data, and generates the compactedData data file.
     *
     * @method compactDataStore
     * @param {File} model
     * @param {File} data
     * @param {Object} options
     * @param {File} compactedData
     */
    this.compactDataStore = function compactDataStore(model, data, options, compactedData) {             };
    
    /**
     * repairs the datastore's data file defined by model and data, and generates the repairedData data file.
     *
     * @method repairDataStore
     * @param {File} model
     * @param {File} data
     * @param {Object} options
     * @param {File} repairedData
     */
    this.repairDataStore = function repairDataStore(model, data, options, repairedData) {             };
    
    /**
     * verifies the structure of the objects contained in a data file designated by model and data.
     *
     * @method verifyDataStore
     * @param {File} model
     * @param {File} data
     * @param {Object} options
     */
    this.verifyDataStore = function verifyDataStore(model, data, options) {             };
    

};


//ds = new Datastore();

Entity = function Entity() {
    
    
    
    /**
     * releases the entity from memory
     *
     * @method release
     */
    this.release = function release() {             };
    
    /**
     * reloads the entity as it is stored in the datastore
     *
     * @method refresh
     */
    this.refresh = function refresh() {             };
    
    /**
     * saves the changes made to the entity in the datastore
     *
     * @method save
     */
    this.save = function save() {             };
    
    /**
     * returns a string representation of the entity or entity collection
     *
     * @method toString
     * @return {String}
     */
    this.toString = function toString() {        return '';     };
    
    /**
     * returns the datastore class (object of the DatastoreClass type)  of the entity
     *
     * @method getDataClass
     * @return {DatastoreClass}
     */
    this.getDataClass = function getDataClass() {        return new DatastoreClass( );     };
    
    /**
     * returns the timestamp of the last save of the entity to which it is applied
     *
     * @method getTimeStamp
     * @return {Date}
     */
    this.getTimeStamp = function getTimeStamp() {        return new Date( );     };
    
    /**
     * removes the entity from the datastore
     *
     * @method remove
     */
    this.remove = function remove() {             };
    
    /**
     * returns True or False depending on whether the entity to which it is applied has just been created in the datastore
     *
     * @method isNew
     * @return {Boolean}
     */
    this.isNew = function isNew() {        return true;     };
    
    /**
     * returns True or False depending on whether the entity to which it is applied has been  modified since the last time it was loaded from the datastore
     *
     * @method isModified
     * @return {Boolean}
     */
    this.isModified = function isModified() {        return true;     };
    
    /**
     * returns True or False depending on whether the entity iterator points to an entity that is currently loaded in memory
     *
     * @method loaded
     * @return {Boolean}
     */
    this.loaded = function loaded() {        return true;     };
    
    /**
     * checks the validity of the pointer to the current entity within an iteration of entities
     *
     * @method valid
     * @return {Boolean}
     */
    this.valid = function valid() {        return true;     };
    
    /**
     * puts the entity pointer on the next entity within an iteration of entities
     *
     * @method next
     * @return {Boolean}
     */
    this.next = function next() {        return true;     };
    

};


EntityCollection = function EntityCollection() {
    
    
    /**
     * 
     *
     * @property queryPath
     * @attributes ReadOnly
     * @type String
     */
    this.queryPath =  ''; 
    
    /**
     * 
     *
     * @property queryPlan
     * @attributes ReadOnly
     * @type String
     */
    this.queryPlan =  ''; 
    
    /**
     * 
     *
     * @property length
     * @attributes ReadOnly
     * @type Number
     */
    this.length =  0; 
    
    
    /**
     * permanently removes entities from the datastore
     *
     * @method remove
     */
    this.remove = function remove() {             };
    
    /**
     * returns a string representation of the entity or entity collection
     *
     * @method toString
     * @return {String}
     */
    this.toString = function toString() {        return '';     };
    
    /**
     * creates an array and returns in it all the distinct values stored in attribute for the entity collection or datastore class
     *
     * @method distinctValues
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Array}
     */
    this.distinctValues = function distinctValues(attribute) {        return [];     };
    
    /**
     * performs, in a single call, all the statistical calculations on the attribute or list of attributes passed as the parameter for the datastore class or entity collection
     *
     * @method compute
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Object}
     */
    this.compute = function compute(attribute, distinct) {        return {};     };
    
    /**
     * returns the arithmetic average of all the non-null values of attribute for the datastore class or entity collection
     *
     * @method average
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.average = function average(attribute, distinct) {        return 0;     };
    
    /**
     * returns the maximum value among all the values of attribute in the entity collection or datastore class
     *
     * @method max
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Number}
     */
    this.max = function max(attribute) {        return 0;     };
    
    /**
     * returns the lowest (or minimum) value among all the values of attribute in the entity collection or datastore class
     *
     * @method min
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Number}
     */
    this.min = function min(attribute) {        return 0;     };
    
    /**
     * searches all the entities in the datastore class or entity collection using the search criteria specified in queryString and returns a new collection containing the entities found
     *
     * @method query
     * @param {String} queryString
     * @return {EntityCollection}
     */
    this.query = function query(queryString) {        return new EntityCollection( );     };
    
    /**
     * applies the search criteria specified in queryString to all the entities of the DatastoreClass or EntityCollection and returns the first entity found in an object of type Entity
     *
     * @method find
     * @param {String} queryString
     * @return {Entity}
     */
    this.find = function find(queryString) {        return new Entity( );     };
    
    /**
     * creates and returns a JavaScript array where each element is an object containing a set of properties and values corresponding to the attribute names and values for a datastore class or an entity collection
     *
     * @method toArray
     * @param {String | DatastoreClassAttribute} attributeList
     * @param {String} sortList
     * @param {String | Boolean} key
     * @param {Number} skip
     * @param {Number} top
     * @return {Array}
     */
    this.toArray = function toArray(attributeList, sortList, key, skip, top) {        return [];     };
    
    /**
     * returns the sum (i.e., total of all the values) of attribute for the datastore class or entity collection
     *
     * @method sum
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.sum = function sum(attribute, distinct) {        return 0;     };
    
    /**
     * executes the callbackFn function on each entity in the entity collection in ascending order
     *
     * @method forEach
     * @param {Function} callbackFn
     */
    this.forEach = function forEach(callbackFn) {             };
    
    /**
     * returns the datastore class (object of the&nbsp;DatastoreClass type) of the entity collection
     *
     * @method getDataClass
     * @return {DatastoreClass}
     */
    this.getDataClass = function getDataClass() {        return new DatastoreClass( );     };
    
    /**
     * adds the entity or entity collection passed in the toAdd parameter to the entity collection
     *
     * @method add
     * @param {EntityCollection | Entity} toAdd
     */
    this.add = function add(toAdd) {             };
    
    /**
     * sorts the entities in the entity collection or datastore class and returns a new entity collection containing sorted data
     *
     * @method orderBy
     * @param {String | DatastoreClassAttribute} attributeList
     * @param {String} sortOrder
     * @return {EntityCollection}
     */
    this.orderBy = function orderBy(attributeList, sortOrder) {        return new EntityCollection( );     };
    
    /**
     * returns the number of entities contained in the entity collection or datastore class
     *
     * @method count
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.count = function count(attribute, distinct) {        return 0;     };
    
    /**
     * returns the entity in the first position of the entity collection or datastore class
     *
     * @method first
     * @return {Entity}
     */
    this.first = function first() {        return new Entity( );     };
    
    /**
     * creates a new entity collection that contains all the entities from the entity collection to which it is applied and all the entities from the collection2 entity collection
     *
     * @method or
     * @param {EntityCollection} collection2
     * @return {EntityCollection}
     */
    this.or = function or(collection2) {        return new EntityCollection( );     };
    
    /**
     * compares the entity collection to  which it is applied and the collection2 and returns a new entity collection that contains only the entities that are referenced in both collections
     *
     * @method and
     * @param {EntityCollection} collection2
     * @return {EntityCollection}
     */
    this.and = function and(collection2) {        return new EntityCollection( );     };
    
    /**
     * excludes from the entity collection to  which it is applied the entities that are in the collection2 and returns the resulting entity collection
     *
     * @method minus
     * @param {EntityCollection} collection2
     * @return {EntityCollection}
     */
    this.minus = function minus(collection2) {        return new EntityCollection( );     };
    

};


dataClass = function dataClass() {
    
    
    /**
     * 
     *
     * @property length
     * @attributes ReadOnly
     * @type Number
     */
    this.length =  0; 
    
    
    /**
     * creates a new blank object of type EntityCollection attached to the datastore class to which it is applied
     *
     * @method createEntityCollection
     * @return {EntityCollection}
     */
    this.createEntityCollection = function createEntityCollection() {        return new EntityCollection( );     };
    
    /**
     * permanently removes entities from the datastore
     *
     * @method remove
     */
    this.remove = function remove() {             };
    
    /**
     * returns the name of the datastore class as a string
     *
     * @method toString
     * @return {String}
     */
    this.toString = function toString() {        return '';     };
    
    /**
     * creates an array and returns in it all the distinct values stored in attribute for the entity collection or datastore class
     *
     * @method distinctValues
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Array}
     */
    this.distinctValues = function distinctValues(attribute) {        return [];     };
    
    /**
     * performs, in a single call, all the statistical calculations on the attribute or list of attributes passed as the parameter for the datastore class or entity collection
     *
     * @method compute
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Object}
     */
    this.compute = function compute(attribute, distinct) {        return {};     };
    
    /**
     * returns the arithmetic average of all the non-null values of attribute for the datastore class or entity collection
     *
     * @method average
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.average = function average(attribute, distinct) {        return 0;     };
    
    /**
     * returns the maximum value among all the values of attribute in the entity collection or datastore class
     *
     * @method max
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Number}
     */
    this.max = function max(attribute) {        return 0;     };
    
    /**
     * returns the lowest (or minimum) value among all the values of attribute in the entity collection or datastore class
     *
     * @method min
     * @param {DatastoreClassAttribute | String} attribute
     * @return {Number}
     */
    this.min = function min(attribute) {        return 0;     };
    
    /**
     * executes the callbackFn function on each entity in the entity collection in ascending order
     *
     * @method forEach
     * @param {Function} callbackFn
     */
    this.forEach = function forEach(callbackFn) {             };
    
    /**
     * sorts the entities in the entity collection or datastore class and returns a new entity collection containing sorted data
     *
     * @method orderBy
     * @param {String | DatastoreClassAttribute} attributeList
     * @param {String} sortOrder
     * @return {EntityCollection}
     */
    this.orderBy = function orderBy(attributeList, sortOrder) {        return new EntityCollection( );     };
    
    /**
     * returns the number of entities contained in the entity collection or datastore class
     *
     * @method count
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.count = function count(attribute, distinct) {        return 0;     };
    
    /**
     * returns the entity in the first position of the entity collection or datastore class
     *
     * @method first
     * @return {Entity}
     */
    this.first = function first() {        return new Entity( );     };
    
    /**
     * returns the sum (i.e., total of all the values) of attribute for the datastore class or entity collection
     *
     * @method sum
     * @param {DatastoreClassAttribute | String} attribute
     * @param {Boolean | String} distinct
     * @return {Number}
     */
    this.sum = function sum(attribute, distinct) {        return 0;     };
    
    /**
     * creates and returns a JavaScript array where each element is an object containing a set of properties and values corresponding to the attribute names and values for a datastore class or an entity collection
     *
     * @method toArray
     * @param {String | DatastoreClassAttribute} attributeList
     * @param {String} sortList
     * @param {String | Boolean} key
     * @param {Number} skip
     * @param {Number} top
     * @return {Array}
     */
    this.toArray = function toArray(attributeList, sortList, key, skip, top) {        return [];     };
    
    /**
     * (re)sets the start value for the autosequence number of the datastore class
     *
     * @method setAutoSequenceNumber
     * @param {Number} counter
     */
    this.setAutoSequenceNumber = function setAutoSequenceNumber(counter) {             };
    
    /**
     * generates entities in the datastore class where it is applied
     *
     * @method fromArray
     * @param {Array} arrayValues
     */
    this.fromArray = function fromArray(arrayValues) {             };
    
    /**
     * creates a new blank object of type Entity based on the datastore class to which it is applied
     *
     * @method createEntity
     * @return {Entity}
     */
    this.createEntity = function createEntity() {        return new Entity( );     };
    
    /**
     * applies the search criteria specified in queryString to all the entities of the DatastoreClass or EntityCollection and returns the first entity found in an object of type Entity
     *
     * @method find
     * @param {String} queryString
     * @return {Entity}
     */
    this.find = function find(queryString) {        return new Entity( );     };
    
    /**
     * searches all the entities in the datastore class or entity collection using the search criteria specified in queryString and returns a new collection containing the entities found
     *
     * @method query
     * @param {String} queryString
     * @return {EntityCollection}
     */
    this.query = function query(queryString) {        return new EntityCollection( );     };
    
    /**
     * returns an object of type EntityCollection containing all the entities in the datastore class to which it was applied
     *
     * @method all
     * @return {EntityCollection}
     */
    this.all = function all() {        return new EntityCollection( );     };
    
    /**
     * returns the name of the datastore class to which it is applied in a string
     *
     * @method getName
     * @return {String}
     */
    this.getName = function getName() {        return '';     };
    

};

