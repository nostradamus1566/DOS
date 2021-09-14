
Swift is a simple MS-DOS word processor.

It looks similar to Microsoft's Edit.com. It features a full pull-down menu system. It works with plain text files. You can customise the colours to suit your preference.

Swift has a clipboard so you can cut, copy and paste text. Swift also features automatic word-wrap, and other featurs such as find and replace.

Shelling out to DOS
If you wish to save the text, but need to format a floppy disk first, you can shell out to DOS from the file menu, format the disk, then when you are finished the DOS session type "exit" to return to Swift and then save the text to the newly formatted disk. Because of the size of the Swift program there is about 75k of the 640k available for a DOS session.

Printing
You can use either a dot matrix or an ink-jet printer.
If you are using an ink jet printer and you want to print on both sides of the page Swift will print out the first side and wait for you to turn the page over to print on the second side. The text of a each page is sent to the printer one at a time. While the printer is preparing to print a page a dialog box pops up asking if you want to print the next page. You have the option of clicking on the "ok" button or the "Cancle" button. If you wish to print the next page you should first wait until the printer has printed the current page. Then when the current page comes on in the out tray you can turn it over, insert it into in tray and then click the "ok" button. The dialog box acts to halt the stream of data to the printer in order not to send data faster than the printer can handle it, otherwise the printer's buffer fills to overflowing and a printer error results.
 


Requirements:
640k internal memory.
A mouse driver must be installed.
Space on hard drive C (at least 1MB) to write two files:
'clipbrd.txt' and 'swift2.var'.

'Clipbrd.txt' hold the contents of the clipboard, and 'Swift2.var' holds the contents of the internal setting of the colours and other variables. So when you start up Swift it will remember your preferences and restore them. The text file you were working on in a previous session will also be automatically loaded in so you can continue where you left off.

Swift is not intended as a replacement for Edit.com.  Swift should not be used for editing program source files; use Edit.com instead. Swift is really intended for writing simple text documents such as memos and drafts of letters. 

Swift is 'freeware'. All the source code is included in the file 'source.zip'. If you want to enhance the program or customise it further then you will need Borland's C++ compiler and assember. To do this decompress all the source files to a folder on drive D called swift. Then using Turbo C++3.0 or Borland C++ 4.52 open the project Swift.prj. Then you should be able to make any changes and then recompile all the files in the project.

If you are using this on a DOS only computer you should have the Smartdrive driver installed. The driver uses memory as a cache to speed up disk data access.

The maximum amount of lines the editor can take is about 2,000. This is roughly 33 A-4 pages.
------------------------------------------------------------------
Michael Finnegan 
Dingle, Kerry,
Ireland
Monday 12 July 1999
