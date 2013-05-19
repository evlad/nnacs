//-*-C++-*-
/* NaDynAr.h */
/* $Id$ */
#ifndef __NaDynAr_hxx
#define __NaDynAr_hxx

#include <NaLogFil.h>
#include <NaExcept.h>


/**
 ***********************************************************************
 * Template class for dynamic array of any type.
 ***********************************************************************/
template <class type> class NaDynAr
{
public:

  /** Create empty array. */
  NaDynAr () {
    pTail = pHead = NULL;
    nItems = 0;
  }

  /** Create copy of the array, meaning there is a type(type&)
      constructor. */
  NaDynAr (const NaDynAr& dynar) {
    pTail = pHead = NULL;
    nItems = 0;
    int	i;
    for(i = 0; i < dynar.nItems; ++i) {
      addh(dynar(i));
    }
  }

  /** Destroy array and all its items. */
  virtual ~NaDynAr () {
    clean();
  }

  /** Assign one array to another. */
  virtual NaDynAr&  operator= (const NaDynAr& dynar) {
    clean();  // previous contents
    int	i;
    for(i = 0; i < dynar.nItems; ++i) {
      addh(dynar(i));
    }
    return *this;
  }

  /** Returns number of items in the array. */
  virtual int	count () const {
    return nItems;
  }

  /** Add an item at the end of array.  Return its index. */
  virtual int	addh (type* pobj) {
    if(NULL == pobj)
      throw(na_null_pointer);

    item	*p = new item(pobj);

#ifdef DynAr_DEBUG
    NaPrintLog("DynAr::addh(this=0x%08p), %d items, new item=0x%08p\n",
	       this, nItems, p);
#endif /* DynAr_DEBUG */

    if(NULL == pHead){
      pHead = pTail = p;
    }
    else{
      pTail->pNext = p;
      pTail = p;
    }
    ++nItems;

    return nItems - 1;
  }

  /** Add an item at the beginning of array.  Return its index. */
  virtual int	addl (type* pobj) {
    if(NULL == pobj)
      throw(na_null_pointer);

    item	*p = new item(pobj);

#ifdef DynAr_DEBUG
    NaPrintLog("DynAr::addl(this=0x%08p), %d items, new item=0x%08p\n",
	       this, nItems, p);
#endif /* DynAr_DEBUG */

    if(NULL == pHead){
      pHead = pTail = p;
    }
    else{
      p->pNext = pHead;
      pHead = p;
    }
    ++nItems;

    return 0;
  }

  /** Insert an item into the array just before given index.  Return
      its index. */
  virtual int   insert (int index, type* pobj) {
    if(NULL == pobj)
      throw(na_null_pointer);

    item  *p = new item(pobj);

#ifdef DynAr_DEBUG
    NaPrintLog("DynAr::insert(this=0x%08p), %d items, new item=0x%08p at %d\n",
	       this, nItems, p, index);
#endif /* DynAr_DEBUG */

    if(nItems == index){
      pTail->pNext = p;
      p->pNext = NULL;
    }
    else{
      item    *pNext = fetch_item(index);
      p->pNext = pNext;
      if(0 == index)
	pHead = p;
      else
	fetch_item(index - 1)->pNext = p;
    }
    ++nItems;

    return index;
  }

  /** Add an item at the end of array.  Return its index.  Create new
      instance of an object by means of copying constructor. */
  virtual int	addh (const type& obj)  {
    type	*pobj = new type(obj);
    return addh(pobj);
  }

  /** Add an item at the beginning of array.  Return its index.
      Create new instance of an object by means of copying
      constructor. */
  virtual int	addl (const type& obj) {
    type	*pobj = new type(obj);
    return addl(pobj);
  }

  /** Insert an item into the array just before given index.  Return
      its index.  Create new instance of an object by means of copying
      constructor. */
  virtual int   insert (int index, const type& obj) {
    type	*pobj = new type(obj);
    return insert(index, pobj);
  }

  /** Add array of items making new for each one at the beginning of
      the array.  Create new instance of an object by means of copying
      constructor. */
  virtual void	addh (const NaDynAr& arr) {
    int     i;
    for(i = 0; i < arr.count(); ++i){
      addh(arr(i));
    }
  }

  /** Add array of items making new for each one at the end of the
      array.  Create new instance of an object by means of copying
      constructor. */
  virtual void	addl (const NaDynAr& arr) {
    int     i;
    for(i = 0; i < arr.count(); ++i){
      addl(arr(arr.count() - i - 1));
    }
  }

  /** Insert array of items making new for each one.  Create new
      instance of an object by means of copying constructor. */
  virtual void  insert (int index, const NaDynAr& arr) {
    int     i;
    for(i = 0; i < arr.count(); ++i){
      insert(index, arr(arr.count() - i - 1));
    }
  }

  /** Remove the item. */
  virtual void	remove (int index) {
    item	*p;

    if(!good(index))
      throw(na_out_of_range);
    else if(0 == index){
      p = pHead;

      pHead = pHead->pNext;
      if(NULL == pHead)
        pTail = NULL;

      delete p;
    }
    else{
      p = fetch_item(index - 1);	/* previous */

      item    *n = p->pNext->pNext;

      if(pTail == p->pNext)
	pTail = p;

      delete p->pNext;

      p->pNext = n;
    }
    --nItems;
  }

  /** Remove all items. */
  virtual void	clean () {
#ifdef DynAr_DEBUG
    NaPrintLog("DynAr::clean(this=0x%08p), %d items, contents:\n",
	       this, nItems);
#endif /* DynAr_DEBUG */

    int   i = 0;
    if(NULL != pHead){
      item	*p = pHead;

      do{
        item	*c = p;
#ifdef DynAr_DEBUG
        NaPrintLog("  [%d]\tcur=0x%08p\t", i);
        c->print_self();
#endif /* DynAr_DEBUG */
	p = p->pNext;
	delete c;
        ++i;
      }while(NULL != p && i < nItems);
    }
    pTail = pHead = NULL;
    nItems = 0;
  }


  /** Get reference at the item. */
  virtual type&	fetch (int index) {
    item	*p = fetch_item(index);

    if(NULL == p)
      throw(na_out_of_range);
    if(NULL == p->pData)
      throw(na_null_pointer);

    return *(p->pData);
  }

  /** Get reference at the item. */
  virtual type&	operator[] (int index) {
    return fetch(index);
  }

  /** Get constant reference at the item. */
  virtual type&	get (int index) const {
    item	*p = fetch_item(index);

    if(NULL == p)
      throw(na_out_of_range);
    if(NULL == p->pData)
      throw(na_null_pointer);

    return *(p->pData);
  }

  /** Get constant reference at the item. */
  virtual type&	operator() (int index) const {
    return get(index);
  }

  /** Print contents of the array */
  virtual void  print_contents () const {
    int	i = 0;
    NaPrintLog("DynAr(this=0x%08p), %d items, contents:\n", this, nItems);
    if(NULL != pHead){
      item	*p = pHead;

      do{
	NaPrintLog("  [%d]\t", i++);
	p->print_self();
	p = p->pNext;
      }while(NULL != p);
    }
  }

protected:

  /** Check index for being out of range.  Return `true' if it's in
      range and `false' otherwise. */
  bool		good (int index) const {
    if(index < 0 || index >= nItems)
      return false;
    return true;
  }

private:

  /** Number of items in the array. */
  int		nItems;

  /** One-direction list is internal array representation. */
  struct item
  {
    type	*pData;
    item	*pNext;

    item (type* ptr) {
      pData = ptr;
      pNext = NULL;
    }

    ~item () {
      if(NULL == pData)
	NaPrintLog("data=0x%08p removed twice\n", pData);
      delete pData;
      pData = NULL;
    }

    /** Print self contents */
    void      print_self () const {
      NaPrintLog("next=0x%08p; data=0x%08p\n", pNext, pData);
    }

  }	*pHead, *pTail;

  /** Fetch item by index and return pointer at it or NULL. */
  item*		fetch_item (int index) const {
    if(!good(index))
      return NULL;

    int	i;
    item	*p = pHead;

    for(i = 0; i < index; ++i)
      p = p->pNext;

    return p;
  }

};


#endif /* NaDynAr.hxx */
