/* linbox/field/gf2.inl
 * Copyright (C) 2003 Bradford Hovinen
 *
 * Written by Bradford Hovinen, Dumas, bds
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __LINBOX_field_gf2_INL
#define __LINBOX_field_gf2_INL

#include <iostream>
#include <time.h>

#include "linbox/field/gf2.h"
#include "linbox/vector/vector-domain.h"
#include "linbox/vector/vector-traits.h"
#include "linbox/vector/bit-vector.h"
#include "linbox/vector/stream.h"
#include "linbox/randiter/mersenne-twister.h"
#include "linbox/matrix/matrix-domain.h"

#include <cctype> //isdigit

template<typename Vector>
std::ostream& afficheVector (std::ostream& o, const Vector& C) {
          for(typename Vector::const_iterator refs =  C.begin();
                                refs != C.end() ;
                                      ++refs )
                          o << (*refs) << " " ;
            return o;
}


namespace LinBox 
{ 

// Specialization of canonical vector types

template <>
class RawVector<bool>
{
    public:
	typedef BitVector<> Dense;
	typedef std::vector<size_t> Sparse;
	typedef std::vector<size_t> SparseSeq;
	typedef std::vector<size_t> SparseMap;
	typedef std::vector<size_t> SparsePar;
	typedef std::pair<std::vector<size_t>, BitVector<> > Hybrid;
};

// Specialization of DotProductDomain for GF2

template <>
class DotProductDomain<GF2> : private virtual VectorDomainBase<GF2>
{
    public:

	typedef bool Element;

	DotProductDomain (const GF2 &F)
		: VectorDomainBase<GF2> (F)
	{}

    protected:
	template <class Vector1, class Vector2>
	inline Element &dotSpecializedDD (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	template <class Vector1, class Vector2>
	inline Element &dotSpecializedDSP (Element &res, const Vector1 &v1, const Vector2 &v2) const;

	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dotSpecializedDD (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2) const;

	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dotSpecializedDSP (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2) const;
};

// Specialization of vector domain

template <>
class VectorDomain<GF2> : private virtual VectorDomainBase<GF2>, private DotProductDomain<GF2>
{
    public:
	typedef bool Element;

	VectorDomain (const VectorDomain &VD)
		: VectorDomainBase<GF2> (VD._F), DotProductDomain<GF2> (VD._F)
	{}

	VectorDomain &operator = (const VectorDomain &) { return *this; }

	const GF2 &field () const { return _F; }
    
	template <class Vector>
	inline std::ostream &write (std::ostream &os, const Vector &x) const
		{ return writeSpecialized (os, x, typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector>
	inline std::istream &read (std::istream &is, Vector &x) const
		{ return readSpecialized (is, x, typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Vector1 &copy (Vector1 &res, const Vector2 &v) const
		{ return copySpecialized (res, v,
					  typename VectorTraits<Vector1>::VectorCategory (),
					  typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Vector>
	inline void swap (Vector &v1, Vector &v2) const
		{ swapSpecialised (v1, v2,
				   typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector>
	inline int firstNonzeroEntry (bool &a, const Vector &v) const
		{ return firstNonzeroEntrySpecialized (a, v, typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Vector1 &copy (Vector1 &res, const Vector2 &v, size_t i, size_t len = 0) const
		{ return copySpecialized (res, v, i, len,
					  typename VectorTraits<Vector1>::VectorCategory ()); }

	template <class Vector, class Iterator>
	inline Vector &permute (Vector   &v,
				Iterator  P_start,
				Iterator  P_end) const
		{ return permuteSpecialized (v, P_start, P_end,
					     typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline bool areEqual (const Vector1 &v1, const Vector2 &v2) const
		{ return areEqualSpecialized (v1, v2,
					      typename VectorTraits<Vector1>::VectorCategory (),
					      typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Vector>
	inline bool isZero (const Vector &v) const
		{ return isZeroSpecialized (v, typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Element &dot (Element &res, const Vector1 &v1, const Vector2 &v2) const
		{ return dotSpecialized (res, v1, v2,
					 typename VectorTraits<Vector1>::VectorCategory (),
					 typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dot (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2) const
		{ return dotSpecialized (res, v1, v2,
					 typename VectorTraits<Vector1>::VectorCategory (),
					 typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Element &dotprod (Element &res, const Vector1 &v1, const Vector2 &v2) const
		{ return dot (res, v1, v2); }

	template <class Vector1, class Vector2, class Vector3>
	inline Vector1 &add (Vector1 &res, const Vector2 &y, const Vector3 &x) const
		{ return addSpecialized (res, y, x,
					 typename VectorTraits<Vector1>::VectorCategory (),
					 typename VectorTraits<Vector2>::VectorCategory (),
					 typename VectorTraits<Vector3>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Vector1 &addin (Vector1 &y, const Vector2 &x) const
		{ return addinSpecialized (y, x,
					   typename VectorTraits<Vector1>::VectorCategory (),
					   typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Vector1, class Vector2, class Vector3>
	inline Vector1 &sub (Vector1 &res, const Vector2 &y, const Vector3 &x) const
		{ return addSpecialized (res, y, x,
					 typename VectorTraits<Vector1>::VectorCategory (),
					 typename VectorTraits<Vector2>::VectorCategory (),
					 typename VectorTraits<Vector3>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Vector1 &subin (Vector1 &y, const Vector2 &x) const
		{ return addinSpecialized (y, x,
					   typename VectorTraits<Vector1>::VectorCategory (),
					   typename VectorTraits<Vector2>::VectorCategory ()); }

	template <class Vector1, class Vector2>
	inline Vector1 &neg (Vector1 &res, const Vector2 &x) const
		{ copy (res, x); return res; }

	template <class Vector>
	inline Vector &negin (Vector &y) const
		{ return y; }

	template <class Vector1, class Vector2>
	inline Vector1 &mul (Vector1 &res, const Vector2 &x, const Element a) const
		{ return mulSpecialized (res, x, a, typename VectorTraits<Vector1>::VectorCategory ()); }

	template <class Vector>
	inline Vector &mulin (Vector &x, const Element a) const
		{ return mulinSpecialized (x, a, typename VectorTraits<Vector>::VectorCategory ()); }

	template <class Vector1, class Vector2, class Vector3>
	inline Vector1 &axpy (Vector1 &res, const Element a, const Vector2 &x, const Vector3 &y) const
		{ if (a) add (res, x, y); else this->copy (res, y); return res; }

	template <class Vector1, class Vector2>
	inline Vector1 &axpyin (Vector1 &y, const Element a, const Vector2 &x) const
		{ if (a) addin (y, x); return y; }

	VectorDomain (const GF2 &F)
		: VectorDomainBase<GF2> (F), DotProductDomain<GF2> (F)
	{}


	// Specialized function implementations
	template <class Vector> 
	std::ostream &writeSpecialized (std::ostream &os, const Vector &x,
					VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector>
	std::ostream &writeSpecialized (std::ostream &os, const Vector &x,
					VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector>
	std::ostream &writeSpecialized (std::ostream &os, const Vector &x,
					VectorCategories::HybridZeroOneVectorTag) const
		{ return writeHybridSpecialized (os, x, LittleEndian<typename std::iterator_traits<typename Vector::second_type::const_word_iterator>::value_type> ()); }
	template <class Vector, class Endianness>
	std::ostream &writeHybridSpecialized (std::ostream &os, const Vector &x, Endianness) const;

	template <class Vector>
	std::istream &readSpecialized (std::istream &is, const Vector &x,
				       VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector>
	std::istream &readSpecialized (std::istream &is, const Vector &x,
				       VectorCategories::SparseZeroOneVectorTag) const;

	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::DenseZeroOneVectorTag) const;
	
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	inline bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					 VectorCategories::SparseZeroOneVectorTag,
					 VectorCategories::DenseZeroOneVectorTag) const
		{ return areEqual (v2, v1); }
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::SparseZeroOneVectorTag,
				  VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::HybridZeroOneVectorTag,
				  VectorCategories::HybridZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::HybridZeroOneVectorTag,
				  VectorCategories::DenseZeroOneVectorTag) const
		{ return areEqual (v2, v1); }
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::HybridZeroOneSequenceVectorTag,
				  VectorCategories::DenseZeroOneVectorTag) const
		{ return areEqual (v2, v1); }
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::HybridZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	bool areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::HybridZeroOneSequenceVectorTag) const;
    

	template <class Vector>
	bool isZeroSpecialized (const Vector &v, VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector>
	inline bool isZeroSpecialized (const Vector &v,
				       VectorCategories::SparseZeroOneVectorTag) const
		{ return v.empty (); }
	template <class Vector>
	inline bool isZeroSpecialized (const Vector &v,
				       VectorCategories::HybridZeroOneVectorTag) const
		{ return v.first.empty (); }

	template <class Vector1, class Vector2>
	inline Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
					 VectorCategories::DenseZeroOneVectorTag,
					 VectorCategories::DenseZeroOneVectorTag) const
		{ std::copy (v.wordBegin (), v.wordEnd (), res.wordBegin ()); return res; }
	template <class Vector1, class Vector2>
	Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
				  VectorCategories::SparseZeroOneVectorTag,
				  VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
				  VectorCategories::HybridZeroOneVectorTag,
				  VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::HybridZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
				  VectorCategories::DenseZeroOneVectorTag,
				  VectorCategories::HybridZeroOneSequenceVectorTag) const;
	template <class Vector1, class Vector2>
	inline Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
					 VectorCategories::SparseZeroOneVectorTag,
					 VectorCategories::SparseZeroOneVectorTag) const
		{ res = v; return res; }
	template <class Vector1, class Vector2>
	inline Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
					 VectorCategories::HybridZeroOneVectorTag,
					 VectorCategories::HybridZeroOneVectorTag) const
		{ res = v; return res; }
	template <class Vector1, class Vector2>
	inline Vector1 &copySpecialized (Vector1 &res, const Vector2 &v,
					 VectorCategories::HybridZeroOneSequenceVectorTag,
					 VectorCategories::HybridZeroOneSequenceVectorTag) const
		{ res = v; return res; }

	template <class Vector>
	inline void swapSpecialised (Vector &v1, Vector &v2,
				     VectorCategories::DenseZeroOneVectorTag) const
		{ std::swap_ranges (v1.wordBegin (), v1.wordEnd (), v2.wordBegin ()); }

	template <class Vector>
	inline void swapSpecialised (Vector &v1, Vector &v2,
				     VectorCategories::GenericVectorTag) const
		{ std::swap (v1, v2); }

	template <class Vector, class Iterator>
	inline Vector &permuteSpecialized (Vector &v, Iterator P_start, Iterator P_end,
					   VectorCategories::DenseZeroOneVectorTag) const;

	template <class Vector1, class Vector2>
	inline Element &dotSpecialized (Element &res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::DenseZeroOneVectorTag,
					VectorCategories::DenseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDD (res, v1, v2); }
	template <class Vector1, class Vector2>
	inline Element &dotSpecialized (Element &res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::DenseZeroOneVectorTag,
					VectorCategories::SparseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDSP (res, v1, v2); }
	template <class Vector1, class Vector2>
	inline Element &dotSpecialized (Element &res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::SparseZeroOneVectorTag,
					VectorCategories::DenseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDSP (res, v2, v1); }
	template <class Vector1, class Vector2>
	Element &dotSpecialized (Element &res, const Vector1 &v1, const Vector2 &v2,
				 VectorCategories::SparseZeroOneVectorTag,
				 VectorCategories::SparseZeroOneVectorTag) const;

	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dotSpecialized (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::DenseZeroOneVectorTag,
					VectorCategories::DenseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDD (res, v1, v2); }
	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dotSpecialized (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::DenseZeroOneVectorTag,
					VectorCategories::SparseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDSP (res, v1, v2); }
	template <class Iterator, class Endianness, class Vector1, class Vector2>
	inline BitVectorReference<Iterator, Endianness> dotSpecialized (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2,
					VectorCategories::SparseZeroOneVectorTag,
					VectorCategories::DenseZeroOneVectorTag) const
		{ return DotProductDomain<GF2>::dotSpecializedDSP (res, v2, v1); }
	template <class Iterator, class Endianness, class Vector1, class Vector2>
	BitVectorReference<Iterator, Endianness> dotSpecialized (BitVectorReference<Iterator, Endianness> res, const Vector1 &v1, const Vector2 &v2,
				 VectorCategories::SparseZeroOneVectorTag,
				 VectorCategories::SparseZeroOneVectorTag) const;

	template <class Vector1, class Vector2, class Vector3>
	Vector1 &addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
				 VectorCategories::DenseZeroOneVectorTag,
				 VectorCategories::DenseZeroOneVectorTag,
				 VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector1, class Vector2, class Vector3>
	Vector1 &addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
				 VectorCategories::DenseZeroOneVectorTag,
				 VectorCategories::DenseZeroOneVectorTag,
				 VectorCategories::SparseZeroOneVectorTag) const
		{ copy (res, y); addin (res, x); }
	template <class Vector1, class Vector2, class Vector3>
	Vector1 &addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
				 VectorCategories::SparseZeroOneVectorTag,
				 VectorCategories::SparseZeroOneVectorTag,
				 VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector1, class Vector2, class Vector3>
	Vector1 &addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
				 VectorCategories::HybridZeroOneVectorTag,
				 VectorCategories::HybridZeroOneVectorTag,
				 VectorCategories::HybridZeroOneVectorTag) const;

	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::DenseZeroOneVectorTag,
				   VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::DenseZeroOneVectorTag,
				   VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::DenseZeroOneVectorTag,
				   VectorCategories::HybridZeroOneVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::DenseZeroOneVectorTag,
				   VectorCategories::HybridZeroOneSequenceVectorTag) const;
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::SparseZeroOneVectorTag,
				   VectorCategories::DenseZeroOneVectorTag) const
		{ Vector1 xp, res; copy (xp, x); add (res, y, xp); copy (y, res); return y; }
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::SparseZeroOneVectorTag,
				   VectorCategories::SparseZeroOneVectorTag) const
		{ static Vector1 res; add (res, y, x); this->copy (y, res); return y; }
	template <class Vector1, class Vector2>
	Vector1 &addinSpecialized (Vector1 &y, const Vector2 &x,
				   VectorCategories::HybridZeroOneVectorTag,
				   VectorCategories::HybridZeroOneVectorTag) const
		{ static Vector1 res; add (res, y, x); this->copy (y, res); return y; }

	template <class Vector1, class Vector2>
	Vector1 &mulSpecialized (Vector1 &res, const Vector2 &x, const Element a,
				 VectorCategories::DenseZeroOneVectorTag tag) const
		{ if (a) this->copy (res, x); else std::fill (res.wordBegin (), res.wordEnd (), 0); return res; }
	template <class Vector1, class Vector2>
	Vector1 &mulSpecialized (Vector1 &res, const Vector2 &x, const Element a,
				 VectorCategories::SparseZeroOneVectorTag tag) const
		{ if (a) this->copy (res, x); else res.clear (); return res; }

	template <class Vector>
	inline Vector &mulinSpecialized (Vector &x, const Element a,
					 VectorCategories::DenseZeroOneVectorTag) const
		{ if (!a) std::fill (x.wordBegin (), x.wordEnd (), 0); return x; }

	template <class Vector>
	inline Vector &mulinSpecialized (Vector &x, const Element a,
					 VectorCategories::SparseZeroOneVectorTag tag) const
		{ if (!a) x.clear (); return x; }
	template <class Vector>
	inline Vector &mulinSpecialized (Vector &x, const Element a,
					 VectorCategories::HybridZeroOneVectorTag tag) const
		{ if (!a) { x.first.clear (); x.second.clear (); } return x; }

	template <class Vector1, class Vector2, class Vector3>
	inline Vector1 &addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
					VectorCategories::GenericVectorTag,
					VectorCategories::GenericVectorTag,
					VectorCategories::GenericVectorTag) const
	{
		typename LinBox::Vector<GF2>::Sparse v;
		typename LinBox::Vector<GF2>::Sparse w;
		typename LinBox::Vector<GF2>::Sparse u;

		copy (v, x);
		copy (w, y);
		add (u, w, v);
		copy (res, u);

		return u;
	}

	template <class Vector1, class Vector2, class Vector3>
	inline Vector1 &subSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
					VectorCategories::GenericVectorTag,
					VectorCategories::GenericVectorTag,
					VectorCategories::GenericVectorTag) const
	{
		typename LinBox::Vector<GF2>::Sparse v;
		typename LinBox::Vector<GF2>::Sparse w;
		typename LinBox::Vector<GF2>::Sparse u;

		copy (v, x);
		copy (w, y);
		sub (u, w, v);
		copy (res, u);

		return u;
	}

	template <class word, class Endianness>
	inline int firstNonzeroEntryInWord (word v) const;

	template <class Vector>
	inline int firstNonzeroEntrySpecialized (bool &a, const Vector &v,
						 VectorCategories::DenseZeroOneVectorTag) const;
	template <class Vector>
	inline int firstNonzeroEntrySpecialized (bool &a, const Vector &v,
						 VectorCategories::SparseZeroOneVectorTag) const;
	template <class Vector>
	inline int firstNonzeroEntrySpecialized (bool &a, const Vector &v,
						 VectorCategories::HybridZeroOneVectorTag) const;
	template <class Vector>
	inline int firstNonzeroEntrySpecialized (bool &a, const Vector &v,
						 VectorCategories::HybridZeroOneSequenceVectorTag) const;
};

// Specialization of RandomDenseStream
template<size_t bitsize> struct MTrandomInt {
    template<typename M32Twister>
    unsigned __LINBOX_INT32 operator() (M32Twister& MT) const {
        return MT.randomInt();
    }
};    

template<> struct MTrandomInt<64> {
    template<typename M32Twister>
    unsigned __LINBOX_INT64 operator() (M32Twister& MT) const {
        unsigned __LINBOX_INT64 tmp = MT.randomInt();
        tmp <<=32;
        return tmp += MT.randomInt();
    }
};

template <class Endianness>
class RandomDenseStreamGF2 : public VectorStream<BitVector<Endianness> >
{
    public:
	typedef BitVector<Endianness> Vector;

	RandomDenseStreamGF2 (const GF2 &, uint32 seed, size_t n, size_t m = 0)
		: MT (seed), _n (n), _m (m), _j (0)
	{}

	Vector &get (Vector &v) 
	{
		typename Vector::word_iterator i;

		if (_m > 0 && _j++ >= _m)
			return v;

		for (i = v.wordBegin (); i != v.wordEnd (); i++)
			*i = MTrandomInt<WordTraits<typename BitVector<Endianness>::word_type>::bits>()(MT);
                
                const size_t zeroing = WordTraits<typename BitVector<Endianness>::word_type>::bits - (v.size() % WordTraits<typename BitVector<Endianness>::word_type>::bits);
                *(v.wordRbegin()) <<= zeroing;
                *(v.wordRbegin()) >>= zeroing;
		return v;
	}

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }

    private:
	MersenneTwister MT;
	size_t          _n;
	size_t          _m;
	size_t          _j;
};

// Specialization of RandomSparseStream

template <class _Vector = Vector<GF2>::Sparse>
class RandomSparseStreamGF2 : public VectorStream<_Vector>
{
    public:
	typedef GF2 Field;
	typedef _Vector Vector;

	RandomSparseStreamGF2 (const GF2 &, uint32 seed, double p, size_t n, size_t m = 0)
		: MT (seed), _n (n), _m (m), _j (0)
	{ setP (p); }

    	RandomSparseStreamGF2 (const GF2 &F, const GF2RandIter& r, double p, size_t n, size_t m = 0)
		: MT (r.getMT()), _n (n), _m (m), _j (0)
	{ setP (p); }

	Vector &get (Vector &v);

	size_t size () const { return _m; }
	size_t pos () const { return _j; }
	size_t dim () const { return _n; }
	operator bool () const { return _m == 0 || _j < _m; }
	void reset () { _j = 0; }

	void setP (double p)
	{
		linbox_check ((p >= 0.0) && (p <= 1.0)); 
		_p = p;
		_1_log_1mp   = 1 / log (1 - _p);
	}

    private:
	MersenneTwister MT;
	size_t _n;
	double _p;
	double _1_log_1mp;
	size_t _m;
	size_t _j;
};

template <class _Vector>
_Vector &RandomSparseStreamGF2<_Vector>::get (_Vector &v)
{
	size_t i = (size_t) -1;
	double val;
	int skip;

	if (_m > 0 && _j++ >= _m)
		return v;

	v.clear ();

	while (1) {
		val = (double) MT.randomDouble ();
		skip = (int) (ceil (log (val) * _1_log_1mp));

		if (skip <= 0)
			i++;
		else
			i += skip;

		if (i >= _n) break;

		v.push_back (i);
	}

	return v;
}

template <class Vector1, class Vector2>
inline bool &DotProductDomain<GF2>::dotSpecializedDD
	(bool          &res,
	 const Vector1 &v1,
	 const Vector2 &v2) const
{
	linbox_check (v1.size () == v2.size ());

	typename Vector1::word_type t = 0;
	typename Vector1::const_word_iterator i = v1.wordBegin ();
	typename Vector2::const_word_iterator j = v2.wordBegin ();

	while (i != v1.wordEnd () - 1)
		t ^= *i++ & *j++;
        
        const size_t zeroing = WordTraits<typename Vector1::word_type>::bits - (v1.size() % WordTraits<typename Vector1::word_type>::bits);
        typename Vector1::word_type lastdot = *i & *j;
        lastdot <<= zeroing;
        lastdot >>= zeroing;
        
        t ^= lastdot;
        return res = WordTraits<typename Vector1::word_type>::ParallelParity (t);
}

template <class Vector1, class Vector2>
inline bool &DotProductDomain<GF2>::dotSpecializedDSP
	(bool          &res,
	 const Vector1 &v1,
	 const Vector2 &v2) const
{
	typename Vector2::const_iterator i;

	res = 0;

	for (i = v2.begin (); i != v2.end (); ++i)
		res ^= v1[*i];

	return res;
}

template <class Iterator, class Endianness, class Vector1, class Vector2>
inline BitVectorReference<Iterator, Endianness> DotProductDomain<GF2>::dotSpecializedDD
	(BitVectorReference<Iterator, Endianness> res,
	 const Vector1 &v1,
	 const Vector2 &v2) const
{
    bool tmp;
    return res = dotSpecializedDD(tmp, v1, v2);
}

template <class Iterator, class Endianness, class Vector1, class Vector2>
inline BitVectorReference<Iterator, Endianness> DotProductDomain<GF2>::dotSpecializedDSP
	(BitVectorReference<Iterator, Endianness> res,
	 const Vector1 &v1,
	 const Vector2 &v2) const
{
	typename Vector2::const_iterator i;

	res = 0;

	for (i = v2.begin (); i != v2.end (); ++i)
		res ^= v1[*i];

	return res;
}

template <class Vector>
std::ostream &VectorDomain<GF2>::writeSpecialized (std::ostream &os, const Vector &x,
						   VectorCategories::DenseZeroOneVectorTag) const
{
	

// TO BE REMOVED
	os << "writeSpec DenseZO, of size " << x.size() << ' ';

	os << "[ ";

	for (typename Vector::const_iterator i = x.begin (); i != x.end (); ++i)
		os << *i << ' ';

	os << ']';

	os << "( ";

	for (typename Vector::const_word_iterator i = x.wordBegin (); i != x.wordEnd (); ++i)
		os << *i << ' ';

	os << ')';

	return os;
}

template <class Vector>
std::ostream &VectorDomain<GF2>::writeSpecialized (std::ostream &os, const Vector &x,
						   VectorCategories::SparseZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector::const_iterator>::value_type index_type;

	typename Vector::const_iterator i;
	index_type idx = 0;

// TO BE REMOVED
	os << "writeSpec SparseZO, of size " << x.size() << ' ';
	os << "[ ";

	for (i = x.begin (); i != x.end (); ++i) {
		while (++idx <= *i)
			os << 0 << ' ';

		os << 1 << ' ';
	}
	os << ']';

	return os;
}

template <class Vector, class Endianness>
std::ostream &VectorDomain<GF2>::writeHybridSpecialized (std::ostream &os, const Vector &x, Endianness) const
{
	typedef typename std::iterator_traits<typename Vector::first_type::const_iterator>::value_type index_type;
	typedef typename std::iterator_traits<typename Vector::second_type::const_word_iterator>::value_type word_type;

	typename Vector::first_type::const_iterator i_idx;
	typename Vector::second_type::const_word_iterator i_elt;
	index_type idx = 0;
	word_type mask;

	os << "[ ";

	for (i_idx = x.first.begin (), i_elt = x.second.wordBegin (); i_idx != x.first.end (); ++i_idx, ++i_elt) {
		while (++idx <= *i_idx << WordTraits<word_type>::logof_size)
			os << "0 ";

		for (mask = Endianness::e_0; mask != 0; mask = Endianness::shift_right (mask, 1)) {
			if (*i_elt & mask)
				os << "1 ";
			else
				os << "0 ";
			++idx;
		}
	}
	os << ']';

	return os;
}

template <class Vector>
std::istream &VectorDomain<GF2>::readSpecialized (std::istream &is, const Vector &x,
						  VectorCategories::DenseZeroOneVectorTag) const
{
	typename Vector::iterator i;
	char c;

	do { is >> c ; } while (!std::isdigit (c));

	is.unget ();

	for (i = x.begin (); i != x.end (); ++i)
		is >> *i;

	return is;
}

template <class Vector>
std::istream &VectorDomain<GF2>::readSpecialized (std::istream &is, const Vector &x,
						  VectorCategories::SparseZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector::const_iterator>::value_type index_type;

	char c;
	index_type idx;

	do { is >> c ; } while (!std::isdigit (c));

	is.unget ();
	x.clear ();

	while (1) {
		is >> c;

		if (!std::isdigit (c) && c != ' ') break;
		is.unget ();
		is >> idx;
		x.push_back (idx);
	}

	return is;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::SparseZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector2::const_iterator>::value_type index_type;

	typename Vector1::const_iterator i = v1.begin ();
	typename Vector2::const_iterator j = v2.begin ();
	index_type idx = 0;

	for (; j != v2.end (); ++j, ++i, ++idx) {
		while (idx < *j) {
			if (*i) return false;
			++idx;
			++i;
		}

		if (!*i) return false;
	}

	for (; i != v1.end (); ++i)
		if (*i) return false;

	return true;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::DenseZeroOneVectorTag) const
{
	typename Vector1::const_word_iterator i = v1.wordBegin ();
	typename Vector2::const_word_iterator j = v2.wordBegin ();

	for (; j != v2.wordEnd (); ++j, ++i)
		if (*i != *j) return false;

	return true;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::SparseZeroOneVectorTag,
					     VectorCategories::SparseZeroOneVectorTag) const
{
	typename Vector1::const_iterator i_1;
	typename Vector2::const_iterator i_2;

	if (v1.size () != v2.size ())
		return false;

	for (i_1 = v1.begin (), i_2 = v2.begin (); i_1 != v1.end (); ++i_1, ++i_2)
		if (*i_1 != *i_2)
			return false;

	return true;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::HybridZeroOneVectorTag,
					     VectorCategories::HybridZeroOneVectorTag) const
{
	typename Vector1::first_type::const_iterator i_1_idx;
	typename Vector2::first_type::const_iterator i_2_idx;
	typename Vector1::second_type::const_iterator i_1_elt;
	typename Vector2::second_type::const_iterator i_2_elt;

	if (v1.first.size () != v2.first.size ())
		return false;

	for (i_1_idx = v1.first.begin (), i_2_idx = v2.first.begin (); i_1_idx != v1.first.end (); ++i_1_idx, ++i_2_idx)
		if (*i_1_idx != *i_2_idx)
			return false;

	for (i_1_elt = v1.second.begin (), i_2_elt = v2.second.begin (); i_1_elt != v1.second.end (); ++i_1_elt, ++i_2_elt)
		if (*i_1_elt != *i_2_elt)
			return false;

	return true;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::HybridZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector2::first_type::const_iterator>::value_type index_type;

	typename Vector1::const_word_iterator i = v1.wordBegin ();
	typename Vector2::first_type::const_iterator j_idx = v2.first.begin ();
	typename Vector2::second_type::const_word_iterator j_elt = v2.second.wordBegin ();
	index_type idx = 0;
	int t;

	for (; j_idx != v2.first.end (); ++j_idx, ++j_elt) {
		while (idx < *j_idx) {
			if (*i) return false;
			++idx;
			++i;
		}

		if (*i++ != *j_elt)
			return false;

		++idx;
	}

	for (; i != v1.wordEnd (); ++i)
		if (*i) return false;

	return true;
}

template <class Vector1, class Vector2>
bool VectorDomain<GF2>::areEqualSpecialized (const Vector1 &v1, const Vector2 &v2,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::HybridZeroOneSequenceVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector2::const_iterator>::value_type::first_type index_type;

	typename Vector1::const_word_iterator i = v1.wordBegin ();
	typename Vector2::const_iterator j = v2.begin ();
	index_type idx = 0;
	int t;

	for (; j != v2.end (); ++j) {
		while (idx < j->first) {
			if (*i) return false;
			++idx;
			++i;
		}

		if (*i++ != j->second)
			return false;

		++idx;
	}

	for (; i != v1.wordEnd (); ++i)
		if (*i) return false;

	return true;
}

template <class Vector>
bool VectorDomain<GF2>::isZeroSpecialized (const Vector &v,
					   VectorCategories::DenseZeroOneVectorTag) const
{
	typename Vector::const_word_iterator i;

	for (i = v.wordBegin (); i != v.wordEnd (); ++i)
		if (*i) return false;

	return true;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::copySpecialized (Vector1 &res, const Vector2 &v,
					     VectorCategories::SparseZeroOneVectorTag,
					     VectorCategories::DenseZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector1::const_iterator>::value_type index_type;

	typename Vector2::const_iterator i;
	index_type idx = 0;

	res.clear ();

	for (i = v.begin (); i != v.end (); ++i, ++idx)
		if (*i) res.push_back (idx);

	return res;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::copySpecialized (Vector1 &res, const Vector2 &v,
					     VectorCategories::HybridZeroOneVectorTag,
					     VectorCategories::DenseZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector1::first_type::const_iterator>::value_type index_type;

	typename Vector2::const_word_iterator i;
	index_type idx = 0;

	res.first.clear ();
	res.second.clear ();

	for (i = v.wordBegin (); i != v.wordEnd (); ++i, ++idx) {
		if (*i) {
			res.first.push_back (idx);
			res.second.push_word_back (*i);
		}
	}

	return res;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::copySpecialized (Vector1 &res, const Vector2 &v,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::SparseZeroOneVectorTag) const
{
	// FIXME
    	size_t sparsesize = *(v.rbegin());
    	if (sparsesize > res.size()) res.resize( *(v.rbegin()) );
	std::fill (res.wordBegin (), res.wordEnd (), 0);

	typename Vector2::const_iterator i;

	for (i = v.begin (); i != v.end (); ++i)
        	res[*i] = true;

	return res;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::copySpecialized (Vector1 &res, const Vector2 &v,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::HybridZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector2::second_type::const_word_iterator>::value_type word_type;

	typename Vector2::first_type::const_iterator i_idx;
	typename Vector2::second_type::const_word_iterator i_elt;

	std::fill (res.wordBegin (), res.wordEnd (), 0);

	for (i_idx = v.first.begin (), i_elt = v.second.wordBegin (); i_idx != v.first.end (); ++i_idx, ++i_elt)
        	*(res.wordBegin () + *i_idx) = *i_elt;

	return res;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::copySpecialized (Vector1 &res, const Vector2 &v,
					     VectorCategories::DenseZeroOneVectorTag,
					     VectorCategories::HybridZeroOneSequenceVectorTag) const
{
	typename Vector2::const_iterator i;

	std::fill (res.wordBegin (), res.wordEnd (), 0);

	for (i = v.begin (); i != v.end (); ++i)
        	*(res.wordBegin () + i->first) = i->second;

	return res;
}

template <class Vector, class Iterator>
inline Vector &VectorDomain<GF2>::permuteSpecialized (Vector &v, Iterator P_start, Iterator P_end,
						      VectorCategories::DenseZeroOneVectorTag) const 
{
	Iterator i;
	bool t;

	for (i = P_start; i != P_end; ++i) {
		t = v[i->first];
		v[i->first] = v[i->second];
		v[i->second] = t;
	}

	return v;
}

template <class Vector1, class Vector2>
bool &VectorDomain<GF2>::dotSpecialized (bool &res, const Vector1 &v1, const Vector2 &v2,
					 VectorCategories::SparseZeroOneVectorTag,
					 VectorCategories::SparseZeroOneVectorTag) const
{
	typename Vector1::const_iterator i = v1.begin ();
	typename Vector2::const_iterator j = v2.begin ();
	res = false;

	while (i != v1.end () || j != v2.end ()) {
		while (i != v1.end () && (j == v2.end () || *i < *j)) { res = !res; ++i; }
		while (j != v2.end () && (i == v1.end () || *j < *i)) { res = !res; ++j; }
		if (i != v1.end () && j != v2.end () && *i == *j) { ++i; ++j; }
	}

	return res;
}

template <class Vector1, class Vector2, class Vector3>
Vector1 &VectorDomain<GF2>::addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
					    VectorCategories::DenseZeroOneVectorTag,
					    VectorCategories::DenseZeroOneVectorTag,
					    VectorCategories::DenseZeroOneVectorTag) const
{
	linbox_check (res.size () == y.size ());
	linbox_check (res.size () == x.size ());

	typename Vector1::word_iterator i = res.wordBegin ();
	typename Vector2::const_word_iterator j = y.wordBegin ();
	typename Vector3::const_word_iterator k = x.wordBegin ();

	for (; i != res.wordEnd (); ++i)
		*i = *j++ ^ *k++;

	return res;
}

template <class Vector1, class Vector2, class Vector3>
Vector1 &VectorDomain<GF2>::addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
					    VectorCategories::SparseZeroOneVectorTag,
					    VectorCategories::SparseZeroOneVectorTag,
					    VectorCategories::SparseZeroOneVectorTag) const
{
	typename Vector2::const_iterator i = y.begin ();
	typename Vector3::const_iterator j = x.begin ();

	res.clear ();

	while (i != y.end () || j != x.end ()) {
		while (i != y.end () && (j == x.end () || *i < *j)) { res.push_back (*i); ++i; }
		while (j != x.end () && (i == y.end () || *j < *i)) { res.push_back (*j); ++j; }
		if (i != y.end () && j != x.end () && *i == *j) { ++i; ++j; }
	}

	return res;
}

template <class Vector1, class Vector2, class Vector3>
Vector1 &VectorDomain<GF2>::addSpecialized (Vector1 &res, const Vector2 &y, const Vector3 &x,
					    VectorCategories::HybridZeroOneVectorTag,
					    VectorCategories::HybridZeroOneVectorTag,
					    VectorCategories::HybridZeroOneVectorTag) const
{
	typename Vector2::first_type::const_iterator i_idx = y.first.begin ();
	typename Vector3::first_type::const_iterator j_idx = x.first.begin ();

	typename Vector2::second_type::const_word_iterator i_elt = y.second.wordBegin ();
	typename Vector3::second_type::const_word_iterator j_elt = x.second.wordBegin ();

	res.first.clear ();
	res.second.clear ();

	while (i_idx != y.first.end () || j_idx != x.first.end ()) {
		while (i_idx != y.first.end () && (j_idx == x.first.end () || *i_idx < *j_idx)) {
			res.first.push_back (*i_idx);
			res.second.push_word_back (*i_elt);
			++i_idx;
			++i_elt;
		}
		while (j_idx != x.first.end () && (i_idx == y.first.end () || *j_idx < *i_idx)) {
			res.first.push_back (*j_idx);
			res.second.push_word_back (*j_elt);
			++j_idx;
			++j_elt;
		}
		if (i_idx != y.first.end () && j_idx != x.first.end () && *i_idx == *j_idx) {
			if (*i_elt ^ *j_elt) {
				res.first.push_back (*i_idx);
				res.second.push_word_back (*i_elt ^ *j_elt);
			}

			++i_idx;
			++j_idx;
			++i_elt;
			++j_elt;
		}
	}

	res.second.fix_size (x.second.size ());

	return res;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::addinSpecialized (Vector1 &y, const Vector2 &x,
					      VectorCategories::DenseZeroOneVectorTag,
					      VectorCategories::DenseZeroOneVectorTag) const
{
	linbox_check (y.size () == x.size ());

	typename Vector1::word_iterator i = y.wordBegin ();
	typename Vector2::const_word_iterator j = x.wordBegin ();

	for (; i != y.wordEnd (); ++i, ++j)
		*i ^= *j;

	return y;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::addinSpecialized (Vector1 &y, const Vector2 &x,
					      VectorCategories::DenseZeroOneVectorTag,
					      VectorCategories::SparseZeroOneVectorTag) const
{
	typename Vector2::const_iterator i;

	for (i = x.begin (); i != x.end (); ++i)
		y[*i] = !y[*i];

	return y;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::addinSpecialized (Vector1 &y, const Vector2 &x,
					      VectorCategories::DenseZeroOneVectorTag,
					      VectorCategories::HybridZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector2::second_type::const_word_iterator>::value_type word_type;

	typename Vector2::first_type::const_iterator i_idx;
	typename Vector2::second_type::const_word_iterator i_elt;
	typename Vector1::word_iterator j = y.wordBegin ();

	for (i_idx = x.first.begin (), i_elt = x.second.wordBegin (); i_idx != x.first.end (); ++i_idx, ++i_elt)
		*(j + *i_idx) ^= *i_elt;

	return y;
}

template <class Vector1, class Vector2>
Vector1 &VectorDomain<GF2>::addinSpecialized (Vector1 &y, const Vector2 &x,
					      VectorCategories::DenseZeroOneVectorTag,
					      VectorCategories::HybridZeroOneSequenceVectorTag) const
{
	typename Vector2::const_iterator i;
	typename Vector1::word_iterator j = y.wordBegin ();

	for (i = x.begin (); i != x.end (); ++i)
		*(j + i->first) ^= i->second;

	return y;
}

template <class word, class Endianness>
inline int VectorDomain<GF2>::firstNonzeroEntryInWord (word v) const
{
	// FIXME: This can be made faster...
	word mask = Endianness::e_0;
	int idx = 0;

	for (; mask != 0; mask = Endianness::shift_right (mask, 1), ++idx)
		if (v & mask)
			return idx;

	return -1;
}

template <class Vector>
inline int VectorDomain<GF2>::firstNonzeroEntrySpecialized (bool &a, const Vector &v,
							    VectorCategories::DenseZeroOneVectorTag) const
{
	typename Vector::const_iterator i;

	for (i = v.begin (); i != v.end (); ++i)
		if (*i)
			return i - v.begin ();

	return -1;
}

template <class Vector>
inline int VectorDomain<GF2>::firstNonzeroEntrySpecialized (bool &a, const Vector &v,
							    VectorCategories::SparseZeroOneVectorTag) const
{
	if (v.empty ())
		return -1;
	else
		return v.front ();
}

template <class Vector>
inline int VectorDomain<GF2>::firstNonzeroEntrySpecialized (bool &a, const Vector &v,
							    VectorCategories::HybridZeroOneVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector::second_type::const_word_iterator>::value_type word_type;

	if (v.first.empty ())
		return -1;
	else
		return firstNonzeroEntryInWord<word_type, typename Vector::second_type::Endianness> (v.second.front_word ())
			+ (v.first.front () << WordTraits<word_type>::logof_size);
}

template <class Vector>
inline int VectorDomain<GF2>::firstNonzeroEntrySpecialized (bool &a, const Vector &v,
							    VectorCategories::HybridZeroOneSequenceVectorTag) const
{
	typedef typename std::iterator_traits<typename Vector::const_iterator>::value_type::second_type word_type;

	if (v.empty ())
		return -1;
	else
		return firstNonzeroEntryInWord<word_type, typename Vector::second_type::Endianness> (v.begin ()->second)
			+ (v.begin ()->first << WordTraits<word_type>::logof_size);
}

} // namespace LinBox

#include "linbox/matrix/matrix-domain-gf2.h"

#endif // __LINBOX_field_gf2_INL

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: t
// c-basic-offset: 8
// End:

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s:syntax=cpp.doxygen:foldmethod=syntax
