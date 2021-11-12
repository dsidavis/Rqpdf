#include "Rqpdf.h"

#include <qpdf/QPDFWriter.hh>

extern "C"
SEXP
R_QPDFWriter_toFile(SEXP r_qpdf, SEXP r_file)
{
    QPDF *qpdf = R_get_QPDF(r_qpdf);
    char const *filename = CHAR(STRING_ELT(r_file, 0));
    
    QPDFWriter w(*qpdf, filename);
 w.setStaticID(true); // for testing only
// w.setStreamDataMode(qpdf_s_uncompress);    
    w.write();
    return(ScalarLogical(1));
}
