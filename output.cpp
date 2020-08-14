#include "stdafx.h"
#ifdef CXX_GENERATOR
#include "cxx_core.h"
#define COMPILER cxx_compiler
#else // CXX_GENERATOR
#include "c_core.h"
#define COMPILER c_compiler
#endif // CXX_GENERATOR

#include "docgen.h"

namespace docgen {
  using namespace std;
  using namespace COMPILER;
  ostream out(cout.rdbuf());
  inline void output_h(string header, string name, const file_t& file)
  {
    out << header << ' ' << name << ' ';
    out << '"' << file.m_name << '"' << ' ' << file.m_lineno;
  }
  inline bool exclude(const usr* u)
  {
    usr::flag_t flag = u->m_flag;
    if (u->isconstant())
      return true;
    if (flag & usr::ENUM_MEMBER)
      return true;
    if (flag & usr::FUNCTION)
      return false;
    if (flag & usr::STATIC)
      return true;
    return false;
  }
  set<usr*> done_u;
  inline addrof* refa(const pair<int, var*>& x)
  {
    var* v = x.second;
    addrof* r = v->addrof_cast();
    if (!r)
      return 0;
    var* vv = r->m_ref;
    usr* u = vv->usr_cast();
    assert(u);
    string name = u->m_name;
    return is_string(name) ? 0 : r;
  }
  inline void output_a(pair<int, var*> x, file_t file)
  {
    addrof* a = refa(x);
    if (!a)
      return;
    var* v = a->m_ref;
    usr* u = v->usr_cast();
    assert(u);
    usr::flag_t flag = u->m_flag;
    if (flag & usr::FUNCTION)
      output_h("    $ref $func ", u->m_name, file);
    else
      output_h("    $ref $var ", u->m_name, file);
    out << ';' << '\n';
  }
  inline void output_w(const with_initial* w)
  {
    const map<int, var*>& value = w->m_value;
    typedef map<int, var*>::const_iterator IT;
    IT p = find_if(value.begin(), value.end(), refa);
    if (p == value.end()) {
      out << ';' << '\n';
      return;
    }
    out << '\n' << '{' << '\n';
    for_each(p, value.end(), bind2nd(ptr_fun(output_a),w->m_file));
    out << '}' << '\n';
  }
  inline void output_t(const type_def* t)
  {
    string name = t->m_name;
    const vector<file_t>& vf = t->m_refed;
    for (const auto& file : vf) {
      output_h("$type $ref", name, file);
      out << ';' << '\n';
    }
  }
  inline void output_u(usr* u, const fundef* current)
  {
    if (done_u.find(u) != done_u.end())
      return;
    done_u.insert(u);
    if (exclude(u))
      return;
    if (current) {
      if (current->m_usr == u)
        return;
    }
    usr::flag_t flag = u->m_flag;
    ostringstream os;
    if (flag & usr::TYPEDEF)
      os << "$type $def";
    else if (flag & usr::FUNCTION)
      os << "$func $decl";
    else {
      os << "$var ";
      if (flag & usr::EXTERN)
        os << "$decl";
      else
        os << "$def";
    }
    output_h(os.str(), u->m_name, u->m_file);
    if (flag & usr::WITH_INI) {
      with_initial* w = static_cast<with_initial*>(u);
      return output_w(w);
    }
    out << ';' << '\n';    
    if (flag & usr::TYPEDEF) {
      type_def* t = static_cast<type_def*>(u);
      return output_t(t);
    }
  }
  inline
  void output_p(const pair <string, vector<usr*> >& p, const fundef* current)
  {
    const vector<usr*>& v = p.second;
    for (auto u : v)
      output_u(u, current);
  }
  void output_usr(const map<string, vector<usr*> >& m, const fundef* current)
  {
    for (auto p : m)
      output_p(p, current);
  }
  set<tag*> done_t;
  inline void output_p(const pair<string, tag*>& p)
  {
    tag* t = p.second;
    if (done_t.find(t) != done_t.end())
      return;
    done_t.insert(t);
    string name = t->m_name;
    if (name[0] == '.')
      return;
    const vector<file_t>& v = t->m_file;
    assert(!v.empty());
    const file_t& file = v.back();
    output_h("$tag $decl", name, file);
    out << ';' << '\n';
  }
  void output_tag(const map<string, tag*>& m)
  {
    for (auto p : m)
      output_p(p);
  }
  inline bool is_top(const COMPILER::scope* ptr)
  {
    using namespace COMPILER;
#ifndef CXX_GENERATOR
    return !ptr->m_parent;
#else // CXX_GENERATOR
    if ( !ptr->m_parent )
      return true;
    if ( ptr->m_id == scope::TAG )
      return is_top(ptr->m_parent);
    if ( ptr->m_id == scope::NAMESPACE )
      return is_top(ptr->m_parent);
    return false;
#endif // CXX_GENERATOR
  }
  inline void output_v(var* v, const file_t& file)
  {
    if (is_external_declaration(v)) {
      usr* u = v->usr_cast();
      assert(u);
      if (!exclude(u)) {
        usr::flag_t flag = u->m_flag;
        if (flag & usr::FUNCTION)
          output_h("    $ref $func", u->m_name, file);
        else
          output_h("    $ref $var", u->m_name, file);
        out << ';' << '\n';
      }
    }
    const type* T = v->m_type;
    T = T->unqualified();
    if (T->m_id == type::POINTER) {
      typedef const pointer_type PT;
      PT* pt = static_cast<PT*>(T);
      T = pt->referenced_type();
      T = T->unqualified();
    }
    if (T->m_id != type::RECORD)
      return;
    typedef const record_type REC;
    REC* rec = static_cast<REC*>(T);
    tag* t = rec->get_tag();
#ifdef CXX_GENERATOR
    if (!is_top(t))
      return;
#else // CXX_GENERATOR 
    if (!is_top(t->m_scope))
      return;
#endif // CXX_GENERATOR
    string name = t->m_name;
    if (name[0] == '.')
      return;
    output_h("    $ref $tag", name, file);
    out << ';' << '\n';
  }
  inline void output_t(const tac* ptr)
  {
    const file_t& file = ptr->m_file;
    if (var* x = ptr->x)
      output_v(x, file);
    if (var* y = ptr->y)
      output_v(y, file);
    if (var* z = ptr->z)
      output_v(z, file);
  }
  using namespace optimize::basic_block;
  inline int id(info_t* bb, const map<info_t*, int>& tbl)
  {
    typedef map<info_t*, int>::const_iterator IT;
    IT p = tbl.find(bb);
    assert(p != tbl.end());
    return p->second;
  }
  inline void edge(info_t* x, info_t* y, const map<info_t*, int>& tbl)
  {
    out << "        ";
    out << '(' << id(x, tbl) << ',' << id(y, tbl) << ')' << ';' << '\n';
  }
  inline void output_b(info_t* bb, const map<info_t*, int>& tbl)
  {
    out << "        " << id(bb, tbl) << ' ';
    tac* leader = *bb->m_leader;
    const file_t file = leader->m_file;
    out << '"' << file.m_name << '"' << ' ' << file.m_lineno << ';' << '\n';
    for (auto f : bb->m_follow)
      edge(bb, f, tbl);
  }
  void output_def(const fundef* fdef, const vector<tac*>* pcode)
  {
    usr* u = fdef->m_usr;
    output_h("$func $def", u->m_name, u->m_file);
    out << '\n';
    done_u.insert(u);
    out << '{' << '\n';
    const vector<tac*>& v = *pcode;
    for (auto t : v)
      output_t(t);
    vector<info_t*> bbs;
    create(v, bbs);
    map<info_t*, int> tbl;
    int i = 0;
    for (auto bb : bbs)
      tbl[bb] = i++;
    out << "    $graph {" << '\n';
    for (auto bb : bbs)
      output_b(bb, tbl);
    out << "    }" << '\n';
    out << '}' << '\n';
    for (auto bb : bbs)
      delete bb;
  }
  void (*create)(const vector<tac*>&, vector<info_t*>&);
} // end of namespace docgen
