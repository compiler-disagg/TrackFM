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
#include "carm_object_config.hpp"


#include <algorithm>

#include "noelle/core/Noelle.hpp"

using namespace llvm::noelle ;


#define DEBUG(str) do { errs() << str;} while (0)
#define PRINT_INS(ins) do { ins->print(errs()); errs()<<"\n";} while (0)


namespace {

  static std::string CARM_MALLOC    = "_Z10carmMallocm";
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
  static std::string CARM_DEBUG                =  "_Z10carm_debugm";



  struct CAT : public FunctionPass {
    static char ID;
    std::unordered_map<Function *, std::vector<Instruction *>> allocas;
    std::unordered_map<Function *, std::vector<Instruction *>> geps;
    std::unordered_map<Function *, std::vector<Instruction *>> loads;
    std::unordered_map<Function *, std::vector<Instruction *>> stores;
    std::unordered_map<Function *, std::set<Instruction *>> carm_functions;
    std::unordered_map<Instruction *, Instruction *> carm_functions_map;
    std::unordered_map<Function *, Value *> cache_deref;
    std::unordered_map<int, Value *> global_var;
    std::unordered_map<Function *, std::vector<Instruction *>> delete_loads;
    std::unordered_map<Function *, std::vector<Instruction *>> delete_stores;
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
    CAT() : FunctionPass(ID) {}
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
      Value * carm_object_addr = currentModule->getOrInsertGlobal("carm_obj_index", Int64Type);
      Value * carm_object_data_addr = currentModule->getOrInsertGlobal("carm_obj_data_addr", Int64Type);
      Value * carm_object_mut_addr = currentModule->getOrInsertGlobal("carm_obj_mut_index", Int64Type);
      Value * carm_object_mut_data_addr = currentModule->getOrInsertGlobal("carm_obj_mut_data_addr", Int64Type);
      global_var[OBJ_STATE] = obj_state;
      return true;
    }

