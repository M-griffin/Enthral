#------------------------------------------------------------------------------
# Enthral BBS - Basic Enthral Embedded Python Functionality TEST.
# Michael Griffin Last updated: 2013-09-29
#
# This is a very basic test, of inital functions that are available.  There is 
# not a lot here at the moment, but alot more like reading data files and more
# internals to comes.
#------------------------------------------------------------------------------

# Inport the bbs functions.
import bbs_io

#garbage Collection (Needed to Clear Memory!)
import gc

#------------------------------------------------------------------------------
# Main Module, Initial IO Function Tests
#
def main_module():

    # Clear Screen, Default color 15 = White Text, Background = |18 Dark Green
    bbs_io.pyPipe2Ansi("|CS|15|18")

    # Put Raw Line of Text, No String Parsing
    bbs_io.pyPutLine("This is just a test!, test.py in the src/scripts folder! :) ")

    # Reset Text color to 15 = White, Background |16 = Black with 2 carriage returns
    bbs_io.pyPipe2Ansi("|15|16|CR|CR")

    # Run Pause Prompt
    bbs_io.pyStartPause()

    # Put String with MCI Codes |09 = Light Blue, with |IP Ip address, |DE = Wait 1 second...  CR = Carriage Return
    bbs_io.pyPipe2Ansi("|CR|09This is a Test with MCI Codes: |07{ |15IP ADDRESS |07} |13|IP, |DE|05Wait Over!  |CR|03Did you like the test?|CR")

    #Put String with MCI Codes
    bbs_io.pyPipe2Ansi("|CR|CR|09This is a simple get HotKey Test, hit any key! |CR");

    #Get Hotkey Input into ch, will Contain All Chars, Minus Escape Sequences.. If ch = 27, then ESC Sequence, Arrow Key, etc..

    # Testing the pyGetKey Functions
    bbs_io.pyPipe2Ansi('|15Hotkey Input: |11')

    # ch = the int key code returned from key press.
    ch = bbs_io.pyGetKey()
    
    # Write out the key that was pressed first. 
    # Convery from a String with Number, to Int Number, Then to Character Value.
    print chr(int(ch))

    # Test Key Input, and Extended Esc Sequences
    # if input Ch == 27, & Extended is not blank, Then Escape
    # Other Extended Sequence capture Arrow, Home Insert, F1 keys etc..
    # Then it could be arror keys, Home, Insert etc...  
   
    myStr = "|CR|03Char (Int): |11" + ch
    bbs_io.pyPipe2Ansi(myStr)

    # Check input character.
    if ch == '27':
    	if bbs_io.pyGetKeyExtended() <> "":
            # Means we got an Arror Key, Home, Insert Etc..
            myStr = "|CR|03GetKeyExtended (ESC Sequence): |11" + bbs_io.pyGetKeyExtended()
    	    bbs_io.pyPipe2Ansi(myStr)
	    
    	else:
            # Means we got just the ESC Key
    	    bbs_io.pyPipe2Ansi("|CR|03GetKeyExtended (ESC Sequence) Empty (ESC Hit!)")
    else:
        # Normal Key/Letter Input
        bbs_io.pyPipe2Ansi("|CR|03Normal Input")

    # Reset Text color to 15 = White, Background |16 = Black with 2 carriage returns
    bbs_io.pyPipe2Ansi("|15|16|CR|CR")

    # Set Length of Input to Return to 15 chars.
    bbs_io.pyPipe2Ansi("|15Get Line Input |08(|15Hit ENTER when done.|08) |07: ")

    len = 15
    getstr = bbs_io.pyGetLine(len);

    bbs_io.pyPipe2Ansi("|03|CR|CRPrinting Line Input: |07")
    print getstr


    # Test Fancy Input, input field,  |IN Limits the input field (overide), |FB sets forground and bg colors.
    text = "|CRInput Field Testing.. : "
    
    # Set default length of input field to 10
    len = 10

    # Append input field to text string
    text = bbs_io.pyInputField(text,len);

    # Write out input field
    bbs_io.pyPipe2Ansi(text)


    # Now getline after inputfield!! :)  Fancy.
    getstr = bbs_io.pyGetLine(len);

    bbs_io.pyPipe2Ansi("|03|CR|CRPrinting Inputfield Line Input: |07")
    print getstr


    bbs_io.pyPipe2Ansi("|15|CR|CRDisplaying an ansi file, hit any key to continue|CR|CR")
    bbs_io.pyStartPause()

    # Will printf out ansi.ans in ansi folder leave off extension .ans
    ansifile = "file"
    bbs_io.pyAnsiPrintf(ansifile)

    # Run Pause Prompt
    bbs_io.pyStartPause()

    # Clear Screen,  Move down two lones, then go back to Home |xy coords . Top left of screen!
    bbs_io.pyPipe2Ansi("|CS|CR|CR|XY0101|13Testing XY top left coner of screen")

    # Print Done.
    bbs_io.pyPipe2Ansi("|CR|05done.")

    # Run Pause Prompt
    bbs_io.pyStartPause()

# Execute Start of Script here.
main_module()

# Garbage Collection to free memory
gc.collect()




