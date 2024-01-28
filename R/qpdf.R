
setClass("QPDFNativeObject", list(ref = "externalptr"))
setClass("QPDF", contains = "QPDFNativeObject")
setClass("QPDFObject", contains = "QPDFNativeObject")
setClass("QPDFDict", contains = "QPDFObject")
setClass("QPDFArray", contains = "QPDFObject")
setClass("QPDFStream", contains = "QPDFObject")

qpdf =
function(filename = character(), obj = new("QPDF"), check = TRUE, description = filename)
{
    if(is.raw(filename)) {
        if(missing(description))
            description = "pdf document in memory"
    } else {
        filename = path.expand(filename)
        if(length(filename) && !file.exists(filename))
            stop(filename, " does not exist as a file")

        if(check && file.info(filename)$size == 0)
            stop(filename, " is an empty file")
    }
    
    qpdf = base::.Call("R_getQPDF", filename, as.character(description), NULL)
    obj@ref = qpdf
    obj
}


# setOldClass("QPDFReference")
setOldClass(c("PDFPage", "Page", "QPDFReference"))

setMethod("[[", c("QPDF", "QPDFReference"),
          function(x, i, streamData = FALSE, asRef = FALSE, ...) {
              # if we use id.gen in C code - as.integer(strsplit(i, ".", fixed = TRUE)[[1]]))
              
              ans = .Call("R_getObjectByID", x@ref, i, as.logical(streamData), as.logical(asRef))
              if(asRef)
                  asQPDFRef(ans)
              else
                  ans
          })


setMethod("[[", c("QPDF", "character"),
          function(x, i, streamData = FALSE, asRef = FALSE, ...) {
              # Why . rather than space ?
              i = strsplit(i, "\\.")[[1]]
              i = as.integer(i)
              if(length(i) != 2 || any(is.na(i)))
                  stop("invalid QPDF object reference")

              x[[ structure(i, class = "QPDFReference"), streamData = streamData, asRef = asRef, ... ]]
          })

setMethod("[[", c("QPDF", "numeric"),
          function(x, i, j, asRef = FALSE, streamData = FALSE, ...) {
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
    base::.Call("R_qpdf_getWarnings", doc@ref)
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
    base::.Call("R_qpdf_setSuppressWarnings", doc@ref, as.logical(val))
}


# See below for alternative version
if(FALSE)
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
#cat("getting ", paste(names(tmp), collapse = ", "), "\n")
        tmp = lapply(names(tmp), function(id) assign(id, doc[[ tmp[[ id ]] ]], e))
#        tmp = unlist(tmp, recursive = FALSE)
    }

    e
}

findQPDFReferences =
    # See below.
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

        tmp = tmp[ !(tmp %in% ls(e)) ]
        if(length(tmp) == 0)
            break

        tmp = mapply(function(id, idx)
                         assign(id, .Call("R_getObjectByID", doc, idx, FALSE, FALSE), e),
                     tmp,
                     lapply(strsplit(tmp, ".", fixed = TRUE), as.integer), SIMPLIFY = FALSE)
    }

    e = as.list(e)
    o = order(as.integer(gsub("\\.0", "", names(e))))
    e = e[o]
    class(e) = "PDFDictionaries"    
    e
}

setOldClass(c("PDFDictionaries", "list"))

setMethod("[[", c("PDFDictionaries", "QPDFReference"),
          function(x, i, ...) {
              browser()
              x[[ paste(i, collapse = ".") ]]
                  
          })


getAllObjects =
function(doc, streamData = TRUE)
{
    doc = as(doc, "QPDF")
    .Call("R_getAllObjects", doc, as.logical(streamData))
}


# setMethod("[[", c("PDFDictionaries", "character"),
#          function(x, i, ...) {
#              if(!(i %in% names(x)))
#                  
#          })

getFilename =
function(doc)
{
  .Call("R_qpdf_getFilename", as(doc, "QPDF"))
}

setMethod("show", "QPDF",
function(object)
 cat("<QPDF>", getFilename(object), "\n")
)


writePDF =
function(pdf, file)
{
    .Call("R_QPDFWriter_toFile", as(pdf, "QPDF"), path.expand(file))
}
