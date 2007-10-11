#ifndef CPPTL_JSON_H_INCLUDED
# define CPPTL_JSON_H_INCLUDED

# include "forwards.h"
# include <string>
# include <vector>

# ifndef JSON_USE_CPPTL_SMALLMAP
#  include <map>
# else
#  include <cpptl/smallmap.h>
# endif
# ifdef JSON_USE_CPPTL
#  include <cpptl/forwards.h>
# endif

/** \brief JSON (JavaScript Object Notation).
*/
namespace Json {

    /** \brief Type of the value held by a Value object.
    */
    enum ValueType
    {
        nullValue = 0, ///< 'null' value
        intValue,      ///< signed integer value
        UIntValue,     ///< UInteger value
        realValue,     ///< double value
        stringValue,   ///< UTF-8 string value
        booleanValue,  ///< bool value
        arrayValue,    ///< array value (ordered list)
        objectValue    ///< object value (collection of name/value pairs).
    };

    enum CommentPlacement
    {
        commentBefore = 0,        ///< a comment placed on the line before a value
        commentAfterOnSameLine,   ///< a comment just after a value on the same line
        commentAfter,             ///< a comment on the line after a value (only make sense for root value)
        numberOfCommentPlacement
    };

    //# ifdef JSON_USE_CPPTL
    //   typedef CppTL::AnyEnumerator<const char *> EnumMemberNames;
    //   typedef CppTL::AnyEnumerator<const Value &> EnumValues;
    //# endif

    /** \brief Lightweight wrapper to tag static string.
    *
    * Value constructor and objectValue member assignement takes advantage of the
    * StaticString and avoid the cost of string duplication when storing the
    * string or the member name.
    *
    * Example of usage:
    * \code
    * Json::Value aValue( StaticString("some text") );
    * Json::Value object;
    * static const StaticString code("code");
    * object[code] = 1234;
    * \endcode
    */
    class JSON_API StaticString
    {
    public:
        StaticString() {}
        explicit StaticString( const char *czstring )
            : str_( czstring )
        {
        }

        operator const char *() const
        {
            return str_;
        }

        const char *c_str() const
        {
            return str_;
        }

    private:
        const char *str_;
    };

    /** \brief Represents a <a HREF="http://www.json.org">JSON</a> value.
    *
    * This class is a discriminated union wrapper that can represents a:
    * - signed integer [range: Value::minInt - Value::maxInt]
    * - UInteger (range: 0 - Value::maxUInt)
    * - double
    * - UTF-8 string
    * - boolean
    * - 'null'
    * - an ordered list of Value
    * - collection of name/value pairs (javascript object)
    *
    * The type of the held value is represented by a #ValueType and 
    * can be obtained using type().
    *
    * values of an #objectValue or #arrayValue can be accessed using operator[]() methods. 
    * Non const methods will automatically create the a #nullValue element 
    * if it does not exist. 
    * The sequence of an #arrayValue will be automatically resize and initialized 
    * with #nullValue. resize() can be used to enlarge or truncate an #arrayValue.
    *
    * The get() methods can be used to obtanis default value in the case the required element
    * does not exist.
    *
    * It is possible to iterate over the list of a #objectValue values using 
    * the getMemberNames() method.
    */
    class JSON_API Value 
    {
        friend class ValueIteratorBase;
    public:
        typedef std::vector<std::string> Members;
        typedef int Int;
        
        typedef ValueIterator iterator;
        typedef ValueConstIterator const_iterator;
        typedef unsigned int ArrayIndex;

        static const Value null;
        static const Int minInt;
        static const Int maxInt;
        static const unsigned int maxUInt;

    private:
#ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION
# ifndef JSON_VALUE_USE_INTERNAL_MAP
        class CZString 
        {
        public:
            enum DuplicationPolicy 
            {
                noDuplication = 0,
                duplicate,
                duplicateOnCopy
            };
            CZString( int index );
            CZString( const char *cstr, DuplicationPolicy allocate );
            CZString( const CZString &other );
            ~CZString();
            CZString &operator =( const CZString &other );
            bool operator<( const CZString &other ) const;
            bool operator==( const CZString &other ) const;
            int index() const;
            const char *c_str() const;
            bool isStaticString() const;
        private:
            void swap( CZString &other );
            const char *cstr_;
            int index_;
        };

