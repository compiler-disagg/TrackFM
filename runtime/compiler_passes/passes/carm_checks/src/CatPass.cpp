#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/ADT/SmallBitVector.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Use.h"
#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
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
#include "llvm/IR/Dominators.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/AliasSetTracker.h"
#include "llvm/Transforms/Utils/LoopSimplify.h"
#include "llvm/IR/GetElementPtrTypeIterator.h"
#include "llvm/IR/DerivedTypes.h"


#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <algorithm>

#include "noelle/core/Noelle.hpp"

using namespace llvm::noelle ;


#define DEBUG(str) do { errs() << str;} while (0)
#define PRINT_INS(ins) do { ins->print(errs()); errs()<<"\n";} while (0)

namespace {

  static std::string CARM_DEREF_DUMMY             = "_Z10carm_derefPv";
  static std::string CARM_DEREF_LOOP_DUMMY             = "carm_deref_loop";
  static std::string CARM_ACQUIRE   = "_Z7acquirev";
  static std::string CARM_RELEASE   = "_Z7releasev";
  struct CAT : public ModulePass {
    static char ID;
    std::unordered_map<Function *, std::set<Instruction *>> allocas;
    std::unordered_map<Function *, std::set<Value *>> arguments;
    std::unordered_map<Function *, std::vector<Instruction *>> geps;
    std::unordered_map<Function *, std::vector<Instruction *>> loads;
    std::unordered_map<Function *, std::vector<Instruction *>> stores;
    std::unordered_map<Function *, std::vector<Instruction *>> call_instructions;
    std::unordered_map<Function *, std::set<Value *>> carm_checks;
    std::unordered_map<Value *, Value *> carm_check_get_pointer_operand;

    Module * currentModule;
    Type * VoidPointerType;
    Type * Int8Type;
    Type * Int16Type;
    Type * Int32Type;
    Type * Int64Type;
    Type * VoidType;
    std::unordered_map<Type *, std::string> cmap;
    CAT() : ModulePass(ID) {}
    bool doInitialization (Module &M) override {
      this->currentModule = &M;
      LLVMContext &TheContext = currentModule->getContext();
      this->VoidPointerType = Type::getInt8PtrTy(TheContext, 0);
      this->Int8Type = Type::getInt8Ty(TheContext);
      this->Int16Type = Type::getInt16Ty(TheContext);
      this->Int32Type = Type::getInt32Ty(TheContext);
      this->Int64Type = Type::getInt64Ty(TheContext);
      this->VoidType = Type::getVoidTy(TheContext);
      return false;
    }


    void print(std::vector<Instruction *> ins, std::string vector_type) {
      DEBUG("\n-----"+vector_type+"-----\n");
      for (auto i:ins)
      {
        PRINT_INS(i);
      }
      DEBUG("\n----------\n");
    }
    void print(std::set<Instruction *> ins, std::string vector_type) {
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
      DEBUG("\n----------\n");
    }
    void print_lib_functions(Function * F) {
      print(geps[F], "GEP");
      print(allocas[F], "Alloca");
    }

    void parse_ldr_str_instructions(Function &F) {
      for (BasicBlock &BB : F) {
        for (Instruction &ins : BB) {
          if (isa<AllocaInst>(&ins)) {
            allocas[&F].insert(&ins);
          }
          else
            if (isa<GetElementPtrInst>(&ins)) {
              geps[&F].push_back(&ins);
            }
            else
              if (isa<LoadInst>(&ins)) {
                loads[&F].push_back(&ins);
              }
              else
                if (isa<StoreInst>(&ins)) {
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
                    call_instructions[&F].push_back(&ins);
                  }
                }
        }
      }
    }


