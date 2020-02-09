library(Rqpdf)
q = qpdf()
processFile(q, "inst/sampleDocs/corrupt.pdf")

#.Call("R_qpdf_setSuppressWarning", q@ref, TRUE)

#.Call("R_qpdf_processFile", q@ref, "inst/sampleDocs/corrupt.pdf", character())

#w = .Call("R_qpdf_getWarnings", q@ref)




