#include "stdafx.h"

#ifdef CXX_GENERATOR
#include "cxx_core.h"
#define COMPILER cxx_compiler
#else // CXX_GENERATOR
#include "c_core.h"
#define COMPILER c_compiler
#endif // CXX_GENERATOR

#include "docgen.h"

#ifdef _MSC_VER
#define DLL_EXPORT __declspec(dllexport)
#else // _MSC_VER
#define DLL_EXPORT
#endif // _MSC_VER

extern "C" DLL_EXPORT int generator_seed()
{
#ifdef _MSC_VER
  int r = _MSC_VER;
#ifndef CXX_GENERATOR
  r += 10000000;
#else // CXX_GENERATOR
  r += 20000000;
#endif // CXX_GENERATOR
#ifdef WIN32
  r += 100000;
#endif // WIN32
#endif // _MSC_VER
#ifdef __GNUC__
  int r = (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__);
#ifndef CXX_GENERATOR
  r += 30000000;
#else // CXX_GENERATOR
  r += 40000000;
#endif // CXX_GENERATOR
#endif // __GNUC__
  return r;
}

namespace docgen {
  using namespace std;
  ofstream* ptr_out;
} // end of namespace docgen

extern "C" DLL_EXPORT int generator_open_file(const char* fn)
{
  using namespace std;
  using namespace docgen;
  ptr_out = new ofstream(fn);
  out.rdbuf(ptr_out->rdbuf());
  return 0;
}

extern "C" DLL_EXPORT
void generator_generate(const COMPILER::generator::interface_t* ptr)
{
  using namespace docgen;
  output_usr(ptr->m_root->m_usrs, ptr->m_func);
  output_tag(ptr->m_root->m_tags);
  if (ptr->m_func)
    output_def(ptr->m_func, ptr->m_code);
}

extern "C" DLL_EXPORT int generator_close_file()
{
  using namespace docgen;
  delete ptr_out;
  return 0;
}

namespace docgen {
  using namespace std;
  using namespace COMPILER;
  void(*output3ac)(ostream&, tac*);
} // end of namespace docgen

extern "C" DLL_EXPORT void generator_spell(void* arg)
{
  using namespace std;
  using namespace COMPILER;
  using namespace docgen;
  void* magic[] = {
    ((char **)arg)[0],
    ((char **)arg)[1],
  };
  int index = 0;
  memcpy(&output3ac, &magic[index], sizeof magic[index]); ++index;
  memcpy(&create,    &magic[index], sizeof magic[index]); ++index;
}
