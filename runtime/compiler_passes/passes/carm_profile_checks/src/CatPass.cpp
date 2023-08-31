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
#include "carm_object_config.hpp"

using namespace llvm::noelle ;


#define DEBUG(str) do { errs() << str;} while (0)
#define PRINT_INS(ins) do { ins->print(errs()); errs()<<"\n";} while (0)

namespace {

  static std::string CARM_DEREF_DUMMY             = "_Z10carm_derefPv";
  static std::string CARM_DEREF_LOOP_DUMMY             = "carm_deref_loop";
  static std::string CARM_DEREF_LOOP_CONST_DUMMY             = "carm_deref_loop_const";
  static std::string CARM_ACQUIRE   = "_Z7acquirev";
  static std::string CARM_RELEASE   = "_Z7releasev";
  struct CAT : public ModulePass {
    static char ID;
    std::unordered_map<Instruction *, LoopStructure *> candidate_geps;
    std::unordered_map<Value *, Value *> carm_check_get_pointer_operand;
    std::unordered_map<Value *, InductionVariable *>  induction_variables;
    std::unordered_map<Function *, std::vector<LoopStructure *>>  loops_function_map;

    Noelle * noelle;
		Hot * hot;

    std::unordered_map<Function *, std::set<Value *>> carm_checks;
    StayConnectedNestedLoopForest * forest;

    Module * currentModule;
    Type * VoidPointerType;
    Type * Int8Type;
    Type * Int16Type;
    Type * Int32Type;
    Type * Int64Type;
    Type * VoidType;

