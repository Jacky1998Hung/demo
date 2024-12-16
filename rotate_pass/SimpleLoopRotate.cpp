#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Dominators.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"

using namespace llvm;

void AnalyzeLoopHeader(Loop *L) {
    // Step 1: Get the header block
    BasicBlock *Header = L->getHeader();
    errs() << "Analyzing Loop Header: " << Header->getName() << "\n";

    // Step 2: Get the terminator instruction
    Instruction *Terminator = Header->getTerminator();

    // Step 3: Check for a conditional branch
    if (BranchInst *BI = dyn_cast<BranchInst>(Terminator)) {
        if (BI->isConditional()) {
            Value *Condition = BI->getCondition();
            errs() << "Loop Condition: ";
            Condition->print(errs());
            errs() << "\n";

            // Step 4: Inspect the condition (ICmpInst)
            if (ICmpInst *Cmp = dyn_cast<ICmpInst>(Condition)) {
                errs() << "Comparison Predicate: " << Cmp->getPredicate() << "\n";
                errs() << "LHS: ";
                Cmp->getOperand(0)->print(errs());
                errs() << "\nRHS: ";
                Cmp->getOperand(1)->print(errs());
                errs() << "\n";

            }
        }
    } else {
        errs() << "Header does not contain a conditional branch!\n";
    }
}

Value* ReturnInductionVariable(Instruction *Terminator) {
    if (BranchInst *BI = dyn_cast<BranchInst>(Terminator)) {
        if (BI->isConditional()) {
            Value *Condition = BI->getCondition();

            // Step 4: Inspect the condition (ICmpInst)
            if (ICmpInst *Cmp = dyn_cast<ICmpInst>(Condition)) {

                // Check if LHS is a load instruction
                if (LoadInst *Load = dyn_cast<LoadInst>(Cmp->getOperand(0))) {
                // Retrieve the pointer operand from the load instruction
                Value *Ptr = Load->getPointerOperand();

                errs() << "Pointer Operand of Load: ";
                Ptr->print(errs());
                errs() << "\n";
		return Ptr;
                } else {
                errs() << "LHS is not a load instruction.\n";
                }
            }
        }
    } else {
        errs() << "Header does not contain a conditional branch!\n";
    	}
    return nullptr;
}    

void CheckLoopBlocks(Loop *L) {
    // Step 1: Get the loop header
    BasicBlock *Header = L->getHeader();
    errs() << "Loop Header: " << Header->getName() << "\n";

    // Step 2: Get the loop body block
    BasicBlock *LoopBody = nullptr;

    if (BranchInst *BI = dyn_cast<BranchInst>(Header->getTerminator())) {
        // Assuming the first successor is the loop body
        LoopBody = BI->getSuccessor(0);
    }

    if (LoopBody) {
        errs() << "Loop Body Block: " << LoopBody->getName() << "\n";
    } else {
        errs() << "Error: Could not determine the loop body block.\n";
    }

    // Step 3: Get the loop exit block
    BasicBlock *ExitBlock = L->getExitBlock();

    if (ExitBlock) {
        errs() << "Loop Exit Block: " << ExitBlock->getName() << "\n";
    } else {
        errs() << "Error: Loop does not have a single exit block.\n";
    }

    // Step 4: Print all the blocks for verification
    errs() << "\n--- Loop PreHeader Block ---\n";
    BasicBlock *Preheader = L->getLoopPreheader();

    Preheader->print(errs());
    errs() << "\n--- Loop Header Block ---\n";
    Header->print(errs());

    if (LoopBody) {
        errs() << "\n--- Loop Body Block ---\n";
        LoopBody->print(errs());
    }

    BasicBlock *Latch = L->getLoopLatch();
    if (Latch) {                                                                                                       
        errs() << "\n--- Loop Latch Block ---\n";                                                                                             	     
	Latch->print(errs());
    }

    if (ExitBlock) {
        errs() << "\n--- Loop Exit Block ---\n";
        ExitBlock->print(errs());
    }
}

namespace {
class SimpleLoopRotate : public PassInfoMixin<SimpleLoopRotate> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        auto &LI = AM.getResult<LoopAnalysis>(F);
        DominatorTree &DT = AM.getResult<DominatorTreeAnalysis>(F);
        bool Changed = false;

        SmallVector<Loop *> Loops = LI.getLoopsInPreorder();

        for (Loop *L : Loops) {
	    errs() << "Loop find" << "\n";
            if (rotateLoop(L, LI, DT))
                Changed = true;
        }

        return Changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }

