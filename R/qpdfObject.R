QPDFTypeClassMap = c(dictionary = "QPDFDict",
    array = "QPDFArray",
    stream = "QPDFStream"
                    )

getQPDFClass =
function(ans)
{
    ty = getTypeName(ans)
    i = match(ty, names(QPDFTypeClassMap))
    if(is.na(i))
        "QPDFObject"
    else
        QPDFTypeClassMap[i]
}

getTypeName =
function(obj)
{
    if(is(obj, "QPDFObject"))
        obj = obj@ref
    .Call("R_QPDFObjectHandle_getTypeName", obj)
}

asQPDFRef =
function(ans)
{
    k = getQPDFClass(ans)
    new(k, ref = ans)
}


setMethod("$<-", "QPDFDict",
          function(x, name, value) {
              if(!grepl("^/", name))
                  name = paste0("/", name)

              .Call("R_QPDFDict_replaceKey", x@ref, name, as(value, "QPDFObject"))
              x
          })


setMethod("names", "QPDFDict",
          function(x) {
              .Call("R_QPDFObjectHandle_getKeys", x@ref)
          })


setAs("ANY", "QPDFObject",
      function(from) {
          ans = .Call("R_RToQPDFObjectHandle", from)
          asQPDFRef(ans)
      })
