\name{getRoot}
\alias{getRoot}
\alias{getTrailer}
\title{Access the root or trailer dictionaries in a PDF object}
\description{
  These functions provide access to two important dictionaries that bookend
  the contents of the PDF document.
}
\usage{
getRoot(doc, streamData = FALSE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{doc}{a filename or a \code{\link{QPDF-class}} object identifying the PDF file}  
  \item{streamData}{}
}
\value{
  an named list containing the individual key-value elements in the corresponding
  PDF dictionary, converted to R objects, including possibly
  \code{\link{QPDFReference-class}} objects identifying other dictionaries.
}
\references{
  libqpdf (\url{https://github.com/qpdf/qpdf})  
}
\author{
Duncan Temple Lang <duncan@r-project.org>
}

\seealso{
  \code{\link{qpdf}},
  \code{link{getAllDicts}}
}
\examples{
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
  start = getRoot(pdf)
  names(start)

  start$Names

  doc = qpdf(pdf)
  nms = doc[[ start$Names ]]

  end = getTrailer(doc)
  names(end)

  identical(doc[[end$Root]], start)
}
\keyword{input}
\concept{PDF}

