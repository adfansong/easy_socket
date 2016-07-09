#include "Buffer.h"

EASY_NS_BEGIN

Buffer::~Buffer()
{
    delete m_buffer;
    //for debug.
    //EASY_LOG("Buffer is delete \n");
}

Buffer::Buffer(int len)
{
    //this help toString() is end with '\0'.
    m_buffer = new char[len + 1];

    m_buffer[len] = '\0';

    m_len = len;

}

Buffer::Buffer(const char* str)
{
    m_len = strlen(str);
    m_buffer = new char[m_len + 1];

    strcpy(m_buffer,str);

}

Buffer::Buffer(char* str, int len)
{
    m_buffer = str;
    m_len = len;    
}

bool Buffer::isBigEndian()
{
    unsigned short test = 0x0201;

    unsigned char* p = (unsigned char*)&test;
    if(p[0] == 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Buffer::write(const char* str, int offset, int len, bool same /* = false */)
{
    if(offset >= m_len)
    {
        return;
    }

    int left = m_len - offset;

    if(same)
    {
        memmove(m_buffer + offset,str, len > left ? left : len);
    }
    else
    {
        memcpy(m_buffer + offset,str, len > left ? left : len);
    }
}

const char* Buffer::toString()
{
    return m_buffer;
}

int  Buffer::length()
{
    return m_len;
}

//note that : end mean the last element
//copy from soff to pt at poff
void Buffer::copy(Buffer* pt, int poff, int soff, int send)
{
    if(soff >= m_len)
    {
        return;
    }

    if(send > m_len)
    {
        send = m_len;
    }

    //check is the same str?
    if(pt == this)
    {
        pt->write(m_buffer+soff,poff,send - soff,true);
    }
    else
    {
        pt->write(m_buffer+soff,poff,send - soff);
    }

}

//note that : end mean the last element -- u need release it,not retain it.
Buffer* Buffer::slice(int offset, int end)
{
    if(offset >= m_len)
    {
        return NULL;
    }

	Buffer* p = new Buffer(end - offset);

    copy(p,0,offset,end);

    return p;
}

//read method
unsigned char  Buffer::readUInt8(int offset)
{
    if(offset > m_len - 1)
    {
        return 0;
    }

    return (unsigned char)(m_buffer[offset]);
}

unsigned short Buffer::readUInt16LE(int offset)
{
    if(offset > m_len - 2)
    {
        return 0;
    }

    unsigned short tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset+1];
        p[1] = m_buffer[offset];
    }
    else
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
    }

    return tmp;

}

unsigned short Buffer::readUInt16BE(int offset)
{
    if(offset > m_len - 2)
    {
        return 0;
    }

    unsigned short tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
    }
    else
    {
        p[0] = m_buffer[offset+1];
        p[1] = m_buffer[offset];
    }

    return tmp;
}

unsigned int Buffer::readUInt32LE(int offset)
{
    if(offset > m_len - 4)
    {
        return 0;
    }

    unsigned int tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset+3];
        p[1] = m_buffer[offset+2];
        p[2] = m_buffer[offset+1];
        p[3] = m_buffer[offset];
    }
    else
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
        p[2] = m_buffer[offset+2];
        p[3] = m_buffer[offset+3];
    }


    return tmp;
}

unsigned int Buffer::readUInt32BE(int offset)
{
    if(offset > m_len - 4)
    {
        return 0;
    }

    unsigned int tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
        p[2] = m_buffer[offset+2];
        p[3] = m_buffer[offset+3];
    }
    else
    {
        p[0] = m_buffer[offset+3];
        p[1] = m_buffer[offset+2];
        p[2] = m_buffer[offset+1];
        p[3] = m_buffer[offset];
    }


    return tmp;
}

signed char Buffer::readInt8(int offset)
{
    return (signed char)readUInt8(offset);
}

signed short Buffer::readInt16LE(int offset)
{
    return (signed short)readUInt16LE(offset);
}

signed short Buffer::readInt16BE(int offset)
{
    return (signed short)readUInt16BE(offset);
}

signed int Buffer::readInt32LE(int offset)
{
    return (signed int)readUInt32LE(offset);
}

signed int Buffer::readInt32BE(int offset)
{
    return (signed int)readUInt32BE(offset);
}

float Buffer::readFloatLE(int offset)
{
    return (float)readUInt32LE(offset);
}

float Buffer::readFloatBE(int offset)
{
    return (float)readUInt32BE(offset);
}

double Buffer::readDoubleLE(int offset)
{
    if(offset > m_len - 8)
    {
        return 0;
    }

    double tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset+7];
        p[1] = m_buffer[offset+6];
        p[2] = m_buffer[offset+5];
        p[3] = m_buffer[offset+4];
        p[4] = m_buffer[offset+3];
        p[5] = m_buffer[offset+2];
        p[6] = m_buffer[offset+1];
        p[7] = m_buffer[offset];
    }
    else
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
        p[2] = m_buffer[offset+2];
        p[3] = m_buffer[offset+3];
        p[4] = m_buffer[offset+4];
        p[5] = m_buffer[offset+5];
        p[6] = m_buffer[offset+6];
        p[7] = m_buffer[offset+7];
    }


    return tmp;
}

