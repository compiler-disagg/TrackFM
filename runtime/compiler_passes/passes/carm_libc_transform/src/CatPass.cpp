#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <algorithm>
#include <ios>
#include <list>
#include <stdlib.h>
#include <string.h>
#include <set>
#include <vector>
#include <iostream>
#include <llvm/Support/raw_ostream.h>
#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Module.h"
#include <unordered_map>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"


#include <algorithm>

#include "noelle/core/Noelle.hpp"
#include "carm_object_config.hpp"

using namespace llvm::noelle ;


#define DEBUG(str) do { errs() << str;} while (0)
#define PRINT_INS(ins) do { ins->print(errs()); errs()<<"\n";} while (0)

namespace {

  static std::string CARM_STRDUP    = "_Z11carm_strdupPc";
  static std::string CARM_MALLOC    = "_Z18carm_remote_mallocm";
  static std::string CARM_DEREF_PTR_LDR     = "_Z17carmDeref_ptr_ldrPv";
  static std::string CARM_DEREF_I8_LDR     = "_Z16carmDeref_i8_ldrPv";
  static std::string CARM_DEREF_I16_LDR     = "_Z17carmDeref_i16_ldrPv";
  static std::string CARM_DEREF_I32_LDR     = "_Z17carmDeref_i32_ldrPv";
  static std::string CARM_DEREF_I64_LDR     = "_Z17carmDeref_i64_ldrPv";
  static std::string CARM_DEREF_PTR_STR     = "_Z17carmDeref_ptr_strPvS_";
  static std::string CARM_DEREF_I8_STR     = "_Z16carmDeref_i8_straPv";
  static std::string CARM_DEREF_I16_STR     = "_Z17carmDeref_i16_strsPv";
  static std::string CARM_DEREF_I32_STR     = "_Z17carmDeref_i32_striPv";
  static std::string CARM_DEREF_I64_STR     = "_Z17carmDeref_i64_strlPv";
  static std::string CARM_ACQUIRE   = "_Z7acquirev";
  static std::string CARM_RELEASE   = "_Z7releasev";
  static std::string CARM_CACHE_ADDR   = "_Z19carm_get_cache_addrv";
  static std::string CARM_CACHE_UB   = "_Z22carm_get_cache_addr_ubv";
  static std::string CARM_CACHE_LB   = "_Z22carm_get_cache_addr_lbv";
  static std::string SET_CARM_CACHE_ADDR   = "_Z19carm_set_cache_addrv";
  static std::string CARM_DEREF_DUMMY             = "_Z10carm_derefPv";
  static std::string CARM_DEREF_LOOP_DUMMY             = "carm_deref_loop";
  static std::string CARM_DEREF_LOOP_WRITE             = "_Z21carm_deref_loop_writem";
  static std::string CARM_DEREF_LOOP_READ              = "_Z20carm_deref_loop_readm";
  static std::string CARM_LOOP_INVALIDATE             = "_Z21invalidate_loop_indexv";
  static std::string CARM_GET_INIT_OBJ_SIZE      = "_Z17carm_get_obj_sizem";
  static std::string CARM_DUMMY1             = "_Z10carm_derefm";
  static std::string CARM_DEREF_CACHE_ADDR     = 	"_Z20get_deref_cache_addrv";
  static std::string CARM_DEREF_MUT_CACHE_ADDR = 	"_Z24get_deref_mut_cache_addrv";
  static std::string CARM_DEREF                   = "_Z10carm_derefPvmm";
  static std::string CARM_DEREF_READ	   = "_Z15carm_deref_readm";
  static std::string CARM_DEREF_WRITE	   = "_Z16carm_deref_writem";
  static std::string CARM_DEREF_BOUNDS_READ	   = "_Z28carm_deref_read_check_boundsm";
  static std::string CARM_DEREF_BOUNDS_WRITE	   = "_Z29carm_deref_write_check_boundsm";
  static std::string CARM_DEREF_READ_NOT_TEMPORAL = "_Z28carm_deref_read_not_temporalm";
  static std::string CARM_DEREF_WRITE_NOT_TEMPORAL = "_Z29carm_deref_write_not_temporalm";
  static std::string CARM_ADD_PREFETCH_TRACE	   = "_Z23carm_add_prefetch_tracem";
  static std::string CARM_MEMCPY                 =  "_Z11carm_memcpyPaS_m";
  static std::string CARM_WMEMCPY                 =  "_Z12carm_wmemcpyPiS_m";
  static std::string CARM_MEMCMP                 =  "_Z11carm_memcmpPvS_m";
  static std::string CARM_STRCMP                 =  "_Z11carm_strcmpPvS_";
  static std::string CARM_WMEMCMP                 =  "_Z12carm_wmemcmpPiS_m";
  static std::string CARM_MEMSET                 =  "_Z11carm_memsetPvim";
  static std::string CARM_CALLOC                 =  "_Z11carm_callocmm";
  static std::string CARM_REALLOC                 =  "_Z12carm_reallocPvm";
  static std::string CARM_STRLEN                 =  "_Z11carm_strlenPv";
  static std::string CARM_MEMCHR                =  "_Z11carm_memchrPvim";
  static std::string CARM_WMEMCHR                =  "_Z12carm_wmemchrPiim";
  static std::string CARM_FWRITE                =  "_Z11carm_fwritePvmmS_";
  static std::string CARM_FREAD                =  "_Z10carm_freadPvmmS_";
  static std::string CARM_DEBUG                =  "_Z10carm_debugm";
  static std::string CARM_STRCPY                =  "_Z11carm_strcpyPvS_";
  static std::string CARM_STRNCPY                =  "_Z12carm_strncpyPvS_m";
  static std::string CARM_VSNPRINTF                =  "_Z14carm_vsnprintfPvmS_P13__va_list_tag";
  static std::string CARM_VSPRINTF                =  "_Z13carm_vsprintfPvS_P13__va_list_tag";
  static std::string CARM_SNPRINTF                =  "_Z13carm_snprintfPvmS_z";
  static std::string CARM_SPRINTF                =  "_Z12carm_sprintfPvS_z";
  static std::string CARM_VFPRINTF                =  "_Z13carm_vfprintfPvS_P13__va_list_tag";
  static std::string CARM_SSCANF                =  "_Z11carm_sscanfPvS_z";
  static std::string CARM_FOPEN                =  "_Z10carm_fopenPvS_";
  static std::string CARM_FCLOSE                =  "_Z11carm_fclosePv";
  static std::string CARM_FREE                =  "_Z9carm_freePv";
  static std::string CARM_LOCAL_MALLOC             = "_Z17carm_local_mallocm";
  static std::string CARM_READ             = "_Z9carm_readiPvl";
  static std::string CARM_STRTOL             = "_Z11carm_strtolPcPS_i";
  static std::string CARM_STRTOUL             = "_Z12carm_strtoulPcPS_i";
  static std::string CARM_FPRINTF = "_Z12carm_fprintfPvS_z";

