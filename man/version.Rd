\name{version}
\alias{version}
\alias{libqpdfVersion}

%- Also NEED an '\alias' for EACH other topic documented here.
\title{Version of the libqpdf library}
\description{
  These provide version information of the underlying libqpdf C++ code.
}
\usage{
version(runTime = TRUE)
libqpdfVersion
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{runTime}{a scalar logical value indicating whether to use the
    version from the library or when the package was installed.
  }
}
\value{
A string of the form "11.2.0", i.e., major, minor and patch number.
}
\references{
libqpdf (\url{https://github.com/qpdf/qpdf})  
}
\author{
Duncan Temple Lang <duncan@r-project.org>
}

\seealso{
\code{\link{pdfVersion}}
}

\examples{
Rqpdf::version()
}
\keyword{input}
\concept{PDF}

