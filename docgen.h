#ifndef _DOCGEN_H_
#define _DOCGEN_H_

namespace docgen {
  using namespace std;
  using namespace COMPILER;
  extern ostream out;
  void output_usr(const map<string, vector<usr*> >&, const fundef*);
  void output_tag(const map<string, tag*>&);
  void output_def(const fundef*, const vector<tac*>*);
  extern void (*create)(const vector<tac*>&, vector<optimize::basic_block::info_t*>&);
} // end of namespace docgen

#endif // _DOCGEN_H_
