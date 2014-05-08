## linedelaybuffer

### Usage:

    linedelaybuffer [options] LINES

**Copy STDIN to STDOUT linewise, storing LINES number of lines.**  
**Each input line is output only after receiving a further LINES lines.**  

### Options:
* -f, --flush  
  Flush leftover lines at the end of the input.  
  Otherwise the last LINES input lines will be discarded.  

### Notes:
* Reads from STDIN are blocking.  
* Writes to STDOUT are blocking.  

### License:
GPLv2
