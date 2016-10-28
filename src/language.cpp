/***************************************************************************
 *   Copyright (C) 2004-2014 by Michael Griffin                            *
 *   mrmisticismo@hotmail.com                                              *
 *                                                                         *
 *   Purpose:                                                              *
 *                                                                         *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Enthral SVN: $Id: language.cpp 1 2014-03-29 07:30:21Z mercyful $
// Source: $HeadURL: file:///home/merc/repo/enthral/trunk/src/language.cpp $
// $LastChangedDate: 2014-03-29 02:30:21 -0500 (Sat, 29 Mar 2014) $
// $LastChangedRevision: 1 $
// $LastChangedBy: mercyful $

# include "language.h"
# include "struct.h"

# include <cstdio>
# include <cctype>
# include <cstring> // gcc 4.3
# include <cstdlib> // gcc 4.3

# include <string>
# include <fstream>
# include <sstream>    // Stream Numbers to Strings.



using namespace std;

/**
 * Remove Language.dat file
 */
void language::lang_remove()
{

    std::string path = DATAPATH;
    path += "language.dat";
    remove(path.c_str());
}

/**
 * Write to Language.dat from Language.txt
 */
int language::lang_write(LangRec *lang, int idx)
{

    std::string path = DATAPATH;
    path += "language.dat";
    int x = 0;

    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream = fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            perror("Error unable to write language.dat, check permissions!");
            return x;
        }
    }
    if(fseek(stream,(int)idx*sizeof(LangRec),SEEK_SET)==0)
        x = fwrite(lang,sizeof(LangRec),1,stream);
    fclose(stream);
    return x;
}

/**
 * Read Language.txt
 */
int language::lang_read(LangRec *lang, int idx)
{


    std::string path = DATAPATH;
    path += "language.dat";

    int x = 0;

    FILE *stream = fopen(path.c_str(),"rb+");
    if(stream == NULL)
    {
        stream=fopen(path.c_str(), "wb");
        if(stream == NULL)
        {
            perror("Error unable to read language.dat, check permissions!");
            return x;
        }
    }
    fclose(stream);

    stream = fopen(path.c_str(), "rb");
    if(fseek(stream,(int)idx*sizeof(LangRec),SEEK_SET)==0)
        x = fread(lang,sizeof(LangRec),1,stream);
    fclose(stream);
    return x;
}

/**
 * Count total items
 */
int language::lang_count()
{

    int i = 0;
    LangRec lang;

    while(lang_read(&lang,i))
    {
        ++i;
    }
    if(i < 1)	i = -1;
    else i--;
    return(i);
}

/**
 * Get Item at index (idx)
 */
void language::lang_get(char *lang, int idx)
{

    int id1 = 0;
    id1 = lang_count();
    LangRec l1;
    --idx;

    if (idx > id1)
    {
        return;
    }
    lang_read(&l1,idx);
    strcpy((char *)lang, (char *)l1.Lang);
}

/**
 * Parses Line in Language File 
 */
void language::lang_check(std::string lang)
{

    std::string temp2;
    // Disgards any Config lines starting with the # or ; Character
    
	char Num[255] = {0};
    int LangNum = 0;
    int cnt     = 0;

	if (lang[0] == '#' || lang[0] == ';') 
		return;

	if (lang.size() < 1)
		return;

    // Parse first 3 Digits for Language string Number
    for (int i = 0; i < 4; i++)
    {
        if (isdigit(lang[cnt]))
        {
            Num[cnt] = lang[cnt];
            ++cnt;
        }
    }

	std::stringstream iconvert(Num);   // In

	// Convert String to Int
	if (iconvert >> LangNum)
	{
		if (LangNum > 0)
			--LangNum;
	}
	else
	{
		// Error Unable to Convert
		return;
	}

	LangRec l1;
	memset(&l1,0,sizeof(LangRec));
    if (lang.size() > 4)
    {
        temp2 = lang.substr( 4);
        strcpy((char *)l1.Lang, (char *)temp2.c_str());
        lang_write(&l1,LangNum);
    }
}

/**
 * Copile Language.txt to Langauge.dat 
 */
void language::lang_compile()
{

    std::string name = INIPATH;
    name += "language.txt";

    ifstream inStream;
    inStream.open( name.c_str() );
    if (!inStream.is_open())
    {
        perror("Error unable to open language.dat, check permissions!");
        return;
    }
    lang_remove();

    std::string cfgdata;
    for (;;)
    {
        getline(inStream,cfgdata);
        lang_check(cfgdata);
        if(inStream.eof()) break;
    }
    inStream.close();
    return;
}