  struct CAT : public ModulePass {
    static char ID;
    std::unordered_map<Function *, std::vector<Instruction *>> mallocs;
    std::unordered_map<Function *, std::vector<Instruction *>> local_mallocs;
    std::unordered_map<Function *, std::vector<Instruction *>> callocs;
    std::unordered_map<Function *, std::vector<Instruction *>> reallocs;
    std::unordered_map<Function *, std::vector<Instruction *>> frees;
    std::unordered_map<Function *, std::vector<Instruction *>> allocas;
    std::unordered_map<Function *, std::vector<Instruction *>> geps;
    std::unordered_map<Function *, std::vector<Instruction *>> loads;
    std::unordered_map<Function *, std::vector<Instruction *>> stores;
    std::unordered_map<Function *, std::vector<Instruction *>> memcpy;
    std::unordered_map<Function *, std::vector<Instruction *>> wmemcpy;
    std::unordered_map<Function *, std::vector<Instruction *>> llvmi_memcpy;
    std::unordered_map<Function *, std::vector<Instruction *>> llvmi_memove;
    std::unordered_map<Function *, std::vector<Instruction *>> memcmp;
    std::unordered_map<Function *, std::vector<Instruction *>> wmemcmp;
    std::unordered_map<Function *, std::vector<Instruction *>> memset;
    std::unordered_map<Function *, std::vector<Instruction *>> strcmp;
    std::unordered_map<Function *, std::vector<Instruction *>> strlen;
    std::unordered_map<Function *, std::vector<Instruction *>> llvmi_memset;
    std::unordered_map<Function *, std::vector<Instruction *>> instructions;
    std::unordered_map<Function *, std::set<Instruction *>> carm_functions;
    std::unordered_map<Function *, std::vector<Instruction *>> carm_loop_functions;
    std::unordered_map<Function *, std::vector<Instruction *>> memchr;
    std::unordered_map<Function *, std::vector<Instruction *>> wmemchr;
    std::unordered_map<Function *, std::vector<Instruction *>> fwrites;
    std::unordered_map<Function *, std::vector<Instruction *>> fprintfs;
    std::unordered_map<Function *, std::vector<Instruction *>> freads;
    std::unordered_map<Instruction *, Instruction *> carm_functions_map;
    std::unordered_map<Instruction *, Instruction *> carm_loop_gep_map;
    std::unordered_map<Function *, std::vector<Instruction *>> strcpys;
    std::unordered_map<Function *, std::vector<Instruction *>> strncpys;
    std::unordered_map<Function *, std::vector<Instruction *>> snprintf;
    std::unordered_map<Function *, std::vector<Instruction *>> sprintf;
    std::unordered_map<Function *, std::vector<Instruction *>> vsnprintf;
    std::unordered_map<Function *, std::vector<Instruction *>> vsprintf;
    std::unordered_map<Function *, std::vector<Instruction *>> vfprintf;
    std::unordered_map<Function *, std::vector<Instruction *>> sscanf;
    std::unordered_map<Function *, std::vector<Instruction *>> fopen;
    std::unordered_map<Function *, std::vector<Instruction *>> fclose;
    std::unordered_map<Function *, std::vector<Instruction *>> strdups;
    std::unordered_map<Function *, std::vector<Instruction *>> reads;
    std::unordered_map<Function *, std::vector<Instruction *>> strtouls;
    std::unordered_map<Function *, std::vector<Instruction *>> strtols;

