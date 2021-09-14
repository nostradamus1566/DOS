                        Welcome to Matrix                              
                            version 2.0
    
    Matrix is a freeware DOS spreadsheet program.
    
    The manual.html file explains the details of the functions and 
    other technical information. This file contains the latest 
    supplemental information not included in the manual.html file. If 
    you experience problems using Matrix the solution may be described 
    here. It is a good idea to print out both this file and the manual 
    file as a reference. 
    
     
    Features
    -------- 
    Matrix features mouse control, pull-down menus, printer support, a 
    clipboard, cut, copy, paste, formula replication across rows and 
    columns. Many of the usual financial, statistical, and other 
    functions found in Lotus 1-2-3 are supported, such as FV, PV, NPV 
    IRR, RATE etc.
    
    A number of example spreadsheet data files are included to 
    demonstrate the program and the use of the functions. There is also
    a manual document in HTML format which explains the functions and 
    other technical details.
    
    
    Minimum hardware requirements: 
    ------------------------------ 
    At least a 386 microprocessor with 2 MB of extended or expanded 
    memory (If using extended memory make sure the auto switch is on.) 
    A math coprocessor and DOS 6 or higher. 
    
    
    Although 2MBs is the minimum, if you have 16MBs to spare (most 
    computers running Windows 95/98 do) then you can have up to 800 
    lines for the main sheet, and another 800 lines for the clipboard.
    
    
    Incompatibility with previous versions 
    -------------------------------------- 
    The spreadsheet file format of this version is not compatible with
    earlier versions. One of the reasons for the change is to simplify the
    file format so that formula strings can be viewed and edited in an    
    ordnary text editor such as Microsoft's edit.com. To see the new      
    format start up edit.com and load in any of the '.mtx' files. You can 
    see that the formulas are stored in an easy to read format. For       
    example, on each line, after the preliminary information, the name of
    the cell comes first then the formula. After the formulas come the
    properties of each cell. Because all of the source code is included
    it is very easy to see how the formulas are read in and how the 
    display properties for each cell is interpreted.      
    
    
    Running Matrix from DOS 6 
    ------------------------- 
    Matrix needs 2mbs of memory to run. It can be either physical 
    expanded memory or extended memory interpreted as expanded memory. 
    If you normally don't have an expanded memory setting in your 
    config.sys file then you should have the line
    
    device = c:\dos\emm386.exe auto 
    
    in the config.sys file after the himem.sys driver. The auto switch 
    means only programs that request expanded memory will be given it. 
    When they close the memory will be returned to the system.
    
    If you are running Matrix from Windows3.1 there is a possibility of
    a conflict over the DOS mouse driver. If you can't use the mouse in
    a DOS session under Windows 3.1 try not loading the DOS mouse 
    driver before windows 3.1 loads. Then when you want to start up 
    Matrix do so from a batch file which contains a command to load the
    DOS mouse driver. This would be set up in advance so that when you 
    click on the Matrix icon in the Windows 3.1 desktop it would refer 
    to the bath file, not the program directly.
    
    Matrix has been tested under the following conditions: 1) Under DOS
    6.0 without Windows 3.1 loaded 2) In a DOS session under Windows 
    3.1 3) Under Windows 95/98 A DOS mouse driver is only required in 
    Windows 3.1 or DOS 6. Windows 95/98 automatically supplies DOS 
    programs with a mouse driver when they are executed in a DOS 
    session.
    
    An earlier version of Matrix that didn't require expanded memory is
    probably still available from http://www.nonags.com There is no 
    advantage in using the earlier version unless memory is very 
    limited on you system.
    
    One advantage of running Matrix in DOS 6 or Windows 3.1 compared to
    Windows 95/98 is that it is much faster. This applies to all DOS 
    programs. 
    
    
    Faster Performance 
    ------------------ 
    The fastest performance can be achieved by reducing the maximum 
    number of lines in the spreadsheet. If you only want 132 lines 
    reduce the expanded setting to just 2,048 KBs in the program's 
    property setting. You will notice a big speed improvement if you 
    are running Matrix on a slow computer if you reduce the expanded 
    memory setting to just 2,048 KBs. But the disadvantage is that you 
    can only have 132 lines for the spreadsheet. But this might be 
    sufficient for you needs.
    
    When Matrix loads up there is a slight delay before the screen 
    displays. This delay may be a couple of seconds if you are running 
    Matrix on a 486 or slow Pentium processor with 16MBs available to 
    Matrix as expanded memory. You will also notice a slow response 
    when inserting and deleting rows if the sheet is filled with over 
    250 lines of data. There are two ways to increase the over all 
    speed of the program. (1) Reduce the Expanded setting to just 2048 
    KBs (2) Run Matrix from DOS 6 instead of Windows 95/98
      
    
    Disappearing columns 
    -------------------- 
    When inserting a column if the adjacent column is hidden the 
    contents of the cells in the current column will moved to it but 
    wont display because its hidden. Normally you would have all 
    columns revealed before inserting a column. The only time when you 
    should hide columns is when the data on the sheet is already 
    prepared and you are ready to add the finishing touches to 
    improve its appearance, such as colouring certain columns of 
    cells, locking cells, and hiding columns. Data on rows can be 
    hidden by giving the cells on a row the same foreground colour as 
    the background colour.
    
    
    Printer Support 
    ---------------- 
    When printing a file any data in hidden rows will be printed, but 
    not data in hidden columns. One way to prevent the printing of 
    certain data is to redirect output to a file. Go to the settings 
    option and change the name from LPT1 to the name of a file, such as
    "print.txt". Then print to the file. The file produced will be an 
    ordinary text file which can be loaded into a word processor or 
    text editor. You can then edit the file, improve its appearance 
    such as by changing the font, italicising certain lines, and deleting
    others. You can also merge the file into a document.   
    
    
    Distribution 
    ------------ 
    The latest Matrix is available from 
    http://members.xoom.com/finnegam/
    Copies of the program may be freely distributed over the Internet,
    or on disk or CD-ROM, provided all the original documentation and
    files are included.
    
    
    Customising the program 
    ----------------------- 
    Matrix was written in order to supply the global freeware market 
    with a good quality spreadsheet which can be used on computers with
    very limited resources.  All the source code is included for the 
    benefit of other programmers who may like to customise or improve 
    the program or just add new functions to the parser. To learn more 
    about how to do this see the readme.txt file in the source.zip 
    archive.
    
    
    --------------------------------------------------------------
    Michael Finnegan 
    Dingle, Co. Kerry 
    Ireland. 
    Wednesday, 23 February 2000
    finnegam@xoommail.com
