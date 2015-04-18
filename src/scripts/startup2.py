#------------------------------------------------------------------------------
# Enthral BBS - Enthral Python BIOS Startup Script.
# - This is a general script to mimic the bootup process of a computer
# - Without root access, some hardward can not be detected so some things
# - Are hard coded, feel free to modify as needed for look and feel.
#
# Michael Griffin Created: 2013-10-13
#
#------------------------------------------------------------------------------

# Import the bbs functions.
import bbs_io

# Load Python Modules
from subprocess import check_output
from select     import select
from time       import sleep
from sys        import stdin, stdout
from re         import sub

# garbage Collection (Needed to Clear Memory!)
import gc


# Repeat String for max percentage bar.
def repstr(string, length):
    return (string * length)[0:length]


# Bootup Sequence
def bootup_sequence():
    
    # Startup Message
    bbs_io.pyPipe2Ansi("|CS|07|16Starting Bootup Sequence... |DE|DE|CS")

    ansifile = "osload"
    bbs_io.pyAnsiPrintf(ansifile)

    bbs_io.pyPipe2Ansi("|CR|CR                             |15Enthral OS |07v|151.0 |07Alpha")
    bbs_io.pyPipe2Ansi("|CR|CR                           '-[                   ]-'")

    bootup_done = False

    # Loop Bootup Bar
    barLen = 20
    percent = 0
    progress = ""
    color = 3

    # Setup Bootup Percentage Bar (ESC) Will abort.
    for i in range(barLen): 

        # Listen for ESC Key, if Hit Abort Mem Test       
        while stdin in select([stdin], [], [], 0)[0]:
            ch = bbs_io.pyGetKey()
            if ch == '27':
                escsequence = bbs_io.pyGetKeyExtended()                
                if not escsequence: # we jsut just ESC no arror keys
                    # We got Escape, exit loop.
                    bootup_done = True                

                    # Display full pencentage bar
                    # Repeat string to max length after abort
                    output = format("\r\x1b[31G|%02d" % (color))
                    bbs_io.pyPipe2Ansi(output)

                    color = 11
                    progress = chr(177)
                    bbs_io.pyPipe2Ansi(repstr(progress,19))
            
                    break

        else:

            # Test if loop is done.
            if bootup_done is True:
                break


            if i < int(barLen * percent):
                progress += chr(177)

            # Format Output with color Pipe Codes.            
            output = format("\r\x1b[31G|%02d" % (color))
            bbs_io.pyPipe2Ansi(output)

            # Slide Fade the update on progress bar.
            for o in range(len(progress)):
                bbs_io.pyPipe2Ansi(progress[o])
                sleep(0.02)

            # Delay between drawing redrawing bar
            sleep(0.15)

            percent += 1

            # Alternate high/low colors.
            if color == 3:
                color = 11
            else:
                color = 3

    bbs_io.pyPipe2Ansi("|CR|CR                                   |07OS Loaded|08.")
    bbs_io.pyPipe2Ansi("|DE|DE")
    bbs_io.pyPipe2Ansi("|CS|07")

# Execute Bootup Sequence
bootup_sequence()

# garbage collection After function to free memory
gc.collect()


