#include <stdio.h>
#include "pin.H"

FILE * trace;

// VOID * jcc_addr = 0;

// VOID jcc_before_handler(VOID *ip) { 
//     fprintf(trace, "before: %p\n", ip); 
//     jcc_addr = ip;
// }

// VOID jcc_fail_through_handler(VOID *ip) {
//     fprintf(trace, "{\"event\": \"branch\", \"inst_addr\": \"%p\", \"jump_taken\": false, \"next_inst_addr\": \"%p\"}\n", jcc_addr, ip);
// }

// VOID jcc_taken_handler(VOID *ip) {
//     // fprintf(trace, "taken: %p\n", ip); 
//     fprintf(trace, "{\"event\": \"branch\", \"inst_addr\": \"%p\", \"jump_taken\": true, \"next_inst_addr\": \"%p\"}\n", jcc_addr, ip);
// }

VOID ProcessBranch(ADDRINT PC, ADDRINT TargetPC, bool BrTaken) {
    // fprintf(trace, "{\"event\": \"branch\", \"inst_addr\": \"%#lx\", \"next_inst_addr\": \"%#lx\", \"jump_taken\": %s},\n", PC, TargetPC, BrTaken ? "true" : "false"); // TargetPC が次の命令のアドレスを返さない。
    fprintf(trace, "{\"event\": \"branch\", \"inst_addr\": \"%#lx\", \"jump_taken\": %s},\n", PC, BrTaken ? "true" : "false");
}

// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)  {
       if ( LEVEL_CORE::INS_IsBranch(ins) ) {
           INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR) ProcessBranch, IARG_ADDRINT, INS_Address(ins), IARG_ADDRINT, LEVEL_PINCLIENT::INS_NextAddress, IARG_BRANCH_TAKEN, IARG_END);
       }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
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
    trace = fopen("branchtrace.out", "w");
    fprintf(trace, "[\n");
    
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
