#ifndef COPASI_CCopasiVector
#define COPASI_CCopasiVector

#ifdef WIN32
# pragma warning (disable: 4786)
#endif  // WIN32

#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

#include "CCopasiMessage.h"

class CReadConfig;

class CWriteConfig;

using std::vector;
using std::ostream;

template < class CType >
      class CCopasiVector : protected vector < CType * >
    {
    public:
      typedef typename vector< CType * >::value_type value_type;
      typedef typename vector< CType * >::iterator iterator;
      typedef typename vector< CType * >::const_iterator const_iterator;

      // Operations

    public:
      /**
       *  Default constructor
       */
      CCopasiVector() : vector < CType * > ()
      {
        CONSTRUCTOR_TRACE;
      }

      /**
       *  Copy constructor
       */
      CCopasiVector(const CCopasiVector < CType > & src) :
          vector < CType * > (src)
      {
        CONSTRUCTOR_TRACE;
        unsigned C_INT32 i, imax = vector < CType * >::size();
        iterator Target = begin();
        const_iterator Source = src.begin();

        for (i = 0; i < imax; i++, Target++, Source++)
          *Target = new CType(**Source);
      }

      /**
       *  Destructor
       */
    virtual ~CCopasiVector() {DESTRUCTOR_TRACE;}

      /**
       *  Cleanup
       */
      virtual void cleanup()
      {
        unsigned C_INT32 i, imax = vector < CType * >::size();
        iterator Target = begin();

        for (i = 0; i < imax; i++, Target++)
          {
            if (*Target)
              {
                (*Target)->cleanup();
                delete *Target;
                *Target = NULL;
              }
          }

        clear();
      }

      /**
       *
       */

      virtual void add
      (const CType & src)
      {
        CType * Element = new CType(src);

        // This is not very efficient !!!
        // It results in a lot of resizing of the vector !!!
        push_back(Element);
      }

      /**
       *
       */
      virtual void add
      (CType * src)
      {
        // This is not very efficient !!!
        // It results in a lot of resizing of the vector !!!
        push_back(src);
      }

      /**
       *  Removes the index-th element from the vector
       *  @param "const unsigned C_INT32 &" indecx
       */
      virtual void remove
      (const unsigned C_INT32 & index)
      {
        iterator Target = begin() + index;
        assert(index < vector < CType * >::size());

        if (*Target)
          {
            (*Target)->cleanup();
            delete *Target;
            *Target = NULL;
          }

        erase(Target, Target + 1);
      }

      /**
       *
       */
      const value_type operator[](unsigned C_INT32 index) const
      {
        assert(index < vector < CType * >::size());
        return *(begin() + index);
      }

      /**
       *
       */
      value_type operator[](unsigned C_INT32 index)
      {
        assert(index < vector < CType * >::size());
        return *(begin() + index);
      }

      /**
       *  Retrieves the size of the vector
       *  @return "unsigned C_INT32" size
       */
      virtual unsigned C_INT32 size() const {return vector < CType * >::size();}

      /**
       *  Resizes the vector
       *  @param "unsigned C_INT32" size
       */
      virtual void resize(unsigned C_INT32 size)
      {
        unsigned C_INT32 OldSize = vector < CType * >::size();

        vector < CType * >::resize(size);
        unsigned C_INT32 i;
        iterator Target = begin() + OldSize;

        for (i = OldSize; i < size; i++, Target++)
          *Target = new CType;
      }

      friend ostream &operator<<(ostream &os, const CCopasiVector<CType> & d)
      {
        os << "   +++Vektor;  size: " << d.size() << endl;

        unsigned int i;

        for (i = 0; i < d.size(); i++)
          os << "   " << *(d[i]);

        if (d.size() == 0)
          os << "   empty" << endl;

        os << "   ---Vektor" << endl;

        return os;
      }
    };

template < class CType >

      class CCopasiVectorS
        : public CCopasiVector < CType >
    {
    public:
      typedef typename vector< CType * >::value_type value_type;
      typedef typename vector< CType * >::iterator iterator;
      typedef typename vector< CType * >::const_iterator const_iterator;

      // Operations

    public:
      /**
       *  Default constructor
       */
      CCopasiVectorS() : CCopasiVector < CType > (){}

      /**
       *  Copy constructor
       */
      CCopasiVectorS(const CCopasiVectorS < CType > & src) :
      CCopasiVector < CType > (src) {}

      /**
       *  Destructor
       */
      virtual ~CCopasiVectorS() {}

      /**
       *  Loads an object with data coming from a CReadConfig object.
       *  (CReadConfig object reads an input stream)
       *  @param "CReadConfig &" configbuffer reference to a CReadConfig object
       *  @param "const unsigend C_INT32 &" size
       */
      virtual void load(CReadConfig & configbuffer, unsigned C_INT32 size)
      {
        unsigned C_INT32 i;

        cleanup();
        vector < CType * >::resize(size);

        iterator Target = begin();

        for (i = 0; i < size; i++, Target++)
          {
            *Target = new CType;
            (*Target)->load(configbuffer);
          }
      }

      /**
       *  Saves the contents of the object to a CWriteConfig object.
       *  (Which usually has a file attached but may also have socket)
       *  @param "CWriteConfig &" configbuffer reference to a CWriteConfig object.
       */
      virtual void save(CWriteConfig & configbuffer)
      {
        unsigned C_INT32 i, imax = size();
        iterator Target = begin();

        for (i = 0; i < imax; i++, Target++)
          (*Target)->save(configbuffer);
      }

      /**
       *
       */
      value_type operator[](unsigned C_INT32 index)
    {return ((CCopasiVector <CType>*) this)->operator [](index);}

      /**
       *
       */
      const value_type operator[](unsigned C_INT32 index) const
      {return ((CCopasiVector <CType>*) this)->operator [](index);}
    };

