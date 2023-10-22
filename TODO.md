+ Allow creation of a new dict/pdf object and insert into document
  + e.g. for adding validation code for a widget.
    + Add an AA element to a widget's dictionary and have its /V element refer to a new PDF object.
+ Method to turn dictionary reference to an R list.

+ Index list of dictionaries with 
```
  pdf = system.file("sampleDocs/europeanPatentForm.pdf", package = "Rqpdf")
  start = getRoot(pdf)
  dicts = getAllDicts(pdf)
  dicts[[ start$Names ]] 
```


# Done

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
