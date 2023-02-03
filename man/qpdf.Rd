\name{qpdf}
\alias{qpdf}
\alias{QPDF-class}
\alias{getFilename}
\alias{show,QPDF-method}
\alias{[[}
\alias{[[,QPDF,character-method}
\alias{[[,QPDF,numeric-method}
\alias{[[,QPDF,QPDFReference-method}
\alias{coerce,character,QPDF-method}

\title{
Create a handle/reference to a PDF document
}
\description{
  This creates a reference to a PDF document  that we can use
  in later computations to query some of its contents,
  e.g., access specific dictionaries, determine the version of PDF,
  This  does not  immediately read the PDF document.
}
\usage{
qpdf(filename = character(), obj = new("QPDF"))
getFilename(doc)
}
\arguments{
  \item{filename}{the path to the PDF file.}
  \item{obj}{}
  \item{doc}{a \code{QPDF} object created via \code{qpdf}}
}

\value{
  A \code{QPDF} object which is an S4 class that contains a reference to a C++
  object that is the handle/reference to the C++ data structure representing
  the PDF document's details.
}
\references{
  libqpdf (\url{https://github.com/qpdf/qpdf})  
}
\author{
Duncan Temple Lang <duncan@r-project.org>
}


\seealso{
  \code{\link{numPages}},
  \code{\link{getImages}},
  \code{\link{getFormValues}}
}
\examples{
pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
doc = qpdf(pdf)

numPages(doc)

pages = getPages(doc)

lapply(pages, function(ref) doc[[ ref ]] )

getFilename(doc)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory (show via RShowDoc("KEYWORDS")):
\keyword{input}
\concept{PDF}
