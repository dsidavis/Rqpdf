#include "Rqpdf.h"

QPDF *R_get_QPDF(SEXP r_qpdf);
#define GET_QPDF(x) (QPDF *) R_get_QPDF((x))

extern "C"
SEXP
R_getRoot(SEXP r_filename)
{
   QPDF qpdf;
   qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
   QPDFObjectHandle r = qpdf.getRoot();
   return(QPDFObjectHandleToR(r, true));
}

extern "C"
SEXP
R_getRoot_QPDF(SEXP r_qpdf, SEXP r_root, SEXP streamData)
{
   QPDF *qpdf = GET_QPDF(r_qpdf);    
   QPDFObjectHandle r = LOGICAL(r_root)[0] ? qpdf->getRoot() : qpdf->getTrailer();
   return(QPDFObjectHandleToR(r, true, true, LOGICAL(streamData)[0]));
}


extern "C"
SEXP
R_getTrailer(SEXP r_filename)
{
   QPDF qpdf;
   qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
   QPDFObjectHandle r = qpdf.getTrailer();
   return(QPDFObjectHandleToR(r, false));
}

void
R_freeQPDF(SEXP v)
{
    QPDF *q = (QPDF *) R_ExternalPtrAddr(v);
    if(q)
        delete q;
}

extern "C"
SEXP
R_getQPDF(SEXP r_filename)
{
    QPDF *qpdf;
    qpdf = new QPDF();
    if(Rf_length(r_filename))
        qpdf->processFile(CHAR(STRING_ELT(r_filename, 0)));

    SEXP ans;
    ans = R_MakeExternalPtr(qpdf, Rf_install("QPDF"), R_NilValue);
    PROTECT(ans);
    R_RegisterCFinalizer(ans, R_freeQPDF);
    UNPROTECT(1);
    return(ans);
}


extern "C"
SEXP
R_qpdf_processFile(SEXP r_qpdf, SEXP r_filename, SEXP r_password)
{
    QPDF *qpdf = GET_QPDF(r_qpdf);
    const char *passwd = NULL;
    if(Rf_length(r_password))
        passwd = CHAR(STRING_ELT(r_password, 0));
    
    qpdf->processFile(CHAR(STRING_ELT(r_filename, 0)), passwd);
    return(R_NilValue);
}


extern "C"
SEXP
R_qpdf_getFilename(SEXP r_qpdf)
{
    QPDF *qpdf = GET_QPDF(r_qpdf);
    std::string sstr = qpdf->getFilename();
    const char * str = sstr.c_str();
    return(ScalarString(str ? mkChar(str) : R_NaString));
}


extern "C"
SEXP
R_qpdf_setSuppressWarnings(SEXP r_qpdf, SEXP val)
{
    QPDF *qpdf = GET_QPDF(r_qpdf);   
    qpdf->setSuppressWarnings(LOGICAL(val)[0]);
    return(R_NilValue);
}

SEXP
R_mkQPDFExc(QPDFExc w)
{
    SEXP ans, names;
    int idx = 0;

    PROTECT(ans = NEW_LIST( 4 ));
    PROTECT(names = NEW_CHARACTER(4));
    SET_VECTOR_ELT(ans, idx, ScalarString(mkChar(w.getMessageDetail().c_str())));
    SET_STRING_ELT(names, idx++, mkChar("message"));
    SET_VECTOR_ELT(ans, idx, ScalarString(mkChar(w.getFilename().c_str())));
    SET_STRING_ELT(names, idx++, mkChar("filename"));    
    SET_VECTOR_ELT(ans, idx, ScalarString(mkChar(w.getObject().c_str())));
    SET_STRING_ELT(names, idx++, mkChar("object"));    
    SET_VECTOR_ELT(ans, idx, ScalarReal(w.getFilePosition()));
    SET_STRING_ELT(names, idx++, mkChar("filePosition"));
    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}

extern "C"
SEXP
R_qpdf_getWarnings(SEXP r_qpdf)
{
    QPDF *qpdf = GET_QPDF(r_qpdf);
    std::vector<QPDFExc> w = qpdf->getWarnings();
    SEXP ans;
    PROTECT(ans = NEW_LIST(w.size()));
    
    for(int i = 0; i < w.size(); i++) 
        SET_VECTOR_ELT(ans, i, R_mkQPDFExc(w[i]));

    UNPROTECT(1);
    return(ans);
}


extern "C"
SEXP
R_QPDFObjectHandle_getKeys(SEXP r_obj)
{
    QPDFObjectHandle *p = (QPDFObjectHandle *) R_ExternalPtrAddr(r_obj);
    if(p->getTypeCode() != QPDFObject::ot_dictionary) {
        PROBLEM "QPDF reference is not a dictionary, but a %s", p->getTypeName()
            ERROR;
    }
    return(R_qpdf_dictKeys(*p));
}


SEXP
R_qpdf_dictKeys(QPDFObjectHandle h)
{
    std::set<std::string> keys = h.getKeys();
    int len = keys.size(), i = 0;
    SEXP names;
    PROTECT(names = NEW_CHARACTER(len));
    std::set<std::string>::iterator it = keys.begin();
    for( ; i < len; i++, ++it) 
        SET_STRING_ELT(names, i, mkChar(it->c_str()));
    
    UNPROTECT(1);
    return(names);
}


extern "C"
SEXP
R_getObjectByID(SEXP r_qpdf, SEXP id, SEXP streamData, SEXP asRef)
{
    QPDF *qpdf = GET_QPDF(r_qpdf);
    QPDFObjectHandle o;
    o = qpdf->getObjectByID(INTEGER(id)[0], INTEGER(id)[1]);
    if(asLogical(asRef)) {
        QPDFObjectHandle *r;
        r = new QPDFObjectHandle(o);
        //XXX  Need to put a finalizer on it.
        return(R_MakeExternalPtr(r, Rf_install("QPDFObjectHandle"), R_NilValue));
    } else
        return(QPDFObjectHandleToR(o, true, true, LOGICAL(streamData)[0]));
}

QPDF *
R_get_QPDF(SEXP r_qpdf)
{
    if(TYPEOF(r_qpdf) == S4SXP) 
        r_qpdf = GET_SLOT(r_qpdf, Rf_install("ref"));
    if(TYPEOF(r_qpdf) != EXTPTRSXP) {
        PROBLEM "need an external pointer or S4 object with a 'ref' slot for a QPDF external object"
         ERROR;
    }
        
    QPDF *q = (QPDF*) R_ExternalPtrAddr(r_qpdf);
    return(q);
}
