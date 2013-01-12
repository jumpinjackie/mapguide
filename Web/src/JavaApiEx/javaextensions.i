//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

//---------------------- Java interface decorations ------------------------------//

//NOTE: If we're eventually targeting JDK7, change to AutoCloseable for automatic try-with-resources syntax support
//%typemap(javainterfaces) MgReader "java.io.Closeable"
//%typemap(javainterfaces) MgFeatureReader "java.io.Closeable"
//%typemap(javainterfaces) MgDataReader "java.io.Closeable"
//%typemap(javainterfaces) MgSqlDataReader "java.io.Closeable"
//%typemap(javainterfaces) MgLongTransactionReader "java.io.Closeable"
//%typemap(javainterfaces) MgSpatialContextReader "java.io.Closeable"

//---------------------- Renames to avoid Java/C++ API clashes ---------------------------//

//Already defined in Java Exception so rename our proxy method
%rename(getExceptionStackTrace) MgException::GetStackTrace;

//delete() is the name of the standard SWIG release method called on finalize(). Unfortunately this conflicts with
//MgPropertyDefinition::Delete, MgClassDefinition::Delete and MgFeatureSchema::Delete when java proxy clases for these
//classes are generated
//
//So rename the java proxies to these methods. This is the most minimally destructive change of all the available options
//available to us
%rename(markAsDeleted) MgPropertyDefinition::Delete;
%rename(markAsDeleted) MgClassDefinition::Delete;
%rename(markAsDeleted) MgFeatureSchema::Delete;

//If we want to implement java.util.Collection, we need to rename this incompatible API (as add() is expected to 
//return boolean in the java.util.Collection API)
%rename(addItem) MgBatchPropertyCollection::Add;
%rename(addItem) MgClassDefinitionCollection::Add;
%rename(addItem) MgFeatureSchemaCollection::Add;
%rename(addItem) MgPropertyDefinitionCollection::Add;
%rename(addItem) MgIntCollection::Add;
%rename(addItem) MgPropertyCollection::Add;
%rename(addItem) MgStringCollection::Add;
%rename(addItem) MgLayerCollection::Add;
%rename(addItem) MgLayerGroupCollection::Add;

//This is a helper macro to implement the required APIs for collection items
//
//This could possibly apply to all proxy classes, but for now we're only interested in classes
//that are part of any collection class that implements java.util.Collection. This is needed for
//contains() and other container tests for equality to work
//
//Implementation borrowed from OGR swig java bindings
%define IMPLEMENT_MG_ITEM_API(item_type)

%typemap(javacode) item_type %{
    public boolean equals(Object obj) {
        boolean equal=false;
        if(obj instanceof $javaclassname)
            equal=((($javaclassname)obj).swigCPtr==this.swigCPtr);
        return equal;
    }
    
    public int hashCode() {
        return (int)swigCPtr;
    }
%}

%enddef

