/*
 * Copyright (c) 2001-2006
 *     DecisionSoft Limited. All rights reserved.
 * Copyright (c) 2004-2006
 *     Progress Software Corporation. All rights reserved.
 * Copyright (c) 2004-2006
 *     Oracle. All rights reserved.
 *
 * See the file LICENSE for redistribution information.
 *
 * $Id: SequenceType.hpp,v 1.14 2006/11/01 16:37:17 jpcs Exp $
 */

#ifndef _SEQUENCETYPE_HPP
#define _SEQUENCETYPE_HPP

#include <xqilla/framework/XQillaExport.hpp>

#include <xqilla/items/Node.hpp>
#include <xqilla/parser/QName.hpp>
#include <xqilla/runtime/Result.hpp>
#include <xqilla/runtime/ResultImpl.hpp>

class Item;
class XPath2MemoryManager;

class XQILLA_API SequenceType : public LocationInfo
{
public:

  class XQILLA_API ItemType
  {
  public:

    /**
     * The type of item that this sequence can hold.
     */
    typedef enum {
      TEST_ELEMENT,      ///< element node
      TEST_ATTRIBUTE,    ///< attribute node
      TEST_SCHEMA_ELEMENT,      ///< element node
      TEST_SCHEMA_ATTRIBUTE,    ///< attribute node
      TEST_NODE,         ///< node
      TEST_PI,           ///< processing instruction node
      TEST_COMMENT,      ///< comment node
      TEST_TEXT,         ///< text node
      TEST_DOCUMENT,     ///< document node
      TEST_ANYTHING,     ///< any item
      TEST_ATOMIC_TYPE   ///< the named atomic type or parent's named atomic type if derived through restriction
    } ItemTestType;

    ItemType(ItemTestType test, QualifiedName* name=NULL, QualifiedName* type=NULL);

    /**
     * Destructor.
     */
    ~ItemType();

    /**
     * Getter for m_nTestType
     */
    ItemTestType getItemTestType(void) const;

    void setAllowNilled(bool value);
    bool getAllowNilled() const;

    QualifiedName *getName(void) const;
    QualifiedName *getType(void) const;

    void setName(QualifiedName * name);
    void setType(QualifiedName * type);

    void setNameURI(const XMLCh* const nameURI);
    void setTypeURI(const XMLCh* const typeURI);

    //Return a URI, firstly the overriding URI string, if not, the URI bond to the QName prefix
    const XMLCh* getTypeURI(const StaticContext* context, const LocationInfo *location) const;
    const XMLCh* getNameURI(const StaticContext* context, const LocationInfo *location) const;

    void getStaticType(StaticType &st, const StaticContext *context,
                       bool &isExact, const LocationInfo *location) const;

    bool matches(const Item::Ptr &toBeTested, DynamicContext* context, const LocationInfo *location) const;
    bool matches(const Node::Ptr &toBeTested, DynamicContext* context, const LocationInfo *location) const;
    bool matchesNameType(const Item::Ptr &toBeTested, DynamicContext* context, const LocationInfo *location) const;

  protected:

    // The ItemTestType of this ItemType
    ItemTestType m_nTestType;

    // The name and type to match
    QualifiedName* m_pName, * m_pType;

    // The forced URIs for name and type
    const XMLCh* m_NameURI, * m_TypeURI;

    // allow elements having the xsi:nil="true" attribute
    bool m_bAllowNil;
  };//ItemType



  /**
   * Number of occurrences of the ItemType.
   * STAR specifies zero or more occurrences.
   * PLUS specifies one or more occurrences.
   * QUESTION_MARK specifies zero or one occurrence.
   */
  typedef enum 
  {
    EXACTLY_ONE=0,
    STAR=1,
    PLUS=2,
    QUESTION_MARK=3
  } OccurrenceIndicator;

  /**
   * Constructor for atomic types
   */
  SequenceType(const XMLCh* typeURI,const XMLCh* typeName, OccurrenceIndicator occur, XPath2MemoryManager *mm);

  /**
   * Generic constructor.
   */
  SequenceType(ItemType* test, OccurrenceIndicator occur = EXACTLY_ONE);

  /**
   * Default constructor.
   */
  SequenceType();

  /**
   * Destructor.
   */
  virtual ~SequenceType();

  /** 
   * Setter for m_pItemType.
   */ 
  void setItemType(ItemType* itemType);

  /**
   * Setter for m_nOccurrencer.
   */
  void setOccurrence(OccurrenceIndicator nOccurrence);

  /**
   * Setter for the nameURI
   */
  void setNameURI(const XMLCh* const nameURI);

  /**
   * Setter for the typeURI
   */
  void setTypeURI(const XMLCh* const typeURI);

  //Return a URI, firstly the overriding URI string, if not, the URI bond to the QName prefix
  const XMLCh* getTypeURI(const StaticContext* context) const;
  const XMLCh* getNameURI(const StaticContext* context) const;

  /**
   * Returns a Result that will throw an XPath2TypeMatchException if
   * the toBeTested Result doesn't match this SequenceType.
   */
  Result matches(const Result &toBeTested, const LocationInfo *location) const;
  Result occurrenceMatches(const Result &toBeTested, const LocationInfo *location) const;
  Result typeMatches(const Result &toBeTested, const LocationInfo *location) const;

  ASTNode *convertFunctionArg(ASTNode *arg, StaticContext *context, bool numericFunction,
                              const LocationInfo *location) const;

  QualifiedName *getConstrainingType(void) const;
  QualifiedName *getConstrainingName(void) const;

  /**
   * Getter for m_pItemType.
   */
  ItemType::ItemTestType getItemTestType(void) const;

  /**
   * Getter for m_nOccurrence.
   */
  OccurrenceIndicator getOccurrenceIndicator(void) const;

  const ItemType *getItemType() const;

  void staticResolution(StaticContext* context) const;

protected:

  // The ItemType of this SequenceType
  ItemType* m_pItemType;
  
  // The OccurrenceIndicator of this SequenceType
  OccurrenceIndicator m_nOccurrence;

  class OccurrenceMatchesResult : public ResultImpl
  {
  public:
    OccurrenceMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const SequenceType *_seqType;
    Result _parent;
    bool _toDo;
  };

  class TypeMatchesResult : public ResultImpl
  {
  public:
    TypeMatchesResult(const Result &parent, const SequenceType *seqType, const LocationInfo *location);

    Item::Ptr next(DynamicContext *context);
    std::string asString(DynamicContext *context, int indent) const;
  private:
    const SequenceType *_seqType;
    Result _parent;
  };
};

#endif // _SEQUENCETYPE_HPP