double Buffer::readDoubleBE(int offset)
{
    if(offset > m_len - 8)
    {
        return 0;
    }

    double tmp;
    char* p = (char*)&tmp;

    if(isBigEndian())
    {
        p[0] = m_buffer[offset];
        p[1] = m_buffer[offset+1];
        p[2] = m_buffer[offset+2];
        p[3] = m_buffer[offset+3];
        p[4] = m_buffer[offset+4];
        p[5] = m_buffer[offset+5];
        p[6] = m_buffer[offset+6];
        p[7] = m_buffer[offset+7];
    }
    else
    {
        p[0] = m_buffer[offset+7];
        p[1] = m_buffer[offset+6];
        p[2] = m_buffer[offset+5];
        p[3] = m_buffer[offset+4];
        p[4] = m_buffer[offset+3];
        p[5] = m_buffer[offset+2];
        p[6] = m_buffer[offset+1];
        p[7] = m_buffer[offset];
    }

    return tmp;
}

//write method
void Buffer::writeUInt8(unsigned char value, int offset)
{
    if(offset > m_len - 1)
    {
        return;
    }

    m_buffer[offset] = value;
}

void Buffer::writeUInt16LE(unsigned short value, int offset)
{
    if(offset > m_len - 2)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[1];
        m_buffer[offset+1] = p[0];
    }
    else
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
    }

}

void Buffer::writeUInt16BE(unsigned short value, int offset)
{
    if(offset > m_len - 2)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
    }
    else
    {
        m_buffer[offset] = p[1];
        m_buffer[offset+1] = p[0];
    }
}

void Buffer::writeUInt32LE(unsigned int value, int offset)
{
    if(offset > m_len - 4)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[3];
        m_buffer[offset+1] = p[2];
        m_buffer[offset+2] = p[1];
        m_buffer[offset+3] = p[0];
    }
    else
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
        m_buffer[offset+2] = p[2];
        m_buffer[offset+3] = p[3];
    }
}

void Buffer::writeUInt32BE(unsigned int value, int offset)
{
    if(offset > m_len - 4)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
        m_buffer[offset+2] = p[2];
        m_buffer[offset+3] = p[3];
    }
    else
    {
        m_buffer[offset] = p[3];
        m_buffer[offset+1] = p[2];
        m_buffer[offset+2] = p[1];
        m_buffer[offset+3] = p[0];
    }
}

void Buffer::writeInt8(char value, int offset)
{
    writeUInt8((unsigned char)value,offset);
}

void Buffer::writeInt16LE(short value, int offset)
{
    writeUInt16LE((unsigned short)value,offset);
}

void Buffer::writeInt16BE(short value, int offset)
{
    writeUInt16BE((unsigned short)value,offset);
}

void Buffer::writeInt32LE(int value, int offset)
{
    writeUInt32LE((unsigned int)value,offset);
}

void Buffer::writeInt32BE(int value, int offset)
{
    writeUInt32BE((unsigned int)value,offset);
}

void Buffer::writeFloatLE(float value, int offset)
{
    writeUInt32LE((unsigned int)value,offset);
}

void Buffer::writeFloatBE(float value, int offset)
{
    writeUInt32BE((unsigned int)value,offset);
}

void Buffer::writeDoubleLE(double value, int offset)
{
    if(offset > m_len - 8)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[7];
        m_buffer[offset+1] = p[6];
        m_buffer[offset+2] = p[5];
        m_buffer[offset+3] = p[4];
        m_buffer[offset+4] = p[3];
        m_buffer[offset+5] = p[2];
        m_buffer[offset+6] = p[1];
        m_buffer[offset+7] = p[0];
    }
    else
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
        m_buffer[offset+2] = p[2];
        m_buffer[offset+3] = p[3];
        m_buffer[offset+4] = p[4];
        m_buffer[offset+5] = p[5];
        m_buffer[offset+6] = p[6];
        m_buffer[offset+7] = p[7];
    }
}

void Buffer::writeDoubleBE(double value, int offset)
{
    if(offset > m_len - 8)
    {
        return;
    }

    char* p = (char*)&value;

    if(isBigEndian())
    {
        m_buffer[offset] = p[0];
        m_buffer[offset+1] = p[1];
        m_buffer[offset+2] = p[2];
        m_buffer[offset+3] = p[3];
        m_buffer[offset+4] = p[4];
        m_buffer[offset+5] = p[5];
        m_buffer[offset+6] = p[6];
        m_buffer[offset+7] = p[7];
    }
    else
    {
        m_buffer[offset] = p[7];
        m_buffer[offset+1] = p[6];
        m_buffer[offset+2] = p[5];
        m_buffer[offset+3] = p[4];
        m_buffer[offset+4] = p[3];
        m_buffer[offset+5] = p[2];
        m_buffer[offset+6] = p[1];
        m_buffer[offset+7] = p[0];
    }
}

EASY_NS_END