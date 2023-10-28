#include "Rqpdf.h"

extern "C"
SEXP
R_getImages(SEXP r_qpdf)
{
        QPDF *qpdf = GET_QPDF(r_qpdf);

        QPDFPageDocumentHelper pdh(*qpdf);
        std::vector<QPDFPageObjectHelper> pages = pdh.getAllPages();
        int page_ctr = 0;

        SEXP ans;
        PROTECT(ans = NEW_LIST(pages.size()));
        for (auto& page: QPDFPageDocumentHelper(*qpdf).getAllPages())         
/*        for (std::vector<QPDFPageObjectHelper>::iterator page_iter = pages.begin();
          page_iter != pages.end(); ++page_iter, page_ctr++) */
        {
            int numImages = 0;
            
            // count the images so we can preallocate list() for this page
            for(auto& iter1 : page.getImages()) { numImages++; }

            SEXP els, names;
            PROTECT(els = NEW_LIST(numImages));
            PROTECT(names = NEW_CHARACTER(numImages));            
            numImages = 0;
            // now loop over them again and put them into a list.            
            for (auto &iter: page.getImages()) {             
                 QPDFObjectHandle &img = iter.second;
                 std::shared_ptr<Buffer> data;
                 bool isRaw = false;
                 try {
                     data = img.getStreamData();
                 } catch(std::exception &ex) {
                     data = img.getRawStreamData();
                     isRaw = true;
                 } 
                 
                 SEXP raw = NEW_RAW(data->getSize());
                 PROTECT(raw);
                 memcpy(RAW(raw), data->getBuffer(), data->getSize());
                 SEXP tmp;
                 SET_CLASS(raw, tmp = NEW_CHARACTER(1));
                 SET_STRING_ELT(tmp, 0, mkChar(isRaw ? "UnfilteredImage" : "FilteredImage"));
                 SET_ATTR(raw, Rf_install("dictionary"), QPDFObjectHandleToR(img.getDict(), true, true, false));

                 // Is this correct?
                 if(img.isInlineImage()) {
                     std::string nm = img.getInlineImageValue();
                     if(nm.c_str())
                         SET_STRING_ELT(names, numImages, mkChar(nm.c_str()));
                 }
                 
                 SET_VECTOR_ELT(els, numImages++, raw);
                 UNPROTECT(1);
            }
            SET_NAMES(ans, names);
            SET_VECTOR_ELT(ans, page_ctr, els);            
            UNPROTECT(2);
        }

        UNPROTECT(1);
        return(ans);
}



