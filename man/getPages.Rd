\name{getPages}
\alias{getPages}
\title{Get dictionaries identifying the page elements in a PDF document}
\description{
}
\usage{
getPages(doc, resolve = TRUE)
}
\arguments{
  \item{doc}{a filename or a \code{\link{QPDF-class}} object identifying the PDF file}    
  \item{resolve}{a scalar logical value which if \code{TRUE}
    will resolve each reference to a page dictionary and return
    a list with those elements.
  }
}
\value{
  If \code{resolve} is \code{TRUE}, a list with an element for each page
  in the PDF document.

  If \code{resolve} is \code{FALSE}, a list of \code{QPDFReference}
  objects that identify the dictionary containing the locations of the page objects.
}
\references{
  libqpdf (\url{https://github.com/qpdf/qpdf})  
}
\author{
Duncan Temple Lang <duncan@r-project.org>
}

\seealso{
\code{\link{qpdf}}
}
\examples{
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
  p = getPages(pdf)

  pu = getPages(pdf, FALSE)
}
\keyword{input}
\concept{PDF}

