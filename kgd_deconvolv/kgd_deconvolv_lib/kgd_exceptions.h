

#ifndef KGD_EXCEPTION_H
#define KGD_EXCEPTION_H


#include <string>  /* string */
#include <vector>
#include <exception>



namespace kellerberrin {    // organization level namespace
namespace deconvolv {          // project level namespace



struct ShouldNotBeCalled : std::exception {
  explicit ShouldNotBeCalled() {}

  virtual ~ShouldNotBeCalled() throw() {}

  virtual const char *what() const noexcept {
    return std::string("Should not reach here").c_str();
  }
};


struct VirtualFunctionShouldNotBeCalled : public ShouldNotBeCalled {
  VirtualFunctionShouldNotBeCalled() : ShouldNotBeCalled() {}

  ~VirtualFunctionShouldNotBeCalled() throw() {}
};


struct InvalidInput : std::exception {
  std::string src;
  std::string reason;
  std::string throwMsg;

  InvalidInput() {
    this->src = "";
    this->reason = "";
  }

  explicit InvalidInput(std::string str) {
    this->src = "\033[1;31m" + str + "\033[0m";
    this->reason = "";
  }

  virtual ~InvalidInput() throw() {}

  virtual const char *what() const noexcept {
    return throwMsg.c_str();
  }
};


struct OutOfVectorSize : std::exception {
  explicit OutOfVectorSize() {}

  virtual ~OutOfVectorSize() throw() {}

  virtual const char *what() const noexcept {
    return std::string("Out of vector size!").c_str();
  }
};


struct InvalidK : public InvalidInput {
  InvalidK() : InvalidInput() {
    this->reason = "k must be at least 2, when using the flag -ibd.";
    throwMsg = this->reason + this->src;
  }

  ~InvalidK() throw() {}
};


struct NotEnoughArg : public InvalidInput {
  NotEnoughArg(std::string str) : InvalidInput(str) {
    this->reason = "Not enough parameters when parsing option: ";
    throwMsg = this->reason + this->src;
  }

  ~NotEnoughArg() throw() {}
};


struct VcfOutUnSpecified : public InvalidInput {
  VcfOutUnSpecified(std::string str) : InvalidInput(str) {
    this->reason = "Missing flag \"-vcfOut\".";
    throwMsg = this->reason + this->src;
  }

  ~VcfOutUnSpecified() throw() {}
};


struct WrongType : public InvalidInput {
  WrongType(std::string str) : InvalidInput(str) {
    this->reason = "Wrong type for parsing: ";
    throwMsg = this->reason + this->src;
  }

  ~WrongType() throw() {}
};


struct BadConversion : public InvalidInput {
  BadConversion(std::string str1, std::string str2) : InvalidInput(str1) {
    this->reason = "Bad conversion: ";
    throwMsg = this->reason + this->src + ", int expected. Check input file" + str2;
  }

  ~BadConversion() throw() {}
};


struct InvalidInputFile : public InvalidInput {
  InvalidInputFile(std::string str) : InvalidInput(str) {
    this->reason = "Invalid input file: ";
    throwMsg = this->reason + this->src;
  }

  ~InvalidInputFile() throw() {}
};


struct FileNameMissing : public InvalidInput {
  FileNameMissing(std::string str) : InvalidInput(str) {
    this->reason = " file path missing!";
    throwMsg = this->src + this->reason;
  }

  ~FileNameMissing() throw() {}
};


struct UnknowArg : public InvalidInput {
  UnknowArg(std::string str) : InvalidInput(str) {
    this->reason = "Unknow option: ";
    throwMsg = this->reason + this->src;
  }

  ~UnknowArg() throw() {}
};


struct FlagsConflict : public InvalidInput {
  FlagsConflict(std::string str1, std::string str2) : InvalidInput(str1) {
    this->reason = "Flag: ";
    throwMsg = this->reason + this->src + std::string(" conflict with flag ") + str2;
  }

  ~FlagsConflict() throw() {}
};


struct OutOfRange : public InvalidInput {
  OutOfRange(std::string str1, std::string str2) : InvalidInput(str1) {
    this->reason = "Flag \"";
    throwMsg = this->reason + this->src + std::string(" ") + str2 + std::string("\" out of range [0, 1].");
  }

  ~OutOfRange() throw() {}
};


struct LociNumberUnequal : public InvalidInput {
  LociNumberUnequal(std::string str) : InvalidInput(str) {
    this->reason = "Number of sites was wrong (compared to ref count) in: ";
    throwMsg = this->reason + this->src;
  }

  ~LociNumberUnequal() throw() {}
};


struct SumOfPropNotOne : public InvalidInput {
  SumOfPropNotOne(std::string str) : InvalidInput(str) {
    this->reason = "Sum of initial proportion is not equal to 1, but equals ";
    throwMsg = this->reason + this->src;
  }

  ~SumOfPropNotOne() throw() {}
};


struct NumOfPropNotMatchNumStrain : public InvalidInput {
  NumOfPropNotMatchNumStrain(std::string str) : InvalidInput(str) {
    this->reason = "Number of initial proportion do not match number of strains!";
    throwMsg = this->reason + this->src;
  }

  ~NumOfPropNotMatchNumStrain() throw() {}
};


struct InitialPropUngiven : public InvalidInput {
  InitialPropUngiven(std::string str) : InvalidInput(str) {
    this->reason = "Initial proportion was not specified.";
    throwMsg = this->reason + this->src;
  }

  ~InitialPropUngiven() throw() {}
};


std::vector<std::vector<int>> enumerateBinaryMatrixOfK(size_t k);
}   // organization level namespace
}   // project level namespace



#endif
