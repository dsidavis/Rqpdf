\name{getMetadata}
\alias{getMetadata}
\alias{getInfo}
\title{Get the Metadata or Info dictionary from a PDF Document}
\description{
  These functions get the (XML) \code{Metadata} or the
  \code{Info} dictionary from a PDF document.
}
\usage{
getMetadata(doc, parse = TRUE)
getInfo(doc)
}
\arguments{
  \item{doc}{the path to the PDF document or the
    object returned from \code{\link{qpdf}}.
   }
   \item{parse}{a logical value controlling whether the
     XML contnt is parsed and returned or just the character or raw data
   from the \code{Metadata} stream}.
}
\value{
  \code{getInfo} returns a named list.
  \code{getMetadata} returns the parsed XML document,
  the character data 
  or the \code{raw} data from the \code{Metadata}
}
\references{
  PDF Specification (version 1.7)
  \url{https://opensource.adobe.com/dc-acrobat-sdk-docs/pdfstandards/PDF32000_2008.pdf}  
}
\author{
Duncan Temple Lang
}

\seealso{
 \code{\link{qpdf}}, \code{\link{getRoot}}, \code{\link{getTrailer}}
}
\examples{
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")

  xml = getMetadata(pdf)
  txt = getMetadata(pdf, parse = FALSE)

  info = getInfo(pdf)
}
\keyword{input}
\concept{PDF}
