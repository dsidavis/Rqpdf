getImages =
function(doc)
{
    doc = as(doc, "QPDF")
    .Call("R_getImages", doc)
}

