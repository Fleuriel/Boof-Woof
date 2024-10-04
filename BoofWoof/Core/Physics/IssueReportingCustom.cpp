#include <Jolt/Jolt.h>
#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

// IssueReportingCustom.cpp

#include "IssueReportingCustom.h"

namespace JPH {

    // Define the extern variables declared in IssueReporting.h
    TraceFunction Trace = MyTrace;
    AssertFailedFunction AssertFailed = MyAssertFailed;

    // Implement the custom trace function
    void MyTrace(const char* inFMT, ...) {
        va_list args;
        va_start(args, inFMT);
        vprintf(inFMT, args);
        va_end(args);
        printf("\n"); // Add a newline for readability
    }

    // Implement the custom assert failed function
    bool MyAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine) {
        // Log the assertion failure
        printf("Assertion failed!\n");
        if (inExpression)
            printf("Expression: %s\n", inExpression);
        if (inMessage)
            printf("Message: %s\n", inMessage);
        if (inFile)
            printf("File: %s\n", inFile);
        printf("Line: %u\n", inLine);

        // Optionally, you can abort the program or trigger a breakpoint
        // For now, we'll abort
        std::abort();
        return true; // Returning true will trigger a breakpoint if a debugger is attached
    }

}

//namespace JPH {
//
//    // Custom AssertFailed function
//    bool AssertFailed(const char* inExpression, const char* inMessage, const char* inFile, uint inLine) {
//        // Log assertion failure details
//        std::cerr << "Assertion failed!\n"
//            << "Expression: " << (inExpression ? inExpression : "") << "\n"
//            << "Message: " << (inMessage ? inMessage : "") << "\n"
//            << "File: " << (inFile ? inFile : "") << "\n"
//            << "Line: " << inLine << std::endl;
//
//        // Decide whether to abort or continue (you can trigger a breakpoint if in debug mode)
//        std::abort();  // Aborts the program. Replace this with 'return true;' if you want to trigger a breakpoint instead.
//        return false;  // Return true to trigger a breakpoint
//    }
//
//    // Custom Trace function for logging messages
//    void CustomTrace(const char* inFMT, ...) {
//        va_list args;
//        va_start(args, inFMT);
//
//        // Use vprintf to handle variadic arguments and log to console
//        vprintf(inFMT, args);
//        printf("\n");
//
//        va_end(args);
//    }
//
//}