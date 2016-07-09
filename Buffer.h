//-------------- notice that : we use utf-8 string ------------//
//notice that : we permit using correct cpu (big little endian)//

#pragma once

#include "Common.h"

EASY_NS_BEGIN

//we need reference
class Buffer : public Ref
{
private:

    char*    m_buffer;

    int      m_len;

public:
	~Buffer();

	Buffer(int len);

    //length() do not include "\0"
	Buffer(const char* str);

    //when you have str , and know its length
	Buffer(char* str, int len);

    bool isBigEndian();

    void write(const char* str,int offset,int len,bool same = false);

    const char* toString();

    int  length();

    //note that : end mean the last element
    //copy from soff to pt at poff
	void copy(Buffer* pt, int poff, int soff, int send);

	Buffer* slice(int offset, int end);

    //read method
    unsigned char  readUInt8(int offset);
    unsigned short readUInt16LE(int offset);
    unsigned short readUInt16BE(int offset);
    unsigned int   readUInt32LE(int offset);
    unsigned int   readUInt32BE(int offset);
    signed char   readInt8(int offset);				//in android,char is unsigned as default
    signed short  readInt16LE(int offset);
    signed short  readInt16BE(int offset);
    signed int    readInt32LE(int offset);
    signed int    readInt32BE(int offset);
    float  readFloatLE(int offset);
    float  readFloatBE(int offset);
    double readDoubleLE(int offset);
    double readDoubleBE(int offset);

    //write method
    void writeUInt8(unsigned char value, int offset);
    void writeUInt16LE(unsigned short value, int offset);
    void writeUInt16BE(unsigned short value, int offset);
    void writeUInt32LE(unsigned int value, int offset);
    void writeUInt32BE(unsigned int value, int offset);
    void writeInt8(char value, int offset);
    void writeInt16LE(short value, int offset);
    void writeInt16BE(short value, int offset);
    void writeInt32LE(int value, int offset);
    void writeInt32BE(int value, int offset);
    void writeFloatLE(float value, int offset);
    void writeFloatBE(float value, int offset);
    void writeDoubleLE(double value, int offset);
    void writeDoubleBE(double value, int offset);

};


EASY_NS_END