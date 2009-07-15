/*
    Gri - A language for scientific graphics programming
    Copyright (C) 2008 Daniel Kelley

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

//#define DEBUG_GRIMATRIX		// debug

// Store items in a matrix.  Items can be builtins or classes.

#if !defined(_GMatrix_h_)
#define _GMatrix_h_

template<class T>
class GriMatrix
{
public:
	GriMatrix();
	~GriMatrix();
	void set_size(unsigned int num_cols, unsigned int num_rows);
	void set_value(T value);
	T& operator()(unsigned int col, unsigned int row);
	T  operator()(unsigned int col, unsigned int row) const;
protected:
	int haveData;
	unsigned int rows;		// row < rows
	unsigned int cols;		// col < cols
	T *contents;
};

template<class T>
GriMatrix<T>::GriMatrix()
{
	haveData = rows = cols = 0;
}

template<class T>
GriMatrix<T>::~GriMatrix()
{
	if (haveData) 
		delete [] contents;
}

template<class T>
void GriMatrix<T>::set_size(unsigned int num_cols, unsigned int num_rows)
{
	if (num_rows == 0 && num_cols == 0) {
		if (haveData)
			delete [] contents;
		haveData = rows = cols = 0;
	} else {
		if (haveData)
			delete [] contents;
		rows = num_rows;
		cols = num_cols;
		contents = new T[rows * cols];
		if (!contents) 
			gr_Error("Out of memory (GriMatrix)");
		haveData = 1;
#ifdef DEBUG_GRIMATRIX
		printf("GriMatrix.set_size(rows=%d,cols=%d) start=%lx end=%lx\n",rows,cols,contents,contents + cols * rows - 1);
#endif
	}
}

template<class T>
void GriMatrix<T>::set_value(T value)
{
	for (unsigned int col = 0; col < cols; col++)
		for (unsigned int row = 0; row < rows; row++)
			contents[row + col * rows] = value;
}

template<class T>
T& GriMatrix<T>::operator()(unsigned int col, unsigned int row)
{
	if (!haveData)
		gr_Error("Trying to get data from empty GriMatrix");
	char errorMsg[100];
	if (row > rows - 1) {
		sprintf(errorMsg, "\
Can't use row %d of matrix; valid range: 0-%d", row, rows-1);
		gr_Error(errorMsg);
	}
	if (col > cols - 1) {
		sprintf(errorMsg, "\
Can't use col %d of matrix; valid range: 0-%d", col, cols-1);
		gr_Error(errorMsg);
	}
	return contents[row + col * rows];
}
template<class T>
T GriMatrix<T>::operator()(unsigned int col, unsigned int row) const
{
	if (!haveData)
		gr_Error("Trying to get data from empty GriMatrix");
	char errorMsg[100];
	if (row > rows - 1) {
		sprintf(errorMsg, "\
Can't use row %d of matrix; valid range: 0-%d", row, rows-1);
		gr_Error(errorMsg);
	}
	if (col > cols - 1) {
		sprintf(errorMsg, "\
Can't use col %d of matrix; valid range: 0-%d", col, cols-1);
		gr_Error(errorMsg);
	}
	return contents[row + col * rows];
}
#endif				// _GMatrix_h_
