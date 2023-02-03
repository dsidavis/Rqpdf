\name{getFormValues}
\alias{getFormValues}
\title{Get contents of PDF form elements from a document}
\description{
  This retrieves any information about PDF form elements in a PDF document,
  including the value of each element and a description of the element
  itself.
}
\usage{
getFormValues(pdf, asDataFrame = TRUE, combinePages = TRUE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{doc}{a filename or a \code{\link{QPDF-class}} object identifying the PDF file}  
  \item{asDataFrame}{a logical scalar value controlling whether the
    results are organized as a data.frame or left
    as a per-page list of form element descriptions for each page.
  }
  \item{combinePages}{a logical scalar value, which if \code{asDataFrame} is
    \code{TRUE},  controls whether one gets a single data.frame
    for all form elements across all pages (\code{TRUE}), or a list of data.frames
    with one for each page (\code{FALSE}).
    This is ignored if \code{asDataFrame} is \code{FALSE}.
  }
}
\value{
  If both \code{data.frame} and \code{combinePages} are \code{TRUE}, a data.frame.
  If \code{data.frame} is \code{TRUE} and \code{combinePages} is \code{FALSE}, a list
  of data.frame objects, with one per page.

  If \code{data.frame} is \code{FALSE}, a list with an element for each page of the PDF document.
  Each element is a list of form element descriptions for that page.
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
frms1 = getFormValues(pdf)

frms2 = getFormValues(pdf, combinePages = FALSE)

frms3 = getFormValues(pdf, FALSE)
}
\keyword{input}
\concept{PDF}
