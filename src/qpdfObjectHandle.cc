#include "Rqpdf.h"


QPDFObjectHandle RToQPDFObjectHandle(SEXP robj);

QPDFObjectHandle
RToQPDFArray(SEXP lvals, R_len_t len)
{
    QPDFObjectHandle ans = QPDFObjectHandle::newArray();

    QPDFObjectHandle el;
    SEXP robj = lvals;
    for(int i = 0; i < len; i++) {
        switch(TYPEOF(lvals)) {
        case VECSXP:
            el = RToQPDFObjectHandle(VECTOR_ELT(lvals, i));
            break;
        case REALSXP:
            el = QPDFObjectHandle::newReal(REAL(robj)[i]);
            break;
        case INTSXP:
            el = QPDFObjectHandle::newInteger(INTEGER(robj)[i]);
            break;
        case LGLSXP:
            el = QPDFObjectHandle::newBool(LOGICAL(robj)[i]);
            break;
        case STRSXP:
            //XXX newUnicodeString
            el = QPDFObjectHandle::newString(std::string( CHAR(STRING_ELT(robj, i))));
            break;
        default:
            PROBLEM "Don't yet convert elements of R type vector %d to PDF", TYPEOF(lvals)
                ERROR;
        }
        ans.insertItem(i, el);
    }
    return(ans);
}


QPDFObjectHandle
RToQPDFDict(SEXP lvals, SEXP names, R_len_t len)
{
    QPDFObjectHandle ans = QPDFObjectHandle::newDictionary();

    QPDFObjectHandle el;
    SEXP robj = lvals;
    for(int i = 0; i < len; i++) {
        switch(TYPEOF(lvals)) {
        case VECSXP:
            el = RToQPDFObjectHandle(VECTOR_ELT(lvals, i));
            break;
        case REALSXP:
            el = QPDFObjectHandle::newReal(REAL(robj)[i]);
            break;
        case INTSXP:
            el = QPDFObjectHandle::newInteger(INTEGER(robj)[i]);
            break;
        case LGLSXP:
            el = QPDFObjectHandle::newBool(LOGICAL(robj)[i]);
            break;
        case STRSXP:
            //XXX newUnicodeString
            el = QPDFObjectHandle::newString(std::string( CHAR(STRING_ELT(robj, i))));
            break;
        default:
            PROBLEM "Don't yet convert elements of R type vector %d to PDF", TYPEOF(lvals)
                ERROR;
        }
        std::string key = std::string(CHAR(STRING_ELT(names, i)));
        ans.replaceKey(key, el);
    }
    return(ans);
}




QPDFObjectHandle 
RToQPDFObjectHandle(SEXP robj)
{
    int len = Rf_length(robj);
    QPDFObjectHandle ans;

    if(len == 0)
        return(QPDFObjectHandle::newNull());

    if(len == 1) {
        switch(TYPEOF(robj)) {
        case REALSXP:
            ans = QPDFObjectHandle::newReal(REAL(robj)[0]);
            break;
        case INTSXP:
            ans = QPDFObjectHandle::newInteger(INTEGER(robj)[0]);
            break;
        case LGLSXP:
            ans = QPDFObjectHandle::newBool(LOGICAL(robj)[0]);
            break;
        case STRSXP:
            //XXX newUnicodeString
            ans = QPDFObjectHandle::newString(std::string( CHAR(STRING_ELT(robj, 0))));
            break;
#if 0            
        case SYMSXP:
            ans = QPDFObjectHandle::newName(std::string( SYMVALUE(robj)));
            break;
#endif            
        case VECSXP:
            PROBLEM "list of length 1 to PDF object"
                ERROR;            
            break;
        default:
            PROBLEM "don't yet know how to convert this R type (%d) to PDF object", TYPEOF(robj)
                ERROR;
        }
    } else {
    
    
        SEXP names = GET_NAMES(robj);
        if(Rf_length(names))
            ans = RToQPDFDict(robj, names, len);
        else
            ans = RToQPDFArray(robj, len);
#if 0            
        switch(TYPEOF(robj)) {
        case VECSXP:            
            break;
        default:
            PROBLEM "not converting R object with names to PDF yet"
                ERROR;
            break;
        }
#endif        
    }
    
    return(ans);
}


extern "C"
SEXP
R_RToQPDFObjectHandle(SEXP robj)
{
    QPDFObjectHandle ans = RToQPDFObjectHandle(robj);
    QPDFObjectHandle *ret = new QPDFObjectHandle(ans);
    return(R_MakeExternalPtr(ret, Rf_install("QPDFObjectHandle"), R_NilValue));
}


extern "C"
SEXP
R_QPDFObjectHandle_getTypeName(SEXP r_obj)
{
    QPDFObjectHandle *obj = (QPDFObjectHandle *) R_ExternalPtrAddr(r_obj);
    return(ScalarString(mkChar(obj->getTypeName())));
}



extern "C"
SEXP
R_QPDFDict_replaceKey(SEXP r_dict, SEXP r_key, SEXP r_obj)
{
    QPDFObjectHandle *dict = (QPDFObjectHandle *) R_get_QPDF(r_dict);
    QPDFObjectHandle *obj = (QPDFObjectHandle *) R_get_QPDF(r_obj);
    dict->replaceKey(std::string(CHAR(STRING_ELT(r_key, 0))), *obj);

    return(R_NilValue);
}

/*
  Very specific for now.
  Takes a list of pairs of elements
 */
extern "C"
SEXP
R_QPDF_replaceKey_with2StrArray(SEXP r_dict, SEXP r_key, SEXP r_vals)
{
    int len = Rf_length(r_vals);

    QPDFObjectHandle ans = QPDFObjectHandle::newArray();

    QPDFObjectHandle el;
    SEXP rel;
    for(int i = 0; i < len; i++) {
        el = QPDFObjectHandle::newArray();
        rel = VECTOR_ELT(r_vals, i);
        el.insertItem(0, QPDFObjectHandle::newString( std::string(CHAR(STRING_ELT(rel, 0)))));
        el.insertItem(1, QPDFObjectHandle::newString( std::string(CHAR(STRING_ELT(rel, 1)))));
        ans.insertItem(i, el);
    }
    
    QPDFObjectHandle *dict = (QPDFObjectHandle *) R_ExternalPtrAddr(r_dict);
    dict->replaceKey(std::string(CHAR(STRING_ELT(r_key, 0))), ans);

    return(ScalarLogical(1));
}



extern "C"
SEXP
R_QPDF_replaceKey_NamedCharacter(SEXP r_dict, SEXP r_key, SEXP r_vals)
{
    int len = Rf_length(r_vals);

    QPDFObjectHandle ans = QPDFObjectHandle::newArray();

    QPDFObjectHandle el;
    SEXP r_names = GET_NAMES(r_vals);
    for(int i = 0; i < len; i++) {
        el = QPDFObjectHandle::newArray();
        el.insertItem(0, QPDFObjectHandle::newString( std::string(CHAR(STRING_ELT(r_names, i)))));
        el.insertItem(1, QPDFObjectHandle::newString( std::string(CHAR(STRING_ELT(r_vals, i)))));
        ans.insertItem(i, el);
    }
    
    QPDFObjectHandle *dict = (QPDFObjectHandle *) R_get_QPDF(r_dict); // R_ExternalPtrAddr(r_dict);
    dict->replaceKey(std::string(CHAR(STRING_ELT(r_key, 0))), ans);

    return(ScalarLogical(1));
}


