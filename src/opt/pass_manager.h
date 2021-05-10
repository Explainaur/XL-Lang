#ifndef XY_LANG_PASS_MANAGER_H
#define XY_LANG_PASS_MANAGER_H

#include <map>
#include <string_view>
#include <unordered_set>
#include <utility>

#include "opt/pass.h"
#include "mid/ir/module.h"

namespace RJIT::opt {
class PassInfo;
class PassManager;
class PassFactory;

using PassInfoPtr     = std::shared_ptr<PassInfo>;
using PassFactoryPtr  = std::shared_ptr<PassFactory>;
using PassPtrList     = std::vector<PassInfoPtr>;
using PassFactoryList = std::vector<PassFactoryPtr>;
using PassNameList    = std::vector<std::string>;
using PassInfoMap     = std::unordered_map<std::string, PassInfoPtr>;
using PassNameSet     = std::unordered_set<std::string>;
using RequirementMap  = std::unordered_map<std::string, PassNameSet>;

// pass factory
class PassFactory {
public:
  virtual ~PassFactory() = default;
  virtual PassInfoPtr CreatePass(PassManager *) = 0;
};


// pass information
class PassInfo {
private:
  PassPtr       _pass;
  std::string   _pass_name;
  bool          _is_analysis;
  std::size_t   _min_opt_level;
  PassNameList  _required_passes;
  PassNameList  _invalidated_passes;

public:
  PassInfo(PassPtr pass, std::string name, bool is_analysis, std::size_t min_opt_level)
      : _pass(std::move(pass)), _pass_name(std::move(name)), _is_analysis(is_analysis),
        _min_opt_level(min_opt_level) {}

  // add required pass by name for current pass
  // all required passes should be run before running current pass
  PassInfo &Requires(const std::string &pass_name);

  // add invalidated pass by name for current pass
  // all invalidated passes should be run again after running current pass
  PassInfo &Invalidates(const std::string &pass_name);

  // setters
  // set if current pass is an analysis pass
  PassInfo &set_is_analysis(bool is_analysis) {
    _is_analysis = is_analysis;
    return *this;
  }
  // set minimum optimization level of current pass required
  PassInfo &set_min_opt_level(std::size_t min_opt_level) {
    _min_opt_level = min_opt_level;
    return *this;
  }

  // getter/setter
  const PassPtr &pass()          const { return _pass;          }
  std::string    name()          const { return _pass_name;     }
  bool           is_analysis()   const { return _is_analysis;   }
  std::size_t    min_opt_level() const { return _min_opt_level; }

  const PassNameList &required_passes()    const { return _required_passes;    }
  const PassNameList &invalidated_passes() const { return _invalidated_passes; }
};

// pass manager
class PassManager {
private:
  std::size_t     _opt_level;
  mid::Module    *_module;
  PassInfoMap     _pass_infos;
  RequirementMap  _requirements;
  PassPtrList     _candidates;
  PassFactoryList _factories;

  void AddFactory(const std::shared_ptr<PassFactory> &factory) {
    _factories.push_back(factory);
  }

  void init();

public:
  static PassManager _instance;

  PassManager() : _opt_level(0), _module(nullptr) {}

  explicit PassManager(mid::Module &module)
    : _opt_level(0), _module(&module) {}

  static void Initialize() { _instance.init(); }

  static PassManager *GetPassManager() { return &_instance; }

  static PassInfoMap &GetPasses() { return _instance._pass_infos; }

  static RequirementMap &GetRequiredBy() { return _instance._requirements; }

  static PassPtrList &Candidates() { return _instance._candidates; }

  static void RequiredBy(const std::string &slave, const std::string &master);

  // run required passes
  static bool RunRequiredPasses(PassNameSet &valid, const PassInfoPtr &info);

  // invalidate the specific pass
  static void InvalidatePass(PassNameSet &valid, const std::string &name);

  // register pass
  static void RegisterPassFactory(const PassFactoryPtr &factory) {
    _instance.AddFactory(factory);
  }

  // run a specific pass
  static bool RunPass(const PassPtr &pass);

  // run a specific pass if it's not valid
  // returns true if changed
  static bool RunPass(PassNameSet &valid, const PassInfoPtr &info);

  // run all passes
  static void RunPasses(const PassPtrList &passes);
  static void RunPasses();

  // getter/setter
  static std::size_t  opt_level() { return _instance._opt_level; }
  static mid::Module &module()    { return *_instance._module; }

  static void SetModule(mid::Module &module) { _instance._module = &module; }
};

template <typename PassClassFactory>
class PassRegisterFactory {
public:
  PassRegisterFactory() {
    auto pass_factory = std::make_shared<PassClassFactory>();
    PassManager::RegisterPassFactory(pass_factory);
  }
};

}

#endif //XY_LANG_PASS_MANAGER_H
