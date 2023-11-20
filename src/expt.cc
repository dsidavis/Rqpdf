#include "Rqpdf.h"

#define NATIVE_CATCH_QPDF_WARNINGS 1

void suppressPDFWarnings(QPDF *qpdf)
{
#ifdef NATIVE_CATCH_QPDF_WARNINGS
    qpdf->setSuppressWarnings(true);
#endif    
}

void showPDFWarnings(QPDF *qpdf)
{
#ifdef NATIVE_CATCH_QPDF_WARNINGS
    qpdf->setSuppressWarnings(false);
    if(qpdf->anyWarnings()) {
        std::vector<QPDFExc> warnings = qpdf->getWarnings();
        for(size_t i = 0; i < warnings.size(); i++) {
            QPDFExc w = warnings[i];
            PROBLEM "%s", w.getMessageDetail().c_str()
                WARN;
        }
    }
#endif    
}




/* Are these used?  If they are, they may have a problem as the QPDF will be
   destroyed at the end of the routine and if the QPDFObjectHandleToR
  returns any external pointers, they would be pointing to something that is destroyed.
*/
extern "C"
SEXP
R_getRoot(SEXP r_filename)
{
   QPDF qpdf;
   suppressPDFWarnings(&qpdf);
      qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
      QPDFObjectHandle r = qpdf.getRoot();
   showPDFWarnings(&qpdf);
   return(QPDFObjectHandleToR(r, true));
}

extern "C"
SEXP
R_getTrailer(SEXP r_filename)
{
   QPDF qpdf;
   suppressPDFWarnings(&qpdf);
     qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));
     QPDFObjectHandle r = qpdf.getTrailer();
   showPDFWarnings(&qpdf);      
   return(QPDFObjectHandleToR(r, false));
}



extern "C"
SEXP
R_getRoot_QPDF(SEXP r_qpdf, SEXP r_root, SEXP streamData)
{
   QPDF *qpdf = GET_QPDF(r_qpdf);    
   QPDFObjectHandle r;
/*¿
     SEXP cont = PROTECT(R_MakeUnwindCont());
     and use R_ContinueUnwind(cont); in the catch()?
*/
   try {
       r = LOGICAL(r_root)[0] ? qpdf->getRoot() : qpdf->getTrailer();
   } catch (std::exception &ex) {
        PROBLEM "failed to get %s in PDF document '%s'", LOGICAL(r_root)[0] ? "root" : "trailer",
                                                             qpdf->getFilename().c_str()
           ERROR ;
   }
   
   return(QPDFObjectHandleToR(r, true, true, LOGICAL(streamData)[0]));
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
R_getQPDF(SEXP r_filename, SEXP r_description, SEXP r_passwd)
{
    QPDF *qpdf;        
    qpdf = new QPDF();    

    if(Rf_length(r_filename)) {
            
        suppressPDFWarnings(qpdf);                    
        try {

            if(TYPEOF(r_filename) == STRSXP)
                qpdf->processFile(CHAR(STRING_ELT(r_filename, 0)));
            else if(TYPEOF(r_filename) == RAWSXP) {
                char const *buf = (char const *) RAW(r_filename);
                qpdf->processMemoryFile(CHAR(STRING_ELT(r_description, 0)), buf, Rf_length(r_filename), NULL);                
            } else {
                PROBLEM "wrong argument type for r_filename in R_getQPDF"
                    ERROR;
            }
            
        } catch(std::exception &e) {
            showPDFWarnings(qpdf);      
            PROBLEM "failed in processFile() for PDF document '%s'",
                (TYPEOF(r_filename) == STRSXP) ? CHAR(STRING_ELT(r_filename, 0)) :  CHAR(STRING_ELT(r_description, 0))
            ERROR
        }

        showPDFWarnings(qpdf);              
    }
    
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

    try {
        qpdf->processFile(CHAR(STRING_ELT(r_filename, 0)), passwd);
    } catch(std::exception &e) {
        PROBLEM "failed in processFile() for PDF document '%s'", CHAR(STRING_ELT(r_filename, 0))
            ERROR
    }
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
    if(p->getTypeCode() != QPDFTypeEnum(ot_dictionary)) { // QPDFObject::ot_dictionary
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


extern "C"
SEXP
R_getAllObjects(SEXP r_qpdf, SEXP r_streamData)
{
   QPDF *qpdf = GET_QPDF(r_qpdf);

   std::vector<QPDFObjectHandle> els = qpdf->getAllObjects();
   
   SEXP ans;
   PROTECT(ans = NEW_LIST(els.size()));
   for(int i = 0; i < els.size(); i++) {
       QPDFObjectHandle h = els[i];
       SET_VECTOR_ELT(ans, i, QPDFObjectHandleToR(h, true, true, LOGICAL(r_streamData)[0]));
   }

   // names on elements.
   UNPROTECT(1);
   return(ans);
}