private:
    bool rotateLoop(Loop *L, LoopInfo &LI, DominatorTree &DT) {
	// Step 1: Get the loop preheader, header, and exit block
	BasicBlock *Preheader = L->getLoopPreheader();
	AnalyzeLoopHeader(L);
        BasicBlock *Header = L->getHeader();

	BasicBlock *ExitBlock = L->getExitBlock();
	
	CheckLoopBlocks(L);

	BasicBlock *LoopBody = nullptr;
	BranchInst *BI = nullptr;
    if (BI = dyn_cast<BranchInst>(Header->getTerminator())) {
        // Assuming the first successor is the loop body
        LoopBody = BI->getSuccessor(0);
	}
    	errs() << "=======================Header's Terminator=========================\n";
    	BI->print(errs());
	errs() << "\n";


	if (!Preheader || !ExitBlock) {
        errs() << "Loop does not have a preheader or an exit block!\n";
        return false;
    	}

	// Assume 'j' is the pointer to a induction variable in the loop header
	Instruction *Terminator = Header->getTerminator();
	// Step 2: Insert the loop guard into the preheader block
        IRBuilder<> Builder(Preheader->getTerminator()); // Insert before the terminator

	Value* IndPtr = ReturnInductionVariable(Terminator);

	if (!IndPtr) {
        	errs() << "No induction pointer found in the loop header!\n";
        	return false;
    	}
	
	// As IndPtr is a pointer, the real value we need is store in that address
	// Thus, create a load instruction first
	LLVMContext &Context = Builder.getContext();
	Value *LoadedIndVar = Builder.CreateLoad(Type::getInt32Ty(Context), IndPtr, "indvar.load");
	errs() << "Created Load Instruction: ";
	LoadedIndVar->print(errs());
	errs() << "\n";

	// Then we create the condition instruction for LoadedIndVar and int (4)
	Value *Condition = Builder.CreateICmpSLT(LoadedIndVar, ConstantInt::get(LoadedIndVar->getType(), 4), "guard.cond");
	errs() << "Created Branch/Terminator Instruction: ";
	Condition->print(errs());
	errs() << "\n";
	

	CheckLoopBlocks(L);
	
	BasicBlock *Latch = L->getLoopLatch();
	
	for (Instruction &I : *Latch) {
    		if (BranchInst *BI = dyn_cast<BranchInst>(&I)) {
        		if (BI->getSuccessor(0) == Header) {
            		// Update the branch to point to the new block (e.g., PreHeader)
            		BI->setSuccessor(0, LoopBody);
            		break;
        		}
    		}
	}

	Instruction *PreheaderTerminator = Preheader->getTerminator();	
        // Make sure PreheaderTerminator is valid
        if (!PreheaderTerminator) {
                errs() << "Error: Preheader has no terminator!\n";
                return false;
        }
	//Finally we add conditional branch, T to LoopBody; F to ExitBlock
        Builder.SetInsertPoint(PreheaderTerminator);
        Builder.CreateCondBr(Condition, LoopBody, ExitBlock);

        PreheaderTerminator->eraseFromParent();
	
	Header->eraseFromParent();
	
	//Enter Latch Block for creating branches to exit block
	//Very similar with the implementation of preheaderblock
	Instruction *LatchTerminator = Latch->getTerminator();
	IRBuilder<> BuilderForLatch(Latch->getTerminator()); // Insert before the terminator
	LLVMContext &LatchContext = BuilderForLatch.getContext();
        Value *LoadedLatchIndVar = BuilderForLatch.CreateLoad(Type::getInt32Ty(LatchContext), IndPtr, "latchindvar.load");
	errs() << "Created Load Instruction: ";
        LoadedIndVar->print(errs());
        errs() << "\n";

	// Then we create the condition instruction for LoadedIndVar and int (4)
        Value *LatchCondition = BuilderForLatch.CreateICmpSLT(LoadedLatchIndVar, ConstantInt::get(LoadedLatchIndVar->getType(), 4), "latchguard.cond");
        errs() << "Created Branch/Terminator Instruction: ";
        Condition->print(errs());
        errs() << "\n";

	BuilderForLatch.SetInsertPoint(LatchTerminator);
	BuilderForLatch.CreateCondBr(LatchCondition, LoopBody, ExitBlock);

        LatchTerminator->eraseFromParent();

//	CheckLoopBlocks(L);
	/*
        // Check basic conditions for loop rotation
	BasicBlock *Latch = L->getLoopLatch();
        if (!Latch || Latch->getTerminator()->getNumSuccessors() != 1) {
            return false; // Can't rotate non-single exit latch or malformed latch
        }

        // Make sure the loop is a proper rotate candidate
        if (Header->hasNPredecessorsOrMore(2)) {
            // Identify new header
            BasicBlock *PreHeader = L->getLoopPreheader();
            if (!PreHeader) {
                return false; // No preheader, rotation not possible without further adjustments
            }

        // Create a new latch block by splitting the old latch
        BasicBlock *NewLatch = SplitBlock(Latch, Latch->getTerminator(), &DT, &LI);

        // Update loop information
        L->addBasicBlockToLoop(NewLatch, LI);

	// Update loop latch manually (no setLoopLatch)
	BasicBlock *OldLatch = L->getLoopLatch();
		if (OldLatch != nullptr) {
    			OldLatch->replaceAllUsesWith(NewLatch); // Redirect all uses to NewLatch
		}		

	// Optional: Move Latch to loop header if needed
	if (Latch != nullptr && L->getHeader() != Latch) {
    		L->moveToHeader(Latch);
	}

            // Update the dominator tree
            DT.changeImmediateDominator(NewLatch, Latch);
            DT.changeImmediateDominator(Latch, PreHeader);

            return true;
*/
	return true;	
    }
    };
}

// Registration of the pass
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "SimpleLoopRotate", LLVM_VERSION_STRING,
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "simple-loop-rotate") {
                        FPM.addPass(SimpleLoopRotate());
                        return true;
                    }
                    return false;
                });
        }
    };
}
