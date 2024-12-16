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

	if (!Preheader || !ExitBlock) {
        errs() << "Loop does not have a preheader or an exit block!\n";
        return false;
    	}

	 // Step 2: Insert the loop guard into the preheader block
	IRBuilder<> Builder(Preheader->getTerminator()); // Insert before the terminator
	

	// Assume 'j' is the induction variable (PHI node) in the loop header
	

	Instruction *Terminator = Header->getTerminator();

        BasicBlock *Latch = L->getLoopLatch();

        // Check basic conditions for loop rotation
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
/*
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
        }

        return false;
		
    }
};

} // end anonymous namespace

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
