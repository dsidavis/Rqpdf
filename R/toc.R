# Table of contents for a PDF document.

toc =
function(doc, root = getRoot(doc))
{
    if(is.character(doc))
        doc = qpdf(doc)
    
    outl = root$Outlines
    o = doc[[ outl ]]
    # get first and use its Next and so on.
    ans = vector("list", o$Count)
    cur = o$First
    for(i in 1:o$Count) {
        if(i == 9) browser()
        el = doc[[ cur ]]
        ans[[i]] = el
#        names(ans)[i] = el$Title
        cur = el$Next
        if(is.null(cur))
            break
    }

    ans = ans[1:i]
    ans
}

tocTitles =
function(doc, root = getRoot(doc), list = toc(doc, root))
{
    browser()
   unname( sapply(list, `[[`, "Title") )
}
