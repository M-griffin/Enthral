# Global Oneliners Script with WallHall Interface at Shroo.ms

# Setup, need sudo apt-get install python-pip
# sudo pip install python-dateutil

import sys
import os
import json
import httplib
import urllib
import re

# Enthral BBS IO
import bbs_io

#garbage Collection (Needed to Clear Memory!)
import gc

# BBS Name (Configure this for your system)
bbs = "hTc"

# Default Ansi Header Screen
ansifile = "onelinerz"

# Keys for API Authorization
idkey = "xxx"
restkey = "xxx"

# Minus the header/Footer
real_limit = bbs_io.pyGetTermHeight() - 12

# Minus 1 col for last doesn't handle |CR Properly in populated with a character
real_width = bbs_io.pyGetTermWidth() - 1


# Thanks to x84 for this Tidbit.
def parse_iso8601(timestamp):
    from dateutil.parser import parse
    from dateutil.tz import tzlocal
    as_localtime = parse(timestamp).astimezone(tzlocal())
    return as_localtime.strftime('%Y-%m-%d %H:%M:%S')


# Grab One-liners and Parse.
def get_wall():
    try:
        connection = httplib.HTTPSConnection('api.parse.com', 443, timeout=5)
        params = urllib.urlencode({"limit": real_limit, "order": "-createdAt"})
        connection.connect()
    except:
        gotwall = 0
        return ""

    connection.request('GET', '/1/classes/wall?%s' % params, '', {
        "X-Parse-Application-Id": idkey,
        "X-Parse-REST-API-Key": restkey
    })

    # Regex for Capitalizing First Letter in each One-liner.
    regex = re.compile("[A-Za-z]")  # find a alpha

    oneliner = ''
    alias = ''
    bbsname = ''
    tstamp = ''
    ostring = ''

    result = json.loads(connection.getresponse().read())
    for item in reversed(result['results']):

        oneliner = str(item['bbstagline'])
        alias = str(item['bbsuser'])
        bbsname = str(item['bbsname'])
        tstamp = str(item['createdAt'])

        # Update TimeStamp
        tstamp = parse_iso8601(tstamp)

        # Cap First Letter in Oneliner string, Look more pretty.
        s = regex.match(oneliner) # find the first alpha
        if s:
            s = regex.search(oneliner).group() # find the first alpha
            oneliner = oneliner.replace(s, s.upper(), 1) # replace only 1 instance

        ilen = len(tstamp[5:-9])+1 + len(alias)+1 + len(bbsname)+3 + len(oneliner)

        if ilen >= real_width:
            oneliner = oneliner[:-(ilen-real_width)]
        ostring = '|08'+tstamp[5:-9].replace('-', '/') + ' |07<|12' + alias + '|08!|12' + bbsname + '|07> |15' + oneliner + '|CR'

        bbs_io.pyPipe2Ansi(ostring)


# Write out One Liner
def write_tag():
    bbs_io.pyPipe2Ansi("|CR")

    # Get currnet UserName from BBS
    username = bbs_io.pyPipe2String("|UH")

    bbsline = '|08  NOW |07<|12' + username + '|04!|12' + bbs + '|07> '

    # Set default length of input field to 10
    strlen = 0
    strlen = len(bbsline) - 22
    strlen = 75 - strlen

    # Append input field to text string
    text = bbs_io.pyInputField(bbsline, strlen)

    # Write out input field
    bbs_io.pyPipe2Ansi(text)

    # Now getline after inputfield!! :)  Fancy.
    getstr = bbs_io.pyGetLine(strlen)

    bbs_io.pyPipe2Ansi("|CR")

    strlen = 0
    strlen = len(getstr.strip())

    if strlen > 0:
        try:
            connection = httplib.HTTPSConnection('api.parse.com', 443, timeout=5)
            connection.connect()
        except:
            return ""

        connection.request('POST', '/1/classes/wall', json.dumps({
            "bbsname": bbs,
            "bbsfakeuser": False,
            "bbsuser": username.strip(),
            "bbstagline": getstr.strip()
        }), {
            "X-Parse-Application-Id": idkey,
            "X-Parse-REST-API-Key": restkey,
            "Content-Type": "application/json"
        })
        result = json.loads(connection.getresponse().read())
        results_length = len(result)
        if results_length == 0:
            bbs_io.pyPipe2Ansi("|CR|04Message Error, Unable to Connect! |CR|CR|PA")
        else:
            bbs_io.pyPipe2Ansi("|CR|15Message Posted Successfully.|CR|CR|PA")
    else:
        bbs_io.pyPipe2Ansi("|CR|12aborted. |CR|CR|PA")


def start_up():
    # Main Startup
    bbs_io.pyAnsiPrintf(ansifile)
    get_wall()
    bbs_io.pyPipe2Ansi("|CR|07Post a line to the wall? |15(|12y|04/|12n|07)|08: ")


# Initial Startup
start_up()

# If Yes, then prompt for one liner message to send.
while True:
    # Wait for Input
    ch = bbs_io.pyGetKey()
    character = chr(int(ch))

    # Add a new One liner
    if character.upper() == 'Y':
        bbs_io.pyPipe2Ansi(character.upper())
        write_tag()
        start_up()

    # Exit Done
    elif character.upper() == 'N':
        bbs_io.pyPipe2Ansi(character.upper())
        bbs_io.pyPipe2Ansi("|CR|12Done. |15|DE")
        break

    # Else, Invalid Key, don't redisplay, just loop back.

# Garbage Collection to free memory
gc.collect()
