
pdfVersion =
function(doc)
{
    if(is(doc, "QPDF"))
        doc = getDocname(doc)
    else
        doc = path.expand(doc)
    
    if(!file.exists(doc))
        stop("no such file ", doc)
    
    .Call("R_get_pdf_version", doc)
}

