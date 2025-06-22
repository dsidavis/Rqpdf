+ Can't trace Rqpdf:::.Call - NOT an Rqpdf issue.
```
trace(Rqpdf:::.Call, quote(print(.NAME)))
Error in methods::.TraceWithMethods(Rqpdf:::.Call, quote(print(.NAME)),  : 
  tracing the internal function ‘.Call’ is not allowed
```
    + note that trace(Rqpdf:::.Call) doesn't give an error but also doesn't do anything.
	
+ Allow creation of a new dict/pdf object and insert into document
  + e.g. for adding validation code for a widget.
    + Add an AA element to a widget's dictionary and have its /V element refer to a new PDF object.

+ Method to turn dictionary reference to an R list.
   + ? Is this done?

+ Index list of dictionaries with 
```
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
  start = getRoot(pdf)
  dicts = getAllDicts(pdf)
  dicts[[ start$Names ]] 
```


# Done

+ √ capture warnings from libqpdf as R warnings.
  + done for 
     + qpdf()/getRoot
	 + getAllObjects()
  + What other functions/routines generate warnings.
    + √ Could intercept all .Call() with a QPDF object and 
  + Can also use a customized logger. - NO. Methods are not virtual.

  + √ getImages() not working
     + uses getByType
```	 
doc = qpdf("inst/sampleDocs/corrupt.pdf")  
getImages(doc)
```
     + getAllDicts() was doing a .Call() with the doc@ref, so bypassing the .Call()
	 + Rather than change the C routine to take the S4 object with the ref slot, 
	   we change the getAllDicts() function to keep the same .Call() (but to base::.Call())
	   and show all the warnings after the mapply().


+ √ Read a PDF document from a raw buffer so don't have to save to drive.

+ √ For a stream, get the associated dictionary contents too, e.g., as an attribute
   + e.g. for the Metadata element in 
```
4 0 obj
<<
  /Subtype /XML
  /Type /Metadata
  /Length 5 0 R
>>
stream
<?xpacket begin="" id="W5M0MpCehiHzreSzNTczkc9d"?>
<x:xmpmeta xmlns:x="adobe:ns:meta/">....
```
   + Added as an attribute named "dictionary" to the stream data.
