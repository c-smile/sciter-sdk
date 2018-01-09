#pragma once

#if defined(_WIN32)
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #define _WINSOCKAPI_
  #include <windows.h>
#else
  #include <atomic>
#endif

namespace aux {

  namespace atomic {

    #if defined(_WINDOWS) || defined(WINDOWS)

      typedef volatile long counter_t;
      inline long _inc(counter_t& v)               {    return InterlockedIncrement((LPLONG)&v);  }
      inline long _inc(counter_t& v,long by)       {    return InterlockedExchangeAdd((LPLONG)&v,by);  }
      inline long _dec(counter_t& v)               {    return InterlockedDecrement((LPLONG)&v);  }
      inline long _set(counter_t& v, long nv)      {    return InterlockedExchange((LPLONG)&v, nv);  }
      inline long _set_when_eq(counter_t& v, long to_set, long eq_to) { return InterlockedCompareExchange ((LPLONG)&v,to_set,eq_to); }

    #elif defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 8) )

      typedef long counter_t;
      inline long _inc(counter_t& v)               {    return __sync_add_and_fetch(&v,1);  }
      inline long _inc(counter_t& v,long by)       {    return __sync_add_and_fetch(&v,by);  }
      inline long _dec(counter_t& v)               {    return __sync_add_and_fetch(&v,-1);  }
      inline long _set(counter_t& v, long nv)      {    return __sync_lock_test_and_set(&v,nv);  }
      inline long _set_when_eq(counter_t& v, long to_set, long eq_to) { return __sync_val_compare_and_swap(&v,eq_to,to_set);
                                                                       /*long t(v); if(t == eq_to) v = to_set; return t;*/ }
    #else

      typedef std::atomic<long> counter_t;
      inline long _inc(counter_t& v)               {    return ++v;  }
      inline long _inc(counter_t& v,long by)       {    return v += by;  }
      inline long _dec(counter_t& v)               {    return --v;  }
      inline long _set(counter_t& v, long nv)      {    return v.exchange(nv);  }
      inline long _set_when_eq(counter_t& v, long to_set, long eq_to) { long t(v); v.compare_exchange_strong(eq_to,to_set); return t; }

    #endif

      struct counter
      {
        counter_t cv;
        counter():cv(0) {}
        counter(long iv):cv(iv) {}
        counter& operator=(long nv) { _set(cv,nv); return *this; }
        operator long() const { return cv; }
        long operator++() { return _inc(cv); }
        long operator--() { return _dec(cv); }
      };
  } // atomic


  // COM::IUnknown alike thing:
  class iasset {
  public:
    // mandatory:
    virtual long  add_ref() = 0;
    virtual long  release() = 0;
    // optional:
    virtual bool  get_interface(const char* name, iasset** out) = 0;
  };


  // intrusive add_ref/release counter
  class asset: public virtual iasset
  {
    atomic::counter _ref_cntr;
  public:
    asset ():_ref_cntr(0) {}
    asset (const asset&/*r*/):_ref_cntr(0) {}

    virtual ~asset ()
    {
      assert ( _ref_cntr == 0 );
    }

    virtual long  release()
    {
        assert(_ref_cntr > 0);
        long t = --_ref_cntr;
        if(t == 0)
          finalize();
        return t;
    }
    virtual long  add_ref() { return ++_ref_cntr; }

    // "name" here is an arbitrary name that includes domain name in reversed order:
    //    "element.dom.sciter.com"
    //    "video-renderer.dom.sciter.com"
    virtual bool  get_interface(const char* name, iasset** out) { return false; }

    virtual void finalize()
    {
      delete this;
    }
  };

  //asset - yet another shared_ptr
  //        R here is something derived from the asset above
  template <class R>
  class asset_ptr
  {
  protected:
	  R* p;

  public:
	  typedef R asset_t;

	  asset_ptr():p(0)                        {}
	  asset_ptr(R* lp):p(0)                   { if (lp) (p = lp)->add_ref();	}
    asset_ptr(const asset_ptr<R>& cp):p(0)  { if (cp.p) (p = cp.p)->add_ref();	}