    Module * currentModule;
    Type * VoidPointerType;
    Type * VoidPointer8Type;
    Type * VoidPointer32Type;
    Type * VoidPointer64Type;
    Type * Int8Type;
    Type * Int16Type;
    Type * Int32Type;
    Type * Int64Type;
    Type * Int128Type;
    Type * Int1Type;
    Type * VoidType;
    std::unordered_map<Type *, std::string> cmap;
    CAT() : ModulePass(ID) {}
    bool doInitialization (Module &M) override {
      this->currentModule = &M;
      LLVMContext &TheContext = currentModule->getContext();
      this->VoidPointerType = Type::getInt64PtrTy(TheContext, 0);
      this->VoidPointer8Type = Type::getInt8PtrTy(TheContext, 0);
      this->VoidPointer32Type = Type::getInt32PtrTy(TheContext, 0);
      this->Int8Type = Type::getInt8Ty(TheContext);
      this->Int16Type = Type::getInt16Ty(TheContext);
      this->Int32Type = Type::getInt32Ty(TheContext);
      this->Int64Type = Type::getInt64Ty(TheContext);
      this->Int128Type = Type::getInt128Ty(TheContext);
      this->Int1Type = Type::getInt1Ty(TheContext);
      this->VoidType = Type::getVoidTy(TheContext);
      Value * obj_state = currentModule->getOrInsertGlobal("carm_obj_state",VoidPointerType);
      return true;
    }

    void init_fmap(std::unordered_map<std::string, int>& fmap) {

      std::string func[] = {
        "Not supported",
        "malloc",
        "mmap",
        "_Znwm",        // c++ new
        "_Znam",       // c++ new []
        "_Znaj",       // c++ new
        "_Znwj",      // c++ new []
        "free",
        "_ZdlPv",      // c++ delete 
        CARM_DEREF_DUMMY,
        "memcpy",
        "memmove",
        "memcmp",
        "llvm.memcpy.p0i8.p0i8.i64",
        "memset",
        "llvm.memmove.p0i8.p0i8.i64",
        "llvm.memset.p0i8.i64",
        "calloc",
        "realloc",
        "wmemcpy",
        "wmemcmp",
        "wmemmove",
        "strcmp",
        "strlen",
        "memchr",
        "wmemchr",
        "fwrite",
        "fread",
        "strcpy",
        "strncpy",
        "snprintf",
        "sprintf",
        "vsnprintf",
        "vsprintf",
        "_ZdaPv",
        "sscanf",
        "fopen",
        "vfprintf",
        "fclose",
        CARM_DEREF_LOOP_DUMMY, 
        "strdup",
        "llvm.var.annotation",
	"read",
	"strtol",
	"strtoul",
	"fprintf"
      };

      int size = 46;
      //build func map
      for (int val = 0; val < size; val++) {
        fmap[func[val]] = val;
      }
    }

    void print_vector(std::vector<Instruction *> ins, std::string vector_type) {
      DEBUG("\n-----"+vector_type+"-----\n");
      for (auto i:ins)
      {
        PRINT_INS(i);
      }
      DEBUG("\n----------\n");
    }

    void print_uses(std::vector<Instruction *> ins, std::string vector_type) {
      DEBUG("\n-----"+vector_type +" USES -----\n");
      for (auto i:ins)
      {
        Value * v = dynamic_cast<Value *>(i);
        for(auto U : v->users()){  // U is of type Use*
          if (auto I = dyn_cast<Instruction>(U)){
            PRINT_INS(I);
            // an instruction uses V
          }
        }
      }
    }
    Value * uncast_ungep(Value * ptr, std::set<Value *> phiset, std::set<Value *>& valueset) {
      auto gepi = dyn_cast<GetElementPtrInst>(ptr);
      auto ci = dyn_cast<CastInst>(ptr);
      auto phiinst = dyn_cast<PHINode>(ptr);
      if (gepi) {
        return gepi;
      }
      else if (ci) {
        BitCastInst *BCI = dyn_cast<BitCastInst>(ci); 
        if (BCI) 
          return uncast_ungep(BCI->getOperand(0), phiset, valueset); 
        /* * Attempt to fetch the operand as a bitcast operator */ 
        BitCastOperator *BCO = dyn_cast<BitCastOperator>(ci); 
        if (BCO) 
          return uncast_ungep(BCO->getOperand(0), phiset, valueset);
      }
#if 1
      else if (phiinst) {
        if (phiset.find(ptr) == phiset.end()) {
          phiset.insert(ptr);
          for (unsigned j = 0 ; j < phiinst->getNumIncomingValues() ; j++) {
            Value * p = uncast_ungep(phiinst->getIncomingValue(j), phiset, valueset);
            valueset.insert(p);
            return p;
          }
        }
      }
#endif

      return ptr;
    }
    bool check_if_gep(std::set<Value *> valueset) {
      for (auto v:valueset) {
        auto gepi = dyn_cast<GetElementPtrInst>(v);

        if (gepi) {
          errs()<<"nested gepi \m";
          errs()<<*gepi;
          return true;
        }
      }
      return false;
    }
    bool check_uses_is_gep(Instruction * ins) {
      Value * v = dynamic_cast<Value *>(ins);
      for(auto U : v->users()){  // U is of type Use*
        if (auto I = dyn_cast<Instruction>(U)){
          std::set<Value *> phiset;
          std::set<Value *> valueset;
          Value * v = uncast_ungep(U, phiset, valueset);
          valueset.insert(v);
          return check_if_gep(valueset);
        }
      }
      return false;
    }

