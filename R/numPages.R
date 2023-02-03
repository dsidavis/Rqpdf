
if(FALSE) {
numPages =
function(file)    
{
    file = path.expand(file)
    if(!file.exists(file))
        stop(file, " does not exist")
    
    .Call("R_pdf_numPages", file)
}
}


numPages =
function(doc)    
{
    doc = as(doc, "QPDF")
    doc[[ getRoot(doc)$Pages ]]$Count
}

