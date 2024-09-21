getHyperlinks =
function(doc, dicts = getAllDicts(doc))
{
    doc = as(doc, "QPDF")
    w = sapply(dicts, isHyperlinkDict)

    sapply(dicts[w], function(x) doc[[ x$A ]]$URI)
}
    

isHyperlinkDict =
function(x)
{
    "Type" %in% names(x) && x$Type == "/Annot" && "A" %in% names(x)
}
