#include "Rqpdf.h"

SEXP FieldObjHelperToR(QPDFFormFieldObjectHelper ffh);

extern "C"
SEXP
R_getFormValues(SEXP r_filename)
{
	QPDF qpdf;
	qpdf.processFile(CHAR(STRING_ELT(r_filename, 0)));

        QPDFAcroFormDocumentHelper afdh(qpdf);
        QPDFPageDocumentHelper pdh(qpdf);
        std::vector<QPDFPageObjectHelper> pages = pdh.getAllPages();

        SEXP ans;
        PROTECT(ans = NEW_LIST(pages.size()));

        int page_ctr = 0;
        for (std::vector<QPDFPageObjectHelper>::iterator page_iter =
                 pages.begin();
             page_iter != pages.end(); ++page_iter, page_ctr++)
        {
            // Get all widget annotations for each page. Widget
            // annotations are the ones that contain the details of
            // what's in a form field.
            std::vector<QPDFAnnotationObjectHelper> annotations = afdh.getWidgetAnnotationsForPage(*page_iter);

            SEXP el, names;
            el = NEW_LIST(annotations.size());
            SET_VECTOR_ELT(ans, page_ctr, el);
            PROTECT(names = NEW_CHARACTER(annotations.size()));
            int annotCtr = 0;
            
            for (std::vector<QPDFAnnotationObjectHelper>::iterator annot_iter = annotations.begin();
                      annot_iter != annotations.end(); ++annot_iter, annotCtr++)
            {
                QPDFFormFieldObjectHelper ffh = afdh.getFieldForAnnotation(*annot_iter);
#if 0                    
                if(ffh.getFieldType() == "/Ch") {
                    Rprintf("got a /Ch.");
                    Rf_PrintValue(QPDFObjectHandleToR(annot_iter->getObjectHandle().getKey("/Opt")));
                }
#endif                
                SET_STRING_ELT(names, annotCtr, mkChar(ffh.getFullyQualifiedName().c_str()));
                SET_VECTOR_ELT(el, annotCtr, FieldObjHelperToR(ffh));
            }
            SET_NAMES(el, names);
            UNPROTECT(1);
        }

        UNPROTECT(1);
        qpdf.closeInputSource();
        return(ans);
}

SEXP
convertQPDFArrayToR(QPDFObjectHandle h, bool streamData = false, bool asRectangle = true)
{
    SEXP ans;
    if(asRectangle && h.isRectangle()) {
        PROTECT(ans = NEW_NUMERIC(4));
        QPDFObjectHandle::Rectangle r = h.getArrayAsRectangle();
        REAL(ans)[0] = r.llx;
        REAL(ans)[1] = r.lly;
        REAL(ans)[2] = r.urx;
        REAL(ans)[3] = r.ury;
        SET_CLASS(ans, ScalarString(mkChar("Rectangle")));
        UNPROTECT(1);
        return(ans);
    }
    
    int len = h.getArrayNItems();
    PROTECT(ans = NEW_LIST(len));
    for(int i = 0; i < len; i++) 
        SET_VECTOR_ELT(ans, i, QPDFObjectHandleToR(h.getArrayItem(i), false, true, streamData));
    
    UNPROTECT(1);
    return(ans);
}

SEXP
convertQPDFDictToR(QPDFObjectHandle h, bool followGen, bool stripSlash, bool streamData)
{
    std::set<std::string> keys = h.getKeys();
    int len = keys.size(), i = 0;
    SEXP ans, names;
    PROTECT(ans = NEW_LIST(len));
    PROTECT(names = NEW_CHARACTER(len));
    std::set<std::string>::iterator it = keys.begin();
    for( ; i < len; i++, ++it) {
        SET_VECTOR_ELT(ans, i, QPDFObjectHandleToR(h.getKey(*it), false, true, streamData));
        SET_STRING_ELT(names, i, mkChar(it->c_str() + 1));
    }
    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}

SEXP
mkPDFIdGenRobj(QPDFObjectHandle h)
{
    SEXP ans;
#if 1
            PROTECT(ans = NEW_INTEGER(2));
            INTEGER(ans)[0] = h.getObjectID();
            INTEGER(ans)[1] = h.getGeneration();
#else
            PROTECT(ans = NEW_CHARACTER(1));
            char buf[100];
            sprintf(buf, "%d.%d",  h.getObjectID(), h.getGeneration());
            SET_STRING_ELT(ans, 0, mkChar(buf));
#endif            
            SET_CLASS(ans, ScalarString(mkChar("QPDFReference")));
            UNPROTECT(1);
            return(ans);
}


