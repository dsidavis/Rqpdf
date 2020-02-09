getPages =
function(doc)
{
    doc = as(doc, "QPDF")
    ans = doc [[ getRoot(doc)$Pages ]]$Kids
    structure(lapply(ans, function(x) { class(x) = c("PDFPage", "Page", class(x)); x}), class = "PDFPages")
}