    void init_fmap(std::unordered_map<std::string, int>& fmap) {

      std::string func[] = {
        "Not supported",
        CARM_DEREF_DUMMY,
        CARM_DEREF_LOOP_DUMMY 
      };

      int size = 3;
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
      int index = 0;
      for (BasicBlock &BB : F) {
        index = 0;
        for (Instruction &ins : BB) {
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
                    {
                      carm_functions[&F].insert(&ins);
                      carm_functions_map[&ins] = ins.getNextNonDebugInstruction();
                      break;
                    }
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


    Value * check_obj_state(Value * obj_state, Instruction * insert_point, bool mut) {


      auto exceptions = (kHotClear | kPresentClear | kEvacuationSet);  
      //auto exceptions = (kPresentClear | kEvacuationSet);  
      if (mut)
        exceptions |= kDirtyClear;    

      IRBuilder<> builder(insert_point);

      Value * obj_state_conditions = builder.CreateAnd(obj_state, exceptions);

      return obj_state_conditions;
    }


    Value * get_data_addr_from_carm_obj_state(Value * obj_state, Instruction * insert_point) {

      IRBuilder<> builder(insert_point);

      Value * obj_data_addr = builder.CreateLShr(obj_state, kObjectDataAddrBitPos);

      return obj_data_addr;
    }

    Value * transform_nested_if(Function * F, Instruction * insert_point, Value * check_inst, bool mut, Value * global_cached_ptr, Value * carm_pointer) {

      IRBuilder<> builder(insert_point);

      Value * object_size_mask  = ConstantInt::get(Int64Type, OBJ_SIZE_MASK, false);
      Value * log_obj_size  = ConstantInt::get(Int64Type, log2(OBJ_SIZE), false);
      Value * prev_obj_index, * is_object_cached ;
      Value * addr1;
      Value * carm_pointer_base  = ConstantInt::get(Int64Type, CARM_PTR, false);
      Value * xorv = builder.CreateXor(carm_pointer, carm_pointer_base);
      //PRINT_INS(ins);
      Value * cached_addr_offset;
      Value * index = builder.CreateLShr(xorv, log_obj_size);

      Value * obj_state = nullptr;

      Value * addr;

      Value * struct_carm_obj_state = builder.CreateGEP(Int64Type, global_cached_ptr, index);

      //Value * a2 = builder.CreatePtrToInt (global_cached_ptr, Int64Type);
      //create_debug_call(a2, insert_point);
      Value * carm_obj_state  = builder.CreateLoad(Int64Type, struct_carm_obj_state);


      obj_state = check_obj_state(carm_obj_state, insert_point, mut);


      Type* func_args[] = {Int64Type};

      std::vector<Value *> args;


      Value * check_object_cached = builder.CreateICmpNE(obj_state, ConstantInt::get(Int64Type, 0, false));

      Instruction *ThenTerm , *ElseTerm ;
      SplitBlockAndInsertIfThenElse(check_object_cached, insert_point, &ThenTerm, &ElseTerm, nullptr);

      builder.SetInsertPoint(ThenTerm);

      args.clear();
      Type* func_args1[] = {Int64Type};
      args.push_back(index);

      if (mut) { 
        addr = create_function(func_args1, CARM_DEREF_WRITE, args, Int64Type, ThenTerm);
      }
      else {
        addr = create_function(func_args1, CARM_DEREF_READ, args, Int64Type, ThenTerm);
      }

      //create_debug_call(carm_pointer, ThenTerm);
      builder.SetInsertPoint(ElseTerm);

      //create_debug_call(carm_pointer, ElseTerm);

      BasicBlock *ThenBB = ThenTerm->getParent();
      BasicBlock *ElseBB = ElseTerm->getParent();
      BasicBlock *MergeBlock = insert_point->getParent();

      builder.SetInsertPoint(&MergeBlock->front());

      PHINode *PN = builder.CreatePHI(Int64Type, 2);
      PN->addIncoming(addr, ThenBB);
      PN->addIncoming(get_data_addr_from_carm_obj_state(carm_obj_state, ElseTerm), ElseBB);

      return builder.CreateAdd(PN, builder.CreateAnd(carm_pointer, object_size_mask));

    }

    void load_cached_global_ptrs(Function & F) {
      BasicBlock *EntryBlock = &(F.getEntryBlock());
      Instruction *InsertionPoint = EntryBlock->getFirstNonPHI();
      if (InsertionPoint == nullptr) {
        errs()<<"No insertion point found\n";
        abort(); // Serious
      }
      IRBuilder<> builder(InsertionPoint);

      Value * global_cached_ptr = builder.CreateLoad(VoidPointerType, global_var[OBJ_STATE]);
      //Value * global_cached_ptr = create_function(CARM_DEREF_CACHE_ADDR, VoidPointerType, InsertionPoint);


      //	  auto arr_type = llvm::VectorType::get(Int128Type, CACHE_SIZE,false); 
      //	  llvm::AllocaInst* arr_alloc = builder.CreateAlloca(arr_type, 0 , "vec" ); 
      //	  Value *scalarToVector =  builder.CreatePointerCast(global_cached_ptr, arr_alloc->getType());


      cache_deref[&F] = global_cached_ptr;
    }


    Value * get_gep_ptr_stride(CallInst * callinst)
    {
      return callinst->getArgOperand(1);
    }

    void transform_function(Function * F) {
      bool mut = false;
#if 0
      auto& noelle = getAnalysis<Noelle>();

      auto loops = noelle.getLoopStructures();
      auto forest = noelle.organizeLoopsInTheirNestingForest(*loops);
#endif
      for (auto carm_check:carm_functions[F]) {
        Instruction * carm_inspected_instruction = carm_functions_map[carm_check];
        Value * ptr = nullptr;
        Value * global_cached_ptr = nullptr;
        Value * transformed_carm_pointer = nullptr;
        IRBuilder<> builder(carm_check);
        Value * pointer = builder.CreatePointerCast(carm_check->getOperand(0), Int64Type);

        Value * carm_pointer_base  = ConstantInt::get(Int64Type, CARM_PTR, false);

        Value * isCarmPtr = builder.CreateICmpUGE(pointer, carm_pointer_base);

        Instruction * cond = static_cast<Instruction *>(isCarmPtr);

        Value * global_cache = cache_deref[F];
        if (isa<LoadInst>(carm_inspected_instruction)) {
          mut = false;
        }
        else {
          mut = true;
        }

        if (global_cache == nullptr) {
          errs()<<"Cache ptr is null; try explicit global loads\n";
          //global_cache = builder.CreateLoad(VoidPointerType, global_var[currentModule].first);
          //global_cache_m = builder.CreateLoad(VoidPointerType, global_var[currentModule].second);
          abort();
        }


        Instruction *ThenTerm , *ElseTerm ;

        SplitBlockAndInsertIfThenElse(isCarmPtr, carm_check ,&ThenTerm, &ElseTerm, nullptr);

        builder.SetInsertPoint(ThenTerm);
        transformed_carm_pointer = transform_nested_if(F, ThenTerm, carm_check, mut, global_cache, pointer);

        BasicBlock *MergeBlock = carm_inspected_instruction->getParent();
        BasicBlock *ThenBB = ThenTerm->getParent();
        BasicBlock *ElseBB = ElseTerm->getParent();


        //Update global cache markers for new basic blocks

#if 0 
        cache_deref[MergeBlock] = global_cache;

        cache_deref[ThenBB] = global_cache;

        cache_deref[ElseBB] = global_cache;

#endif

        builder.SetInsertPoint(&MergeBlock->front());

        PHINode *PN = builder.CreatePHI(Int64Type, 2);
        PN->addIncoming(transformed_carm_pointer, ThenBB);
        PN->addIncoming(pointer, ElseBB);

        Value * new_inst;
        if (!mut) {
          LoadInst * ld = static_cast<LoadInst *>(carm_inspected_instruction);
          ptr = builder.CreateIntToPtr (PN, ld->getPointerOperand()->getType()); 
          new_inst = builder.CreateLoad(ld->getType(), ptr);
          delete_loads[F].push_back(carm_inspected_instruction);
        }
        else {
          StoreInst * st = static_cast<StoreInst *>(carm_inspected_instruction);
          ptr = builder.CreateIntToPtr (PN, st->getPointerOperand()->getType()); 
          new_inst = builder.CreateStore(st->getValueOperand(), ptr);
          delete_stores[F].push_back(carm_inspected_instruction);
        }
        carm_inspected_instruction->replaceAllUsesWith(new_inst);

      }

    }

    void delete_dummy_functions(Function *F) {

      delete_instructions(carm_functions[F]);
      delete_instructions(delete_stores[F]);
      delete_instructions(delete_loads[F]);
    }

    bool runOnFunction(Function &F) override {
      //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
      //of globals.
      //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
      allocas.clear();
      geps.clear();
      loads.clear();
      stores.clear();
      carm_functions.clear();
      carm_functions_map.clear();
      cache_deref.clear();
      delete_loads.clear();
      delete_stores.clear();
      if (F.getName().str().find("cxx_global_var_init") == std::string::npos) {
        parse_lib_functions(F);
        load_cached_global_ptrs(F);
        transform_function(&F);
        delete_dummy_functions(&F);
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
