# See 14.3 of the PDF Specification, Version 1.7
getMetadata =
function(doc, parse = TRUE)
{
    if(is.character(doc))
        doc = qpdf(doc)

    r = getRoot(doc)
    if(!"Metadata" %in% names(r))
        stop("no Metadata element in PDF root dictionary")
    
    s = doc [[ mkQPDFRef(r$Metadata), stream = TRUE ]]
    dict = attr(s, "dict")
    if("Subtype" %in% names(dict) && dict$Subtype == "/XML") {
        x = rawToChar(s)
        if(parse)
            XML::xmlParse(x)
        else
            x
    } else
        s
}

getInfo =
function(doc)
{
    if(is.character(doc))
        doc = qpdf(doc)

    tr = getTrailer(doc)
    if(!"Info" %in% names(tr))
        stop("no Info element in PDF trailer dictionary")
        
    doc[[ tr$Info ]] 
}


mkQPDFRef =
function(x)    
{
    if(inherits(x, "QPDFRefrence"))
        return(x)
    
    if(is.numeric(x))
        return(structure(as.integer(x), class = "QPDFReference"))

    w = grepl("^[0-9]+ [0-9]+ R", x)
    if(!w)
        stop("not a reference")

    structure(as.integer(strsplit(x, " ")[[1]][1:2]), class = "QPDFReference")
}