	  ~asset_ptr()                  { if (p)	p->release();}
	  operator R*() const	          {	return p;	}
    R* operator->() const         { assert(p != 0); return p; }

    bool operator!() const        {	return p == 0;	}
    operator bool() const         { return  p != 0;	}
	  bool operator!=(R* pR) const  {	return p != pR;	}
	  bool operator==(R* pR) const  {	return p == pR;	}

	  // release the interface and set it to NULL
	  void release()                {	if (p)	{ R* pt = p; p = 0; pt->release(); }}

    // attach to an existing interface (does not AddRef)
	  void attach(R* p2)            { release();	p = p2;	}
	  // detach the interface (does not Release)
	  R* detach()                   {	R* pt = p; p = 0;	return pt; }

    static R* assign(R* &pp, R* lp)
    {
	    if (lp != 0) lp->add_ref();
	    if (pp) pp->release();
	    pp = lp;
	    return lp;
    }

    R* operator=(R* lp)                   { if(p != lp) return assign(p, lp); return p;	}
    R* operator=(const asset_ptr<R>& lp)	{ if(p != lp) return assign(p, lp.p); return p;	}

    R** target()                     { release(); return &p; }

  };

}

#if defined(_WINDOWS)

namespace com {

  //asset - yet another smart pointer
  template <class T>
  class ptr
  {
  protected:
	  T* p;

  public:
	  typedef T asset_t;

	  ptr():p(0)                    {}
	  ptr(T* lp):p(0)               { if (lp) (p = lp)->AddRef();	}
    ptr(const ptr<T>& cp):p(0)    { if (cp.p) (p = cp.p)->AddRef();	}

	  ~ptr()
    {
        ULONG c = 0;
        if (p)
          c = p->Release();
    }
	  operator T*() const	          {	return p;	}
    T* operator->() const         { assert(p != 0); return p; }

    // used as target T** pointer to pointer - in places receiving newly created objects (initially add-refed)
    T** target()                  { release(); return &p; }

    bool operator!() const        {	return p == 0;	}
    operator bool() const         { return p != 0;	}
	  bool operator!=(T* pT) const  {	return p != pT;	}
	  bool operator==(T* pT) const  {	return p == pT;	}

	  // release the interface and set it to NULL
	  void release()                {	if (p)	{ T* pt = p; p = 0; pt->Release(); }}

    // attach to an existing interface (does not AddRef)
	  void attach(T* p2)            { release();	p = p2;	}
	  // detach the interface (does not Release)
	  T* detach()                   {	T* pt = p; p = 0;	return pt; }

    static T* assign(T* &pp, T* lp)
    {
	    if (lp != 0) lp->AddRef();
	    if (pp) pp->Release();
	    pp = lp;
	    return lp;
    }

    T* operator=(T* lp)               { if(p != lp) return assign(p, lp); return p;	}
    T* operator=(const ptr<T>& lp)	{ if(p != lp) return assign(p, lp.p); return p;	}

    T* acquire() { if( p ) p->AddRef(); return p; }

    HRESULT CoCreateInstance (REFCLSID classUUID, DWORD dwClsContext = CLSCTX_INPROC_SERVER)
    {
        HRESULT hr = ::CoCreateInstance (classUUID, 0, dwClsContext, __uuidof (T), (void**) target());
        assert (hr != CO_E_NOTINITIALIZED); // You haven't called CoInitialize for the current thread!
        return hr;
    }

    template <class OTHER_COM_CLASS>
      HRESULT QueryInterface (REFCLSID classUUID, ptr<OTHER_COM_CLASS>& dest_object) const
      {
          if (p == 0)
              return E_POINTER;
          return p->QueryInterface (classUUID, (void**) dest_object.target());
      }

    template <class OTHER_COM_CLASS>
      HRESULT QueryInterface (ptr<OTHER_COM_CLASS>& dest_object) const
      {
          return this->QueryInterface (__uuidof (OTHER_COM_CLASS), dest_object);
      }

  };

}

#endif