    void print_lib_functions(Function * F) {
      print_vector(mallocs[F], "Malloc");
      print_vector(frees[F], "free");
      print_vector(geps[F], "GEP");
      print_vector(allocas[F], "Alloca");
    }


    Function * get_or_insert_function(std::string carm_function, FunctionType * signature) {
      auto nnew_set_function = currentModule->getOrInsertFunction(carm_function, signature);
      if (!nnew_set_function) 
        errs()<<"Function Null\n";
      return currentModule->getFunction(carm_function);
    }
    Value * _create_function(std::string carm_function, FunctionType * signature, Instruction * insertion_point,
        std::vector<Value *> arg) {
      IRBuilder<> builder(insertion_point);
      Function * f =  get_or_insert_function(carm_function, signature);
      Value * newfunc = builder.CreateCall(f, arg);
      return newfunc;
    }
    Value * _create_function(std::string carm_function, FunctionType * signature, Instruction * insertion_point) {
      IRBuilder<> builder(insertion_point);
      Function * f =  get_or_insert_function(carm_function, signature);
      Value * newfunc = builder.CreateCall(f);
      return newfunc;
    }
    Value * create_function(std::string carm_function, Type * return_type, 
        Instruction * insertion_point) {
      FunctionType* signature = FunctionType::get(return_type, false);
      return _create_function(carm_function, signature, insertion_point);
    }

    Value * create_function(Type* func_args[], std::string carm_function, std::vector<Value *> arg, 
        Type * return_type, Instruction * insertion_point) {

      FunctionType* signature = FunctionType::get(return_type,
          ArrayRef<Type*>(func_args, arg.size()),
          false);
      return _create_function(carm_function, signature, insertion_point, arg);
    }


    Value * create_function(Type* func_args[], std::string carm_function, std::vector<Value *> arg, 
        Type * return_type, Instruction * insertion_point, bool varg) {

      FunctionType* signature = FunctionType::get(return_type,
          ArrayRef<Type*>(func_args, arg.size()),
          varg);
      return _create_function(carm_function, signature, insertion_point, arg);
    }
    Value * create_function(std::vector<Type * > func_args, std::string carm_function, std::vector<Value *> arg, 
        Type * return_type, Instruction * insertion_point, bool varg) {

      FunctionType* signature = FunctionType::get(return_type,
          ArrayRef<Type*>(func_args),
          varg);
      return _create_function(carm_function, signature, insertion_point, arg);
    }
    Instruction * get_first_instruction(Module & M) {

      for (auto &F:M) {
        if (!F.empty())
        {
          errs()<<F.getName().str();
          if (F.getName().str().compare("_Z7do_workPv") == 0) {
            for (auto &BB :F) {
              for (Instruction &ins : BB) {
                return &ins;
              }
            }
          }
        }
      }
      DEBUG("ERROR all functions empty\n");
      return nullptr;
    }