    public:
        typedef std::map<CZString, Value> ObjectValues;
# endif // ifndef JSON_VALUE_USE_INTERNAL_MAP
#endif // ifndef JSONCPP_DOC_EXCLUDE_IMPLEMENTATION

    public:
        /** \brief Create a default Value of the given type.

        This is a very useful constructor.
        To create an empty array, pass arrayValue.
        To create an empty object, pass objectValue.
        Another Value can then be set to this one by assignment.
        This is useful since clear() and resize() will not alter types.

        Examples:
        \code
        Json::Value null_value; // null
        Json::Value arr_value(Json::arrayValue); // []
        Json::Value obj_value(Json::objectValue); // {}
        \endcode
        */
        Value( ValueType type = nullValue );
        Value( Int value );
        Value( unsigned int value );
        Value( double value );
        Value( const char *value );
        /** \brief Constructs a value from a static string.

        * Like other value string constructor but do not duplicate the string for
        * internal storage. The given string must remain alive after the call to this
        * constructor.
        * Example of usage:
        * \code
        * Json::Value aValue( StaticString("some text") );
        * \endcode
        */
        Value( const StaticString &value );
        Value( const std::string &value );
        Value( bool value );
        Value( const Value &other );
        ~Value();

        Value &operator=( const Value &other );
        /// Swap values.
        /// \note Currently, comments are intentionally not swapped, for
        /// both logic and efficiency.
        void swap( Value &other );

        ValueType type() const;

        bool operator <( const Value &other ) const;
        bool operator <=( const Value &other ) const;
        bool operator >=( const Value &other ) const;
        bool operator >( const Value &other ) const;

        bool operator ==( const Value &other ) const;
        bool operator !=( const Value &other ) const;

        int compare( const Value &other );

        const char *asCString() const;
        std::string asString() const;
        Int asInt() const;
        unsigned int asUInt() const;
        double asDouble() const;
        bool asBool() const;

        bool isNull() const;
        bool isBool() const;
        bool isInt() const;
        bool isUInt() const;
        bool isIntegral() const;
        bool isDouble() const;
        bool isNumeric() const;
        bool isString() const;
        bool isArray() const;
        bool isObject() const;

        bool isConvertibleTo( ValueType other ) const;

        /// Number of values in array or object
        int size() const;

        /// \brief Return true if empty array, empty object, or null;
        /// otherwise, false.
        bool empty() const;

        /// Return isNull()
        bool operator!() const;

        /// Remove all object members and array elements.
        /// \pre type() is arrayValue, objectValue, or nullValue
        /// \post type() is unchanged
        void clear();

        /// Resize the array to size elements. 
        /// New elements are initialized to null.
        /// May only be called on nullValue or arrayValue.
        /// \pre type() is arrayValue or nullValue
        /// \post type() is arrayValue
        void resize( int size );

        /// Access an array element (zero based index ).
        /// If the array contains less than index element, then null value are inserted
        /// in the array so that its size is index+1.
        /// (You may need to say 'value[0u]' to get your compiler to distinguish
        ///  this from the operator[] which takes a string.)
        Value &operator[]( int index );
        /// Access an array element (zero based index )
        /// (You may need to say 'value[0u]' to get your compiler to distinguish
        ///  this from the operator[] which takes a string.)
        const Value &operator[]( int index ) const;
        /// If the array contains at least index+1 elements, returns the element value, 
        /// otherwise returns defaultValue.
        Value get( int index, 
            const Value &defaultValue ) const;
        /// Return true if index < size().
        bool isValidIndex( int index ) const;
        /// \brief Append value to array at the end.
        ///
        /// Equivalent to jsonvalue[jsonvalue.size()] = value;
        Value &append( const Value &value );

