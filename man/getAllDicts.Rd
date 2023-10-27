\name{getAllDicts}
\alias{getAllDicts}
\alias{getAllObjects}
\title{List of each PDF dictionary in a PDF document}
\description{
  Provides access to all of the dictionary objects in the PDF document,
  each converted to an R (list) object, including references to other dictionary objects
  as appropriate.
}
\usage{
getAllDicts(doc, root = getRoot(doc), trailer = getTrailer(doc))
getAllObjects(doc, streamData = TRUE)
}
\arguments{
  \item{doc}{a filename or a \code{\link{QPDF-class}} object identifying the PDF file}
  \item{root}{the initial/root dictionary of the PDF document which
    provides references to the important other dictionaries such as the one identifying the pages}
  \item{trailer}{the final/wrapup dictionary in the PDF document}
  \item{streamData}{a logical value controlling whether to convert a stream to raw data or leave as
    a reference to the stream object.}
}
\value{
 \code{getAllDicts} returns  a named list.
  The names are the identifiers for the dictionary, e.g., \code{"9.0"}.

  \code{getAllObjects} returns an unnamed list.
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
dicts = getAllDicts(pdf)
names(dicts)

sort(table(unlist(lapply(dicts, names))), decreasing = TRUE)
}
\keyword{input}
\concept{PDF}

