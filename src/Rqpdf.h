#ifndef RQPDF_H
#define RQPDF_H 1

#include <qpdf/QPDF.hh>
#include <qpdf/QPDFPageDocumentHelper.hh>
#include <qpdf/QPDFAcroFormDocumentHelper.hh>


#include <Rdefines.h>
#undef isNull 

SEXP QPDFObjectHandleToR(QPDFObjectHandle h, bool followGen = false, bool stripSlash = true, bool streamData = false);
SEXP convertQPDFDictToR(QPDFObjectHandle h, bool followGen = false, bool stripSlash = true, bool streamData = false);
SEXP R_qpdf_dictKeys(QPDFObjectHandle h);

QPDF *R_get_QPDF(SEXP);
#define GET_QPDF(x) (QPDF *) R_get_QPDF((x))


// make conditional
#define QPDFTypeEnum(ty) ::ty
//QPDFObject::ty

#ifndef PROBLEM

#define R_PROBLEM_BUFSIZE	4096
#define PROBLEM			{char R_problem_buf[R_PROBLEM_BUFSIZE];(snprintf)(R_problem_buf, R_PROBLEM_BUFSIZE,
#define ERROR			),Rf_error(R_problem_buf);}
#define WARNING(x)		),Rf_warning(R_problem_buf);}
#define WARN			WARNING(NULL)


#endif

#endif
