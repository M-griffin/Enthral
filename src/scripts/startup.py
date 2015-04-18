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


#------------------------------------------------------------------------------
# Main Module, Initial IO Function Tests
#
def main_module():

    # Clear Screen, Default color 07 = Grey Text, Background = |16 Black
    bbs_io.pyPipe2Ansi("|CS|07|16Wake on Lan Detected... |DE|DE|CS")

    # Write out BIOS Info Line
    bbs_io.pyPipe2Ansi("|15Enthral |07Python BIOS v1.0, An |15BBS-Scene |07Alley (|15irc.bbs-scene.org #|07enthral, |15#|07bbs|07)|CR")
    bbs_io.pyPipe2Ansi("Copyright (c) 2004-2014, Michael Griffin                  (|15irc.bbses.info |15#|07bbs|07)|CR|CR")

    #Detect and write out Basic CPU Info
    command = "cat /proc/cpuinfo"
    all_info = check_output(command, shell=True).strip()
    for line in all_info.split("\n"):
        if "model name" in line:
            bbs_io.pyPipe2Ansi(sub(".*model name.*:", "", line,1).strip()+"|CR")
            break

    # Clear Data
    del all_info

    #Detect Memory size in KB
    command = "cat /proc/meminfo"
    all_info = check_output(command, shell=True).strip()
    for line in all_info.split("\n"):
        if "MemTotal" in line:
            mem_string = sub(".*MemTotal.*:", "", line,1).strip()
            mem_size = sub(r"kB", "", mem_string,1).strip()   
            break

    # Clear Data
    del all_info

    # Convert String to Number for Loop.
    iMem_size = int(mem_size)

    # Memory test.
    bbs_io.pyPipe2Ansi("Memory Test : \x1b[30G|15ESC |07to Abort Memory Test|07")

    # Break from look when done.
    memtest_done = False

    try:
        # Loop Memory Test Counter
        for i in range(iMem_size): 

            # Listen for ESC Key, if Hit Abort Mem Test       
            while stdin in select([stdin], [], [], 0)[0]:
                ch = bbs_io.pyGetKey();
                if ch == '27':
                    escsequence = bbs_io.pyGetKeyExtended()                
                    if not escsequence:# we jsut just ESC no arror keys
                        # We got Escape, exit loop.
                        memtest_done = True
                        break               
            else:                            
                # Test if loop is done.
                if memtest_done is True:
                    break

                # Write out counter
                if 0 == i % 1024:
                    stdout.write("\r\x1b[15G%dK" %i)
                    stdout.flush()

    except:
        sys.exit(1)        

    # Write out final counter
    stdout.write("\r\x1b[15G%dK OK" % iMem_size)
    stdout.flush()

    # Write Generic Information, can't detect this. 
    bbs_io.pyPipe2Ansi("|CR|07Memory Information: |15DDR 400 mhz CL 2, T2 Dual Channel, 128 bit|CR")

    # Write Generic Init Output With a Delay at the End.
    bbs_io.pyPipe2Ansi("|CR|07Enthral Plug and Play Extensions v1.0A")
    bbs_io.pyPipe2Ansi("|CRInitialize Python Extensions...")
    bbs_io.pyPipe2Ansi("|CRPython Init Completed|DE|DE")

    # Detect Hard Drives (dmesg) - Wmvare  
    #[    1.869546] scsi 1:0:0:0: CD-ROM            NECVMWar VMware IDE CDR10 1.00 PQ: 0 ANSI: 5
    #[    2.652638] scsi 2:0:0:0: Direct-Access     VMware,  VMware Virtual S 1.0  PQ: 0 ANSI: 2

    bbs_io.pyPipe2Ansi("|CR|CRDetecting Hard Disks...|CR|DE")

    # Detect Hard Disks and PNP devices from dmesg 
    # Currently setup for Arch Linux
    command = "dmesg"
    all_info = check_output(command, shell=True).strip()
    for line in all_info.split("\n"):
        # Get Fist Scsi Drive
        if "scsi 0:0:0:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Primary  : |07");
            tmp = sub(".*scsi 0:0:0:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")

        # Get Second Scsi Drive
        elif "scsi 0:0:1:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Secondary: |07");
            tmp = sub(".*scsi 0:0:1:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")
        
        elif "scsi 2:0:0:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Primary  : |07");
            tmp = sub(".*scsi 2:0:0:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")

        # Get Second Scsi Drive
        elif "scsi 3:0:0:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Secondary: |07");
            tmp = sub(".*scsi 3:0:0:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")
                        
        # Get Second Scsi Drive
        elif "scsi 4:0:0:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Primary  : |07");
            tmp = sub(".*scsi 4:0:0:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")
            
        # Get Second Scsi Drive
        elif "scsi 4:0:1:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Secondary: |07");
            tmp = sub(".*scsi 4:0:1:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")
            
        # Get Second Scsi Drive
        elif "scsi 5:0:0:0:" in line:
            bbs_io.pyPipe2Ansi("|15SCSI Secondary: |07");
            tmp = sub(".*scsi 5:0:0:0:*:", "", line,1).strip()
            value = tmp.split("PQ:")
            bbs_io.pyPipe2Ansi(value[0] + "|CR")
            

        # Get Plug and Play (Mouse)
        elif "i8042: PNP:" in line:
            pnp = sub(".*i8042: PNP:*:", "", line,1).strip()    

        # Get Input Keyboard
        elif "serio: i8042 KBD" in line:
            value = line.split("serio: i8042")
            pnp2 = value[1].strip()

        # Get Input Mouse
        elif "serio: i8042 AUX" in line:
            value = line.split("serio: i8042")
            pnp3 = value[1].strip()

    # Clear Data
    del all_info

    bbs_io.pyPipe2Ansi("|CRDetecting PNP Devices...|CR|DE")
    bbs_io.pyPipe2Ansi("|15Detected : |07" + pnp);
    bbs_io.pyPipe2Ansi("|CR|15Detected : |07" + pnp2);
    bbs_io.pyPipe2Ansi("|CR|15Detected : |07" + pnp3);

    bbs_io.pyPipe2Ansi("|CR");
    
    loopcount = 4
    
    bbs_io.pyPipe2Ansi("|CR|15ESC |07to Continue Bootup Sequence or |15Wait ")

    # Listen for ESC Key, to Exit Script and Continue into BBS.
    #while True:
    #    ch = bbs_io.pyGetKey()
    #    if ch == "27":
    #        break
    # Listen for ESC Key, if Hit Abort Mem Test       
    
    # 4 Seconds until we start boot up sequence if ESC is not hit.
    waittest_done = False
    for i in range(10,-1,-1):
        while stdin in select([stdin], [], [], 0)[0]:
            ch = bbs_io.pyGetKey();
            if ch == '27':
                escsequence = bbs_io.pyGetKeyExtended()                
                if not escsequence: # we just just ESC no arrow keys
                    # We got Escape, exit loop.
                    waittest_done = True
                    break            
        else:

            # Format Output with color Pipe Codes.            
            output = format("\r\x1b[41G|07%01d seconds." % i)
            bbs_io.pyPipe2Ansi(output)

            sleep(1)
            if waittest_done is True:
                break


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
    bbs_io.pyPipe2Ansi("|CR|CR                           '-[                   ]-'");

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
            ch = bbs_io.pyGetKey();
            if ch == '27':
                escsequence = bbs_io.pyGetKeyExtended()                
                if not escsequence:# we jsut just ESC no arror keys
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


# Execute Start of Bios Script here.
main_module()

#garbage collection After function to free memory
gc.collect()