        /// Access an object value by name, create a null member if it does not exist.
        Value &operator[]( const char *key );
        /// Access an object value by name, returns null if there is no member with that name.
        const Value &operator[]( const char *key ) const;
        /// Access an object value by name, create a null member if it does not exist.
        Value &operator[]( const std::string &key );
        /// Access an object value by name, returns null if there is no member with that name.
        const Value &operator[]( const std::string &key ) const;
        /** \brief Access an object value by name, create a null member if it does not exist.

        * If the object as no entry for that name, then the member name used to store
        * the new entry is not duplicated.
        * Example of use:
        * \code
        * Json::Value object;
        * static const StaticString code("code");
        * object[code] = 1234;
        * \endcode
        */
        Value &operator[]( const StaticString &key );
        /// Return the member named key if it exist, defaultValue otherwise.
        Value get( const char *key, 
            const Value &defaultValue ) const;
        /// Return the member named key if it exist, defaultValue otherwise.
        Value get( const std::string &key,
            const Value &defaultValue ) const;
        /// \brief Remove and return the named member.  
        ///
        /// Do nothing if it did not exist.
        /// \return the removed Value, or null.
        /// \pre type() is objectValue or nullValue
        /// \post type() is unchanged
        Value removeMember( const char* key );
        /// Same as removeMember(const char*)
        Value removeMember( const std::string &key );

        /// Return true if the object has a member named key.
        bool isMember( const char *key ) const;
        /// Return true if the object has a member named key.
        bool isMember( const std::string &key ) const;

        /// \brief Return a list of the member names.
        ///
        /// If null, return an empty list.
        /// \pre type() is objectValue or nullValue
        /// \post if type() was nullValue, it remains nullValue
        Members getMemberNames() const;

        //# ifdef JSON_USE_CPPTL
        //      EnumMemberNames enumMemberNames() const;
        //      EnumValues enumValues() const;
        //# endif

        /// Comments must be //... or /* ... */
        void setComment( const char *comment,
            CommentPlacement placement );
        /// Comments must be //... or /* ... */
        void setComment( const std::string &comment,
            CommentPlacement placement );
        bool hasComment( CommentPlacement placement ) const;
        /// Include delimiters and embedded newlines.
        std::string getComment( CommentPlacement placement ) const;

        std::string toStyledString() const;

        const_iterator begin() const;
        const_iterator end() const;

        iterator begin();
        iterator end();

    private:
        Value &resolveReference( const char *key, 
            bool isStatic );

    private:
        struct CommentInfo
        {
            CommentInfo();
            ~CommentInfo();

            void setComment( const char *text );

            char *comment_;
        };

        //struct MemberNamesTransform
        //{
        //   typedef const char *result_type;
        //   const char *operator()( const CZString &name ) const
        //   {
        //      return name.c_str();
        //   }
        //};

        union ValueHolder
        {
            Int int_;
            unsigned int UInt_;
            double real_;
            bool bool_;
            char *string_;
            ObjectValues *map_;
        } value_;
        ValueType type_ : 8;
        int allocated_ : 1;     // Notes: if declared as bool, bitfield is useless.
        CommentInfo *comments_;
    };


    /** \brief Experimental and untested: represents an element of the "path" to access a node.
    */
    class PathArgument
    {
    public:
        friend class Path;

        PathArgument();
        PathArgument( unsigned int index );
        PathArgument( const char *key );
        PathArgument( const std::string &key );

    private:
        enum Kind
        {
            kindNone = 0,
            kindIndex,
            kindKey
        };
        std::string key_;
        unsigned int index_;
        Kind kind_;
    };

    /** \brief Experimental and untested: represents a "path" to access a node.
    *
    * Syntax:
    * - "." => root node
    * - ".[n]" => elements at index 'n' of root node (an array value)
    * - ".name" => member named 'name' of root node (an object value)
    * - ".name1.name2.name3"
    * - ".[0][1][2].name1[3]"
    * - ".%" => member name is provided as parameter
    * - ".[%]" => index is provied as parameter
    */
    class Path
    {
    public:
        Path( const std::string &path,
            const PathArgument &a1 = PathArgument(),
            const PathArgument &a2 = PathArgument(),
            const PathArgument &a3 = PathArgument(),
            const PathArgument &a4 = PathArgument(),
            const PathArgument &a5 = PathArgument() );

        const Value &resolve( const Value &root ) const;
        Value resolve( const Value &root, 
            const Value &defaultValue ) const;
        /// Creates the "path" to access the specified node and returns a reference on the node.
        Value &make( Value &root ) const;

    private:
        typedef std::vector<const PathArgument *> InArgs;
        typedef std::vector<PathArgument> Args;

