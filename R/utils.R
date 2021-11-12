getPages =
function(doc)
{
    doc = as(doc, "QPDF")
    ans = doc [[ getRoot(doc)$Pages ]]$Kids
    structure(lapply(ans, function(x) { class(x) = c("PDFPage", "Page", class(x)); x}), class = "PDFPages")
}


getAnnots =
function(doc, dicts = getAllDicts(doc))
{
    doc = as(doc, "QPDF")    
    getByType(doc, dicts, "/Annot")
}

getByType =
function(doc, dicts = getAllDicts(doc), type = NA, subType = NA)    
{
    w = sapply(dicts, function(x) {
                         (!is.na(type) && !is.null(x[["Type"]]) && x[["Type"]] == type) ||
                             (!is.na(subType) && !is.null(x[["Subtype"]]) && x[["Subtype"]] == subType)
                      })
    dicts[w]
}


getImages =
function(doc, dicts = getAllDicts(doc))    
{
  doc = as(doc, "QPDF")    
  getByType(doc, dicts, subType = "/Image")
}