template < class CType >

      class CCopasiVectorN
        : public CCopasiVector < CType >
    {
    public:
      typedef typename vector< CType * >::value_type value_type;
      typedef typename vector< CType * >::iterator iterator;
      typedef typename vector< CType * >::const_iterator const_iterator;

      // Operations

    public:
      /**
       *  Default constructor
       */
      CCopasiVectorN() : CCopasiVector < CType > (){}

      /**
       *  Copy constructor
       */
      CCopasiVectorN(const CCopasiVectorN < CType > & src) :
      CCopasiVector < CType > (src) {}

      /**
       *  Destructor
       */
      virtual ~CCopasiVectorN() {}

      /**
       *
       */

      virtual void add
      (const CType & src)
      {
        if (!isInsertAllowed(&src))
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCCopasiVector + 2, src.getName().c_str());

        CType * Element = new CType(src);

        push_back(Element);
      }

      /**
       *
       */
      virtual void add
      (CType * src)
      {
        if (!isInsertAllowed(src))
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCCopasiVector + 2, src->getName().c_str());

        // This is not very efficient !!!
        // It results in a lot of resizing of the vector !!!
        push_back(src);
      }

      /**
       *
       */
      virtual void remove
      (const string & name)
      {
        unsigned C_INT32 Index = getIndex(name);

        if ((signed) Index == -1)
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCCopasiVector + 1, name.c_str());

        CCopasiVector< CType >::remove
        (Index);

        return;
      }

      /**
       *
       */
      value_type operator[](unsigned C_INT32 index)
    {return ((CCopasiVector <CType>*) this)->operator [](index);}

      /**
       *
       */
      const value_type operator[](unsigned C_INT32 index) const
      {return ((CCopasiVector <CType>*) this)->operator [](index);}

      /**
       *
       */
      value_type operator[](const string &name)
      {
        C_INT32 Index = getIndex(name);

        if (Index == -1)
          CCopasiMessage(CCopasiMessage::ERROR,
                         MCCopasiVector + 1, name.c_str());

        return *(begin() + Index);
      }

      /**
       *
       */
      const value_type operator[](const string &name) const
      {
        C_INT32 Index = getIndex(name);

        if (Index == -1)
        CCopasiMessage(CCopasiMessage::ERROR,
                       MCCopasiVector + 1, name.c_str());

        return *(begin() + Index);
      }

      /**
       *
       */
      virtual unsigned C_INT32 getIndex(const string &name) const
        {
          unsigned C_INT32 i, imax = size();
          const_iterator Target = begin();

          for (i = 0; i < imax; i++, Target++)
          if ((*Target)->getName() == name)
            return i;

            return - 1;
          }

private:
          /**
           *
           */
          virtual C_INT16 isInsertAllowed(const CType * src)
          {return (getIndex(src->getName()) == (unsigned C_INT32) - 1);}
    };

template < class CType >

      class CCopasiVectorNS
        : public CCopasiVectorN < CType >
    {
    public:
      typedef typename vector< CType * >::value_type value_type;
      typedef typename vector< CType * >::iterator iterator;
      typedef typename vector< CType * >::const_iterator const_iterator;

      // Operations

    public:
      /**
       *  Default constructor
       */
      CCopasiVectorNS() : CCopasiVectorN < CType > (){}

      /**
       *  Copy constructor
       */
      CCopasiVectorNS(const CCopasiVectorNS < CType > & src) :
      CCopasiVectorN < CType > (src) {}

      /**
       *  Destructor
       */
      virtual ~CCopasiVectorNS() {}

      /**
       *  Loads an object with data coming from a CReadConfig object.
       *  (CReadConfig object reads an input stream)
       *  @param "CReadConfig &" configbuffer reference to a CReadConfig object
       *  @param "const unsigend C_INT32 &" size
       */
      virtual void load(CReadConfig & configbuffer, unsigned C_INT32 size)
      {
        unsigned C_INT32 i;

        cleanup();
        vector < CType * >::resize(size);

        iterator Target = begin();

        for (i = 0; i < size; i++, Target++)
          {
            *Target = new CType;
            (*Target)->load(configbuffer);
          }
      }

      /**
       *  Saves the contents of the object to a CWriteConfig object.
       *  (Which usually has a file attached but may also have socket)
       *  @param "CWriteConfig &" configbuffer reference to a CWriteConfig object.
       */
      virtual void save(CWriteConfig & configbuffer)
      {
        unsigned C_INT32 i, imax = size();
        iterator Target = begin();

        for (i = 0; i < imax; i++, Target++)
          (*Target)->save(configbuffer);
      }

      /**
       *
       */
      value_type operator[](unsigned C_INT32 index)
    {return ((CCopasiVector <CType>*) this)->operator [](index);}

      /**
       *
       */
      const value_type operator[](unsigned C_INT32 index) const
      {return ((const CCopasiVector <CType>) *this)[index];}

      /**
       *
       */
      value_type operator[](const string &name)
      {return ((CCopasiVectorN <CType>*) this)->operator [](name);}

      /**
       *
       */
      const value_type operator[](const string &name) const
      {return ((CCopasiVectorN <CType>*) this)->operator [](name);}

    private:

      /** @associates <{CCopasiVectorS< CType >}>
       * @directed*/
      /*# int lnkCCopasiVectorS; */
    };

#endif // COPASI_CCopasiVector
