#!/usr/bin/env python2.7
# uses python2-requests-2.2.1-1

import requests
import xml.etree.ElementTree
import bbs_io
import string
import re

#garbage Collection (Needed to Clear Memory!)
import gc

bbs = 'hTc'
usrname = u'mrmisticismo@hotmail.com'
pssword = u'******'

# Minus the header/Footer
real_limit = bbs_io.pyGetTermHeight() - 12

# Minus 1 col for last doesn't handle |CR Properly in populated with a character
real_width = bbs_io.pyGetTermWidth() - 1

url = 'http://bbs-scene.org/api/onelinerz.php?limit=%s' % str(real_limit)

bbs_io.pyPipe2Ansi("|CS|15 Loading Global BBS-Scene OneLiners|CR|CR")

req = requests.get(url, auth=(usrname, pssword))
if 200 != req.status_code:
    print 'error %s' % req.content
    print 'error %s' % req.status_code
    exit(1)

ansifile = "onelinerz"
bbs_io.pyAnsiPrintf(ansifile)

# can throw exceptions when xml is invalid, as a thread, nobody needs
# to catch it. theres some things that should be CDATA wrapped .. these
# break even viewing it in firefox, but upstream doesn't seem to
# notice, probably upstream does print('<xml_node>' + var +
# '</xml_node>'), i've found more than a few nasty escape flaws,
# we're breaking the shit out of encoding here, but most other bbs's
# are US-ASCII (cp437)-only, and bbs-api doesn't care

buf = ''.join((byte for byte in req.content
    if (ord(byte) >= 0x20
    or ord(byte) in (0x09, 0x0a, 0x0d, 0x7f))
    and ord(byte) not in (0x9f,)))

# Remove any unicode characters from string before transforamtion.
buf = "".join((c if ord(c) < 128 else '?' for c in buf))

xml_nodes = xml.etree.ElementTree.XML(buf).findall('node')

# Regex for Capatalizing First Letter in each Oneliner.
regex = re.compile("[A-Za-z]") # find a alpha

for node in xml_nodes:

    oneliner = node.find('oneliner').text

    if oneliner is not None:
        # Cap First Letter in Oneliner string, Look more pretty.
        s = regex.match(oneliner) # find the first alpha
        if s:
            s = regex.search(oneliner).group() # find the first alpha
            oneliner = oneliner.replace(s, s.upper(), 1) # replace only 1 instance

        alias = node.find('alias').text
        bbsname = node.find('bbsname').text
        tstamp = node.find('timestamp').text
        ilen = len(tstamp[5:-9])+1 + len(alias)+1 + len(bbsname)+3 + len(oneliner)
        if ilen >= real_width:
            oneliner = oneliner[:-(ilen-real_width)]
        ostring =  '|08'+tstamp[5:-9].replace('-', '/') + ' |07<|12' + alias + '|08!|12' + bbsname + '|07> |15' + oneliner + '|CR'

        bbs_io.pyPipe2Ansi(ostring)


bbs_io.pyPipe2Ansi("|CR|07Add a global Oneliners? |15(|12y|04/|12n|07)|08: ")

ch = bbs_io.pyGetKey()
character = chr(int(ch))

bbs_io.pyPipe2Ansi(character.upper())

if character.upper() == 'Y':

    bbs_io.pyPipe2Ansi("|CR")

    # Get currnet UserName from BBS
    username = bbs_io.pyPipe2String("|UH")

    bbsline = '|08  NOW |07<|12' + username + '|04!|12' + bbs + '|07> '
    # Set default length of input field to 10
    strlen = 0
    strlen = len(bbsline) - 22

    strlen = 75 - strlen

    # Append input field to text string
    text = bbs_io.pyInputField(bbsline,strlen);

    # Write out input field
    bbs_io.pyPipe2Ansi(text)

    # Now getline after inputfield!! :)  Fancy.
    getstr = bbs_io.pyGetLine(strlen);

    bbs_io.pyPipe2Ansi("|CR")
    #bbs_io.pyPipe2Ansi(getstr)

    strlen = 0
    strlen = len(getstr.strip())

    if strlen > 0:
        url = 'http://bbs-scene.org/api/onelinerz.xml'
        data = {
            'bbsname': bbs.strip(),
            'alias': username.strip(),
            'oneliner': getstr.strip(),
        }
        # post to bbs-scene.rog
        req = requests.post(url,auth=(usrname, pssword), data=data)
        if 200 != req.status_code:
            print 'error %s' % req.content
            print 'error %s' % req.status_code
            bbs_io.pyPipe2Ansi("|CR|04Message Error, Unable to Connect! |CR|CR|PA")
        else:
            bbs_io.pyPipe2Ansi("|CR|15Message Posted Successfully.|CR|CR|PA")
    else:
        bbs_io.pyPipe2Ansi("|CR|12aborted. |CR|CR|PA")

# Garbage Collection to free memory
gc.collect()

