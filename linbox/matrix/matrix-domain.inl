/* linbox/matrix/matrix-domain.inl
 * Copyright 2002 Bradford Hovinen
 *
 * Written by Bradford Hovinen <bghovinen@math.uwaterloo.ca>
 *
 * ------------------------------------
 *
 * See COPYING for license information.
 */

#ifndef __LINBOX_matrix_domain_INL
#define __LINBOX_matrix_domain_INL

#include "linbox/matrix/transpose-matrix.h"

namespace LinBox
{

template <class Field>
template <class Vector1, class Matrix, class Vector2>
Vector2 &MatrixDomainSupportGeneric<Field>::gemvRowSpecialized (const typename Field::Element &alpha,
								const Matrix                  &A,
								const Vector1                 &x,
								const typename Field::Element &beta,
								Vector2                       &y,
								VectorCategories::DenseVectorTag) const
{
	linbox_check (A.rowdim () == y.size ());

	typename Matrix::ConstRowIterator i = A.rowBegin ();
	typename Vector2::iterator j = y.begin ();

	typename Field::Element d;

	for (; i != A.rowEnd (); ++j, ++i) {
		_VD.dot (d, x, *i);
		_F.mulin (*j, beta);
		_F.axpyin (*j, alpha, d);
	}

	return y;
}

template <class Field>
template <class Vector1, class Matrix, class Vector2>
Vector2 &MatrixDomainSupportGeneric<Field>::gemvRowSpecialized (const typename Field::Element &alpha,
								const Matrix                  &A,
								const Vector1                 &x,
								const typename Field::Element &beta,
								Vector2                       &y,
								VectorCategories::SparseVectorTag) const
{
	typename Matrix::ConstRowIterator i = A.rowBegin ();
	typename Field::Element t;
	unsigned int idx = 0;

	std::vector<std::pair<size_t, typename Field::Element> > yp;

	if (_F.isZero (beta))
		y.clear ();
	else
		_VD.mulin (y, beta);

	for (; i != A.rowEnd (); ++i, ++idx) {
		_VD.dot (t, x, *i);
		_F.mulin (t, alpha);

		if (!_F.isZero (t))
			yp.push_back (std::pair<size_t, typename Field::Element> (idx, t));
	}

	return _VD.addin (y, yp);
}

template <class Field>
template <class Vector1, class Matrix, class Vector2>
Vector2 &MVProductDomain<Field>::gemvColDense (const VectorDomain<Field>     &VD,
					       const typename Field::Element &alpha,
					       const Matrix                  &A,
					       const Vector1                 &x,
					       const typename Field::Element &beta,
					       Vector2                       &y) const
{
	linbox_check (A.coldim () == x.size ());
	linbox_check (A.rowdim () == y.size ());

	typename Matrix::ConstColIterator i = A.colBegin ();
	typename Vector1::const_iterator j = x.begin ();
	typename Field::Element d;

	VD.mulin (y, beta);

	for (; j != x.end (); ++j, ++i) {
		VD.field ().mul (d, alpha, *j);
		VD.axpyin (y, d, *i);
	}

	return y;
}

template <class Field>
template <class Vector1, class Matrix, class Vector2>
Vector2 &MatrixDomainSupportGeneric<Field>::gemvColSpecialized (const typename Field::Element &alpha,
								const Matrix                  &A,
								const Vector1                 &x,
								const typename Field::Element &beta,
								Vector2                       &y,
								VectorCategories::SparseVectorTag) const
{
	linbox_check (A.rowdim () == y.size ());

	typename Vector1::const_iterator j = x.begin ();
	typename Field::Element d;

	_VD.mulin (y, beta);

	for (; j != x.end (); ++j) {
		typename Matrix::ConstColIterator i = A.colBegin () + j->first;
		_F.mul (d, alpha, j->second);
		_VD.axpyin (y, d, *i);
	}

	return y;
}

template <class Field>
template <class Vector1, class Vector2, class Matrix>
inline Matrix &MatrixDomainSupportGeneric<Field>::gerRowSpecialised (const typename Field::Element &a, const Vector1 &x, const Vector2 &y, Matrix &A,
								     VectorCategories::DenseVectorTag) const
{
	linbox_check (A.rowdim () == x.size ());

	typename Matrix::RowIterator i_A;
	typename Vector1::const_iterator i_x;

	typename Field::Element axi;

	for (i_A = A.rowBegin (), i_x = x.begin (); i_A != A.rowEnd (); ++i_A, ++i_x) {
		_F.mul (axi, a, *i_x);
		_VD.axpyin (*i_A, axi, y);
	}

	return A;
}

template <class Field>
template <class Vector1, class Vector2, class Matrix>
inline Matrix &MatrixDomainSupportGeneric<Field>::gerRowSpecialised (const typename Field::Element &a, const Vector1 &x, const Vector2 &y, Matrix &A,
								     VectorCategories::SparseVectorTag) const
{
	typename Vector1::const_iterator i_x;

	typename Field::Element axi;

	for (i_x = x.begin (); i_x != x.end (); ++i_x) {
		_F.mul (axi, a, i_x->second);
		_VD.axpyin (*(A.rowBegin () + i_x->first), axi, y);
	}

	return A;
}

template <class Field>
template <class Vector1, class Vector2, class Matrix>
inline Matrix &MatrixDomainSupportGeneric<Field>::gerColSpecialised (const typename Field::Element &a, const Vector1 &x, const Vector2 &y, Matrix &A,
								     VectorCategories::DenseVectorTag) const
{
	linbox_check (A.coldim () == y.size ());

	typename Matrix::ColIterator i_A;
	typename Vector2::const_iterator i_y;

	typename Field::Element ayi;

	for (i_A = A.colBegin (), i_y = y.begin (); i_A != A.colEnd (); ++i_A, ++i_y) {
		_F.mul (ayi, a, *i_y);
		_VD.axpyin (*i_A, ayi, x);
	}

	return A;
}

template <class Field>
template <class Vector1, class Vector2, class Matrix>
inline Matrix &MatrixDomainSupportGeneric<Field>::gerColSpecialised (const typename Field::Element &a, const Vector1 &x, const Vector2 &y, Matrix &A,
								     VectorCategories::SparseVectorTag) const
{
	typename Vector2::const_iterator i_y;

	typename Field::Element ayi;

	for (i_y = y.begin (); i_y != y.end (); ++i_y) {
		_F.mul (ayi, a, i_y->second);
		_VD.axpyin (*(A.colBegin () + i_y->first), ayi, x);
	}

	return A;
}

template <class Field>
template <class Matrix, class Vector>
Vector &MatrixDomainSupportGeneric<Field>::trsvSpecialized (const Matrix &A, Vector &x,
							    MatrixCategories::RowMatrixTag,
							    VectorCategories::DenseVectorTag) const
{
	linbox_check (A.coldim () == A.rowdim ());
	linbox_check (A.rowdim () == x.size ());

	typename Field::Element ai, ai_p_1, neg_ai_inv, d;
	int i = A.rowdim ();

	while (--i >= 0) {
		if (!VectorWrapper::getEntry (*(A.rowBegin () + i), ai, i))
			continue;

		_VD.dot (d, *(A.rowBegin () + i), x);

		_F.invin (ai);
		_F.add (ai_p_1, ai, _F.one ());
		_F.mulin (x[i], ai_p_1);
		_F.neg (neg_ai_inv, ai);
		_F.axpyin (x[i], neg_ai_inv, d);
	}

	return x;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix1 &MatrixDomainSupportGeneric<Field>::copyRow (Matrix1 &A, const Matrix2 &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::RowIterator i;
	typename Matrix2::ConstRowIterator j;

	i = A.rowBegin ();
	j = B.rowBegin ();

	for (; i != A.rowEnd (); ++i, ++j)
		_VD.copy (*i, *j);

	return A;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix1 &MatrixDomainSupportGeneric<Field>::copyCol (Matrix1 &A, const Matrix2 &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::ColIterator i;
	typename Matrix2::ConstColIterator j;

	i = A.colBegin ();
	j = B.colBegin ();

	for (; i != A.colEnd (); ++i, ++j)
		_VD.copy (*i, *j);

	return A;
}

template <class Field>
template <class Matrix1, class Matrix2>
bool MatrixDomainSupportGeneric<Field>::areEqualRow (const Matrix1 &A, const Matrix2 &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::ConstRowIterator i;
	typename Matrix2::ConstRowIterator j;

	i = A.rowBegin ();
	j = B.rowBegin ();

	for (; i != A.rowEnd (); ++i, ++j)
		if (!_VD.areEqual (*i, *j))
			return false;

	return true;
}

template <class Field>
template <class Matrix1, class Matrix2>
bool MatrixDomainSupportGeneric<Field>::areEqualCol (const Matrix1 &A, const Matrix2 &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::ConstColIterator i;
	typename Matrix2::ConstColIterator j;

	i = A.colBegin ();
	j = B.colBegin ();

	for (; i != A.colEnd (); ++i, ++j)
		if (!_VD.areEqual (*i, *j))
			return false;

	return true;
}

template <class Field>
template <class Matrix>
bool MatrixDomainSupportGeneric<Field>::isZeroRow (const Matrix &A) const
{
	typename Matrix::ConstRowIterator i;

	i = A.rowBegin ();

	for (; i != A.rowEnd (); ++i)
		if (!_VD.isZero (*i))
			return false;

	return true;
}

template <class Field>
template <class Matrix>
bool MatrixDomainSupportGeneric<Field>::isZeroCol (const Matrix &A) const
{
	typename Matrix::ConstColIterator i;

	i = A.colBegin ();

	for (; i != A.colEnd (); ++i)
		if (!_VD.isZero (*i))
			return false;

	return true;
}

template <class Field>
template <class Matrix>
Matrix &MatrixDomainSupportGeneric<Field>::scalRow (Matrix &B, const typename Field::Element &a) const
{
	typename Matrix::RowIterator i;

	for (i = B.rowBegin (); i != B.rowEnd (); ++i)
		_VD.mulin (*i, a);

	return B;
}

template <class Field>
template <class Matrix>
Matrix &MatrixDomainSupportGeneric<Field>::scalCol (Matrix &B, const typename Field::Element &a) const
{
	typename Matrix::ColIterator i;

	for (i = B.colBegin (); i != B.colEnd (); ++i)
		_VD.mulin (*i, a);

	return B;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix2 &MatrixDomainSupportGeneric<Field>::axpyRow (const typename Field::Element &a,
						     const Matrix1                 &A,
						     Matrix2                       &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::ConstRowIterator i;
	typename Matrix2::RowIterator j;

	i = A.rowBegin ();
	j = B.rowBegin ();

	for (; i != A.rowEnd (); ++i, ++j)
		_VD.axpyin (*j, a, *i);

	return B;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix2 &MatrixDomainSupportGeneric<Field>::axpyCol (const typename Field::Element &a,
						     const Matrix1                 &A,
						     Matrix2                       &B) const
{
	linbox_check (A.rowdim () == B.rowdim ());
	linbox_check (A.coldim () == B.coldim ());

	typename Matrix1::ConstColIterator i;
	typename Matrix2::ColIterator j;

	i = A.colBegin ();
	j = B.colBegin ();

	for (; i != A.colEnd (); ++i, ++j)
		_VD.axpyin (*j, a, *i);

	return B;
}

template <class Field>
template <class Matrix1, class Matrix2, class Matrix3>
Matrix3 &MatrixDomainSupportGeneric<Field>::gemmRowRowCol (const typename Field::Element &alpha,
							   const Matrix1                 &A,
							   const Matrix2                 &B,
							   const typename Field::Element &beta,
							   Matrix3                       &C) const
{
	linbox_check (A.coldim () == B.rowdim ());
	linbox_check (A.rowdim () == C.rowdim ());
	linbox_check (B.coldim () == C.coldim ());

	typename Matrix1::ConstRowIterator i;
	typename Matrix2::ConstColIterator j;
	typename Matrix3::RowIterator l1;
	typename Matrix3::Row::iterator l2;
	typename Field::Element d;

	for (i = A.rowBegin (), l1 = C.rowBegin (); i != A.rowEnd (); ++i, ++l1) {
		for (j = B.colBegin (), l2 = l1->begin (); j != B.colEnd (); ++j, ++l2) {
			_VD.dot (d, *i, *j);
			_F.mulin (*l2, beta);
			_F.axpyin (*l2, alpha, d);
		}
	}

	return C;
}

template <class Field>
template <class Matrix1, class Matrix2, class Matrix3>
Matrix3 &MatrixDomainSupportGeneric<Field>::gemmColRowCol (const typename Field::Element &alpha,
							   const Matrix1                 &A,
							   const Matrix2                 &B,
							   const typename Field::Element &beta,
							   Matrix3                       &C) const
{
	linbox_check (A.coldim () == B.rowdim ());
	linbox_check (A.rowdim () == C.rowdim ());
	linbox_check (B.coldim () == C.coldim ());

	typename Matrix1::ConstRowIterator i;
	typename Matrix2::ConstColIterator j;
	typename Matrix3::ColIterator l1;
	typename Matrix3::Col::iterator l2;
	typename Field::Element d;

	for (j = B.colBegin (), l1 = C.colBegin (); j != B.colEnd (); ++j, ++l1) {
		for (i = A.rowBegin (), l2 = l1->begin (); i != A.rowEnd (); ++i, ++l2) {
			_VD.dot (d, *i, *j);
			_F.mulin (*l2, beta);
			_F.axpyin (*l2, alpha, d);
		}
	}

	return C;
}

template <class Field>
template <class Matrix1, class Matrix2, class Matrix3>
Matrix3 &MatrixDomainSupportGeneric<Field>::gemmRowRowRow (const typename Field::Element &alpha,
							   const Matrix1                 &A,
							   const Matrix2                 &B,
							   const typename Field::Element &beta,
							   Matrix3                       &C) const
{
	linbox_check (A.coldim () == B.rowdim ());
	linbox_check (A.rowdim () == C.rowdim ());
	linbox_check (B.coldim () == C.coldim ());

	typename Matrix1::ConstRowIterator i = A.rowBegin ();
	typename Matrix3::RowIterator j = C.rowBegin ();

	TransposeMatrix<const Matrix2> BT (B);

	for (; i != A.rowEnd (); ++i, ++j)
		gemv (alpha, BT, *i, beta, *j);

	return C;
}

template <class Field>
template <class Matrix1, class Matrix2, class Matrix3>
Matrix3 &MatrixDomainSupportGeneric<Field>::gemmColColCol (const typename Field::Element &alpha,
							   const Matrix1                 &A,
							   const Matrix2                 &B,
							   const typename Field::Element &beta,
							   Matrix3                       &C) const
{
	linbox_check (A.coldim () == B.rowdim ());
	linbox_check (A.rowdim () == C.rowdim ());
	linbox_check (B.coldim () == C.coldim ());

	typename Matrix2::ConstColIterator i = B.colBegin ();
	typename Matrix3::ColIterator j = C.colBegin ();

	for (; i != B.colEnd (); ++i, ++j)
		gemv (alpha, A, *i, beta, *j);

	return C;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix2 &MatrixDomainSupportGeneric<Field>::trsmSpecialized (const typename Field::Element &alpha, const Matrix1 &A, Matrix2 &B,
							     MatrixCategories::RowMatrixTag,
							     MatrixCategories::RowMatrixTag) const
{
	linbox_check (A.coldim () == A.rowdim ());
	linbox_check (A.rowdim () == B.rowdim ());

	typename Field::Element ai, ai_p_1, neg_ai_inv;
	int i = A.rowdim () - 1;

	TransposeMatrix<const Matrix1> AT (A);

	while (--i >= 0) {
		if (_VD.firstNonzeroEntry (ai, *(A.rowBegin () + i)) == -1)
			continue;

		_F.add (ai_p_1, ai, _F.one ());
		_F.inv (neg_ai_inv, ai);
		_F.negin (neg_ai_inv);

		gemv (neg_ai_inv, AT, *(A.rowBegin () + i), ai_p_1, *(B.rowBegin () + i));
	}

	return B;
}

template <class Field>
template <class Matrix1, class Matrix2>
Matrix2 &MatrixDomainSupportGeneric<Field>::trsmSpecialized (const typename Field::Element &alpha, const Matrix1 &A, Matrix2 &B,
							     MatrixCategories::RowMatrixTag,
							     MatrixCategories::ColMatrixTag) const
{
	typename Matrix2::ColIterator i_B;

	for (i_B = B.colBegin (); i_B != B.colEnd (); ++i_B) {
		trsv (A, *i_B);
		_VD.mulin (*i_B, alpha);
	}

	return B;
}

template <class Field>
template <class Matrix, class Iterator>
Matrix &MatrixDomainSupportGeneric<Field>::permuteRowsByRow (Matrix   &A,
							     Iterator  P_start,
							     Iterator  P_end) const
{
	Iterator i;
	typename Matrix::RowIterator j, k;

	for (i = P_start; i != P_end; ++i) {
		j = A.rowBegin () + i->first;
		k = A.rowBegin () + i->second;

		_VD.swap (*j, *k);
	}

	return A;
}

template <class Field>
template <class Matrix, class Iterator>
Matrix &MatrixDomainSupportGeneric<Field>::permuteRowsByCol (Matrix   &A,
							     Iterator  P_start,
							     Iterator  P_end) const
{
	typename Matrix::ColIterator j;

	for (j = A.colBegin (); j != A.colEnd (); ++j)
		_VD.permute (*j, P_start, P_end);

	return A;
}

template <class Field>
template <class Matrix, class Iterator>
Matrix &MatrixDomainSupportGeneric<Field>::permuteColsByRow (Matrix   &A,
							     Iterator  P_start,
							     Iterator  P_end) const
{
	typename Matrix::RowIterator j;

	for (j = A.rowBegin (); j != A.rowEnd (); ++j)
		_VD.permute (*j, P_start, P_end);

	return A;
}

template <class Field>
template <class Matrix, class Iterator>
Matrix &MatrixDomainSupportGeneric<Field>::permuteColsByCol (Matrix   &A,
							     Iterator  P_start,
							     Iterator  P_end) const
{
	Iterator i;
	typename Matrix::ColIterator j, k;

	for (i = P_start; i != P_end; ++i) {
		j = A.colBegin () + i->first;
		k = A.colBegin () + i->second;

		_VD.swap (*j, *k);
	}

	return A;
}

} // namespace LinBox

#endif // __LINBOX_matrix_domain_INL

// Local Variables:
// mode: C++
// tab-width: 8
// indent-tabs-mode: t
// c-basic-offset: 8
// End:

// vim:sts=8:sw=8:ts=8:noet:sr:cino=>s,f0,{0,g0,(0,\:0,t0,+0,=s:syntax=cpp.doxygen:foldmethod=syntax