SEXP
QPDFObjectHandleToR(QPDFObjectHandle h, bool followGen, bool stripSlash, bool streamData)
{
    bool isOID = (h.getObjectID() > 0); 

    SEXP ans = R_NilValue;
    switch(h.getTypeCode()) {

    case QPDFTypeEnum(ot_boolean):
        ans = ScalarLogical(h.getBoolValue());
        break;
    case QPDFTypeEnum(ot_integer):
        ans = ScalarInteger(h.getIntValue());
        break;
    case QPDFTypeEnum(ot_real):
        ans = ScalarReal(h.getNumericValue());
        break;
    case QPDFTypeEnum(ot_string):
        ans = ScalarString(mkChar(h.getStringValue().c_str()));
        break;
    case QPDFTypeEnum(ot_name):
        ans = ScalarString(mkChar(h.getName().c_str()));
        break;
    case QPDFTypeEnum(ot_array):
        ans = convertQPDFArrayToR(h, streamData);
        break;
    case QPDFTypeEnum(ot_dictionary):
        if(!followGen && isOID)   // do we want to do this generally for all types of objects.
            ans = mkPDFIdGenRobj(h);
        else
            ans = convertQPDFDictToR(h, followGen, stripSlash, streamData);
        break;
    case QPDFTypeEnum(ot_stream):
        if(streamData) {
            PointerHolder<Buffer> d = h.getStreamData();
            size_t sz = d->getSize();
            PROTECT(ans = Rf_allocVector(RAWSXP, sz));
            memcpy(RAW(ans), d->getBuffer(), sz);
            SET_CLASS(ans, ScalarString(mkChar("PDFStream")));
        } else {
            PROTECT(ans = ScalarString(mkChar(h.unparse().c_str())));
            SET_NAMES(ans, ScalarString(mkChar(h.getTypeName())));
        }
        
        UNPROTECT(1);
        break;
    case QPDFTypeEnum(ot_operator):
        PROTECT(ans = ScalarString(mkChar(h.getOperatorValue().c_str())));        
        SET_NAMES(ans, ScalarString(mkChar(h.getTypeName())));
        UNPROTECT(1);
        break;        
    case QPDFTypeEnum(ot_inlineimage):
        PROTECT(ans = ScalarString(mkChar(h.getInlineImageValue().c_str())));        
        SET_NAMES(ans, ScalarString(mkChar(h.getTypeName())));
        UNPROTECT(1);
        break;
    default:
        ans = R_NilValue;            
        break;            
    }

    return(ans);
}


SEXP
FieldObjHelperToR(QPDFFormFieldObjectHelper ffh)
{

    // isNull, getFieldType(), getMappingName(), getValue(), getDefaultValue()
    SEXP ans, names;
    int numFields = 7, field = 0;
    PROTECT(ans = NEW_LIST(numFields));
    PROTECT(names = NEW_CHARACTER(numFields));

// Doesn't seem useful.
//    SET_VECTOR_ELT(ans, field, ScalarLogical(ffh.isNull()));
//    SET_STRING_ELT(names, field++, mkChar("isNull"));

#if 0    
    if(ffh.getFieldType() == "/Ch") {
        Rprintf("got a /Ch.  /Opt = '%s'\n",    ffh.getInheritableFieldValueAsString("/V").c_str());
    }
#endif
    
    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getFieldType().c_str())));
    SET_STRING_ELT(names, field++, mkChar("fieldType"));

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getMappingName().c_str())));
    SET_STRING_ELT(names, field++, mkChar("name"));

    SET_VECTOR_ELT(ans, field, QPDFObjectHandleToR(ffh.getValue(), true)); // XX add streamData - ever needed?
    SET_STRING_ELT(names, field++, mkChar("value"));

    SET_VECTOR_ELT(ans, field, QPDFObjectHandleToR(ffh.getDefaultValue()));
    SET_STRING_ELT(names, field++, mkChar("defaultValue"));    

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getValue().getTypeName())));
    SET_STRING_ELT(names, field++, mkChar("valueType"));

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getFullyQualifiedName().c_str())));
    SET_STRING_ELT(names, field++, mkChar("qualifiedName"));

    SET_VECTOR_ELT(ans, field, ScalarString(mkChar(ffh.getPartialName().c_str())));
    SET_STRING_ELT(names, field++, mkChar("partialName"));                            


    SET_NAMES(ans, names);
    UNPROTECT(2);
    return(ans);
}