    void parse_lib_functions(Function &F) {
      std::unordered_map<std::string, int> fmap;
      init_fmap(fmap);
      bool local_malloc = false;
      for (BasicBlock &BB : F) {
        for (Instruction &ins : BB) {
          instructions[&F].push_back(&ins);
          if (isa<AllocaInst>(&ins)) {
            allocas[&F].push_back(&ins);
          }
          else if (isa<GetElementPtrInst>(&ins)) {
            geps[&F].push_back(&ins);
          }
          else if (isa<LoadInst>(&ins)) {
            loads[&F].push_back(&ins);
          }
          else if (isa<StoreInst>(&ins)) {
            stores[&F].push_back(&ins);
          }
          else if (isa<CallInst>(&ins) || isa<InvokeInst>(&ins)) {
            Function * fp;
            if (CallInst * ci = dyn_cast<CallInst>(&ins)) {
              fp = ci->getCalledFunction();
            }
            if (InvokeInst * ci = dyn_cast<InvokeInst>(&ins)) {
              fp = ci->getCalledFunction();
            }
            if (fp != nullptr) {
              if (fp->empty()) {
                StringRef funcName = fp->getName();

                int32_t val = fmap[funcName.str()];

                switch (val) {

                  case 1:
                  case 2:
                    {
                      if (local_malloc) {
                        local_mallocs[&F].push_back(&ins);
                        local_malloc = false;
                      }
                      else
                      mallocs[&F].push_back(&ins);
                      break;
                    }	
#if 0 
                    //not needed to handle new as libc++ abi is instrumented
                  case 3:
                  case 4:
                  case 5:
                  case 6:
#endif
                  case 7:
                    frees[&F].push_back(&ins);
                    break;
#if 0 
                    //not needed to handle delete as libc++ abi is instrumented
                  case 8:
                    frees[&F].push_back(&ins);
                    break;
#endif
                  case 9:
                    {
                      carm_functions[&F].insert(&ins);
                      carm_functions_map[&ins] = ins.getNextNonDebugInstruction();
                      break;
                    }
                  case 10:
                  case 11:
                    memcpy[&F].push_back(&ins);
                    break;
                  case 12:
                    memcmp[&F].push_back(&ins);
                    break;
                  case 13:
                    llvmi_memcpy[&F].push_back(&ins);
                    break;
                  case 14:
                    memset[&F].push_back(&ins);
                    break;
                  case 15:
                    llvmi_memove[&F].push_back(&ins);
                    break;
                  case 16:
                    llvmi_memset[&F].push_back(&ins);
                    break;
                  case 17:
                    callocs[&F].push_back(&ins);
                    break;
                  case 18:
                    reallocs[&F].push_back(&ins);
                    break;
                  case 19:
                    wmemcpy[&F].push_back(&ins);
                    break;
                  case 20:
                    wmemcmp[&F].push_back(&ins);
                    break;
                  case 21:
                    wmemcpy[&F].push_back(&ins);
                    break;
                  case 22:
                    strcmp[&F].push_back(&ins);
                    break;
                  case 23:
                    strlen[&F].push_back(&ins);
                    break;
                  case 24:
                    memchr[&F].push_back(&ins);
                    break;
                  case 25:
                    wmemchr[&F].push_back(&ins);
                    break;
                  case 26:
                    fwrites[&F].push_back(&ins);
                    break;
                  case 27:
                    freads[&F].push_back(&ins);
                    break;
                  case 28:
                    strcpys[&F].push_back(&ins);
                    break;
                  case 29:
                    strncpys[&F].push_back(&ins);
                    break;
                  case 30:
                    snprintf[&F].push_back(&ins);
                    break;
                  case 31:
                    sprintf[&F].push_back(&ins);
                    break;
                  case 32:
                    vsnprintf[&F].push_back(&ins);
                    break;
                  case 33:
                    vsprintf[&F].push_back(&ins);
                    break;
#if 0 
                    //not needed to handle delete as libc++ abi is instrumented
                  case 34:
                    frees[&F].push_back(&ins);
                    break;
#endif
                  case 35:
                    sscanf[&F].push_back(&ins);
                    break;
                  case 36:
                    fopen[&F].push_back(&ins);
                    break;
                  case 37:
                    vfprintf[&F].push_back(&ins);
                    break;
                  case 38:
                    fclose[&F].push_back(&ins);
                    break;
                  case 39:
                    carm_loop_functions[&F].push_back(&ins);
                    carm_loop_gep_map[&ins] = ins.getNextNonDebugInstruction();
                    break;
                  case 40:
                    strdups[&F].push_back(&ins);
                    break;
                  case 41:
                    {
                      if (CallInst * ci1 = dyn_cast<CallInst>(&ins)) {
                        auto gepi = dyn_cast<GetElementPtrInst>(ins.getOperand(1));
                        if (gepi) {

                          Value *gep = gepi->getOperand(0);

                          auto a4 = dyn_cast<GlobalVariable>(gep);
                          if (a4) {
                            auto a5 = dyn_cast<ConstantDataArray>(a4->getInitializer());
                            if (a5) {
                              auto a6 = a5->getAsCString();

                              if(a6.str().find(LOCAL_MALLOC) != std::string::npos)
                                local_malloc = true;
                              break;
                            }

                          }
                        }
                      }
                      break;
                    }
		  case 42:
		    reads[&F].push_back(&ins);
		    break;
		  case 43:
		    strtols[&F].push_back(&ins);
		    break;
		  case 44:
		    strtouls[&F].push_back(&ins);
		    break;
                  case 45:
                    fprintfs[&F].push_back(&ins);
                    break;
                  default:
#if 0
                    DEBUG("\n##########\n");
                    PRINT_INS((&ins));
                    DEBUG("\nNot supported yet!! Key " + funcName +" VAl "+std::to_string(val));
                    DEBUG("\n##########\n");
#endif
                    break;
                }

              }
            }
          }
        }
      }
    }

    void delete_instructions(std::vector<Instruction *> & ins) {
      for (auto l:ins) {
        l->eraseFromParent();
      }
    }
    void delete_instructions(std::set<Instruction *> & ins) {
      for (auto l:ins) {
        l->eraseFromParent();
      }
    }
    void create_debug_call(Value * ins, Instruction * inst) {
      std::vector<Value *> args;
      Type* func_args[] = {Int64Type};
      args.push_back(ins);
      create_function(func_args, CARM_DEBUG, args, VoidType, inst);
    }
    Value *  must_alias(Value * ptr, std::set<Value *> ptrs_instrumented) {
      AliasAnalysis &aliasAnalysis = getAnalysis<AAResultsWrapperPass>().getAAResults();
      for (auto s:ptrs_instrumented) { 
        if (aliasAnalysis.isMustAlias(ptr, s))
        {
          return s;
        }
      }
      return nullptr;
    }


