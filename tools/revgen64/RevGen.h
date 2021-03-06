///
/// Copyright (C) 2016, Dependable Systems Laboratory, EPFL
/// Copyright (C) 2014-2017, Cyberhaven
/// All rights reserved.
///
/// Licensed under the Cyberhaven Research License Agreement.
///

#ifndef REVGEN_H

#define REVGEN_H

#include <llvm/Support/Path.h>

#include <CFG/BinaryCFG.h>
#include <Translator/Translator.h>
#include <lib/Utils/Log.h>
#include <vmi/ExecutableFile.h>
#include <vmi/FileProvider.h>

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/DenseSet.h>

namespace s2etools {

class RevGen {
public:
    typedef llvm::DenseMap<uint64_t, llvm::BasicBlock *> LLVMBasicBlockMap;

    static void InjectArray(llvm::Module *m, const std::string &ptrName, const std::string &arrName,
                            llvm::SmallVector<llvm::Constant *, 4> &arrValues);

private:
    static LogKey TAG;

    std::string m_binaryFile;
    std::string m_bitcodeLibrary;

    std::shared_ptr<vmi::ExecutableFile> m_binary;
    std::shared_ptr<vmi::FileSystemFileProvider> m_fp;
    Translator *m_translator;

    llvm::BinaryFunctions m_functions;
    llvm::BinaryBasicBlocks m_bbs;
    llvm::DenseMap<uint64_t, TranslatedBlock *> m_tbs;
    llvm::DenseMap<uint64_t, llvm::Function *> m_llvmFunctions;

    void injectDataSections();
    llvm::Function *createLLVMPrototype(llvm::BinaryFunction *bf);
    void createFunctionPrototypes();
    void injectFunctionPointers();

    llvm::Constant *injectDataSection(const std::string &name, uint64_t va, uint8_t *data, unsigned size);
    void injectSections();
    void generateFunctionCall(TranslatedBlock *tb);
    llvm::Function *getCallMarker();
    llvm::Function *getIncompleteMarker();
    llvm::Function *getTraceFunction();
    void generateIncompleteMarker(llvm::IRBuilder<> &builder, uint64_t pc);
    void generateTrace(llvm::IRBuilder<> &builder, uint64_t pc);

    void generateIndirectJump(llvm::IRBuilder<> &builder, LLVMBasicBlockMap &allBbs, uint64_t bbPc);

    llvm::Function *reconstructFunction(llvm::BinaryFunction *bf);

    void enableLibraryCallDetector();
    void eraseTbFunctions();

public:
    RevGen(const std::string &binary, const std::string &bitcodeLibrary)
        : m_binaryFile(binary), m_bitcodeLibrary(bitcodeLibrary) {
        m_binary = NULL;
        m_fp = NULL;
        m_translator = NULL;
    }

    ~RevGen();

    bool initialize(void);

    void translate(const llvm::BinaryFunctions &functions, const llvm::BinaryBasicBlocks &tbs);

    TranslatedBlock *translate(uint64_t start, uint64_t end);

    void writeBitcodeFile(const std::string &bitcodeFile);

    std::shared_ptr<const vmi::ExecutableFile> getBinary() const {
        return m_binary;
    }
};
}

#endif
