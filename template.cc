// Instantiate templates.
// NB. This code may need changing as compilers evolve :-(

#define _PUT_STATIC_DATA_MEMBERS_HERE
#include <string>
#include <vector>		// part of STL
#include <algorithm>		// part of STL
#include <stdio.h>
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
// DEC c++ compiler
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
#pragma define_template vector<const char*>
#pragma define_template GriMatrix<bool>
#pragma define_template GriMatrix<double>
#endif // DEC compiler


// GNU c++ compiler
#if defined(__GNUC__)

#if __GNUC__ >= 3		// avoid deficiency in old compilers

//void std::reverse(std::vector<double>::iterator, std::vector<double>::iterator);
//void std::sort(std::vector<double>::iterator, std::vector<double>::iterator);

#else

template void std::reverse(std::vector<double>::iterator, std::vector<double>::iterator);
template void std::sort(std::vector<double>::iterator, std::vector<double>::iterator);

#endif

template class std::vector<BlockSource>;
template class std::vector<CmdFile>;
template class std::vector<DataFile>;
template class std::vector<GriNamedColor>;
template class std::vector<GriState>;
template class std::vector<GriSynonym>;
template class std::vector<GriVariable>;
template class std::vector<RpnItem>;
template class std::vector<bool>;
template class std::vector<double>;
template class std::vector<float>;
template class std::vector<int>;
template class std::vector<unsigned char>;
template class std::vector<const char*>;
template class GriMatrix<bool>;
template class GriMatrix<double>;

#endif

#endif