    uint64_t total_guard3_checks_needed = 1;
    void transform_memset_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int32Type, Int64Type};
      for (auto ins:memset[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int32Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_MEMSET, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(memset[F]);
    }
    void transform_fwrite_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int64Type, Int64Type, VoidPointer8Type};
      for (auto ins:fwrites[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        Value * a4 = builder.CreateBitCast (ins->getOperand(3), VoidPointer8Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        args.push_back(a4);
        Value * newfunc = create_function(func_args, CARM_FWRITE, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(fwrites[F]);
    }
    void transform_fread_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int64Type, Int64Type, VoidPointer8Type};
      for (auto ins:freads[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        Value * a4 = builder.CreateBitCast (ins->getOperand(3), VoidPointer8Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        args.push_back(a4);
        Value * newfunc = create_function(func_args, CARM_FREAD, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(freads[F]);
    }
    void transform_read_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {Int32Type, VoidPointer8Type, Int64Type};
      for (auto ins:reads[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), Int32Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_READ, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(reads[F]);
    }
    void transform_strtol_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int32Type};
      for (auto ins:strtols[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int32Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_STRTOL, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strtols[F]);
    }
    void transform_strtoul_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int32Type};
      for (auto ins:strtouls[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int32Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_STRTOUL, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strtouls[F]);
    }
    void transform_fclose_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type};
      for (auto ins:fclose[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        args.push_back(a1);
        Value * newfunc = create_function(func_args, CARM_FCLOSE, args, Int32Type, ins);
        Value * newfunc1 =  builder.CreateBitCast(newfunc, ins->getType());
        ins->replaceAllUsesWith(newfunc1);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(ins)) {
          builder.CreateBr(OrigInvoke->getNormalDest());

        }
        args.clear();
      }
      delete_instructions(fclose[F]);
    }
    void transform_fopen_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type};
      for (auto ins:fopen[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        args.push_back(a1);
        args.push_back(a2);
        Value * newfunc = create_function(func_args, CARM_FOPEN, args, VoidPointer8Type, ins);
        Value * newfunc1 =  builder.CreateBitCast(newfunc, ins->getType());
        ins->replaceAllUsesWith(newfunc1);
        args.clear();
      }
      delete_instructions(fopen[F]);
    }
    void transform_llvmi_memset_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int32Type, Int64Type};
      for (auto ins:llvmi_memset[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateIntCast (ins->getOperand(1), Int32Type, false);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        create_function(func_args, CARM_MEMSET, args, VoidPointer8Type, ins);
        args.clear();
      }
      delete_instructions(llvmi_memset[F]);
    }
    void transform_memchr_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int32Type, Int64Type};
      for (auto ins:memchr[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int32Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_MEMCHR, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(memchr[F]);
    }
    void transform_wmemchr_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer32Type, Int32Type, Int64Type};
      for (auto ins:wmemchr[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer32Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int32Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_WMEMCHR, args, VoidPointer32Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(wmemchr[F]);
    }
    void transform_malloc_functions(Function * F) {
      std::vector<Value *> args;
      Type* func_args[] = {Int64Type};
      for (auto call:mallocs[F])
      {
        args.push_back(call->getOperand(0));
        Value * newfunc = create_function(func_args, CARM_MALLOC, args, call->getType(), call);
        call->replaceAllUsesWith(newfunc);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(call)) {
          PRINT_INS(call);
          IRBuilder<> Builder(static_cast<Instruction *> (call));
          Builder.CreateBr(OrigInvoke->getNormalDest());

        }
        args.clear();
      }
      delete_instructions(mallocs[F]);
    }
    void transform_local_malloc_functions(Function * F) {
      std::vector<Value *> args;
      Type* func_args[] = {Int64Type};
      for (auto call:local_mallocs[F])
      {
        args.push_back(call->getOperand(0));
        Value * newfunc = create_function(func_args, CARM_LOCAL_MALLOC, args, call->getType(), call);
        call->replaceAllUsesWith(newfunc);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(call)) {
          PRINT_INS(call);
          IRBuilder<> Builder(static_cast<Instruction *> (call));
          Builder.CreateBr(OrigInvoke->getNormalDest());

        }
        args.clear();
      }
      delete_instructions(local_mallocs[F]);
    }
    void transform_free_functions(Function * F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type};
      for (auto call:frees[F])
      {
        args.push_back(call->getOperand(0));
        Value * newfunc = create_function(func_args, CARM_FREE, args, call->getType(), call);
        call->replaceAllUsesWith(newfunc);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(call)) {
          PRINT_INS(call);
          IRBuilder<> Builder(static_cast<Instruction *> (call));
          Builder.CreateBr(OrigInvoke->getNormalDest());
        }
        args.clear();
      }
      delete_instructions(frees[F]);
    }
    void transform_strdup_functions(Function * F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type};
      for (auto call:strdups[F])
      {
        args.push_back(call->getOperand(0));
        Value * newfunc = create_function(func_args, CARM_STRDUP, args, call->getType(), call);
        call->replaceAllUsesWith(newfunc);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(call)) {
          PRINT_INS(call);
          IRBuilder<> Builder(static_cast<Instruction *> (call));
          Builder.CreateBr(OrigInvoke->getNormalDest());
        }
        args.clear();
      }
      delete_instructions(strdups[F]);
    }
    void transform_memcpy_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int64Type};
      for (auto ins:memcpy[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_MEMCPY, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(memcpy[F]);
    }
    void transform_wmemcpy_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer32Type, VoidPointer32Type, Int64Type};
      for (auto ins:wmemcpy[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer32Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer32Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_WMEMCPY, args, VoidPointer32Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(wmemcpy[F]);
    }
    void transform_strcpy_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type};
      for (auto ins:strcpys[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        args.push_back(a1);
        args.push_back(a2);
        Value * newfunc = create_function(func_args, CARM_STRCPY, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strcpys[F]);
    }
    void transform_strncpy_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int64Type};
      for (auto ins:strncpys[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_STRNCPY, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strncpys[F]);
    }
    void transform_snprintf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(Int64Type);
      func_args.push_back(VoidPointer8Type);
      for (auto ins:snprintf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateIntToPtr (ins->getOperand(0), ins->getOperand(0)->getType());
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        Value * a3 = builder.CreateIntToPtr (ins->getOperand(2), ins->getOperand(2)->getType());
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        for(unsigned i = 3 ; i < ins->getNumOperands() - 1; i++) {
          //func_args.push_back(ins->getOperand(i)->getType());
          args.push_back(ins->getOperand(i));
        }
        Value * newfunc = create_function(func_args, CARM_SNPRINTF, args, Int32Type, ins, true);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(snprintf[F]);
    }
    void transform_sprintf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(VoidPointer8Type);
      for (auto ins:sprintf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateIntToPtr (ins->getOperand(0), ins->getOperand(0)->getType());
        Value * a2 = builder.CreateIntToPtr (ins->getOperand(1), ins->getOperand(1)->getType());
        args.push_back(a1);
        args.push_back(a2);
        for(unsigned i = 2 ; i < ins->getNumOperands() - 1; i++) {
          //func_args.push_back(ins->getOperand(i)->getType());
          args.push_back(ins->getOperand(i));
        }
        Value * newfunc = create_function(func_args, CARM_SPRINTF, args, Int32Type, ins, true);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(sprintf[F]);
    }
    void transform_vsnprintf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(Int64Type);
      func_args.push_back(VoidPointer8Type);
      bool first_ins = true;
      for (auto ins:vsnprintf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateIntToPtr (ins->getOperand(0), ins->getOperand(0)->getType());
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        Value * a3 = builder.CreateIntToPtr (ins->getOperand(2), ins->getOperand(2)->getType());
        if (first_ins) {
          func_args.push_back(ins->getOperand(3)->getType());
          first_ins = false;
        }
        //Value * a4 = builder.CreateIntToPtr (ins->getOperand(3), ins->getOperand(3)->getType());
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        args.push_back(ins->getOperand(3));
        Value * newfunc = create_function(func_args, CARM_VSNPRINTF, args, Int32Type, ins, false);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(vsnprintf[F]);
    }
    void transform_vsprintf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(VoidPointer8Type);
      bool first_ins = true;
      for (auto ins:vsprintf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateIntToPtr (ins->getOperand(0), ins->getOperand(0)->getType());
        Value * a2 = builder.CreateIntToPtr (ins->getOperand(1), ins->getOperand(1)->getType());
        //Value * a3 = builder.CreateIntToPtr (ins->getOperand(2), ins->getOperand(2)->getType());
        if (first_ins) {
          func_args.push_back(ins->getOperand(2)->getType());
          first_ins = false;
        }
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(ins->getOperand(2));
        Value * newfunc = create_function(func_args, CARM_VSPRINTF, args, Int32Type, ins, false);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(vsprintf[F]);
    }
    void transform_vfprintf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(VoidPointer8Type);
      bool first_ins = true;
      for (auto ins:vfprintf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateIntToPtr (ins->getOperand(1), ins->getOperand(1)->getType());
        //Value * a3 = builder.CreateIntToPtr (ins->getOperand(2), ins->getOperand(2)->getType());
        if (first_ins) {
          func_args.push_back(ins->getOperand(2)->getType());
          first_ins = false;
        }
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(ins->getOperand(2));
        Value * newfunc = create_function(func_args, CARM_VFPRINTF, args, Int32Type, ins, false);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(vfprintf[F]);
    }
    void transform_sscanf_functions(Function *F) {
      std::vector<Value *> args;
      std::vector<Type*> func_args;
      func_args.push_back(VoidPointer8Type);
      func_args.push_back(VoidPointer8Type);
      for (auto ins:sscanf[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateIntToPtr (ins->getOperand(0), ins->getOperand(0)->getType());
        Value * a2 = builder.CreateIntToPtr (ins->getOperand(1), ins->getOperand(1)->getType());
        args.push_back(a1);
        args.push_back(a2);
        for(unsigned i = 2 ; i < ins->getNumOperands() - 1; i++) {
          args.push_back(ins->getOperand(i));
        }
        Value * newfunc = create_function(func_args, CARM_SSCANF, args, Int32Type, ins, true);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(sscanf[F]);
    }
    void transform_calloc_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {Int64Type, Int64Type};
      for (auto ins:callocs[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), Int64Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        Value * newfunc = create_function(func_args, CARM_CALLOC, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(callocs[F]);
    }
    void transform_realloc_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, Int64Type};
      for (auto ins:reallocs[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        Value * newfunc = create_function(func_args, CARM_REALLOC, args, VoidPointer8Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(reallocs[F]);
    }
    void transform_llvm_memcpy_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int64Type};
      for (auto ins:llvmi_memcpy[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        create_function(func_args, CARM_MEMCPY, args, VoidPointer8Type, ins);
        //ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(llvmi_memcpy[F]);
    }
    void transform_llvm_memove_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int64Type};
      for (auto ins:llvmi_memove[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        create_function(func_args, CARM_MEMCPY, args, VoidPointer8Type, ins);
        //ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(llvmi_memove[F]);
    }
    void transform_strcmp_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer32Type, VoidPointer32Type};
      for (auto ins:strcmp[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer32Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer32Type);
        args.push_back(a1);
        args.push_back(a2);
        Value * newfunc = create_function(func_args, CARM_STRCMP, args, Int32Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strcmp[F]);
    }
    void transform_strlen_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer32Type};
      for (auto ins:strlen[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer32Type);
        args.push_back(a1);
        Value * newfunc = create_function(func_args, CARM_STRLEN, args, Int64Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(strlen[F]);
    }

    void transform_fprintf_functions(Function *F) {
	    std::vector<Value *> args;
	    std::vector<Type*> func_args;
	    func_args.push_back(VoidPointer8Type);
	    func_args.push_back(VoidPointer8Type);
	    for (auto ins:fprintfs[F]) {
		    IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
		    Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
		    args.push_back(a1);
		    args.push_back(a2);
		    for(unsigned i = 2 ; i < ins->getNumOperands() - 1; i++) {
			    //func_args.push_back(ins->getOperand(i)->getType());
			    args.push_back(ins->getOperand(i));
		    }
		    Value * newfunc = create_function(func_args, CARM_FPRINTF, args, Int32Type, ins, true);
		    ins->replaceAllUsesWith(newfunc);
        if (auto *OrigInvoke = dyn_cast<InvokeInst>(ins)) {
          builder.CreateBr(OrigInvoke->getNormalDest());

        }
		    args.clear();
	    }
	    delete_instructions(fprintfs[F]);
    }

    void transform_wmemcmp_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer32Type, VoidPointer32Type, Int64Type};
      for (auto ins:wmemcmp[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer32Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer32Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_WMEMCMP, args, Int32Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(wmemcmp[F]);
    }
    void transform_memcmp_functions(Function *F) {
      std::vector<Value *> args;
      Type* func_args[] = {VoidPointer8Type, VoidPointer8Type, Int64Type};
      for (auto ins:memcmp[F]) {
        IRBuilder<> builder(ins);
        Value * a1 = builder.CreateBitCast (ins->getOperand(0), VoidPointer8Type);
        Value * a2 = builder.CreateBitCast (ins->getOperand(1), VoidPointer8Type);
        Value * a3 = builder.CreateBitCast (ins->getOperand(2), Int64Type);
        args.push_back(a1);
        args.push_back(a2);
        args.push_back(a3);
        Value * newfunc = create_function(func_args, CARM_MEMCMP, args, Int32Type, ins);
        ins->replaceAllUsesWith(newfunc);
        args.clear();
      }
      delete_instructions(memcmp[F]);
    }

    bool runOnFunction(Function &F) {
      //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
      //of globals.
      //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
      parse_lib_functions(F);
      //errs()<<"Function "<<F.getName().str()<<"\n";
      transform_malloc_functions(&F);
      transform_local_malloc_functions(&F);
      transform_free_functions(&F);
      transform_memcpy_functions(&F);
      transform_memchr_functions(&F);
      transform_wmemchr_functions(&F);
      transform_wmemcpy_functions(&F);
      transform_memcmp_functions(&F);
      transform_wmemcmp_functions(&F);
      transform_strcmp_functions(&F);
      transform_strlen_functions(&F);
      transform_llvm_memcpy_functions(&F);
      transform_llvm_memove_functions(&F);
      transform_memset_functions(&F);
      transform_fwrite_functions(&F);
      transform_fread_functions(&F);
      transform_read_functions(&F);
      transform_fopen_functions(&F);
      transform_fprintf_functions(&F);
      transform_fclose_functions(&F);
      transform_llvmi_memset_functions(&F);
      transform_calloc_functions(&F);
      transform_realloc_functions(&F);
      transform_strcpy_functions(&F);
      transform_strncpy_functions(&F);
      transform_snprintf_functions(&F);
      transform_sprintf_functions(&F);
      transform_vsnprintf_functions(&F);
      transform_vsprintf_functions(&F);
      transform_vfprintf_functions(&F);
      transform_sscanf_functions(&F);
      transform_strdup_functions(&F);
      transform_strtol_functions(&F);
      transform_strtoul_functions(&F);
      return true;
    }

    bool runOnModule(Module &M) override {

      for (auto &F:M) {
        carm_functions_map.clear();
        carm_loop_gep_map.clear();
        if (F.empty() || !(F.getInstructionCount()) || (F.isIntrinsic())) {
          continue;
        }
        //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
        //of globals.
        //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
        if (F.getName().str().find("cxx_global_var_init") == std::string::npos) {

          runOnFunction(F);
        }

      }
      return true;
    }
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      //errs() << "Hello LLVM World at \"getAnalysisUsage\"\n" ;
      AU.addRequired<Noelle>();
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<ScalarEvolutionWrapperPass>();
      AU.addRequired< AAResultsWrapperPass >();
      AU.addRequired< CallGraphWrapperPass>();
      AU.addRequired< DominatorTreeWrapperPass>();
      AU.setPreservesAll();
    }
  };

}  // end of anonymous namespace


// Next there is code to register your pass to "opt"
char CAT::ID = 0;
static RegisterPass<CAT> X("CAT", "Simple user of the Noelle framework");

// Next there is code to register your pass to "clang"
static CAT * _PassMaker = NULL;
static RegisterStandardPasses _RegPass1(PassManagerBuilder::EP_OptimizerLast,
    [](const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    if(!_PassMaker){ PM.add(_PassMaker = new CAT());}}); // ** for -Ox
static RegisterStandardPasses _RegPass2(PassManagerBuilder::EP_EnabledOnOptLevel0,
    [](const PassManagerBuilder&, legacy::PassManagerBase& PM) {
    if(!_PassMaker){ PM.add(_PassMaker = new CAT()); }}); // ** for -O0
