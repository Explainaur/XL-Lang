#include "AST.h"
#include "type.h"
#include <sstream>


namespace RJIT::AST {
Operator string2Operator(const std::string &op, bool isUnary) {
  if (op == "~") {
    return Operator::Not;
  } else if (op == "!") {
    return Operator::LNot;
  } else if (op == "+") {
    return Operator::Add;
  } else if (op == "-") {
    return (isUnary) ? Operator::Neg : Operator::Sub;
  } else if (op == "*") {
    return Operator::Mul;
  } else if (op == "/") {
    return Operator::Div;
  } else if (op == "%") {
    return Operator::Mod;
  } else if (op == "&") {
    return Operator::And;
  } else if (op == "|") {
    return Operator::Or;
  } else if (op == "^") {
    return Operator::Xor;
  } else if (op == ">>") {
    return Operator::Shr;
  } else if (op == "<<") {
    return Operator::Shl;
  } else if (op == "&&") {
    return Operator::LAnd;
  } else if (op == "||") {
    return Operator::LOr;
  } else if (op == "==") {
    return Operator::Equal;
  } else if (op == "!=") {
    return Operator::NotEqual;
  } else if (op == "<") {
    return Operator::Less;
  } else if (op == "<=") {
    return Operator::LessEq;
  } else if (op == ">") {
    return Operator::Great;
  } else if (op == ">=") {
    return Operator::GreatEq;
  } else if (op == "=") {
    return Operator::Assign;
  } else if (op == "+=") {
    return Operator::AssAdd;
  } else if (op == "-=") {
    return Operator::AssSub;
  } else if (op == "*=") {
    return Operator::AssMul;
  } else if (op == "%=") {
    return Operator::AssMod;
  } else if (op == "/=") {
    return Operator::AssDiv;
  } else if (op == "&=") {
    return Operator::AssAnd;
  } else if (op == "|=") {
    return Operator::AssOr;
  } else if (op == "^=") {
    return Operator::AssXor;
  } else if (op == "<<=") {
    return Operator::AssShl;
  } else if (op == ">>=") {
    return Operator::AssShr;
  }
  return Operator::Dam;
}

std::string operator2String(Operator oper) {
  switch (oper) {
    case Operator::Not:
      return "~";
    case Operator::Neg:
      return "-";
    case Operator::LNot:
      return "!";
    case Operator::Add:
      return "+";
    case Operator::Sub:
      return "-";
    case Operator::Mul:
      return "*";
    case Operator::Div:
      return "/";
    case Operator::Mod:
      return "%";
    case Operator::And:
      return "&";
    case Operator::Or:
      return "|";
    case Operator::Xor:
      return "^";
    case Operator::Shl:
      return "<<";
    case Operator::Shr:
      return ">>";
    case Operator::LAnd:
      return "&&";
    case Operator::LOr:
      return "||";
    case Operator::Equal:
      return "==";
    case Operator::NotEqual:
      return "!=";
    case Operator::Less:
      return "<";
    case Operator::LessEq:
      return "<=";
    case Operator::Great:
      return ">";
    case Operator::GreatEq:
      return ">=";
    case Operator::Assign:
      return "=";
    case Operator::AssAdd:
      return "+=";
    case Operator::AssSub:
      return "-=";
    case Operator::AssMul:
      return "*=";
    case Operator::AssDiv:
      return "/=";
    case Operator::AssMod:
      return "%=";
    case Operator::AssAnd:
      return "&=";
    case Operator::AssOr:
      return "|=";
    case Operator::AssXor:
      return "^=";
    case Operator::AssShl:
      return "<<=";
    case Operator::AssShr:
      return ">>=";
    case Operator::Dam:
      return "";
  }
  return "";
}
}

namespace RJIT::TYPE {
Type string2Type(const std::string &type) {
  if (type == "void") {
    return Type::Void;
  } else if (type == "bool") {
    return Type::Bool;
  } else if (type == "int8") {
    return Type::Int8;
  } else if (type == "int" || type == "int32") {
    return Type::Int32;
  } else if (type == "char" || type == "uint8") {
    return Type::UInt8;
  } else if (type == "uint32" || type == "uint") {
    return Type::UInt32;
  } else if (type == "string") {
    return Type::String;
  } else {
    return Type::Dam;
  }
}

std::string type2String(Type type) {
  switch (type) {
    case Type::Void:
      return "void";
    case Type::Bool:
      return "bool";
    case Type::Int8:
      return "char";
    case Type::Int32:
      return "int32";
    case Type::UInt8:
      return "uint8";
    case Type::UInt32:
      return "int";
    case Type::String:
      return "string";
    case Type::Dam:
      return "";
  }
  return "";
}

std::string PrimType::GetTypeId() const {
  return type2String(this->type_);
}

TypeInfoPtr PrimType::GetValueType(bool is_right) const {
  return std::make_shared<PrimType>(type_, is_right);
}

std::size_t PrimType::GetSize() const {
  switch (type_) {
    case Type::Bool:
    case Type::Int8:
    case Type::UInt8:
      return 1;
    case Type::Int32:
    case Type::UInt32:
      return 4;
    default:
      return 0;
  }
}

std::string ConstType::GetTypeId() const {
  return this->type->GetTypeId();
}

TypeInfoPtr ConstType::GetValueType(bool is_right) const {
  auto type_ = type->GetValueType(is_right);
  return std::make_shared<ConstType>(std::move(type_));
}

std::string FuncType::GetTypeId() const {
  std::ostringstream oss;
  oss << '$' << _args.size() << 'f';
  for (const auto &i : _args) oss << i->GetTypeId();
  oss << '$';
  oss << _ret->GetTypeId();
  return oss.str();
}

TypeInfoPtr FuncType::GetValueType(bool is_right) const {
  return std::make_shared<FuncType>(_args, _ret, is_right);
}

TypeInfoPtr FuncType::GetReturnType() const {
  return _ret;
}
bool FuncType::operator==(const TypeInfoPtr &typeInfo) {
  DBG_ASSERT(typeInfo->IsFunction(), "compare with non-function type");

  // compare return type
  auto ret = typeInfo->GetReturnType();

  if (_ret != ret) return false;

  // compare parameters
  auto args = typeInfo->GetArgsType().value();

  // check size first
  if (_args.size() != args.size()) {
    return false;
  }

  for (std::size_t i = 0; i < args.size(); i++) {
    if (args[i] != _args[i]) return false;
  }
  return true;
}

TypeInfoPtr PointerType::GetValueType(bool is_right) const {
  return std::make_shared<PointerType>(_base, is_right);
}

}