//This is a helper macro to implement the bulk of the java.util.Collection interface for any proxy class
//that needs one
//
//Kinda like C++ templates for Java without the suck. And we all know how much Java generics suck :D
//
//TODO: Not sure if a SWIG macro can call into another SWIG macro, but if it can it should call into
//the IMPLEMENT_MG_JAVA_ITERABLE_API
%define IMPLEMENT_MG_JAVA_COLLECTION_API(collection_type, item_type)
//Necessary imports
%typemap(javaimports) collection_type %{
import java.util.Collection;
import java.util.Iterator;
import java.util.ArrayList;
import java.util.NoSuchElementException;
%}
//Java Interfaces implemented by the java proxy class
%typemap(javainterfaces) collection_type "Collection<item_type>"
//This is the java.util.Collection implementation that is injected into each java proxy class
%typemap(javacode) collection_type %{
    /**
     * An iterator to iterate over this collection
     */
    class ItemIterator implements Iterator<item_type> {
        private collection_type _collection;
        private int _pos;
        private int _count;
        
        public ItemIterator(collection_type c) { 
            _collection = c; 
            _count = _collection.getCount();
            _pos = -1;
        }
        
        /**
         * Returns true if the iteration has more elements
         */
        public boolean hasNext() {
            return _pos + 1 < _count;
        }
        
        /**
         * Returns the next item_type in the collection
         */
        public item_type next() {
            _pos++;
            if (_pos >= _count)
                throw new NoSuchElementException();
            return _collection.getItem(_pos);
        }
        
        /**
         * Removes from the underlying collection the last element returned by the iterator (not supported).
         */
        public void remove() {
            throw new UnsupportedOperationException();
        }
    }
    
    /**
     * Returns an iterator over the elements in this collection.
     */
    public Iterator<item_type> iterator() { return new ItemIterator(this); }

    /**
     * Ensures that this collection contains the specified element (optional operation).
     */
    public boolean add(item_type item) {
        this.addItem(item);
        return true;
    }

    /**
     * Adds all of the elements in the specified collection to this collection (optional operation).
     */
    public boolean addAll(Collection<? extends item_type> c) {
        int added = 0;
        for (item_type item : c) {
            this.addItem(item);
            added++;
        }
        return added > 0;
    }

    /**
     *  Returns true if this collection contains the specified element.
     */
    public boolean contains(Object o) {
        if (o instanceof item_type) {
            return this.contains((item_type)o);
        }
        return false;
    }

    /**
     * Returns true if this collection contains all of the elements in the specified collection.
     */
    public boolean containsAll(Collection<?> c) {
        for (Object o : c) {
            if (!this.contains(o))
                return false;
        }
        return true;
    }

    /**
     * Returns true if this collection contains no elements.
     */
    public boolean isEmpty() { return this.getCount() == 0; }

    /**
     * Removes a single instance of the specified element from this collection, if it is present (optional operation).
     */
    public boolean remove(Object o) {
        if (o instanceof item_type) {
            return this.remove((item_type)o);
        }
        return false;
    }

    /**
     * Removes all of this collection's elements that are also contained in the specified collection (optional operation).
     */
    public boolean removeAll(Collection<?> c) {
        int removed = 0;
        for (Object o : c) {
            if (this.remove(o))
                removed++;
        }
        return removed > 0;
    }

    /**
     * Retains only the elements in this collection that are contained in the specified collection (optional operation).
     */
    public boolean retainAll(Collection<?> c) {
        int removed = 0;
        ArrayList<item_type> remove = new ArrayList<item_type>();
        for (int i = 0; i < this.getCount(); i++) {
            item_type item = this.getItem(i);
            if (!c.contains(item))
                remove.add(item);
        }
        if (remove.size() > 0) {
            return this.removeAll(remove);
        } else {
            return false;
        }
    }

    /**
     * Returns the number of elements in this collection.
     */
    public int size() { return this.getCount(); }

    /**
     * Returns an array containing all of the elements in this collection.
     */
    public Object[] toArray() {
        int count = this.getCount();
        Object[] items = new Object[count];
        for (int i = 0; i < count; i++) {
            items[i] = this.getItem(i);
        }
        return items;
    }

    /**
     * Returns an array containing all of the elements in this collection; the runtime type of the returned array is that of the specified array.
     */
    public <item_type> item_type[] toArray(item_type[] a) {
        Object[] items = this.toArray();
        if (a.length >= items.length) {
            for (int i = 0; i < items.length; i++) {
                a[i] = (item_type)items[i];
            }
            for (int i = items.length; i < a.length; i++) {
                a[i] = null;
            }
            return a;
        } else {
            item_type[] retVal = (item_type[])java.lang.reflect.Array.newInstance(a.getClass().getComponentType(), items.length);
            for (int i = 0; i < retVal.length; i++) {
                retVal[i] = (item_type)items[i];
            }
            return retVal;
        }
    }

%}
%enddef

//This helper macro implements the java.lang.Iterable API allowing the instance of the
//java proxy to be used in an enhanced for loop (akin to IEnumerable/foreach in C#)
%define IMPLEMENT_MG_JAVA_ITERABLE_API(collection_type, item_type)
//Necessary imports
%typemap(javaimports) collection_type %{
import java.util.Iterator;
import java.util.NoSuchElementException;
%}
//Java Interfaces implemented by the java proxy class
%typemap(javainterfaces) collection_type "java.lang.Iterable<item_type>"
//This is the java.lang.Iterable<T> implementation that is injected into each java proxy class
%typemap(javacode) collection_type %{
    /**
     * An iterator to iterate over this collection
     */
    class ItemIterator implements Iterator<item_type> {
        private collection_type _collection;
        private int _pos;
        private int _count;
        
        public ItemIterator(collection_type c) { 
            _collection = c; 
            _count = _collection.getCount();
            _pos = -1;
        }
        
        /**
         * Returns true if the iteration has more elements
         */
        public boolean hasNext() {
            return _pos + 1 < _count;
        }
        
        /**
         * Returns the next item_type in the collection
         */
        public item_type next() {
            _pos++;
            if (_pos >= _count)
                throw new NoSuchElementException();
            return _collection.getItem(_pos);
        }
        
        /**
         * Removes from the underlying collection the last element returned by the iterator (not supported).
         */
        public void remove() {
            throw new UnsupportedOperationException();
        }
    }

    /**
     * Returns an iterator over the elements in this collection.
     */
    public Iterator<item_type> iterator() { return new ItemIterator(this); }
    
%}
%enddef

//Plug the stock implementation for our MapGuide collection and item classes
IMPLEMENT_MG_ITEM_API(MgClassDefinition)
IMPLEMENT_MG_ITEM_API(MgFeatureSchema)
IMPLEMENT_MG_ITEM_API(MgPropertyDefinition)
IMPLEMENT_MG_ITEM_API(MgProperty)
IMPLEMENT_MG_ITEM_API(MgLayerBase)
IMPLEMENT_MG_ITEM_API(MgLayerGroup)

//IMPLEMENT_MG_JAVA_COLLECTION_API(MgBatchPropertyCollection, MgPropertyCollection)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgClassDefinitionCollection, MgClassDefinition)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgFeatureSchemaCollection, MgFeatureSchema)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgPropertyDefinitionCollection, MgPropertyDefinition)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgPropertyCollection, MgProperty)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgStringCollection, String)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgLayerCollection, MgLayerBase)
IMPLEMENT_MG_JAVA_COLLECTION_API(MgLayerGroupCollection, MgLayerGroup)
IMPLEMENT_MG_JAVA_ITERABLE_API(MgReadOnlyLayerCollection, MgLayerBase)