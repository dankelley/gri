// Instantiate templates.
// BUG: this code may well need changing as compilers evolve
// BUG: in how they handle templates.  :=(

#define _PUT_STATIC_DATA_MEMBERS_HERE
#include <string>
#include <stdio.h>

#include <algorithm>		// part of STL
#include <vector>		// part of STL
#include "gr.hh"
#include "defaults.hh"
#include "private.hh"
#include "types.hh"
#include "gr_coll.hh"
#include "GriColor.hh"
#include "GMatrix.hh"
#include "GriState.hh"
#include "Synonym.hh"
#include "Variable.hh"

#if defined(USING_CXX_REPO)
template void  sort(vector<double>::iterator, vector<double>::iterator);
#else
// Instantiate on DEC c++ compiler
#if defined(__DECCXX)
//#pragma define_template void reverse(vector<double>::iterator, vector<double>::iterator);
//#pragma define_template void sort(vector<double>::iterator, vector<double>::iterator);
#pragma define_template vector<BlockSource>
#pragma define_template vector<CmdFile>
#pragma define_template vector<DataFile>
#pragma define_template vector<GriState>
#pragma define_template vector<GriSynonym>
#pragma define_template vector<GriVariable>
#pragma define_template vector<RpnItem>
#pragma define_template vector<bool>
#pragma define_template vector<double>
#pragma define_template vector<float>
#pragma define_template vector<int>
#pragma define_template vector<unsigned char>
#pragma define_template vector<char*>
#pragma define_template vector<string>
#pragma define_template GriMatrix<bool>
#pragma define_template GriMatrix<double>
#endif // DEC compiler


// Instantiate on GNU c++ compiler
#if defined(__GNUC__)
template void  reverse(vector<double>::iterator, vector<double>::iterator);
template void  sort(vector<double>::iterator, vector<double>::iterator);
template class vector<BlockSource>;
template class vector<CmdFile>;
template class vector<DataFile>;
template class vector<GriNamedColor>;
template class vector<GriState>;
template class vector<GriSynonym>;
template class vector<GriVariable>;
template class vector<RpnItem>;
template class vector<bool>;
template class vector<double>;
template class vector<float>;
template class vector<int>;
template class vector<unsigned char>;
template class vector<char*>;
template class vector<string>;
template class GriMatrix<bool>;
template class GriMatrix<double>;

#endif

#endif
