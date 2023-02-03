\name{numPages}
\alias{numPages}
\alias{getImages}
\alias{getAnnots}
\alias{pdfVersion}
\title{Query aspects of a PDF document}
\description{
  These functions query the number of pages,
  the image objects, annotations in a PDF document and also the PDF version of
  the PDF document.
}
\usage{
numPages(doc)
getImages(doc, dicts = getAllDicts(doc))
getAnnots(doc, dicts = getAllDicts(doc))
pdfVersion(doc)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{doc}{a filename or a \code{\link{QPDF-class}} object identifying the PDF file}  
  \item{dicts}{a list of the dictionaries in the PDF document.}
}
\value{
  \code{numPages} returns an integer giving the number of pages in the PDF document.

  \code{getImages} returns a list.

  \code{pdfVersion} returns a string of the form "1.6"
}
\references{
  libqpdf (\url{https://github.com/qpdf/qpdf})  
}
\author{
Duncan Temple Lang <duncan@r-project.org>
}

\seealso{
  \code{\link{qpdf}}
  \code{\link{getPages}}
}
\examples{
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
  numPages(pdf)

  doc = qpdf(pdf)
  numPages(doc)
}
\keyword{input}
\concept{PDF}

