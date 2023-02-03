getPages =
function(doc, resolve = TRUE)
{
    doc = as(doc, "QPDF")
    ans = doc [[ getRoot(doc)$Pages ]]$Kids
    ans = structure(lapply(ans, function(x) { class(x) = c("PDFPage", "Page", class(x)); x}), class = "PDFPages")

    if(resolve)
        resolvePages(ans, doc)
    else
        ans
}

resolvePages =
function(pages, doc)    
{
    p3 = lapply(pages, function(ref) lapply(doc[[ref]]$Kids, function(r) doc[[r]]))
    unlist(p3, recursive = FALSE)
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
