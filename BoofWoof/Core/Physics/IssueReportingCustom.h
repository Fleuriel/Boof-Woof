// IssueReportingCustom.h

#pragma once

namespace JPH {

	// Custom trace function
	void MyTrace(const char* inFMT, ...);

	// Custom assert failed function
	bool MyAssertFailed(const char* inExpression, const char* inMessage, const char* inFile, unsigned int inLine);

}