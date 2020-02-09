
setClass("QPDF", list(ref = "externalptr"))

qpdf = function(filename = character(), obj = new("QPDF"))
{
    filename = path.expand(filename)
    if(length(filename) && !file.exists(filename))
        stop(filename, " does not exist as a file")

    qpdf = .Call("R_getQPDF", filename)
    obj@ref = qpdf
    obj
}


# setOldClass("QPDFReference")
setOldClass(c("PDFPage", "Page", "QPDFReference"))

setMethod("[[", c("QPDF", "QPDFReference"),
          function(x, i, streamData = FALSE, ...) {
              .Call("R_getObjectByID", x@ref, i, as.logical(streamData)) # if we use id.gen in C code - as.integer(strsplit(i, ".", fixed = TRUE)[[1]]))
          })


setMethod("[[", c("QPDF", "numeric"),
          function(x, i, asRef = FALSE, streamData = FALSE, ...) {
             p = getPages(x)
             ref = p[[i]]
             if(asRef)
                 return(ref)

             x[[ ref, streamData = streamData ]]
          })


getRoot =
function(doc, streamData = FALSE)
{
    doc = as(doc, "QPDF")
    .Call("R_getRoot_QPDF", doc@ref, TRUE, as.logical(streamData))
}

getTrailer =
function(doc, streamData = FALSE)
{
    doc = as(doc, "QPDF")
    .Call("R_getRoot_QPDF", doc@ref, FALSE, as.logical(streamData))
}

getWarnings =
function(doc)
{
    doc = as(doc, "QPDF")
    .Call("R_qpdf_getWarnings", doc@ref)
}

processFile =
function(doc, file, passwd = character(), warnings = TRUE)
{
    file = path.expand(file)
    if(!file.exists(file))
        stop(file, " does not exist")
    
    doc = as(doc, "QPDF")
    if(warnings)
        suppressWarnings(doc, TRUE)
    .Call("R_qpdf_processFile", doc@ref, file, as.character(passwd))
    if(warnings)
        showWarnings(doc)
    else
        TRUE
}

showWarnings =
function(qpdf)
{
    w = getWarnings(qpdf)
    if(length(w))
        sapply(w, function(x) warning(x$message))
    length(w) == 0
}

suppressWarnings =
function(doc, val = TRUE)
{
    doc = as(doc, "QPDF")
    .Call("R_qpdf_setSuppressWarnings", doc@ref, as.logical(val))
}


# See below for alternative version
getAllDicts =
function(doc, root = getRoot(doc), trailer = getTrailer(doc))
{
    doc = as(doc, "QPDF")
     tmp = c(root, trailer)
    #tmp = trailer
    e = new.env( parent = emptyenv())
    while(TRUE) {
        browser()
        tmp = findQPDFReferences(tmp)
        tmp = tmp[ !(names(tmp) %in% ls(e)) ]
        if(length(tmp) == 0)
            break
cat("getting ", paste(names(tmp), collapse = ", "), "\n")
        tmp = lapply(names(tmp), function(id) assign(id, doc[[ tmp[[ id ]] ]], e))
#        tmp = unlist(tmp, recursive = FALSE)
    }

    e
}

findQPDFReferences =
function(obj)
{
   tmp = lapply(obj, function(x)
                  if(is(x, 'QPDFReference'))
                     x
                  else if(is.list(x))
                     x[sapply(x, is, 'QPDFReference')] #, recursive = FALSE)
                  else NULL)
   
   ans = tmp[sapply(tmp, length) > 0]

   if(any(sapply(ans, is.list)))
       ans = unlist(ans, recursive = FALSE)
   

   names(ans) = sapply(ans, paste, collapse = ".")
   ans
}


####################################


findQPDFReferences =
function(obj)
{
   tmp = lapply(obj, function(x)
                  if(is(x, 'QPDFReference'))
                     paste(x, collapse = ".")
                  else if(is.list(x))
                     findQPDFReferences(x)
                    # paste(x[sapply(x, is, 'QPDFReference')], collapse = ".") #, recursive = FALSE)
                  else NULL)
   unlist(tmp)
}

setAs("character", "QPDF",
      function(from)
        qpdf(from))


getAllDicts =
    #
    # R_getObjectByID currently does a copy of the contents of arrays, but not top-level dictionaries
    # So we don't get all the top-level elements with an Obj ID in the PDF since they are arrays or literals.
    # z = getAllDicts()
    # setdiff(1:max(as.numeric(ls(z))), sort(floor(as.numeric(ls(z)))))
    #
function(doc, root = getRoot(doc), trailer = getTrailer(doc))
{
    doc = as(doc, "QPDF")
    tmp = c(root, trailer)
    e = new.env( parent = emptyenv())
    while(TRUE) {
        tmp = findQPDFReferences(tmp)
#browser()        
        tmp = tmp[ !(tmp %in% ls(e)) ]
        if(length(tmp) == 0)
            break
#cat("getting ", paste(tmp, collapse = ", "), "\n")
        tmp = mapply(function(id, idx) assign(id, .Call("R_getObjectByID", doc@ref, idx), e),
                     tmp,
                     lapply(strsplit(tmp, ".", fixed = TRUE), as.integer), SIMPLIFY = FALSE)
    }

    e
}
