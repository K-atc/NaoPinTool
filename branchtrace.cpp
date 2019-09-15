#include <stdio.h>
#include "pin.H"

KNOB<std::string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "branchtrace.out", "trace output file name");

FILE * trace;

VOID ImageLoad(IMG img, VOID *v) { 
    fprintf(trace, "{\"event\": \"image_load\", \"image_name\": \"%s\", \"image_id\": %d, \"base_addr\": \"%#lx\"}\n", IMG_Name(img).c_str(), IMG_Id(img), IMG_LowAddress(img));
}

VOID ProcessBranch(ADDRINT PC, bool BrTaken) {
    fprintf(trace, "{\"event\": \"branch\", \"inst_addr\": \"%#lx\", \"branch_taken\": %s},\n", PC, BrTaken ? "true" : "false");
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)  {
       if ( LEVEL_CORE::INS_IsBranch(ins) ) {
           INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) ProcessBranch, IARG_ADDRINT, INS_Address(ins), IARG_BRANCH_TAKEN, IARG_END);
       }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "{\"event\": \"exit\", \"exit_code\": %d}\n", code);
    fprintf(trace, "]\n");
    fclose(trace);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize tracer output file
    trace = fopen(KnobOutputFile.Value().c_str(), "w");
    fprintf(trace, "[\n");

    // Initialize symbol processing
    PIN_InitSymbols();
    
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // Register ImageLoad to be called when an image is loaded
    IMG_AddInstrumentFunction(ImageLoad, 0); 

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
