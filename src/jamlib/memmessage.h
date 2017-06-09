#ifndef MEMMESSAGE_H
#define MEMMESSAGE_H

// Enthral SVN: $Id$
// Source: $HeadURL$
// $LastChangedDate$
// $LastChangedRevision$
// $LastChangedBy$

#define PKT_CHUNKLEN  10000
#define PKT_NUM2D        50
#define PKT_NUMPATH      10

struct TextChunk {
    struct TextChunk *Next;
    uint32_t Length;
    uint8_t Data[PKT_CHUNKLEN];
};

struct Nodes2D {
    struct Nodes2D *Next;
    uint16_t Nodes;
    uint16_t Net[PKT_NUM2D];
    uint16_t Node[PKT_NUM2D];
};

struct Path {
    struct Path *Next;
    uint16_t Paths;
    uint8_t Path[PKT_NUMPATH][100];
};

#define MMFLAG_RESCANNED   1
#define MMFLAG_EXPORTED    2
#define MMFLAG_TOSSED      4
#define MMFLAG_NOSECURITY  8
#define MMFLAG_AUTOGEN    16
#define MMFLAG_TWIT       32
#define MMFLAG_KILL       64




#endif