        void makePath( const std::string &path,
            const InArgs &in );
        void addPathInArg( const std::string &path, 
            const InArgs &in, 
            InArgs::const_iterator &itInArg, 
            PathArgument::Kind kind );
        void invalidPath( const std::string &path, 
            int location );

        Args args_;
    };

    /** \brief Allocator to customize member name and string value memory management done by Value.
    *
    * - makeMemberName() and releaseMemberName() are called to respectively duplicate and
    *   free an Json::objectValue member name.
    * - duplicateStringValue() and releaseStringValue() are called similarly to
    *   duplicate and free a Json::stringValue value.
    */
    class ValueAllocator
    {
    public:
        enum { unknown = (unsigned)-1 };

        virtual ~ValueAllocator();

        virtual char *makeMemberName( const char *memberName ) = 0;
        virtual void releaseMemberName( char *memberName ) = 0;
        virtual char *duplicateStringValue( const char *value, 
            unsigned int length = unknown ) = 0;
        virtual void releaseStringValue( char *value ) = 0;
    };

    /** \brief Experimental and untested: base class for Value iterators.
    *
    */
    class ValueIteratorBase
    {
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef ValueIteratorBase SelfType;

        ValueIteratorBase();
        explicit ValueIteratorBase( const Value::ObjectValues::iterator &current );

        bool operator ==( const SelfType &other ) const
        {
            return isEqual( other );
        }

        bool operator !=( const SelfType &other ) const
        {
            return !isEqual( other );
        }

        difference_type operator -( const SelfType &other ) const
        {
            return computeDistance( other );
        }

        /// Return either the index or the member name of the referenced value as a Value.
        Value key() const;

        /// Return the index of the referenced Value. -1 if it is not an arrayValue.
        unsigned int index() const;

        /// Return the member name of the referenced Value. "" if it is not an objectValue.
        const char *memberName() const;

    protected:
        Value &deref() const;

        void increment();

        void decrement();

        difference_type computeDistance( const SelfType &other ) const;

        bool isEqual( const SelfType &other ) const;

        void copy( const SelfType &other );

    private:
        Value::ObjectValues::iterator current_;
    };

    /** \brief Experimental and untested: const iterator for object and array value.
    *
    */
    class ValueConstIterator : public ValueIteratorBase
    {
        friend class Value;
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef const Value &reference;
        typedef const Value *pointer;
        typedef ValueConstIterator SelfType;

        ValueConstIterator();
    private:
        /*! \internal Use by Value to create an iterator.
        */
        explicit ValueConstIterator( const Value::ObjectValues::iterator &current );
    public:
        SelfType &operator =( const ValueIteratorBase &other );

        SelfType operator++( int )
        {
            SelfType temp( *this );
            ++*this;
            return temp;
        }

        SelfType operator--( int )
        {
            SelfType temp( *this );
            --*this;
            return temp;
        }

        SelfType &operator--()
        {
            decrement();
            return *this;
        }

        SelfType &operator++()
        {
            increment();
            return *this;
        }

        reference operator *() const
        {
            return deref();
        }
    };


    /** \brief Experimental and untested: iterator for object and array value.
    */
    class ValueIterator : public ValueIteratorBase
    {
        friend class Value;
    public:
        typedef unsigned int size_t;
        typedef int difference_type;
        typedef Value &reference;
        typedef Value *pointer;
        typedef ValueIterator SelfType;

        ValueIterator();
        ValueIterator( const ValueConstIterator &other );
        ValueIterator( const ValueIterator &other );
    private:
        /*! \internal Use by Value to create an iterator.
        */
        explicit ValueIterator( const Value::ObjectValues::iterator &current );
    public:

        SelfType &operator =( const SelfType &other );

        SelfType operator++( int )
        {
            SelfType temp( *this );
            ++*this;
            return temp;
        }

        SelfType operator--( int )
        {
            SelfType temp( *this );
            --*this;
            return temp;
        }

        SelfType &operator--()
        {
            decrement();
            return *this;
        }

        SelfType &operator++()
        {
            increment();
            return *this;
        }

        reference operator *() const
        {
            return deref();
        }
    };


} // namespace Json

#endif // CPPTL_JSON_H_INCLUDED