    void parse_function_arguments(Function &F) {
      for (auto a = F.arg_begin(); a != F.arg_end(); a++) {
        arguments[&F].insert(dynamic_cast<Value *>(a));
      }
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

    //TODO use variadic types
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

    void transform_loads(Function * F) {
      Type* func_args[] = {VoidPointerType};
      std::vector<Value *> args;
      for (auto l:loads[F]) {
        args.clear();
        LoadInst * ld = static_cast<LoadInst *>(l);
        IRBuilder<> builder(ld);
        args.push_back(builder.CreatePointerCast(ld->getPointerOperand(), VoidPointerType));

        Value * carm_check_call = create_function(func_args, CARM_DEREF_DUMMY, args, VoidType, ld);
        carm_checks[F].insert(carm_check_call);
        carm_check_get_pointer_operand[carm_check_call] = ld->getPointerOperand();
      }
    }

    void transform_stores(Function * F) {
      Type* func_args[] = {VoidPointerType};
      std::vector<Value *> args;
      for (auto s:stores[F]) {
        args.clear();
        StoreInst * st = static_cast<StoreInst *>(s);
        IRBuilder<> builder(st);

        args.push_back(builder.CreatePointerCast(st->getPointerOperand(), VoidPointerType));

        Value * carm_check_call = create_function(func_args, CARM_DEREF_DUMMY, args, VoidType, st);
        carm_checks[F].insert(carm_check_call);
        carm_check_get_pointer_operand[carm_check_call] = st->getPointerOperand();
      }
    }
    bool must_alias(Function *F, Value * stack_variable, std::set<Instruction *> alloca_list) {
      if (stack_variable == nullptr)
        return false;
      AliasAnalysis &aliasAnalysis = getAnalysis<AAResultsWrapperPass>(*F).getAAResults();
      for (auto s:alloca_list) { 
        if (aliasAnalysis.isMustAlias(stack_variable, s))
        {
          return true;
        }
      }
      return false;
    }
    bool must_alias(Function *F, std::set<Value *> value_list, std::set<Instruction *> alloca_list) {
      for (auto s:value_list) { 
        if (!must_alias(F, s, alloca_list))
        {
          return false;
        }
      }
      return true;
    }

    Value * uncast_ungep(Value * ptr, std::set<Value *> phiset, std::set<Value *>& valueset) {
      auto gepi = dyn_cast<GetElementPtrInst>(ptr);
      auto ci = dyn_cast<CastInst>(ptr);
      auto phiinst = dyn_cast<PHINode>(ptr);
      if (gepi) {
        return uncast_ungep(gepi->getPointerOperand(), phiset, valueset);
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

    void delete_carm_check_on_stack_variables(Function * F) {
      std::set<Instruction *> alloca_list = allocas[F];
      std::set<Value *> argument_list = arguments[F];
      std::set<Value *> phiset;
      std::set<Value *> valueset;
      std::set<Value *> deleted_checks;
      std::set<Instruction *> delete_stack;
      uint64_t size = delete_stack.size();
      do {
        size = delete_stack.size();
        for (auto ins:carm_checks[F]) {
          if (alloca_list.find(static_cast<Instruction *>(carm_check_get_pointer_operand[ins])) != alloca_list.end()) {
            delete_stack.insert(static_cast<Instruction *>(ins));
            deleted_checks.insert(ins);
          }
          else if (must_alias(F, carm_check_get_pointer_operand[ins], alloca_list)) {
            alloca_list.insert(static_cast<Instruction *>(carm_check_get_pointer_operand[ins]));
            delete_stack.insert(static_cast<Instruction *>(ins));
            deleted_checks.insert(ins);
          }
          else {
            Value * bp = uncast_ungep(carm_check_get_pointer_operand[ins], phiset, valueset);
            valueset.insert(bp);
            if (must_alias(F, valueset, alloca_list)) {
              alloca_list.insert(static_cast<Instruction *>(carm_check_get_pointer_operand[ins]));
              delete_stack.insert(static_cast<Instruction *>(ins));
              deleted_checks.insert(ins);
            }
            valueset.clear();
            phiset.clear();
#if 0
            else if (argument_list.find(static_cast<Instruction *>(carm_check_get_pointer_operand[ins])) != argument_list.end()) {
              delete_stack.insert(static_cast<Instruction *>(ins));
            }
            else if (must_alias(carm_check_get_pointer_operand[ins], argument_list)) {
              argument_list.insert(static_cast<Instruction *>(carm_check_get_pointer_operand[ins]));
              delete_stack.insert(static_cast<Instruction *>(ins));
            }
            else if (must_alias(uncast_ungep(carm_check_get_pointer_operand[ins], phiset), argument_list)) {
              argument_list.insert(static_cast<Instruction *>(carm_check_get_pointer_operand[ins]));
              delete_stack.insert(static_cast<Instruction *>(ins));
            }
#endif

          }
        }
        if (size && (size != delete_stack.size()))
          errs()<<"Optimization avoided carm checks on "<<delete_stack.size()<<" Variables size "<<size<<" delete_stack "<<delete_stack.size()<<"\n";
      }
      while (size != delete_stack.size());
      for (auto ins:deleted_checks) 
        carm_checks[F].erase(ins);
      delete_instructions(delete_stack);
    }


    bool runOnFunction(Function &F) {
      //F.dump();
#if 1

    allocas.clear();
    arguments.clear();
    geps.clear();
    loads.clear();
    stores.clear();
    call_instructions.clear();
    carm_checks.clear();
    carm_check_get_pointer_operand.clear();
      //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
      //of globals.
      //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
      if (F.getName().str().find("cxx_global_var_init") == std::string::npos) {
        parse_function_arguments(F);
        parse_ldr_str_instructions(F);
        transform_loads(&F);
        transform_stores(&F);
        delete_carm_check_on_stack_variables(&F);
      }
#endif
      return true;
    }

    bool runOnModule(Module &M) override {

      //loops_noelle();
      for (auto &F:M) {
        runOnFunction(F);
      }
      return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<Noelle>();
      AU.addRequired<LoopInfoWrapperPass>();
      AU.addRequired<ScalarEvolutionWrapperPass>();
      AU.addRequired< AAResultsWrapperPass >();
      AU.addRequired< CallGraphWrapperPass>();
      AU.addRequired< DominatorTreeWrapperPass>();
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
