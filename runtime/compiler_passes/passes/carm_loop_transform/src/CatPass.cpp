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
  static std::string CARM_DEREF_PTR_LDR     = "_Z17carmDeref_ptr_ldrPv";
  static std::string CARM_ACQUIRE   = "_Z7acquirev";
  static std::string CARM_RELEASE   = "_Z7releasev";
  static std::string CARM_CACHE_ADDR   = "_Z19carm_get_cache_addrv";
  static std::string CARM_CACHE_UB   = "_Z22carm_get_cache_addr_ubv";
  static std::string CARM_CACHE_LB   = "_Z22carm_get_cache_addr_lbv";
  static std::string SET_CARM_CACHE_ADDR   = "_Z19carm_set_cache_addrv";
  static std::string CARM_DEREF_DUMMY             = "_Z10carm_derefPv";
  static std::string CARM_DEREF_LOOP_DUMMY             = "carm_deref_loop";
  static std::string CARM_DEREF_LOOP_CONST_DUMMY             = "carm_deref_loop_const";
  static std::string CARM_DEREF_LOOP_WRITE             = "_Z21carm_deref_loop_writem";
  static std::string CARM_DEREF_LOOP_READ             = "_Z20carm_deref_loop_readm";
  static std::string CARM_LOOP_INVALIDATE             = "_Z21invalidate_loop_indexm";
  static std::string CARM_GET_INIT_OBJ_SIZE      = "_Z14carm_loop_initmm";
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

  struct CAT : public ModulePass {
    static char ID;
    std::unordered_map<Function *, std::vector<Instruction *>> instructions;
    std::unordered_map<Function *, std::set<Instruction *>> carm_functions;
    std::unordered_map<Function *, std::vector<Instruction *>> carm_loop_functions;
    std::unordered_map<Function *, std::vector<Instruction *>> carm_loop_const_functions;
    std::unordered_map<Instruction *, Instruction *> carm_functions_map;
    std::unordered_map<Instruction *, Instruction *> carm_loop_gep_map;
    std::unordered_map<LoopStructure *, BasicBlock *> loop_pre_header_updates;
    std::unordered_map<LoopStructure *, std::unordered_set<BasicBlock *>> loop_latch_updates;
    std::unordered_map<LoopStructure *, std::unordered_set<BasicBlock *>> loop_basic_blocks_update;

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
      return true;
    }

     int id = 0;
    void init_fmap(std::unordered_map<std::string, int>& fmap) {

      std::string func[] = {
        "Not supported",
        CARM_DEREF_DUMMY,
        CARM_DEREF_LOOP_DUMMY, 
        CARM_DEREF_LOOP_CONST_DUMMY 
      };

      int size = 4;
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
          //errs()<<"nested gepi \m";
          //errs()<<*gepi;
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

    class  pointer_to_optimize{

      public:
        Value * ptr_operand;
        Value * loopid_and_objsize;;
        Value * init_bounds_check;
        Value * loop_id;

    };

    void invalidate_loop_cache_at_exit(LoopStructure * LS, pointer_to_optimize * gep_state) {

      std::vector<Value *> args;
      Type* func_args[] = {Int64Type};
      args.push_back(gep_state->loop_id);
      FunctionType* signature = FunctionType::get(Int64Type,
          ArrayRef<Type*>(func_args, args.size()),
          false);
      for (auto es:LS->getLoopExitBasicBlocks()) {
        IRBuilder<> builder(es->getFirstNonPHI());
        //errs()<<*es<<"\n";
        Function * f =  get_or_insert_function(CARM_LOOP_INVALIDATE, signature);
        builder.CreateCall(f, args);
      }
    }

    Value * get_gep_ptr_stride(CallInst * callinst)
    {
      return callinst->getArgOperand(1);
    }
    Value * get_gep_ptr_start(CallInst * callinst)
    {
      return callinst->getArgOperand(2);
    }

    bool get_gep_ptr_mutable(CallInst * callinst)
    {
            if (llvm::ConstantInt* mut = dyn_cast<llvm::ConstantInt>(callinst->getArgOperand(3))) {

              if (mut->getSExtValue()) {
                return true;
              }
              else 
                return false;
            }
            errs()<<"Mutable arhument not constat int\n";
            abort();
            return false;
    }

    void _transform_loop_function(Function & F, std::vector<Instruction *> & loop_dummy_functions, bool step_value_const,  StayConnectedNestedLoopForest * forest) {
      bool mut = false;
      std::vector<GetElementPtrInst *> delete_geps;
      for (auto carm_check:loop_dummy_functions) {
        auto gepi = dyn_cast<GetElementPtrInst>(carm_loop_gep_map[carm_check]);
        auto callinst = dyn_cast<CallInst>(carm_check);

        /*
         * Fetch the innermost loop that contains the instruction.
         */
        auto fs = forest->getInnermostLoopThatContains(carm_loop_gep_map[carm_check]);

        if (fs) {
          auto ls = fs->getLoop();
          if (ls) {
            //maintain a list of updated blocks
            if (loop_basic_blocks_update.find(ls) == loop_basic_blocks_update.end())
            {

              std::unordered_set<BasicBlock *> BBs = ls->getBasicBlocks();
              for (auto bb:BBs) {
                loop_basic_blocks_update[ls].insert(bb);
              }

            }
            pointer_to_optimize * gep_state = _createCARMLoop(ls, get_gep_ptr_stride(callinst), get_gep_ptr_start(callinst), gepi, F, step_value_const, get_gep_ptr_mutable(callinst));
            invalidate_loop_cache_at_exit(ls, gep_state);
            delete_geps.push_back(gepi);
          }
          else {	
            errs()<<"Block not found fo loop optimization\n";
            abort();

          }
        }
        else {
          errs()<<"Block not found fo loop optimization\n";
          errs()<<*gepi;
          errs()<<*carm_loop_gep_map[carm_check];
          abort();
        }

      }
      for (auto s:delete_geps) {
        s->eraseFromParent();
      }
    }
    void transform_loop_function(Function & F,  StayConnectedNestedLoopForest * fs) {
              _transform_loop_function(F, carm_loop_functions[&F], false, fs);
    }
    void transform_loop_step_value_constant_function(Function & F, StayConnectedNestedLoopForest * fs) {
              _transform_loop_function(F, carm_loop_const_functions[&F], true, fs);
    }

    uint64_t total_guard3_checks_needed = 1;
    void delete_loop_functions(Function *F) {
      delete_instructions(carm_loop_functions[F]);
      delete_instructions(carm_loop_const_functions[F]);
    }
    void _redirect_origpreheader_to_outerloopheader(
        LLVMContext &context,
        LoopStructure *ls,
        BasicBlock *carmHeader) {

      errs() << "Get the PreHeader of the loop\n";
      errs() << *carmHeader << "\n";

      BasicBlock * preHeader;

      if (loop_pre_header_updates.find(ls) != loop_pre_header_updates.end()) 
        preHeader = loop_pre_header_updates[ls];
      else  
        preHeader = ls->getPreHeader();


      errs() << "Redirect the preheader branch\n";

      /* Redirect the preheader branch to enter the
       *   TEXAS version at the beginning.
       */
      auto term = preHeader->getTerminator();
      errs() << *preHeader << "\n";
      if (auto termAsBranch = dyn_cast<BranchInst>(term)) {
        for (unsigned idx = 0; idx < termAsBranch->getNumSuccessors(); idx++) {
          termAsBranch->setSuccessor(idx, carmHeader);
        }
      }

      errs() << "return\n";

      return;
    }
    BasicBlock * _createInnerLoopEntry(
        LLVMContext &context,
        Function *func,
        BasicBlock *InnerLoopBB,
        LoopStructure * ls) {

      errs() << "_createInnerLoopEntryBB start\n";

      // Create new basic block and insert at end of function.
      auto bb = BasicBlock::Create(context, ".carm.originalloop.entry", func);
      loop_basic_blocks_update[ls].insert(bb);

      IRBuilder<> builder(bb);

      //builder.CreateCall(repackFunc, args);

      // Inject branch to the shared latch basic block.
      builder.CreateBr(InnerLoopBB);


      //check if InnnerLoopBB is a predecessor



      errs() << "_createInnerLoopEntryBB end\n";

      return bb;
    }
    void add_branch_from_outerloopheader_to_originheader(BasicBlock * outerLoopHeaderBB, BasicBlock * originheaderBB) {
      Instruction * term = outerLoopHeaderBB->getTerminator();
      if (auto branchInst = dyn_cast<BranchInst>(term)) {
        for (unsigned i = 0; i < branchInst->getNumSuccessors(); i++) {
          branchInst->setSuccessor(i, originheaderBB);
        }

      }

      errs() << "add_branch_from_outerloopheader_to_originheader end\n";
    }


    void  _add_branch_origlatch_to_outerlooplatch(LoopStructure *ls, BasicBlock * carm_object_checkBB, BasicBlock * originHeader) {

      std::unordered_set<BasicBlock *> latchBBs;
      if (loop_latch_updates.find(ls) != loop_latch_updates.end()) 
        latchBBs = loop_latch_updates[ls];
      else
        latchBBs = ls->getLatches();
      for (auto bb : latchBBs) {
        // loop through all back edges of original loop and replace with sharedBB.
        auto term = bb->getTerminator();

        if (auto branchInst = dyn_cast<BranchInst>(term)) {

          for (unsigned i = 0; i < branchInst->getNumSuccessors(); i++) {
            if (branchInst->getSuccessor(i) == originHeader) {
              branchInst->setSuccessor(i, carm_object_checkBB);
            }
          }
        }
      }
    }

    void _redirect_origpreheaderPHIs_to_outerloopheader(LoopStructure *ls, BasicBlock * outerLoopHeaderBB, Function & F, BasicBlock * preheader) {
      auto BBs = loop_basic_blocks_update[ls];
      for (auto BB :BBs) {
        for (auto &phi : BB->phis()) {
          for (unsigned i = 0; i < phi.getNumIncomingValues(); i++) {
            auto origBB = phi.getIncomingBlock(i);
            if (preheader == origBB) {
              phi.setIncomingBlock(i, outerLoopHeaderBB);
            }
          }
        }

      }
    }
    BasicBlock *_createOuterLoopBodyBB(
        LLVMContext &context,
        Function *func,
        LoopStructure *ls,
        BasicBlock *originHeader) {

      errs() << "_OuterLoopBodyBB start\n";
      // Create new basic block and insert at end of function.
      auto bb = BasicBlock::Create(context, ".carm.outerloop.body", func);
      loop_basic_blocks_update[ls].insert(bb);

      // Create builder for the new basic block.
      IRBuilder<> builder(bb);

      builder.CreateBr(originHeader);

      errs() << "_createOuterLoopBB end\n";
      return bb;
    }

    //TODO:call intp carm runtime for object bound
    pointer_to_optimize * get_initial_object_bound( BasicBlock * bb, GetElementPtrInst * gepi, Value * start_value, bool step_value_constant, Value * step_value) {

        errs()<<"get_initial_object_bound\n";
      pointer_to_optimize * ptr_state = new pointer_to_optimize();
      Instruction * insert_pt;
      if(bb->getFirstNonPHI()) {

        insert_pt = bb->getFirstNonPHI();
        errs()<<*bb<<"\n";
        //abort();
      }
      else
      {
        errs()<<"no instruction found in get_initial_object_bound\n";
        insert_pt = bb->getTerminator();
      }
      IRBuilder<> builder(insert_pt);

      std::vector<Value *> args;

      Value * vinst =  builder.CreatePtrToInt(gepi->getPointerOperand(), Int64Type);
      Value * vaddr =  builder.CreateAdd(vinst, start_value);
      //Value * vaddr =  builder.CreatePtrToInt(gepi, Int64Type);
      args.push_back(vaddr);
      if (step_value_constant)
          args.push_back(ConstantInt::get(Int64Type, 0));
      else
          //args.push_back(ConstantInt::get(Int64Type, 0));
          args.push_back(step_value);
      Type* func_args[] = {Int64Type, Int64Type};
      FunctionType* signature = FunctionType::get(Int64Type,
          ArrayRef<Type*>(func_args, args.size()),
          false);

      Function * f =  get_or_insert_function(CARM_GET_INIT_OBJ_SIZE, signature);
      ptr_state->ptr_operand = vaddr;
      ptr_state->loopid_and_objsize = builder.CreateCall(f, args);


      ptr_state->loop_id =  builder.CreateLShr(ptr_state->loopid_and_objsize, ConstantInt::get(Int64Type, LOOP_ID_POS));

      ptr_state->init_bounds_check = builder.CreateAnd(ptr_state->loopid_and_objsize, ConstantInt::get(Int64Type, LOOP_OBJ_MASK));

      return ptr_state;
    }

    BasicBlock * _create_CARM_OBJECT_TrackerBB(
        LLVMContext &context,
        Function *func,
        LoopStructure *ls,
        BasicBlock * outerLoopBodyBB,
        BasicBlock * innerLoopEntryBB) {

      auto bb = BasicBlock::Create(context, ".carm.check", func);
      loop_basic_blocks_update[ls].insert(bb);

      // Create builder for the new basic block.
      IRBuilder<> builder(bb);


      auto zeroAsConst = ConstantInt::get(Int64Type, 0);

      // Create a stub for the index check
      auto constantFalse = ConstantInt::getFalse(context);

      // Inject branch to either shared latch or memory packing BB
      builder.CreateCondBr(constantFalse, outerLoopBodyBB, innerLoopEntryBB);

      // Inject branch to either shared latch or memory packing BB

      errs()<<"_create_CARM_OBJECT_TrackerBB end\n";

      return bb;
    }
    void  _redirect_origlatchPHIs_to_outerlooplatch(LoopStructure *ls, BasicBlock * outerLoopLatchBB, Function & F) {

    std::unordered_set<BasicBlock *> latchBBs;
    if (loop_latch_updates.find(ls) != loop_latch_updates.end()) 
      latchBBs = loop_latch_updates[ls];
    else
      latchBBs = ls->getLatches();

      auto BBs = loop_basic_blocks_update[ls];

      for (auto BB : BBs) {
        for (auto &phi : BB->phis()) {
          for (unsigned i = 0; i < phi.getNumIncomingValues(); i++) {
            auto origBB = phi.getIncomingBlock(i);
            auto origValue = phi.getIncomingValue(i);
            if (latchBBs.find(origBB) != latchBBs.end()) {
              phi.setIncomingBlock(i, outerLoopLatchBB);
            }
          }
        }

      }
    }
    BasicBlock * _createOuterLoopHeaderBB(
        LLVMContext &context,
        Function *func,
        BasicBlock *InnerLoopBB,
        LoopStructure *ls
        ) {

      
      errs() << "_createOuterLoopHeaderBB start\n";

      // Create new basic block and insert at end of function.
      auto bb = BasicBlock::Create(context, ".carm.outerloop.header", func);
      loop_basic_blocks_update[ls].insert(bb);

      IRBuilder<> builder(bb);

      //builder.CreateCall(repackFunc, args);

      errs() << "creating branch to shared latch BB\n";
      // Inject branch to the shared latch basic block.

      builder.CreateBr(InnerLoopBB);


      errs() << "_OuterLoopBB end\n";


      return bb;
    }

    Value * uncast_PointerOperand(Value * ptr) {
      auto gepi = dyn_cast<GetElementPtrInst>(ptr);
      auto ci = dyn_cast<CastInst>(ptr);
      auto phiinst = dyn_cast<PHINode>(ptr);
      if (gepi) {
        return uncast_PointerOperand(gepi->getPointerOperand());
      }
      else if (ci) {
        BitCastInst *BCI = dyn_cast<BitCastInst>(ci); 
        if (BCI) 
          return uncast_PointerOperand(BCI->getOperand(0)); 
        /* * Attempt to fetch the operand as a bitcast operator */ 
        BitCastOperator *BCO = dyn_cast<BitCastOperator>(ci); 
        if (BCO) 
          return uncast_PointerOperand(BCO->getOperand(0));
      }

      return ptr;
    }

    pointer_to_optimize *  _createCARMLoopIter(
        LLVMContext &context,
        LoopStructure *loop,
        BasicBlock * outerLoopHeader,
        BasicBlock * carm_object_checkBB,
        BasicBlock * outerLoopBody,
        BasicBlock * innerLoopEntry,
        Value      * carm_check_step_value,
        Value      * carm_check_sart_value,
        GetElementPtrInst * gepi,
        Function * F,
        bool loop_step_value_constant,           //no induction
        bool mut           //no induction
        ) {

      errs() << "_createCarmLoopIter start\n";

      BasicBlock * orig_header = loop->getHeader();

      IRBuilder<> Builder(orig_header->getFirstNonPHI());

      //errs() << "Create a PHI node for the iterator\n";

      auto zeroAsConst = ConstantInt::get(Int64Type, 0);


      pointer_to_optimize * gep_state = nullptr;

      auto carm_bounds_check = Builder.CreatePHI(Int64Type, 0, ".carm.bounds_check");

      for (auto IT = pred_begin(orig_header); IT != pred_end(orig_header); ++IT) {
        auto incomingBB = *IT;
        if (incomingBB == outerLoopHeader) {
          gep_state = get_initial_object_bound(outerLoopHeader, gepi, carm_check_sart_value,
                                               loop_step_value_constant, carm_check_step_value);
          //carm_bounds_check->addIncoming(gep_state->init_bounds_check, incomingBB);
        } else if (incomingBB == innerLoopEntry) {
          //  carm_bounds_check->addIncoming(ConstantInt::get(Int64Type, OBJ_SIZE), incomingBB);
        }
      }


      IRBuilder<> CheckBuilder(carm_object_checkBB->getFirstNonPHI());
      IRBuilder<> OuterLoopBodyBuilder(outerLoopBody->getFirstNonPHI());

      std::vector<Value *> args;
      Type* func_args[] = {Int64Type};

      args.push_back(gep_state->loop_id);

      FunctionType* signature = FunctionType::get(Int64Type,
          ArrayRef<Type*>(func_args, args.size()),false);

      Function * f;
      if (mut)
        f =  get_or_insert_function(CARM_DEREF_LOOP_WRITE, signature);
      else
        f =  get_or_insert_function(CARM_DEREF_LOOP_READ, signature);

      Value* body_deref_call = OuterLoopBodyBuilder.CreateCall(f, args);
      Value * bound_pointer =  OuterLoopBodyBuilder.CreateAdd(body_deref_call, ConstantInt::get(Int64Type, OBJ_SIZE, false));

      IRBuilder<> OuterLoopHeaderBuilder(static_cast<Instruction *>(gep_state->init_bounds_check)->getNextNonDebugInstruction());

      Function * f1;
      if (mut)
        f1 =  get_or_insert_function(CARM_DEREF_LOOP_WRITE, signature);
      else
        f1 =  get_or_insert_function(CARM_DEREF_LOOP_READ, signature);


      //errs() << "creating second call to carm deref from header \n";

      //Value * carm_check_call = create_function(func_args, CARM_DEREF_DUMMY, args, VoidType, ld);
      Value * header_deref_call = OuterLoopHeaderBuilder.CreateCall(f1, args);

      Value * bound_pointer_header =  OuterLoopHeaderBuilder.CreateAdd(header_deref_call, gep_state->init_bounds_check);

      carm_bounds_check->addIncoming(bound_pointer_header, outerLoopHeader);


      args.clear();

      IRBuilder<> innerLoopEntryBuilder (innerLoopEntry->getFirstNonPHI());



      auto carm_pointer_operand = Builder.CreatePHI(Int64Type, 0, ".carm.pointer.operand" + std::to_string(id));
      for (auto IT = pred_begin(orig_header); IT != pred_end(orig_header); ++IT) {
        auto incomingBB = *IT;
        if (incomingBB == outerLoopHeader) {
          carm_pointer_operand->addIncoming(header_deref_call, incomingBB);
        } else if (incomingBB == innerLoopEntry) {
          //carm_pointer_operand->addIncoming(header_deref_call, incomingBB);
        }
      }

      Value * inc_operand_checkBB;  
      if (loop_step_value_constant) {
            inc_operand_checkBB = carm_pointer_operand;  
      }
      else
        inc_operand_checkBB = CheckBuilder.CreateAdd(carm_pointer_operand, carm_check_step_value);  

    //  auto indexRem = CheckBuilder.CreateURem(indexInc, carm_bounds_check, ".carm.counter.rem");
      auto indexCmp = CheckBuilder.CreateICmpUGE(inc_operand_checkBB, carm_bounds_check, ".carm.counter.cmp"+ std::to_string(id));

      auto checkTerm =  carm_object_checkBB->getTerminator();
      auto checkCondBr = cast<BranchInst>(checkTerm);
      checkCondBr->setCondition(indexCmp);

      auto tmp_carm_pointer_operand = innerLoopEntryBuilder.CreatePHI(Int64Type, 0, ".tmp.carm.pointer.operand"+ std::to_string(id));

      for (auto IT = pred_begin(innerLoopEntry); IT != pred_end(innerLoopEntry); ++IT) {
        auto incomingBB = *IT;
        if (incomingBB == outerLoopBody) {
          tmp_carm_pointer_operand->addIncoming(body_deref_call, incomingBB);
        } else if (incomingBB == carm_object_checkBB) {
          tmp_carm_pointer_operand->addIncoming(inc_operand_checkBB, incomingBB);
        }
      }

      carm_pointer_operand->addIncoming(tmp_carm_pointer_operand, innerLoopEntry);

      auto tmp_carm_bound_check = innerLoopEntryBuilder.CreatePHI(Int64Type, 0, ".tmp.carm.bound.check"+ std::to_string(id));

      for (auto IT = pred_begin(innerLoopEntry); IT != pred_end(innerLoopEntry); ++IT) {
        auto incomingBB = *IT;
        if (incomingBB == outerLoopBody) {
          tmp_carm_bound_check->addIncoming(bound_pointer, incomingBB);
        } else if (incomingBB == carm_object_checkBB) {
          tmp_carm_bound_check->addIncoming(carm_bounds_check, incomingBB);
        }
      }


      carm_bounds_check->addIncoming(tmp_carm_bound_check, innerLoopEntry);



    //  errs() << "After transformation:\n" << *outerLoopBody << "\n";

     // errs() << "_createOuterLoopLoopIter end\n";

      Value * new_gep = Builder.CreateIntToPtr(carm_pointer_operand, gepi->getType());
      delete_carm_checks_on_gep(F, gepi);
      gepi->replaceAllUsesWith(new_gep);
      
      id++;

      //gepi->setOperand(0, Builder.CreateIntToPtr(carm_pointer_operand, gepi->getPointerOperand()->getType())); 
      return gep_state;
    }
    pointer_to_optimize * _createCARMLoop(LoopStructure * ls, Value * outer_loop_step_value, Value * outer_loop_start_value, GetElementPtrInst * gepi, Function & F, bool step_value_constant, bool mut) {

      std::vector<BasicBlock *> orderedCloneBBs;
      std::unordered_map<BasicBlock *, ValueToValueMapTy *> bbVMap;
      bool create_preHeader_BB = false;

      auto func = ls->getFunction();
      auto &context = func->getContext();

      // Clone loop header.

      BasicBlock * orig_pre_Header;
      BasicBlock * outerLoopHeaderBB;

      if (loop_pre_header_updates.find(ls) != loop_pre_header_updates.end()) { 
        orig_pre_Header = loop_pre_header_updates[ls];
        outerLoopHeaderBB = orig_pre_Header;
      }
      else { 
        orig_pre_Header = ls->getPreHeader();
        create_preHeader_BB = true;
      }

      std::vector<BasicBlock *> toRedirect;



      if (create_preHeader_BB) {
         outerLoopHeaderBB = _createOuterLoopHeaderBB(context, func, ls->getHeader(), ls);

        _redirect_origpreheader_to_outerloopheader(context, ls, outerLoopHeaderBB);

        add_branch_from_outerloopheader_to_originheader(outerLoopHeaderBB, ls->getHeader());
      _redirect_origpreheaderPHIs_to_outerloopheader(ls, outerLoopHeaderBB, F, orig_pre_Header);
      }

      auto InnerLoopEntryBB = _createInnerLoopEntry(context, func, ls->getHeader(), ls);
      auto outerLoopBodyBB =  _createOuterLoopBodyBB(context, func, ls, InnerLoopEntryBB);


      auto carm_object_checkBB = _create_CARM_OBJECT_TrackerBB(context, func, ls, outerLoopBodyBB, InnerLoopEntryBB);


      _add_branch_origlatch_to_outerlooplatch(ls, carm_object_checkBB, ls->getHeader());

      _redirect_origlatchPHIs_to_outerlooplatch(ls, InnerLoopEntryBB, F);


      pointer_to_optimize * gp = _createCARMLoopIter(context, ls, outerLoopHeaderBB, carm_object_checkBB,  outerLoopBodyBB, InnerLoopEntryBB, outer_loop_step_value, outer_loop_start_value,gepi, &F, step_value_constant, mut);

      loop_pre_header_updates[ls] = outerLoopHeaderBB; 
      loop_latch_updates[ls].clear(); 
      loop_latch_updates[ls].insert(InnerLoopEntryBB); 
      //loop_latch_updates[ls].insert(outerLoopHeaderBB); 

      return gp;

    }

    Value * return_gep(Value * ptr) {
      auto gepi = dyn_cast<GetElementPtrInst>(ptr);
      auto ci = dyn_cast<CastInst>(ptr);
      if (gepi) {
        return gepi;
      }
      else if (ci) {
        BitCastInst *BCI = dyn_cast<BitCastInst>(ci); 
        if (BCI) 
          return return_gep(BCI->getOperand(0)); 
        /* * Attempt to fetch the operand as a bitcast operator */ 
        BitCastOperator *BCO = dyn_cast<BitCastOperator>(ci); 
        if (BCO) 
          return return_gep(BCO->getOperand(0));
      }

      return ptr;
    }
    void parse_carm_loop_instructions(Function &F) {
      for (BasicBlock &BB : F) {
        for (Instruction &ins : BB) {
          instructions[&F].push_back(&ins);
          if (isa<CallInst>(&ins) || isa<InvokeInst>(&ins)) {
            Function * fp;
            if (CallInst * ci = dyn_cast<CallInst>(&ins)) {
              fp = ci->getCalledFunction();
            }
            if (InvokeInst * ci = dyn_cast<InvokeInst>(&ins)) {
              fp = ci->getCalledFunction();
            }
            if (fp != nullptr) {
              if (fp->getName().compare(CARM_DEREF_LOOP_DUMMY) == 0) {
                carm_loop_functions[&F].push_back(&ins);
                //errs()<<ins<<"\n";
                carm_loop_gep_map[&ins] = ins.getNextNonDebugInstruction();
                //errs()<<*carm_loop_gep_map[&ins]<<"\n";
              }
              else if (fp->getName().compare(CARM_DEREF_LOOP_CONST_DUMMY) == 0) {
                carm_loop_const_functions[&F].push_back(&ins);
                //errs()<<ins<<"\n";
                carm_loop_gep_map[&ins] = ins.getNextNonDebugInstruction();
                //errs()<<*carm_loop_gep_map[&ins]<<"\n";

              }
              else if (fp->getName().compare(CARM_DEREF_DUMMY) == 0) {

                carm_functions[&F].insert(&ins);
                carm_functions_map[&ins] = ins.getNextNonDebugInstruction();

              }
            }
          }
        }
      }
    }
    void delete_carm_checks_on_gep(Function * F, GetElementPtrInst * gp) {

      std::set<Instruction *> deleted_checks;
      for (auto ins:carm_functions[F]) {
        Instruction * carm_inspected_instruction = carm_functions_map[ins];
        Value * ptrOperand = nullptr;
        if (isa<LoadInst>(carm_inspected_instruction)) { 
          LoadInst * ld = static_cast<LoadInst *>(carm_inspected_instruction);
          ptrOperand = ld->getPointerOperand(); 
        }
        else {
          StoreInst * st = static_cast<StoreInst *>(carm_inspected_instruction);
          ptrOperand = st->getPointerOperand(); 
        }
        Value * ptr = return_gep(ptrOperand);
        auto gepi = dyn_cast<GetElementPtrInst>(ptr);     
        if (gepi == gp) {
          deleted_checks.insert(static_cast<Instruction *>(ins));
        }
        else {
#if 0 
          errs()<<"Found different geps \n";
          if (gepi)
            errs()<<*gepi<<"\n";
          errs()<<*gp<<"\n";
#endif
        }
      }
      for (auto ins:deleted_checks) 
        carm_functions[F].erase(ins);
      delete_instructions(deleted_checks);
      //errs()<<"Deleted Instrumented GEP CARM Checks "<<deleted_checks.size()<<" checks\n";
    }

    bool runOnFunction(Function &F,  StayConnectedNestedLoopForest * fs) {
      //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
      //of globals.
      //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
      instructions.clear();
      carm_functions.clear();
      carm_loop_functions.clear();
      carm_loop_const_functions.clear();
      carm_functions_map.clear();
      carm_loop_gep_map.clear();
      parse_carm_loop_instructions(F);
      transform_loop_function(F, fs);
      transform_loop_step_value_constant_function(F, fs);
      delete_loop_functions(&F);
      //F.dump();
      return true;
    }

    bool runOnModule(Module & M) override {
      auto& noelle = getAnalysis<Noelle>();
      auto loops = noelle.getLoopStructures();
      if (loops->size() > 0) {
        auto forest = noelle.organizeLoopsInTheirNestingForest(*loops);
        if (forest) {
          for (auto &F:M) {
            //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
            //of globals.
            //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
            if (F.empty() || !(F.getInstructionCount()) || (F.isIntrinsic())) {
              continue;
            }
            if (F.getName().str().find("cxx_global_var_init") == std::string::npos) {
              //if (F.getName().str().find("trip_duration") != std::string::npos) {
                /*
                 * Organize loops in a forest.
                 */
                runOnFunction(F, forest);
                //	}
            }
          }
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
      //AU.setPreservesAll();
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
