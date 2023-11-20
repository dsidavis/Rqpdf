.Call =
function(.NAME, ..., PACKAGE)    
{
print(.NAME)    
    # determine which arguments are QPDF objects and
    args = list(...)
    w = sapply(args, is, "QPDF")

    # 
    if(any(w)) {
        lapply(args[w], suppressWarnings)
        on.exit( 
            lapply(args[w],
                   function(q) {
                       suppressWarnings(q, FALSE)
                       showWarnings(q)
                }))
    }

    if(missing(PACKAGE))
        base::.Call(.NAME, ...)
    else
        base::.Call(.NAME, ..., PACKAGE = PACKAGE)
}

