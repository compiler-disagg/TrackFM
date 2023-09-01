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

  static std::string CARM_INIT             = "_Z17init_carm_runtimev";
  static std::string CARM_GET_ARGS             = "_Z8get_argsv";
  static std::string CARM_GET_ARGC             = "_Z13get_arg_countv";
  static std::string CARM_WORK             = "_Z7do_workPv";
  static std::string CARM_TRACE =	"_Z10carm_tracev";
  struct CAT : public ModulePass {
    static char ID;
    std::unordered_map<Function *, std::set<Instruction *>> allocas;
    std::unordered_map<Function *, std::set<Value *>> arguments;
    std::unordered_map<Function *, std::vector<Instruction *>> geps;
    std::unordered_map<Function *, std::vector<Instruction *>> loads;
    std::unordered_map<Function *, std::vector<Instruction *>> stores;
    std::unordered_map<Function *, std::vector<Instruction *>> instructions;
    std::unordered_map<Function *, std::vector<Instruction *>> call_instructions;
    std::unordered_map<BasicBlock *, std::pair<Instruction *, Instruction *>> basic_blocks_to_instrument;
    std::unordered_map<Function *, std::vector<Value *>> carm_checks;
    std::unordered_map<Value *, Value *> carm_check_ptr;
    std::vector<Instruction *> init;
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
          instructions[&F].push_back(&ins);
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


    void parse_basic_blocks(Function &F) {
      if (!F.doesNotRecurse()) {
        errs()<<"Recursive function found "<<F.getName()<<"\n";
      }
      bool interested = false;
      for (BasicBlock &BB : F) {
        interested = false;
        for (Instruction &ins : BB) {
          if (isa<LoadInst>(&ins)) {
            interested = true;
          }
          else if (isa<StoreInst>(&ins)) {
            interested = true;
          }
        }
        if (interested) {
          if (BB.getFirstNonPHI() && (!BB.isLandingPad())) { 
            basic_blocks_to_instrument[&BB] = std::make_pair (BB.getFirstNonPHI(), BB.getTerminator());	
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


    bool runOnFunction(Function &F) {
      //std::vector<Value *> args;
      BasicBlock *EntryBlock = &(F.getEntryBlock());
      Instruction *InsertionPoint = EntryBlock->getFirstNonPHI();
      if (InsertionPoint == nullptr) {
        errs()<<"No insertion point found\n";
        abort(); // Serious
      }
      //args.push_back(F.arg_begin());

      Value * carm_check_call = create_function(CARM_INIT, VoidType, InsertionPoint);



#ifdef TRACKFM_TRACE
      errs()<<"TrackFM trace enabled \n";


      for (auto &BB:F) {
	      for (auto &I:BB) {

		      if (isa<ReturnInst>(&I)) {
			      carm_check_call = create_function(CARM_TRACE, VoidType,  static_cast<Instruction *>(&I));
		      }
	      }
      }


#endif

      return true;
    }


		Function * clone_function(Function & F) {
		
		ValueToValueMapTy ValueMap;
		std::vector<Type*> TP; 

		TP.push_back(VoidPointerType);

		FunctionType *NewFTy = FunctionType::get(VoidType, TP, false);


		Function *NewF = Function::Create(NewFTy,
			F.getLinkage(),
		  CARM_WORK,
			currentModule);

		
			NewF->getBasicBlockList().splice(NewF->begin(), F.getBasicBlockList());

			std::vector<Instruction *> delete_ret;
			for (auto &BB:*NewF) {
				for (auto &I:BB) {

					if (isa<ReturnInst>(&I)) {

						IRBuilder<> builder(static_cast<ReturnInst *>(&I));

						Instruction * st = static_cast<Instruction *>(builder.CreateRetVoid());

						delete_ret.push_back(static_cast<ReturnInst *>(&I));
					}

				}

			}			

			for (auto i:delete_ret) {
				i->eraseFromParent();
			}

			NewF->dump();

			return NewF;
		}

		bool transform_main_function(Function &F) {
			std::vector<Value *> args;

			BasicBlock *EntryBlock = &(F.getEntryBlock());
			Instruction *InsertionPoint = EntryBlock->getFirstNonPHI();
			if (InsertionPoint == nullptr) {
				errs()<<"No insertion point found\n";
				abort(); // Serious
			}
			IRBuilder<> builder(InsertionPoint);
			//args.push_back(F.arg_begin());
			for(Function::arg_iterator II = F.arg_begin(); 
					II != F.arg_end(); II++) {
				args.push_back(II);
			}

			Value * carm_arg_count = create_function(CARM_GET_ARGC, args[0]->getType(), InsertionPoint);


			Value * carm_args = create_function(CARM_GET_ARGS, args[1]->getType(), InsertionPoint);



			args[0]->replaceAllUsesWith(carm_arg_count);
			args[1]->replaceAllUsesWith(carm_args);

			Function * carm_work = clone_function(F);


			runOnFunction(*carm_work);
			
			//Instruction * a0 = static_cast<Instruction*>(args[0]);
			//Instruction * a1 = static_cast<Instruction*>(args[1]);
			//a0->eraseFromParent();
			//a1->eraseFromParent();
			errs()<<"erase complete\n";
			F.dump();

      return true;
    }
		void create_fake_main_for_noelle() {
					Type* func_args[] = {VoidPointerType};
					std::vector<Value *> args;
          FunctionCallee mul_add_fun = currentModule->getOrInsertFunction("main", Int32Type);
          Function *mul_add = cast<Function> (mul_add_fun.getCallee());
          BasicBlock *block = BasicBlock::Create(currentModule->getContext(), "entry", mul_add);
          IRBuilder<> builder(block);
          Instruction * st = static_cast<Instruction *>(builder.CreateRet (ConstantInt::get(Int32Type, 0, false)));
          args.push_back(ConstantPointerNull::get(PointerType::get(Int8Type, 0)));

          Value * carm_check_call = create_function(func_args, CARM_WORK, args, VoidType, st);

		}

    bool runOnModule(Module &M) override {

			Function * mf;
			Function * carm_work;
			bool manual_instrument = false;
			
      for (auto &F:M) { 

        if (F.getName().str().compare("main") == 0) { 
			
						mf = &F;

				}
				else if (F.getName().str().compare(CARM_WORK) == 0) {
					manual_instrument = true;
					carm_work = &F;
					break;
				}
			}

			if (!manual_instrument) { 
				transform_main_function(*mf);

				mf->eraseFromParent();

				create_fake_main_for_noelle();

			}
			else {
				runOnFunction(*carm_work);
			}
		
      return true;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequired<LoopInfoWrapperPass>();
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