    class CARM_GEP{
      public:
        GetElementPtrInst * gep;
        Value *  ptr_offset_step_value;
        Value *  ptr_offset_start_value;
        //no induction variable found
        bool is_step_value_constant;
    };

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
    //conservative if there is a escape the gep is mutable 
    bool is_mutable(Instruction * ins, Instruction * orig_gep, int depth,  std::set<Instruction *>& seen_instructions) {
      if (seen_instructions.find(ins) != seen_instructions.end()) {
        return false;
      }
      seen_instructions.insert(ins);
      //give up
      if (!depth)
        return true;
      depth--;
      Value * v = dynamic_cast<Value *>(ins);
      for(auto U : v->users()){  // U is of type Use*
        if (auto I = dyn_cast<Instruction>(U)){
          if (seen_instructions.find(I) == seen_instructions.end()) {
            if (isa<StoreInst>(I) ||  (isa<GetElementPtrInst>(I) && orig_gep != v)) {
              return true;
            }
            else if (isa<CastInst>(I)) {
              bool mut = is_mutable(I, orig_gep, depth, seen_instructions);
              if (mut)
                return true;
            }
            else if (isa<CallInst>(I) || isa<InvokeInst>(I)) {
              Function * fp;
              if (CallInst * ci = dyn_cast<CallInst>(I)) {
                fp = ci->getCalledFunction();
              }
              if (InvokeInst * ci = dyn_cast<InvokeInst>(I)) {
                fp = ci->getCalledFunction();
              }
              if (fp != nullptr) {
                if (fp->getName().compare(CARM_DEREF_DUMMY) != 0) {
                  return true;
                }
              }
            }
          }

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


		void parse_carm_check_gep_instructions(Function &F) {
			for (BasicBlock &BB : F) {
				for (Instruction &ins : BB) {
					if (isa<CallInst>(&ins) || isa<InvokeInst>(&ins)) {
						Function * fp;
						if (CallInst * ci = dyn_cast<CallInst>(&ins)) {
							fp = ci->getCalledFunction();
						}
						if (InvokeInst * ci = dyn_cast<InvokeInst>(&ins)) {
							fp = ci->getCalledFunction();
						}
						if (fp != nullptr) {
							if (fp->getName().compare(CARM_DEREF_DUMMY) == 0) {

								Value * ptr = return_gep(ins.getOperand(0));


								if (!ptr || !isa<Instruction>(ptr)) {
									//errs()<<"not a instruction or ptr is null\n";
									continue;
								}


								carm_checks[&F].insert(&ins);
								carm_check_get_pointer_operand[&ins] = ptr;


							}
						}
					}
				}
			}
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


    int identify_candidate_geps(Function * F) {
      bool instrument = false;
      double min_ratio = 730;
      //errs()<<"Identify candidate gep carm checks to consider " << carm_checks[F].size()<<"\n";

      for (auto ins:carm_checks[F]) {

        Value * ptr = carm_check_get_pointer_operand[ins];

        if (!ptr || !isa<Instruction>(ptr)) {
          //errs()<<"not a instruction or ptr is null\n";
          continue;
        }

        /*
         * Fetch the innermost loop that contains the instruction.
         */
        auto fs = forest->getInnermostLoopThatContains(static_cast<Instruction *>(ptr));

        if (fs) {

          auto loopStructure = fs->getLoop();

          //errs()<<"Identify candidate gep " << *ptr <<"\n";
          auto gepi = dyn_cast<GetElementPtrInst>(ptr);     
          instrument = true;
          if (gepi) {

						if (!hot->getInvocations(loopStructure))
							continue;

						uint64_t inst_per_invocation = hot->getSelfInstructions(gepi) / hot->getInvocations(loopStructure); 

						//errs()<<"Identify candidate gep " << *ptr <<inst_per_invocation<< " Instruction count "<< hot->getSelfInstructions(gepi)<<" Invocations "<<hot->getInvocations(loopStructure)<<"\n";
						if (inst_per_invocation < min_ratio) {
							continue;
						}

            if (loopStructure->isLoopInvariant(gepi->getPointerOperand())) {

              errs()<<"yayyyyyyy found geps which can be instrumented\n";
              PRINT_INS(ptr);
              PRINT_INS(carm_check_get_pointer_operand[ins]);
              candidate_geps[gepi] = loopStructure ;
            }
            else {

              //errs()<<"GEPpointer not loop invariant"<<"\n";
              //errs()<<*gepi->getPointerOperand()<<"\n";
              //errs()<<*gepi<<"\n";
              instrument = false;
              continue;
            }
          }

          else {
            //nongep instructions not hoisted
            if(loopStructure->isLoopInvariant(static_cast<Instruction *>(ptr))) {
              //errs()<<"Not a gep instruction nut is loop invariant " <<*ptr<<"\n";
            }
          }
        }
        else {
//          errs()<<"Instruction not inside loop" <<*ptr<<"\n";

        }
      }

      return 0;
    }


    void add_branch_from_outerloopheader_to_originheader(BasicBlock * outerLoopHeaderBB, BasicBlock * originheaderBB) {
      Instruction * term = outerLoopHeaderBB->getTerminator();
      if (auto branchInst = dyn_cast<BranchInst>(term)) {
        for (unsigned i = 0; i < branchInst->getNumSuccessors(); i++) {
          branchInst->setSuccessor(i, originheaderBB);
        }

      }

    }


    bool collectOffset(
        GetElementPtrInst * gep, 
        const DataLayout &DL, unsigned BitWidth,
        std::unordered_map<Value *, APInt> &VariableOffsets,
        APInt &ConstantOffset) const {
      //assert(BitWidth == DL.getIndexSizeInBits(gp.getPointerAddressSpace()) &&
      //     "The offset bit width does not match DL specification.");

      auto CollectConstantOffset = [&](APInt Index, uint64_t Size) {
        Index = Index.sextOrTrunc(BitWidth);
        APInt IndexedSize = APInt(BitWidth, Size);
        ConstantOffset += Index * IndexedSize;
      };

      gep_type_iterator GTI = gep_type_begin(gep);

      for (unsigned I = 1, E = gep->getNumOperands(); I != E; ++I, ++GTI) {
        // Scalable vectors are multiplied by a runtime constant.
        bool ScalableType = isa<VectorType>(GTI.getIndexedType());

        Value *V = GTI.getOperand();
        StructType *STy = GTI.getStructTypeOrNull();
        // Handle ConstantInt if possible.
        if (auto ConstOffset = dyn_cast<ConstantInt>(V)) {
          // If the type is scalable and the constant is not zero (vscale * n * 0 =
          // 0) bailout.
          // TODO: If the runtime value is accessible at any point before DWARF
          // emission, then we could potentially keep a forward reference to it
          // in the debug value to be filled in later.
          if (ScalableType)
            return false;
          // Handle a struct index, which adds its field offset to the pointer.
          if (STy) {
            unsigned ElementIdx = ConstOffset->getZExtValue();
            const StructLayout *SL = DL.getStructLayout(STy);
            // Element offset is in bytes.
            CollectConstantOffset(APInt(BitWidth, SL->getElementOffset(ElementIdx)),
                1);
            continue;
          }
          CollectConstantOffset(ConstOffset->getValue(),
              DL.getTypeAllocSize(GTI.getIndexedType()));
          continue;
        }

        if (STy)
        {
          //errs()<<"Found gep value which is  struct type and not a constant\n";
          return false;
        }
        if (ScalableType)
        {
          //errs()<<"Found gep value which is  vector type and is not a constant\n";
          return false;
        }
        APInt IndexedSize =
          APInt(BitWidth, DL.getTypeAllocSize(GTI.getIndexedType()));
        // Insert an initial offset of 0 for V iff none exists already, then
        // increment the offset by IndexedSize.
        //checks if Indexed Size is zero LLVM9 APIINT
        if (!IndexedSize) {
        }
        else {
          VariableOffsets.insert({V, APInt(BitWidth, 0)});
          VariableOffsets[V] += IndexedSize;
        }
      }
      return true;
    }




    bool calculate_offset_gep( Function * F, GetElementPtrInst * gp, std::unordered_map<Value *, APInt>& VariableOffsets,
        APInt & COffset) {


      unsigned BitWidth = currentModule->getDataLayout().getIndexSizeInBits(gp->getPointerAddressSpace());
      bool offset_constant = collectOffset(gp, currentModule->getDataLayout() ,BitWidth, VariableOffsets, COffset);

      if (offset_constant) {
        //errs()<<"Offet "<<COffset.getSExtValue()<<"\n";
        for (auto & v:VariableOffsets) {
          if (induction_variables.find(v.first) == induction_variables.end()) {
            //errs()<<"Not an induction variable reject gep\n";
            //errs()<<*v.first<<"\n";
            return false;
          }
          //				errs()<<"Offet "<<v.second.getSExtValue()<<"\n";
        }
        //do not support constant offset hoisting yet
        return true;
      }
      return false;
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
    void check_gep_offsets(Function * F, std::set<CARM_GEP *>& geps_to_be_instrumented)
    {

			int const_size = 0;
      for (auto g:candidate_geps) {
				const_size = 0;
        errs()<<"Geps considered "<<*g.first<<"\n";
        auto gepi = dyn_cast<GetElementPtrInst>(g.first);
        std::unordered_map<Value *, APInt> VariableOffsets;
        unsigned BitWidth = currentModule->getDataLayout().getIndexSizeInBits(gepi->getPointerAddressSpace());
        APInt COffset(BitWidth, 0);
        bool offset_determined = calculate_offset_gep(F, gepi, VariableOffsets, COffset);
        if (offset_determined) {
          int64_t  const_sum_offset = COffset.getSExtValue(); 
          Value * start_value_for_outerloop = 0;
          Value * step_value_for_outerloop = 0;

          IRBuilder<> builder(g.second->getPreHeader()->getTerminator());

          std::vector<Value *> step_value_interim;
          std::vector<Value *> start_value_interim;
          errs()<<"Gepi "<<*gepi<<"\n";
          for (auto v:VariableOffsets) {
            InductionVariable * IV = induction_variables[v.first];
	    if (!check_if_loop_nesting_level_is_equal(v.first, g.first)) {
            step_value_interim.push_back(builder.CreateMul(ConstantInt::get(Int64Type, 0), v.first));
            start_value_interim.push_back(builder.CreateMul(ConstantInt::get(Int64Type, v.second.getSExtValue()),  v.first));
						const_size++;
	    }
	    else {
            step_value_interim.push_back(builder.CreateMul(ConstantInt::get(Int64Type, v.second.getSExtValue()), IV->getSingleComputedStepValue()));
            start_value_interim.push_back(builder.CreateMul(ConstantInt::get(Int64Type, v.second.getSExtValue()),  IV->getStartValue()));
          }
	  }


	  if (!start_value_interim.empty()) {
            Value * start_prev = start_value_interim[0];
            Value * add;
            for (int i = 1; i < start_value_interim.size(); i++) {
              add = builder.CreateAdd(start_value_interim[i], start_prev);
              start_prev = add;
            }

            Value * step_prev = step_value_interim[0];
            for (int i = 1; i < step_value_interim.size(); i++) {
              add = builder.CreateAdd(step_value_interim[i], step_prev);
              step_prev = add;
            }

            start_value_for_outerloop = builder.CreateAdd(start_prev, ConstantInt::get(Int64Type, const_sum_offset));

            //step_value_for_outerloop = builder.CreateAdd(step_prev, ConstantInt::get(Int64Type, const_sum_offset));
            step_value_for_outerloop = step_prev;


						//object density too low
						if (const_sum_offset > OBJ_SIZE) 
								continue;
              
						else {

            CARM_GEP  * cgep = new CARM_GEP();
            //errs()<<*gepi<<" can be hoistrd "<<"\n";
            cgep->gep = gepi;



            cgep->ptr_offset_step_value = step_value_for_outerloop;
            cgep->ptr_offset_start_value = start_value_for_outerloop;
            if (VariableOffsets.size() == 0) 
		continue;

            if (VariableOffsets.size() == const_size) 
              cgep->is_step_value_constant = true;
            else
              cgep->is_step_value_constant = false;
            geps_to_be_instrumented.insert(cgep);
					}
        }
	  else {

          errs()<<"Found a constant offset value" << *gepi<<"\n";
	  }
	}
        else {

          errs()<<"Found a value whose offste cannot be determined" << *gepi<<"\n";

        }
      }
    }

    bool add_loops_to_instrument(Function &F, int nesting_level) {
			bool instrument = false;

			errs()<<__func__<<"\n";
      for (auto ls : loops_function_map[&F]) {
        if (ls->getNestingLevel() > nesting_level){
          continue ;
        }
        auto ldi = noelle->getLoop(ls);

				if (!ldi)
					continue;

        auto LS = ldi->getLoopStructure();


        auto IVM = ldi->getInductionVariableManager();

        auto GIV = IVM->getLoopGoverningInductionVariable(*LS);
        if (GIV != nullptr){
          //errs() << "   GIV: " << *GIV->getLoopEntryPHI()  << "\n";
        }
#if 1
        auto IVs = IVM->getInductionVariables(*LS);
        for (auto IV : IVs){
          if (!IV->isStepValueLoopInvariant()){
            //errs() << "   Step value isn't constant\n";
            //errs()<<*IV->getSingleComputedStepValue()<<"\n";
          }
          else {

            //errs() << "   IV: " << *IV->getLoopEntryPHI() << "\n";
            //errs() << "   Step value is constant\n";
            //errs()<<*IV->getSingleComputedStepValue()<<"\n";

            induction_variables[IV->getLoopEntryPHI()] = IV;
						instrument = true;

          }
        }
#endif
        //											if (loop->doesHaveCompileTimeKnownTripCount()){   
        //errs() << " Trip count = " << loop->getCompileTimeTripCount() << "\n";     
        //										}

      }
			return instrument;
    }

    bool analyze_loops (Function &F) {


	    errs() << "Found function " << F.getName() << "Loop Size " <<loops_function_map[&F].size()<<  "\n";
	    if (loops_function_map[&F].size() > 0) {
		    int deep_nest = 3;
		    return add_loops_to_instrument(F, deep_nest);
	    }

	    errs() << "Found function analyzed " << F.getName() <<  "\n";
	    return false;
    }

    void delete_carm_checks_on_gep(Function * F, GetElementPtrInst * gp) {

      std::set<Instruction *> deleted_checks;
      for (auto ins:carm_checks[F]) {
        Value * ptr = carm_check_get_pointer_operand[ins];
        auto gepi = dyn_cast<GetElementPtrInst>(ptr);     
        if (gepi == gp) {
          deleted_checks.insert(static_cast<Instruction *>(ins));
        }
      }
      for (auto ins:deleted_checks) 
        carm_checks[F].erase(ins);
      delete_instructions(deleted_checks);
      //errs()<<"Deleted Instrumented GEB CARM Checks "<<deleted_checks.size()<<" checks\n";
    }


    bool check_if_loop_nesting_level_is_equal(Value * ptr1, Value * ptr2) {


	    /*
	     * Fetch the innermost loop that contains the instruction.
	     */
	    auto fs = forest->getInnermostLoopThatContains(static_cast<Instruction *>(ptr1));
	    auto fs1 = forest->getInnermostLoopThatContains(static_cast<Instruction *>(ptr2));

	    if (fs && fs1) {

		    auto lS0 = fs->getLoop();
		    auto lS1 = fs1->getLoop();

		    if (lS0->getNestingLevel() == lS1->getNestingLevel())
			    return true;
		    else
			    return false;


		}

	    return false;
    }

    void _instrument_geps(std::set<CARM_GEP *>& geps_to_be_instrumented) {

      //track instrumented loops to avoid nested loop instrumentation which
      //is not supported
      std::set<LoopStructure *> loops_instrumented;
      Type* func_args[] = {VoidPointerType, Int64Type, Int64Type, Int64Type};
      std::vector<Value *> args;
      for(auto gp : geps_to_be_instrumented) {
        if (gp->gep == nullptr)
        {
          errs()<<"gep is null\n";
          abort();
        }
        errs()<<"gep to be instrumented"<<gp->gep<<"\n";
   //     if ((loops_instrumented.find(candidate_geps[gp->gep]) == loops_instrumented.end())) {
          args.clear();
          IRBuilder<> builder(gp->gep);
          args.push_back(builder.CreatePointerCast(gp->gep->getPointerOperand(), VoidPointerType));
          args.push_back(gp->ptr_offset_step_value);
          args.push_back(gp->ptr_offset_start_value);
          Value * carm_check_call = nullptr;

          std::set<Instruction *> ins;
          bool mut = is_mutable(gp->gep, gp->gep, 3, ins);

          if (mut) {
              args.push_back(ConstantInt::get(Int64Type, 1));
          }
          else {
              args.push_back(ConstantInt::get(Int64Type, 0));
          }

          if (gp->is_step_value_constant) {
            carm_check_call = create_function(func_args, CARM_DEREF_LOOP_CONST_DUMMY, args, VoidType, gp->gep);
          }
          else {
            carm_check_call = create_function(func_args, CARM_DEREF_LOOP_DUMMY, args, VoidType, gp->gep);

          }
          loops_instrumented.insert(candidate_geps[gp->gep]);
        }
     // }
      for(auto gp : geps_to_be_instrumented) {
        delete gp;
      }

    }
    void instrument_geps(Function &F) {
      std::set<CARM_GEP *> geps_to_be_instrumented;
      check_gep_offsets(&F, geps_to_be_instrumented);
      _instrument_geps(geps_to_be_instrumented);
    }

		void map_loops_function(vector<llvm::noelle::LoopStructure *>  * ls_list) {

      for (auto LS : *ls_list){
        loops_function_map[LS->getFunction()].push_back(LS);
			}

		}

    bool runOnFunction(Function &F) {
      //F.dump();
#if 1

		  errs()<<" Function being analyzed "<<F.getName()<<"\n";
      induction_variables.clear();
      candidate_geps.clear();
      carm_check_get_pointer_operand.clear();
      carm_checks.clear();


			if (analyze_loops(F)) {
				parse_carm_check_gep_instructions(F);
				identify_candidate_geps(&F);
				instrument_geps(F);
			}
#endif
      return true;
    }

    bool runOnModule(Module &M) override {

    	vector<llvm::noelle::LoopStructure *>  * ls_list;
	
      noelle = &getAnalysis<Noelle>();

      ls_list = noelle->getLoopStructures();



      if (!ls_list) {
	      //errs()<<"Loop size is null " <<"\n";
	      return true;
      }
      if (ls_list->size() == 0) {
	      //errs()<<"Loop size is 0 " <<"\n";
	      return true;
      }

      /*
       * Organize loops in a forest.
       */
      forest = noelle->organizeLoopsInTheirNestingForest(*ls_list);

      hot = noelle->getProfiles();

			map_loops_function(ls_list);
    //  abort();
      //loops_noelle();
      for (auto &F:M) {
        //avoid instrumenting this function as this is invoked at the very begining and there might be case that the globals have not yet been loaded, looks like an ordering issue
        //of globals.
        //https://lists.llvm.org/pipermail/llvm-dev/2020-June/142663.html
        if (F.empty() || !(F.getInstructionCount()) || (F.isIntrinsic())) {
          continue;
        }
        if (F.getName().str().find("cxx_global_var_init") == std::string::npos) {
          //if (F.getName().str().find("trip_duration") != std::string::npos) {
          runOnFunction(F);
          //	}
        }
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
